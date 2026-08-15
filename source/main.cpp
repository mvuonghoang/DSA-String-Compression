#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "common/Metrics.h"
#include "algorithms/Huffman.h"
#include "algorithms/RLE.h"
#include "algorithms/LZW.h"
#include "algorithms/Deflate.h"

void printUsage() {
    std::cout << "Usage: compressor -a [algorithm] -m [mode] -i [input_file] -o [output_file]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -a [algorithm]  Select algorithm: rle, huff, lzw, deflate\n";
    std::cout << "  -m [mode]       Select mode: c (compress), d (decompress)\n";
    std::cout << "  -i [input_file] Path to the source file\n";
    std::cout << "  -o [output_file] Path to the resulting file\n";
}

int main(int argc, char* argv[]) {
    std::string algorithm = "";
    std::string mode = "";
    std::string inputFile = "";
    std::string outputFile = "";

    // Parse CLI arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-a" && i + 1 < argc) {
            algorithm = argv[++i];
        } else if (arg == "-m" && i + 1 < argc) {
            mode = argv[++i];
        } else if (arg == "-i" && i + 1 < argc) {
            inputFile = argv[++i];
        } else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "-h" || arg == "--help") {
            printUsage();
            return 0;
        } else {
            std::cerr << "Error: Unknown or incomplete option '" << arg << "'\n\n";
            printUsage();
            return 1;
        }
    }

    // Validate required arguments
    if (algorithm.empty() || mode.empty() || inputFile.empty() || outputFile.empty()) {
        std::cerr << "Error: Missing required arguments.\n\n";
        printUsage();
        return 1;
    }

    if (mode != "c" && mode != "d") {
        std::cerr << "Error: Invalid mode '" << mode << "'. Must be 'c' (compress) or 'd' (decompress).\n\n";
        printUsage();
        return 1;
    }

    try {
        CompressionMetrics metrics;

        if (algorithm == "huff") {
            if (mode == "c") {
                metrics = Huffman::compress(inputFile, outputFile);
                metrics.printCompressionSummary();
            } else {
                metrics = Huffman::decompress(inputFile, outputFile);
                metrics.printDecompressionSummary();
            }
        } else if (algorithm == "rle") {
            if (mode == "c") {
                metrics = RLE::compress(inputFile, outputFile);
                metrics.printCompressionSummary();
            } else {
                metrics = RLE::decompress(inputFile, outputFile);
                metrics.printDecompressionSummary();
            }
        } else if (algorithm == "lzw") {
            if (mode == "c") {
                metrics = LZW::compress(inputFile, outputFile);
                metrics.printCompressionSummary();
            } else {
                metrics = LZW::decompress(inputFile, outputFile);
                metrics.printDecompressionSummary();
            }
        } else if (algorithm == "deflate") {
            if (mode == "c") {
                metrics = Deflate::compress(inputFile, outputFile);
                metrics.printCompressionSummary();
            } else {
                metrics = Deflate::decompress(inputFile, outputFile);
                metrics.printDecompressionSummary();
            }
        } else {
            std::cerr << "Error: Unknown algorithm '" << algorithm << "'. Valid options: rle, huff, lzw, deflate.\n\n";
            printUsage();
            return 1;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Execution Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
