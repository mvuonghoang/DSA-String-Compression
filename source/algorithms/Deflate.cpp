#include "Deflate.h"
#include "../common/Timer.h"
#include <iostream>

CompressionMetrics Deflate::compress(const std::string& inputPath, const std::string& outputPath) {
    CompressionMetrics metrics;
    metrics.algorithmName = "Deflate";
    metrics.executionTimeMs = 0.0;
    metrics.originalSizeBytes = 0;
    metrics.compressedSizeBytes = 0;

    std::cout << "[INFO] Deflate Compression is planned as team bonus.\n";
    return metrics;
}

CompressionMetrics Deflate::decompress(const std::string& inputPath, const std::string& outputPath) {
    CompressionMetrics metrics;
    metrics.algorithmName = "Deflate";
    metrics.executionTimeMs = 0.0;
    metrics.originalSizeBytes = 0;
    metrics.compressedSizeBytes = 0;

    std::cout << "[INFO] Deflate Decompression is planned as team bonus.\n";
    return metrics;
}
