================================================================================
LOSSLESS STRING COMPRESSION - PROJECT USER GUIDE & DOCUMENTATION
================================================================================
Course:      CSC10004 - Data Structures & Algorithms (Summer 2026)
Institution: Faculty of Information Technology, VNUHCM - University of Science
Class:       25CA
Group:       Group 6

Group Members:
  - Member 1 (Leader): 25127258
  - Member 2:          24127587
  - Member 3:          25127146

================================================================================
1. SYSTEM REQUIREMENTS & COMPILER COMPLIANCE
================================================================================
- Language:             C++ (strictly adheres to C++17 standard)
- Compiler:             GNU g++ with standard libraries only (no external dependencies)
- Supported Platforms:  Windows, Linux, macOS

================================================================================
2. COMPILATION INSTRUCTIONS
================================================================================
To build the "compressor" executable from source, open a terminal in the root
directory and run the following command:

[Windows (PowerShell / Command Prompt)]
> g++ source/main.cpp source/algorithms/*.cpp -o compressor.exe -std=c++17 -O3

[Linux / macOS]
$ g++ source/main.cpp source/algorithms/*.cpp -o compressor -std=c++17 -O3

================================================================================
3. COMMAND-LINE INTERFACE (CLI) USAGE
================================================================================
Syntax:
  compressor -a [algorithm] -m [mode] -i [input_file] -o [output_file]

Options:
  -a [algorithm]   Algorithm selection:
                     - 'huff'    : Huffman Coding
                     - 'rle'     : Run-Length Encoding (PackBits format)
                     - 'lzw'     : Lempel-Ziv-Welch
                     - 'deflate' : DEFLATE (LZ77 + Huffman) [Bonus Algorithm]
  -m [mode]        Execution mode:
                     - 'c'       : Compression mode
                     - 'd'       : Decompression mode
  -i [input_file]  Path to the source file
  -o [output_file] Path to the resulting output file

================================================================================
4. USAGE EXAMPLES
================================================================================
A. Huffman Coding:
  - Compress:   ./compressor.exe -a huff -m c -i tests/sample.txt -o tests/sample.huff
  - Decompress: ./compressor.exe -a huff -m d -i tests/sample.huff -o tests/sample_huff_decomp.txt

B. Run-Length Encoding (RLE):
  - Compress:   ./compressor.exe -a rle -m c -i tests/sample.txt -o tests/sample.rle
  - Decompress: ./compressor.exe -a rle -m d -i tests/sample.rle -o tests/sample_rle_decomp.txt

C. Lempel-Ziv-Welch (LZW):
  - Compress:   ./compressor.exe -a lzw -m c -i tests/sample.txt -o tests/sample.lzw
  - Decompress: ./compressor.exe -a lzw -m d -i tests/sample.lzw -o tests/sample_lzw_decomp.txt

D. DEFLATE (LZ77 + Huffman Bonus):
  - Compress:   ./compressor.exe -a deflate -m c -i tests/sample.txt -o tests/sample.deflate
  - Decompress: ./compressor.exe -a deflate -m d -i tests/sample.deflate -o tests/sample_deflate_decomp.txt

================================================================================
5. LOSSLESS INTEGRITY VERIFICATION
================================================================================
To verify that the decompression output matches the original input byte-for-byte:

[Windows]
> fc.exe /b tests\sample.txt tests\sample_huff_decomp.txt
(Expected output: "FC: no differences encountered")

[Linux / macOS]
$ diff tests/sample.txt tests/sample_huff_decomp.txt
(Expected output: empty output / zero exit code indicates identical files)

================================================================================
6. DIRECTORY STRUCTURE
================================================================================
.
├── readme.txt                  # User and installation manual (this file)
├── README.md                   # Markdown overview for GitHub
├── video.txt                   # Video demonstration submission link & timestamps
├── docs/                       # Project report and documentation
│   ├── Report.tex              # Formal LaTeX source code for Report.pdf
│   ├── Report.md               # Markdown copy of the report
│   ├── hcmus_logo.png          # Official university emblem
│   └── chart[1-3]_*.png        # Benchmark experimental charts
├── source/                     # C++17 source code
│   ├── main.cpp                # CLI entry point and argument parsing
│   ├── common/                 # Core framework utilities
│   │   ├── BitStream.h         # BitWriter and BitReader bit-level I/O
│   │   ├── Metrics.h           # Console performance summary formatter
│   │   └── Timer.h             # High-precision execution timer (ms)
│   └── algorithms/             # Compression algorithm implementations
│       ├── Huffman.h / .cpp    # Huffman Coding
│       ├── RLE.h / .cpp        # Run-Length Encoding (PackBits)
│       ├── LZW.h / .cpp        # Lempel-Ziv-Welch
│       └── Deflate.h / .cpp    # DEFLATE (LZ77 + Huffman)
└── tests/                      # Test suites and benchmarking datasets
    ├── sample.txt              # Standard sample text
    ├── test_english.txt        # English text dataset
    ├── scenario1_*.txt         # File size scaling datasets (10KB to 10MB)
    ├── scenario2_*.txt         # Data entropy datasets (Repetitive, English, Random)
    ├── generate_testcases.py   # Dataset generator script
    ├── run_benchmarks.py       # Automated benchmarking script
    └── benchmark_results.csv   # Raw benchmark measurement records
================================================================================
