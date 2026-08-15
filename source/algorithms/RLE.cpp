#include "RLE.h"
#include "../common/Timer.h"
#include <fstream>
#include <stdexcept>
#include <iostream>

CompressionMetrics RLE::compress(const std::string& inputPath, const std::string& outputPath) {
    CompressionMetrics metrics;
    metrics.algorithmName = "RLE";

    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputPath);
    }

    inFile.seekg(0, std::ios::end);
    uint64_t originalSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);
    metrics.originalSizeBytes = originalSize;

    Timer timer;
    timer.start();

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputPath);
    }

    // Basic standard Byte-Pair RLE implementation: [count (uint8_t)][byte]
    // Counts runs of identical bytes up to 255
    if (originalSize > 0) {
        char currentByte;
        inFile.get(currentByte);
        uint8_t count = 1;
        char nextByte;

        while (inFile.get(nextByte)) {
            if (nextByte == currentByte && count < 255) {
                count++;
            } else {
                outFile.put(static_cast<char>(count));
                outFile.put(currentByte);
                currentByte = nextByte;
                count = 1;
            }
        }
        outFile.put(static_cast<char>(count));
        outFile.put(currentByte);
    }

    outFile.flush();
    outFile.seekp(0, std::ios::end);
    metrics.compressedSizeBytes = outFile.tellp();

    timer.stop();
    metrics.executionTimeMs = timer.elapsedMilliseconds();

    inFile.close();
    outFile.close();

    return metrics;
}

CompressionMetrics RLE::decompress(const std::string& inputPath, const std::string& outputPath) {
    CompressionMetrics metrics;
    metrics.algorithmName = "RLE";

    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Cannot open input file: " + inputPath);
    }

    inFile.seekg(0, std::ios::end);
    metrics.compressedSizeBytes = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    Timer timer;
    timer.start();

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputPath);
    }

    uint64_t decompressedSize = 0;
    char countChar, byteChar;
    while (inFile.get(countChar) && inFile.get(byteChar)) {
        uint8_t count = static_cast<uint8_t>(countChar);
        for (uint8_t i = 0; i < count; ++i) {
            outFile.put(byteChar);
            decompressedSize++;
        }
    }

    outFile.flush();
    metrics.originalSizeBytes = decompressedSize;

    timer.stop();
    metrics.executionTimeMs = timer.elapsedMilliseconds();

    inFile.close();
    outFile.close();

    return metrics;
}
