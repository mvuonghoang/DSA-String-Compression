#ifndef LZW_H
#define LZW_H

#include <string>
#include "../common/Metrics.h"

class LZW {
public:
    /**
     * @brief Compresses an input file using Lempel-Ziv-Welch (LZW).
     * To be implemented by Member 3.
     */
    static CompressionMetrics compress(const std::string& inputPath, const std::string& outputPath);

    /**
     * @brief Decompresses an LZW compressed file.
     * To be implemented by Member 3.
     */
    static CompressionMetrics decompress(const std::string& inputPath, const std::string& outputPath);
};

#endif // LZW_H
