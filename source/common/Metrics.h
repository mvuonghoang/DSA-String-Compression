#ifndef METRICS_H
#define METRICS_H

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>

struct CompressionMetrics {
    std::string algorithmName;
    double executionTimeMs;
    uint64_t originalSizeBytes;
    uint64_t compressedSizeBytes;

    void printCompressionSummary() const {
        std::cout << "Compression complete.\n";
        std::cout << "--------------------------------\n";
        std::cout << "Algorithm: " << algorithmName << "\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Execution Time: " << executionTimeMs << " ms\n";
        std::cout << "Original Size: " << originalSizeBytes << " bytes\n";
        std::cout << "Compressed Size: " << compressedSizeBytes << " bytes\n";

        if (compressedSizeBytes > 0) {
            double ratio = static_cast<double>(originalSizeBytes) / static_cast<double>(compressedSizeBytes);
            std::cout << std::setprecision(2) << "Compression Ratio: " << ratio << "\n";
        } else {
            std::cout << "Compression Ratio: N/A\n";
        }

        if (originalSizeBytes > 0) {
            double savings = (1.0 - static_cast<double>(compressedSizeBytes) / static_cast<double>(originalSizeBytes)) * 100.0;
            std::cout << std::setprecision(1) << "Space Savings: " << savings << "%\n";
        } else {
            std::cout << "Space Savings: 0.0%\n";
        }
    }

    void printDecompressionSummary() const {
        std::cout << "Decompression complete.\n";
        std::cout << "--------------------------------\n";
        std::cout << "Algorithm: " << algorithmName << "\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Execution Time: " << executionTimeMs << " ms\n";
        std::cout << "Compressed Size: " << compressedSizeBytes << " bytes\n";
        std::cout << "Decompressed Size: " << originalSizeBytes << " bytes\n";
    }
};

#endif // METRICS_H
