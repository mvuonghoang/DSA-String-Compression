#include "LZW.h"
#include "../common/Timer.h"
#include <fstream>
#include <stdexcept>
#include <iostream>

CompressionMetrics LZW::compress(const std::string& inputPath, const std::string& outputPath) {
    // Member 3 will implement full LZW with dynamic dictionary
    CompressionMetrics metrics;
    metrics.algorithmName = "LZW";
    metrics.executionTimeMs = 0.0;
    metrics.originalSizeBytes = 0;
    metrics.compressedSizeBytes = 0;

    std::cout << "[INFO] LZW Compression is reserved for Member 3.\n";
    return metrics;
}

CompressionMetrics LZW::decompress(const std::string& inputPath, const std::string& outputPath) {
    // Member 3 will implement full LZW decompression
    CompressionMetrics metrics;
    metrics.algorithmName = "LZW";
    metrics.executionTimeMs = 0.0;
    metrics.originalSizeBytes = 0;
    metrics.compressedSizeBytes = 0;

    std::cout << "[INFO] LZW Decompression is reserved for Member 3.\n";
    return metrics;
}
