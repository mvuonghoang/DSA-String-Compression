#include "RLE.h"
#include "../common/Timer.h"
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cstdint>

CompressionMetrics RLE::compress(const std::string& inputPath, const std::string& outputPath) {
    CompressionMetrics metrics;
    metrics.algorithmName = "RLE (PackBits Flag Format)";

    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputPath);
    }

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

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputPath);
    }

    // Write RLE Header:
    // 1. Magic bytes: 'R', 'L', 'E', '\1'
    outFile.put('R'); outFile.put('L'); outFile.put('E'); outFile.put('\1');

    // 2. Original file size (8 bytes)
    outFile.write(reinterpret_cast<const char*>(&originalSize), sizeof(originalSize));

    // PackBits RLE Encoding Loop:
    // Control Byte:
    //   Bit 7 = 1 (0x80): Repeat run of length = (control & 0x7F) + 1 (1 to 128). Followed by 1 byte.
    //   Bit 7 = 0 (0x00): Literal run of length = (control & 0x7F) + 1 (1 to 128). Followed by length bytes.
    size_t pos = 0;
    while (pos < originalSize) {
        // Check for repeat run
        size_t runLen = 1;
        while (pos + runLen < originalSize && runLen < 128 && inputData[pos + runLen] == inputData[pos]) {
            runLen++;
        }

        if (runLen >= 3) {
            // Encode repeat run
            uint8_t control = static_cast<uint8_t>(0x80 | (runLen - 1));
            outFile.put(static_cast<char>(control));
            outFile.put(static_cast<char>(inputData[pos]));
            pos += runLen;
        } else {
            // Encode literal run
            size_t litLen = 0;
            while (pos + litLen < originalSize && litLen < 128) {
                // Peek ahead: stop literal run if a 3+ repeat run starts
                size_t aheadRun = 1;
                while (pos + litLen + aheadRun < originalSize && aheadRun < 128 &&
                       inputData[pos + litLen + aheadRun] == inputData[pos + litLen]) {
                    aheadRun++;
                }
                if (aheadRun >= 3) {
                    break;
                }
                litLen++;
            }

            if (litLen > 0) {
                uint8_t control = static_cast<uint8_t>(litLen - 1);
                outFile.put(static_cast<char>(control));
                outFile.write(reinterpret_cast<const char*>(&inputData[pos]), litLen);
                pos += litLen;
            }
        }
    }

    outFile.flush();
    outFile.seekp(0, std::ios::end);
    metrics.compressedSizeBytes = outFile.tellp();

    timer.stop();
    metrics.executionTimeMs = timer.elapsedMilliseconds();
    outFile.close();

    return metrics;
}

CompressionMetrics RLE::decompress(const std::string& inputPath, const std::string& outputPath) {
    CompressionMetrics metrics;
    metrics.algorithmName = "RLE (PackBits Flag Format)";

    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputPath);
    }

    inFile.seekg(0, std::ios::end);
    metrics.compressedSizeBytes = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    Timer timer;
    timer.start();

    // Read Header
    char magic[4];
    if (!inFile.read(magic, 4) || magic[0] != 'R' || magic[1] != 'L' || magic[2] != 'E' || magic[3] != '\1') {
        throw std::runtime_error("Invalid or corrupted RLE compressed file format.");
    }

    uint64_t originalSize = 0;
    inFile.read(reinterpret_cast<char*>(&originalSize), sizeof(originalSize));
    metrics.originalSizeBytes = originalSize;

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputPath);
    }

    uint64_t decodedBytes = 0;
    char controlChar;
    while (decodedBytes < originalSize && inFile.get(controlChar)) {
        uint8_t control = static_cast<uint8_t>(controlChar);
        bool isRepeat = (control & 0x80) != 0;
        size_t count = (control & 0x7F) + 1;

        if (isRepeat) {
            char byteVal;
            if (!inFile.get(byteVal)) {
                throw std::runtime_error("Unexpected end of file while reading RLE repeat byte.");
            }
            std::vector<char> repeatBuf(count, byteVal);
            outFile.write(repeatBuf.data(), count);
            decodedBytes += count;
        } else {
            std::vector<char> litBuf(count);
            if (!inFile.read(litBuf.data(), count)) {
                throw std::runtime_error("Unexpected end of file while reading RLE literal bytes.");
            }
            outFile.write(litBuf.data(), count);
            decodedBytes += count;
        }
    }

    if (decodedBytes != originalSize) {
        throw std::runtime_error("RLE Decompression error: decoded bytes do not match original size header.");
    }

    outFile.flush();
    timer.stop();
    metrics.executionTimeMs = timer.elapsedMilliseconds();

    inFile.close();
    outFile.close();

    return metrics;
}

