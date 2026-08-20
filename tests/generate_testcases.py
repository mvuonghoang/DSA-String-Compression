#!/usr/bin/env python3
"""
Testcase Data Generator for Lossless String Compression Project
Generates test files for Scenario 1 and Scenario 2.
"""

import os
import random

# Sample English text base corpus
ENGLISH_BASE = (
    "Data structure and algorithm analysis is a fundamental discipline in computer science. "
    "Lossless data compression algorithms reduce the byte size of graphics, text documents, "
    "and binary executables without losing any original information. "
    "Run-Length Encoding (RLE) compresses consecutive identical bytes. "
    "Huffman coding constructs optimal variable-length prefix codes based on character frequencies. "
    "Lempel-Ziv-Welch (LZW) builds a dynamic dictionary of recurring sub-strings during a single pass. "
    "Deflate combines LZ77 sliding window dictionary matching with Huffman entropy coding to achieve high compression efficiency. "
    "In algorithm engineering, empirical benchmark measurements analyze execution time, compression ratio, and space savings. "
)

REPETITIVE_PATTERN = (
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"
    "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
    "DATASTRUCTUREDATASTRUCTUREDATASTRUCTURE"
    "COMPRESSIONCOMPRESSIONCOMPRESSION"
    "\n\n\n\n\n\n\n\n\n\n"
)

def generate_english_file(filepath, target_bytes):
    """Generates an English text file of exact target_bytes length."""
    print(f"[GEN] Generating English file: {filepath} ({target_bytes:,} bytes)...")
    current_bytes = 0
    base_encoded = ENGLISH_BASE.encode('utf-8')
    base_len = len(base_encoded)

    with open(filepath, 'wb') as f:
        while current_bytes < target_bytes:
            chunk_size = min(base_len, target_bytes - current_bytes)
            f.write(base_encoded[:chunk_size])
            current_bytes += chunk_size

def generate_repetitive_file(filepath, target_bytes):
    """Generates a highly repetitive file of exact target_bytes length."""
    print(f"[GEN] Generating Repetitive file: {filepath} ({target_bytes:,} bytes)...")
    current_bytes = 0
    pattern_encoded = REPETITIVE_PATTERN.encode('utf-8')
    pattern_len = len(pattern_encoded)

    with open(filepath, 'wb') as f:
        while current_bytes < target_bytes:
            chunk_size = min(pattern_len, target_bytes - current_bytes)
            f.write(pattern_encoded[:chunk_size])
            current_bytes += chunk_size

def generate_random_file(filepath, target_bytes):
    """Generates a high-entropy uniform random binary file of target_bytes length."""
    print(f"[GEN] Generating Random Binary file: {filepath} ({target_bytes:,} bytes)...")
    chunk_size = 64 * 1024
    current_bytes = 0

    with open(filepath, 'wb') as f:
        while current_bytes < target_bytes:
            to_write = min(chunk_size, target_bytes - current_bytes)
            random_bytes = bytes(random.getrandbits(8) for _ in range(to_write))
            f.write(random_bytes)
            current_bytes += to_write

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Scenario 1: English text files of varying sizes
    scenario1_files = {
        'scenario1_10k.txt': 10 * 1024,
        'scenario1_50k.txt': 50 * 1024,
        'scenario1_100k.txt': 100 * 1024,
        'scenario1_500k.txt': 500 * 1024,
        'scenario1_1m.txt': 1 * 1024 * 1024,
        'scenario1_5m.txt': 5 * 1024 * 1024,
        'scenario1_10m.txt': 10 * 1024 * 1024,
    }

    print("=== Generating Scenario 1 Test Cases ===")
    for filename, size in scenario1_files.items():
        filepath = os.path.join(script_dir, filename)
        generate_english_file(filepath, size)

    # Scenario 2: 3 Files of 1MB with different characteristics
    print("\n=== Generating Scenario 2 Test Cases (1MB Each) ===")
    size_1mb = 1 * 1024 * 1024
    
    generate_repetitive_file(os.path.join(script_dir, 'scenario2_repetitive_1m.txt'), size_1mb)
    generate_english_file(os.path.join(script_dir, 'scenario2_english_1m.txt'), size_1mb)
    generate_random_file(os.path.join(script_dir, 'scenario2_random_1m.txt'), size_1mb)

    print("\n[SUCCESS] All test cases generated successfully in tests/ directory.")

if __name__ == '__main__':
    main()
