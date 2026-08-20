DATA STRUCTURE & ALGORITHM PROJECT: STRING COMPRESSION - Group 6

Project structure

source file: contains all of source code files
test file: contains all of test files
readme.txt: contains some guides about this project

2. Compilation

-This program is written in standard C++ requires a compiler supporting the C++17 standard

-To compile and run the source code, please open cmd (command) in your computer

3. Command line usage

-a [algorithm] : Select algorithm to use -> rle, huff, or lzw
-m [mode]      : Select mode -> c (compress) or d (decompress)
-i [inputfile]: Path to the source input file
-o [outputfile]: Path to the resulting output file

Examples:

+To compress a text file using LZW:
compressor.exe -a lzw -m c -i tests/input.txt -o tests/compressed.lzw

+To decompress the file back to the test
compressor.exe -a lzw -m d -i tests/compressed.lzw -o tests/output.txt
