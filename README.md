
# Lossless String Compression - DSA Project 

> **Course**: Data Structures & Algorithms  
> **Institution**: VNUHCM - University of Science (HCMUS)  
> **Topic**: Group Project - Lossless String Compression (`RLE`, `Huffman`, `LZW`, and `Deflate`)

---

## 👥 Group Members & Task Allocation

| Member | Student ID | Algorithm Focus | Responsibilities |
| :--- | :--- | :--- | :--- |
| **Member 1 ** | *25127258* | **Huffman Coding** | CLI Framework, Timer & Metrics, BitStream I/O, Huffman Tree, Report Lead |
| **Member 2 ** | *24127587* | **Run-Length Encoding (RLE)** | RLE Implementation, Test Data Generator, Benchmark Experiments, Report Results |
| **Member 3 ** | *25127146* | **Lempel-Ziv-Welch (LZW)** | LZW Dynamic Dictionary, Bit-packing, Video Demo, Readme Guide |
| **Team ** | *All* | **Deflate** | Combination of LZ77 sliding window + Huffman coding |

---

## 🛠️ Build & Compilation

The project strictly adheres to **C++17** using standard libraries only.

```bash
# Compile all source files into the compressor executable
g++ source/*.cpp -o compressor -std=c++17 -O3
```

---

## 🚀 CLI Usage

```bash
compressor -a [algorithm] -m [mode] -i [input_file] -o [output_file]
```

### Options:
* `-a [algorithm]`:
  * `rle` - Run-Length Encoding
  * `huff` - Huffman Coding
  * `lzw` - Lempel-Ziv-Welch
  * `deflate` - Deflate (LZ77 + Huffman) *(Bonus)*
* `-m [mode]`:
  * `c` - Compression mode
  * `d` - Decompression mode
* `-i [input_file]`: Path to the input file
* `-o [output_file]`: Path to the output file

### Example:
```bash
# Compress using RLE
./compressor -a rle -m c -i tests/sample.txt -o tests/sample.rle

# Decompress using RLE
./compressor -a rle -m d -i tests/sample.rle -o tests/sample_decompressed.txt
```

---

## 📂 Project Structure

```text
├── .gitignore
├── README.md
├── source/              # C++ source and header files
│   ├── main.cpp         # Entry point & CLI parsing
│   ├── common/          # Common utilities (Timer, BitStream, Metrics)
│   └── algorithms/      # RLE, Huffman, LZW, Deflate implementations
├── tests/               # Test datasets and test runners
│   └── sample.txt
└── docs/                # Report drafting, LaTeX / markdown notes
```

---

## 🌿 Git Branching Workflow

To collaborate smoothly, each member works on their feature branch before creating a Pull Request (PR) to merge into `main`:

1. `main`: Stable code that compiles and passes all checks.
2. `feat/huffman`: Huffman implementation and BitStream utilities.
3. `feat/rle`: RLE algorithm & benchmark test generation.
4. `feat/lzw`: LZW dictionary-based compression.
5. `feat/deflate`: LZ77 + Huffman integration.
