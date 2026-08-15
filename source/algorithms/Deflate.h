#ifndef DEFLATE_H
#define DEFLATE_H

#include <string>
#include "../common/Metrics.h"

class Deflate {
public:
    /**
     * @brief Compresses an input file using Deflate (LZ77 + Huffman Coding).
     * Bonus Algorithm (+1 Point).
     */
    static CompressionMetrics compress(const std::string& inputPath, const std::string& outputPath);

    /**
     * @brief Decompresses a Deflate compressed file.
     * Bonus Algorithm (+1 Point).
     */
    static CompressionMetrics decompress(const std::string& inputPath, const std::string& outputPath);
};

#endif // DEFLATE_H
