#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cstdint>
#include "../common/Metrics.h"

/**
 * @brief Node representation in the Huffman Tree.
 */
struct HuffmanNode {
    uint8_t symbol;
    uint64_t frequency;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(uint8_t sym, uint64_t freq)
        : symbol(sym), frequency(freq), left(nullptr), right(nullptr) {}

    HuffmanNode(uint64_t freq, HuffmanNode* l, HuffmanNode* r)
        : symbol(0), frequency(freq), left(l), right(r) {}

    bool isLeaf() const {
        return left == nullptr && right == nullptr;
    }
};

/**
 * @brief Comparator for Priority Queue (Min-Heap) to ensure deterministic tree building.
 */
struct HuffmanNodeComparator {
    bool operator()(const HuffmanNode* a, const HuffmanNode* b) const {
        if (a->frequency != b->frequency) {
            return a->frequency > b->frequency; // Smallest frequency has highest priority
        }
        // Deterministic tie-breaking by symbol or pointer address
        return a->symbol > b->symbol;
    }
};

class Huffman {
private:
    static void generateCodes(const HuffmanNode* node, const std::string& currentCode, std::vector<std::string>& codeTable);
    static void freeTree(HuffmanNode* node);
    static HuffmanNode* buildTree(const std::vector<uint64_t>& frequencies);

public:
    /**
     * @brief Compresses an input file using Huffman Coding.
     * @return Metrics struct with execution time, sizes, and ratio.
     */
    static CompressionMetrics compress(const std::string& inputPath, const std::string& outputPath);

    /**
     * @brief Decompresses a Huffman-compressed file back to its original form.
     * @return Metrics struct with execution time and sizes.
     */
    static CompressionMetrics decompress(const std::string& inputPath, const std::string& outputPath);
};

#endif // HUFFMAN_H
