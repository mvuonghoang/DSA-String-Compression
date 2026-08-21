#include "LZW.h"
#include "../common/Timer.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cstdint>

CompressionMetrics LZW::compress(const std::string& inputPath, const std::string& outputPath) {
    CompressionMetrics metrics;
    metrics.algorithmName = "LZW";

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

    std::string text((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    std::vector<uint16_t> resultCodes;
    if (!text.empty()) {
        std::unordered_map<std::string, uint16_t> dictionary;
        for (int i = 0; i < 256; i++) {
            dictionary[std::string(1, (char)i)] = static_cast<uint16_t>(i);
        }

        std::string w = "";
        uint16_t dictSize = 256;

        for (char c : text) {
            std::string wc = w + c;
            if (dictionary.count(wc)) {
                w = wc;
            } else {
                resultCodes.push_back(dictionary[w]);
                if (dictSize < 65535) { 
                    dictionary[wc] = dictSize++;
                }
                w = std::string(1, c);
            }
        }

        if (!w.empty()) {
            resultCodes.push_back(dictionary[w]);
        }
    }

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputPath);
    }

    size_t codeCount = resultCodes.size();
    outFile.write(reinterpret_cast<const char*>(&codeCount), sizeof(codeCount));
    if (codeCount > 0) {
        outFile.write(reinterpret_cast<const char*>(resultCodes.data()), codeCount * sizeof(uint16_t));
    }

    outFile.flush();
    outFile.seekp(0, std::ios::end);
    uint64_t compressedSize = outFile.tellp();
    metrics.compressedSizeBytes = compressedSize;

    timer.stop();
    metrics.executionTimeMs = timer.elapsedMilliseconds();
    outFile.close();

    return metrics; 
}

CompressionMetrics LZW::decompress(const std::string& inputPath, const std::string& outputPath) {
    CompressionMetrics metrics;
    metrics.algorithmName = "LZW";

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

    size_t codeCount = 0;
    inFile.read(reinterpret_cast<char*>(&codeCount), sizeof(codeCount));

    std::vector<uint16_t> compressedCodes(codeCount);
    if (codeCount > 0) {
        inFile.read(reinterpret_cast<char*>(compressedCodes.data()), codeCount * sizeof(uint16_t));
    }
    inFile.close();

    std::string result = "";
    if (!compressedCodes.empty()) {
        std::vector<std::string> dictionary(256);
        for (int i = 0; i < 256; i++) {
            dictionary[i] = std::string(1, (char)i);
        }

        uint16_t dictSize = 256;
        uint16_t oldCode = compressedCodes[0];
        std::string s = dictionary[oldCode];
        result = s;
        std::string c = std::string(1, s[0]);

        for (size_t i = 1; i < compressedCodes.size(); i++) {
            uint16_t newCode = compressedCodes[i];
            std::string entry = "";
            if (newCode < dictSize) {
                entry = dictionary[newCode];
            } else if (newCode == dictSize) {
                entry = s + c;
            } else {
                throw std::runtime_error("Invalid LZW compressed code during decompression.");
            }

            result += entry;
            c = std::string(1, entry[0]);
            if (dictSize < 65535) {
                dictionary.push_back(s + c);
                dictSize++;
            }
            s = entry;
        }
    }

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Cannot open output file: " + outputPath);
    }

    outFile.write(result.data(), result.size());
    outFile.flush();

    outFile.seekp(0, std::ios::end);
    uint64_t originalSize = outFile.tellp();
    metrics.originalSizeBytes = originalSize;

    timer.stop();
    metrics.executionTimeMs = timer.elapsedMilliseconds();
    outFile.close();

    return metrics;
}
