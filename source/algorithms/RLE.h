#ifndef RLE_H
#define RLE_H

#include <string>
#include "../common/Metrics.h"

class RLE {
public:
    /**
     * @brief Compresses an input file using Run-Length Encoding (RLE).
     * To be implemented by Member 2.
     */
    static CompressionMetrics compress(const std::string& inputPath, const std::string& outputPath);

    /**
     * @brief Decompresses an RLE compressed file.
     * To be implemented by Member 2.
     */
    static CompressionMetrics decompress(const std::string& inputPath, const std::string& outputPath);
};

#endif // RLE_H
