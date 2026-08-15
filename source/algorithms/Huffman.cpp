#include "Huffman.h"
#include "../common/BitStream.h"
#include "../common/Timer.h"
#include <fstream>
#include <stdexcept>
#include <iostream>

void Huffman::generateCodes(const HuffmanNode* node, const std::string& currentCode, std::vector<std::string>& codeTable) {
    if (!node) return;

    if (node->isLeaf()) {
        // Special case: If the input consists of only 1 unique character
        codeTable[node->symbol] = currentCode.empty() ? "0" : currentCode;
        return;
    }

    generateCodes(node->left, currentCode + "0", codeTable);
    generateCodes(node->right, currentCode + "1", codeTable);
}

void Huffman::freeTree(HuffmanNode* node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

HuffmanNode* Huffman::buildTree(const std::vector<uint64_t>& frequencies) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, HuffmanNodeComparator> pq;

    for (int i = 0; i < 256; ++i) {
        if (frequencies[i] > 0) {
            pq.push(new HuffmanNode(static_cast<uint8_t>(i), frequencies[i]));
        }
    }

    if (pq.empty()) {
        return nullptr;
    }

    // If only one unique character exists, wrap it in a root
    if (pq.size() == 1) {
        HuffmanNode* singleLeaf = pq.top();
        pq.pop();
        HuffmanNode* root = new HuffmanNode(singleLeaf->frequency, singleLeaf, nullptr);
        return root;
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top();
        pq.pop();
        HuffmanNode* right = pq.top();
        pq.pop();

        HuffmanNode* parent = new HuffmanNode(left->frequency + right->frequency, left, right);
        // Parent symbol can be minimum of children's symbols for deterministic tie-breaking
        parent->symbol = (left->symbol < right->symbol) ? left->symbol : right->symbol;
        pq.push(parent);
    }

    return pq.top();
}

CompressionMetrics Huffman::compress(const std::string& inputPath, const std::string& outputPath) {
    CompressionMetrics metrics;
    metrics.algorithmName = "Huffman";

    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputPath);
    }

    // Get input file size
    inFile.seekg(0, std::ios::end);
    uint64_t originalSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);
    metrics.originalSizeBytes = originalSize;

    Timer timer;
    timer.start();

    // Pass 1: Frequency analysis
    std::vector<uint64_t> frequencies(256, 0);
    const size_t BUFFER_SIZE = 64 * 1024;
    std::vector<char> buffer(BUFFER_SIZE);

    while (inFile.read(buffer.data(), BUFFER_SIZE) || inFile.gcount() > 0) {
        std::streamsize bytesRead = inFile.gcount();
        for (std::streamsize i = 0; i < bytesRead; ++i) {
            frequencies[static_cast<uint8_t>(buffer[i])]++;
        }
    }

    // Build Huffman tree & code table
    HuffmanNode* root = buildTree(frequencies);
    std::vector<std::string> codeTable(256);
    if (root) {
        generateCodes(root, "", codeTable);
    }

    // Open output file
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        freeTree(root);
        throw std::runtime_error("Cannot open output file: " + outputPath);
    }

    // Write Header:
    // 1. Magic bytes: 'H', 'U', 'F'
    outFile.put('H');
    outFile.put('U');
    outFile.put('F');

    // 2. Original size (8 bytes)
    outFile.write(reinterpret_cast<const char*>(&originalSize), sizeof(originalSize));

    // 3. Number of distinct symbols (2 bytes)
    uint16_t distinctCount = 0;
    for (int i = 0; i < 256; ++i) {
        if (frequencies[i] > 0) distinctCount++;
    }
    outFile.write(reinterpret_cast<const char*>(&distinctCount), sizeof(distinctCount));

    // 4. Frequency table entries
    for (int i = 0; i < 256; ++i) {
        if (frequencies[i] > 0) {
            uint8_t sym = static_cast<uint8_t>(i);
            outFile.put(static_cast<char>(sym));
            uint64_t freq = frequencies[i];
            outFile.write(reinterpret_cast<const char*>(&freq), sizeof(freq));
        }
    }

    // Pass 2: Encode data into bitstream
    if (originalSize > 0 && root != nullptr) {
        inFile.clear();
        inFile.seekg(0, std::ios::beg);

        BitWriter bitWriter(outFile);
        while (inFile.read(buffer.data(), BUFFER_SIZE) || inFile.gcount() > 0) {
            std::streamsize bytesRead = inFile.gcount();
            for (std::streamsize i = 0; i < bytesRead; ++i) {
                const std::string& code = codeTable[static_cast<uint8_t>(buffer[i])];
                for (char bitChar : code) {
                    bitWriter.writeBit(bitChar == '1');
                }
            }
        }
        bitWriter.flush();
    }

    outFile.flush();
    outFile.seekp(0, std::ios::end);
    uint64_t compressedSize = outFile.tellp();
    metrics.compressedSizeBytes = compressedSize;

    timer.stop();
    metrics.executionTimeMs = timer.elapsedMilliseconds();

    freeTree(root);
    inFile.close();
    outFile.close();

    return metrics;
}

CompressionMetrics Huffman::decompress(const std::string& inputPath, const std::string& outputPath) {
    CompressionMetrics metrics;
    metrics.algorithmName = "Huffman";

    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputPath);
    }

    inFile.seekg(0, std::ios::end);
    uint64_t compressedSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);
    metrics.compressedSizeBytes = compressedSize;

    Timer timer;
    timer.start();

    // Read Magic bytes
    char magic[3];
    if (!inFile.read(magic, 3) || magic[0] != 'H' || magic[1] != 'U' || magic[2] != 'F') {
        throw std::runtime_error("Invalid or corrupted Huffman compressed file format.");
    }

    // Read original size
    uint64_t originalSize = 0;
    inFile.read(reinterpret_cast<char*>(&originalSize), sizeof(originalSize));
    metrics.originalSizeBytes = originalSize;

    // Read distinct symbol count
    uint16_t distinctCount = 0;
    inFile.read(reinterpret_cast<char*>(&distinctCount), sizeof(distinctCount));

    // Reconstruct frequency table
    std::vector<uint64_t> frequencies(256, 0);
    for (uint16_t i = 0; i < distinctCount; ++i) {
        char symChar;
        inFile.get(symChar);
        uint8_t sym = static_cast<uint8_t>(symChar);

        uint64_t freq = 0;
        inFile.read(reinterpret_cast<char*>(&freq), sizeof(freq));
        frequencies[sym] = freq;
    }

    // Rebuild tree
    HuffmanNode* root = buildTree(frequencies);

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        freeTree(root);
        throw std::runtime_error("Cannot open output file: " + outputPath);
    }

    // Decode bitstream
    if (originalSize > 0 && root != nullptr) {
        BitReader bitReader(inFile);
        uint64_t decodedBytes = 0;

        // If only 1 distinct symbol, root might be special single-child tree
        if (distinctCount == 1) {
            uint8_t singleSym = 0;
            for (int i = 0; i < 256; ++i) {
                if (frequencies[i] > 0) {
                    singleSym = static_cast<uint8_t>(i);
                    break;
                }
            }
            std::vector<char> outBuf(originalSize, static_cast<char>(singleSym));
            outFile.write(outBuf.data(), originalSize);
            decodedBytes = originalSize;
        } else {
            const size_t OUT_BUFFER_SIZE = 64 * 1024;
            std::vector<char> outBuffer(OUT_BUFFER_SIZE);
            size_t outIndex = 0;

            HuffmanNode* current = root;
            bool bit;
            while (decodedBytes < originalSize && bitReader.readBit(bit)) {
                current = bit ? current->right : current->left;
                if (!current) {
                    throw std::runtime_error("Error decoding Huffman bitstream: reached null node.");
                }

                if (current->isLeaf()) {
                    outBuffer[outIndex++] = static_cast<char>(current->symbol);
                    decodedBytes++;
                    if (outIndex == OUT_BUFFER_SIZE) {
                        outFile.write(outBuffer.data(), OUT_BUFFER_SIZE);
                        outIndex = 0;
                    }
                    current = root;
                }
            }

            if (outIndex > 0) {
                outFile.write(outBuffer.data(), outIndex);
            }

            if (decodedBytes != originalSize) {
                throw std::runtime_error("Decompression warning: decoded byte count does not match original size.");
            }
        }
    }

    timer.stop();
    metrics.executionTimeMs = timer.elapsedMilliseconds();

    freeTree(root);
    inFile.close();
    outFile.close();

    return metrics;
}
