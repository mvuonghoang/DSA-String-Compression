#include "Deflate.h"
#include "Huffman.h"
#include "../common/BitStream.h"
#include "../common/Timer.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <queue>

// =============================================================================
// MEMBER 1 IMPLEMENTATION: Huffman Entropy Coding for Deflate Tokens
// =============================================================================

void Deflate::encodeTokensWithHuffman(const std::vector<LZ77Token>& tokens, uint64_t originalSize, std::ostream& out) {
    // 1. Frequency analysis for literal symbols (0-255)
    std::vector<uint64_t> literalFreqs(256, 0);
    for (const auto& tok : tokens) {
        if (!tok.isMatch) {
            literalFreqs[tok.literal]++;
        }
    }

    // Build Huffman tree for literals
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, HuffmanNodeComparator> pq;
    for (int i = 0; i < 256; ++i) {
        if (literalFreqs[i] > 0) {
            pq.push(new HuffmanNode(static_cast<uint8_t>(i), literalFreqs[i]));
        }
    }

    HuffmanNode* root = nullptr;
    if (pq.size() == 1) {
        HuffmanNode* single = pq.top();
        pq.pop();
        root = new HuffmanNode(single->frequency, single, nullptr);
    } else if (pq.size() > 1) {
        while (pq.size() > 1) {
            HuffmanNode* l = pq.top(); pq.pop();
            HuffmanNode* r = pq.top(); pq.pop();
            HuffmanNode* parent = new HuffmanNode(l->frequency + r->frequency, l, r);
            parent->symbol = (l->symbol < r->symbol) ? l->symbol : r->symbol;
            pq.push(parent);
        }
        root = pq.top();
    }

    std::vector<std::string> codeTable(256);
    auto genCodes = [&](auto self, const HuffmanNode* node, const std::string& code) -> void {
        if (!node) return;
        if (node->isLeaf()) {
            codeTable[node->symbol] = code.empty() ? "0" : code;
            return;
        }
        self(self, node->left, code + "0");
        self(self, node->right, code + "1");
    };
    if (root) genCodes(genCodes, root, "");

    // 2. Write Deflate File Header
    // Magic bytes 'D', 'E', 'F'
    out.put('D'); out.put('E'); out.put('F');

    // Original size (8 bytes)
    out.write(reinterpret_cast<const char*>(&originalSize), sizeof(originalSize));

    // Token count (8 bytes)
    uint64_t tokenCount = tokens.size();
    out.write(reinterpret_cast<const char*>(&tokenCount), sizeof(tokenCount));

    // Number of distinct literal symbols (2 bytes)
    uint16_t distinctCount = 0;
    for (int i = 0; i < 256; ++i) {
        if (literalFreqs[i] > 0) distinctCount++;
    }
    out.write(reinterpret_cast<const char*>(&distinctCount), sizeof(distinctCount));

    // Write literal frequency table
    for (int i = 0; i < 256; ++i) {
        if (literalFreqs[i] > 0) {
            uint8_t sym = static_cast<uint8_t>(i);
            out.put(static_cast<char>(sym));
            uint64_t freq = literalFreqs[i];
            out.write(reinterpret_cast<const char*>(&freq), sizeof(freq));
        }
    }

    // 3. Write Token Bitstream
    BitWriter bitWriter(out);
    for (const auto& tok : tokens) {
        if (!tok.isMatch) {
            // Write bit 0 for literal
            bitWriter.writeBit(false);
            // Write Huffman code for literal
            const std::string& code = codeTable[tok.literal];
            for (char b : code) {
                bitWriter.writeBit(b == '1');
            }
        } else {
            // Write bit 1 for (Distance, Length) match
            bitWriter.writeBit(true);
            // Write 16-bit length and 16-bit distance
            bitWriter.writeBits(tok.length, 16);
            bitWriter.writeBits(tok.distance, 16);
        }
    }
    bitWriter.flush();

    // Clean up tree
    auto freeTree = [](auto self, HuffmanNode* node) -> void {
        if (!node) return;
        self(self, node->left);
        self(self, node->right);
        delete node;
    };
    freeTree(freeTree, root);
}

std::vector<LZ77Token> Deflate::decodeTokensWithHuffman(std::istream& in, uint64_t& originalSize) {
    // 1. Read Header
    char magic[3];
    if (!in.read(magic, 3) || magic[0] != 'D' || magic[1] != 'E' || magic[2] != 'F') {
        throw std::runtime_error("Invalid or corrupted Deflate compressed file.");
    }

    in.read(reinterpret_cast<char*>(&originalSize), sizeof(originalSize));

    uint64_t tokenCount = 0;
    in.read(reinterpret_cast<char*>(&tokenCount), sizeof(tokenCount));

    uint16_t distinctCount = 0;
    in.read(reinterpret_cast<char*>(&distinctCount), sizeof(distinctCount));

    std::vector<uint64_t> literalFreqs(256, 0);
    for (uint16_t i = 0; i < distinctCount; ++i) {
        char symChar;
        in.get(symChar);
        uint8_t sym = static_cast<uint8_t>(symChar);
        uint64_t freq = 0;
        in.read(reinterpret_cast<char*>(&freq), sizeof(freq));
        literalFreqs[sym] = freq;
    }

    // Reconstruct Huffman tree
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, HuffmanNodeComparator> pq;
    for (int i = 0; i < 256; ++i) {
        if (literalFreqs[i] > 0) {
            pq.push(new HuffmanNode(static_cast<uint8_t>(i), literalFreqs[i]));
        }
    }

    HuffmanNode* root = nullptr;
    if (pq.size() == 1) {
        HuffmanNode* single = pq.top(); pq.pop();
        root = new HuffmanNode(single->frequency, single, nullptr);
    } else if (pq.size() > 1) {
        while (pq.size() > 1) {
            HuffmanNode* l = pq.top(); pq.pop();
            HuffmanNode* r = pq.top(); pq.pop();
            HuffmanNode* parent = new HuffmanNode(l->frequency + r->frequency, l, r);
            parent->symbol = (l->symbol < r->symbol) ? l->symbol : r->symbol;
            pq.push(parent);
        }
        root = pq.top();
    }

    // 2. Decode Bitstream into Tokens
    std::vector<LZ77Token> tokens;
    tokens.reserve(tokenCount);

    BitReader bitReader(in);
    for (uint64_t i = 0; i < tokenCount; ++i) {
        bool isMatchBit;
        if (!bitReader.readBit(isMatchBit)) {
            throw std::runtime_error("Unexpected end of bitstream while reading Deflate tokens.");
        }

        if (!isMatchBit) {
            // Literal token: traverse Huffman tree
            if (distinctCount == 1) {
                // Single unique symbol edge case
                uint8_t sym = 0;
                for (int s = 0; s < 256; ++s) {
                    if (literalFreqs[s] > 0) { sym = static_cast<uint8_t>(s); break; }
                }
                bool dummy;
                bitReader.readBit(dummy); // consume bit
                tokens.push_back(LZ77Token::createLiteral(sym));
            } else {
                HuffmanNode* current = root;
                bool bit;
                while (current && !current->isLeaf() && bitReader.readBit(bit)) {
                    current = bit ? current->right : current->left;
                }
                if (!current || !current->isLeaf()) {
                    throw std::runtime_error("Error decoding literal token from Huffman bitstream.");
                }
                tokens.push_back(LZ77Token::createLiteral(current->symbol));
            }
        } else {
            // Match token: read 16-bit length and 16-bit distance
            uint32_t lengthVal = 0, distanceVal = 0;
            if (!bitReader.readBits(lengthVal, 16) || !bitReader.readBits(distanceVal, 16)) {
                throw std::runtime_error("Unexpected end of bitstream while reading match token.");
            }
            tokens.push_back(LZ77Token::createMatch(static_cast<uint16_t>(lengthVal), static_cast<uint16_t>(distanceVal)));
        }
    }

    // Clean up tree
    auto freeTree = [](auto self, HuffmanNode* node) -> void {
        if (!node) return;
        self(self, node->left);
        self(self, node->right);
        delete node;
    };
    freeTree(freeTree, root);

    return tokens;
}

// =============================================================================
// MEMBER 2 & 3 HOOKS: LZ77 Sliding Window Compression & Decompression
// =============================================================================

std::vector<LZ77Token> Deflate::lz77Compress(const std::vector<uint8_t>& input) {
    // Member 2 will enhance this with sliding window longest-match search
    // Current baseline converts bytes into literal tokens
    std::vector<LZ77Token> tokens;
    tokens.reserve(input.size());
    for (uint8_t byte : input) {
        tokens.push_back(LZ77Token::createLiteral(byte));
    }
    return tokens;
}

std::vector<uint8_t> Deflate::lz77Decompress(const std::vector<LZ77Token>& tokens) {
    // Member 3 will enhance this with (Distance, Length) dictionary buffer copying
    std::vector<uint8_t> output;
    for (const auto& tok : tokens) {
        if (!tok.isMatch) {
            output.push_back(tok.literal);
        } else {
            size_t startPos = output.size() - tok.distance;
            for (uint16_t i = 0; i < tok.length; ++i) {
                output.push_back(output[startPos + i]);
            }
        }
    }
    return output;
}

// =============================================================================
// HIGH-LEVEL DEFLATE PIPELINE
// =============================================================================

CompressionMetrics Deflate::compress(const std::string& inputPath, const std::string& outputPath) {
    CompressionMetrics metrics;
    metrics.algorithmName = "Deflate (LZ77 + Huffman)";

    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) throw std::runtime_error("Cannot open input file: " + inputPath);

    inFile.seekg(0, std::ios::end);
    uint64_t originalSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);
    metrics.originalSizeBytes = originalSize;

    std::vector<uint8_t> inputData(originalSize);
    if (originalSize > 0) {
        inFile.read(reinterpret_cast<char*>(inputData.data()), originalSize);
    }
    inFile.close();

    Timer timer;
    timer.start();

    // Stage 1: LZ77 tokenization (Member 2)
    std::vector<LZ77Token> tokens = lz77Compress(inputData);

    // Stage 2: Huffman Entropy Coding (Member 1)
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) throw std::runtime_error("Cannot open output file: " + outputPath);

    encodeTokensWithHuffman(tokens, originalSize, outFile);
    outFile.flush();
    outFile.seekp(0, std::ios::end);
    metrics.compressedSizeBytes = outFile.tellp();
    outFile.close();

    timer.stop();
    metrics.executionTimeMs = timer.elapsedMilliseconds();

    return metrics;
}

CompressionMetrics Deflate::decompress(const std::string& inputPath, const std::string& outputPath) {
    CompressionMetrics metrics;
    metrics.algorithmName = "Deflate (LZ77 + Huffman)";

    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) throw std::runtime_error("Cannot open input file: " + inputPath);

    inFile.seekg(0, std::ios::end);
    metrics.compressedSizeBytes = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    Timer timer;
    timer.start();

    // Stage 1: Huffman Token Decoding (Member 1)
    uint64_t originalSize = 0;
    std::vector<LZ77Token> tokens = decodeTokensWithHuffman(inFile, originalSize);
    inFile.close();

    // Stage 2: LZ77 Reconstruction (Member 3)
    std::vector<uint8_t> outputData = lz77Decompress(tokens);

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) throw std::runtime_error("Cannot open output file: " + outputPath);

    if (!outputData.empty()) {
        outFile.write(reinterpret_cast<const char*>(outputData.data()), outputData.size());
    }
    outFile.close();

    metrics.originalSizeBytes = outputData.size();
    timer.stop();
    metrics.executionTimeMs = timer.elapsedMilliseconds();

    return metrics;
}
