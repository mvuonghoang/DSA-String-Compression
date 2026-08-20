#!/usr/bin/env python3
"""
Benchmark Runner & Automated Cross-Testing Script
Executes compressor across Scenario 1 & 2 test cases and records metrics into CSV.
"""

import os
import sys
import subprocess
import filecmp
import csv
import time

def compile_project(root_dir):
    """Compiles the C++ compressor project."""
    executable = os.path.join(root_dir, "compressor.exe" if os.name == 'nt' else "compressor")
    cmd = [
        "g++",
        os.path.join(root_dir, "source", "main.cpp"),
        os.path.join(root_dir, "source", "algorithms", "Deflate.cpp"),
        os.path.join(root_dir, "source", "algorithms", "Huffman.cpp"),
        os.path.join(root_dir, "source", "algorithms", "LZW.cpp"),
        os.path.join(root_dir, "source", "algorithms", "RLE.cpp"),
        "-o", executable,
        "-std=c++17",
        "-O3"
    ]
    print(f"[BUILD] Compiling C++ compressor executable...")
    result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if result.returncode != 0:
        print(f"[ERROR] Compilation failed:\n{result.stderr}")
        sys.exit(1)
    print(f"[BUILD] Compilation successful -> {executable}\n")
    return executable

def parse_cli_output(output):
    """Parses execution time (ms) from CLI output."""
    time_ms = 0.0
    for line in output.splitlines():
        if "Execution Time:" in line:
            parts = line.split(":")
            if len(parts) >= 2:
                time_str = parts[1].strip().replace("ms", "").strip()
                try:
                    time_ms = float(time_str)
                except ValueError:
                    pass
    return time_ms

def run_test(executable, algo, input_file, temp_dir):
    """Runs compression, decompression, and verifies binary exactness."""
    base_name = os.path.basename(input_file)
    compressed_file = os.path.join(temp_dir, f"{base_name}.{algo}")
    decompressed_file = os.path.join(temp_dir, f"{base_name}_{algo}_dec.txt")

    original_size = os.path.getsize(input_file)

    # 1. Compress
    cmd_c = [executable, "-a", algo, "-m", "c", "-i", input_file, "-o", compressed_file]
    res_c = subprocess.run(cmd_c, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if res_c.returncode != 0:
        return None

    comp_time = parse_cli_output(res_c.stdout)
    compressed_size = os.path.getsize(compressed_file) if os.path.exists(compressed_file) else 0

    # 2. Decompress
    cmd_d = [executable, "-a", algo, "-m", "d", "-i", compressed_file, "-o", decompressed_file]
    res_d = subprocess.run(cmd_d, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    if res_d.returncode != 0:
        return None

    decomp_time = parse_cli_output(res_d.stdout)

    # 3. Cross-testing verification
    is_exact = filecmp.cmp(input_file, decompressed_file, shallow=False) if os.path.exists(decompressed_file) else False

    # Cleanup temp files
    if os.path.exists(compressed_file): os.remove(compressed_file)
    if os.path.exists(decompressed_file): os.remove(decompressed_file)

    # Metrics
    ratio = original_size / compressed_size if compressed_size > 0 else 0.0
    space_savings = (1.0 - (compressed_size / original_size)) * 100.0 if original_size > 0 else 0.0

    return {
        'algorithm': algo,
        'original_size': original_size,
        'compressed_size': compressed_size,
        'compress_time_ms': comp_time,
        'decompress_time_ms': decomp_time,
        'ratio': ratio,
        'space_savings_percent': space_savings,
        'exact_match': is_exact
    }

def main():
    root_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    tests_dir = os.path.join(root_dir, "tests")
    temp_dir = os.path.join(tests_dir, "temp_bench")
    os.makedirs(temp_dir, exist_ok=True)

    executable = compile_project(root_dir)

    test_files = [
        # Scenario 1
        'scenario1_10k.txt', 'scenario1_50k.txt', 'scenario1_100k.txt',
        'scenario1_500k.txt', 'scenario1_1m.txt', 'scenario1_5m.txt', 'scenario1_10m.txt',
        # Scenario 2
        'scenario2_repetitive_1m.txt', 'scenario2_english_1m.txt', 'scenario2_random_1m.txt'
    ]

    algorithms = ['huff', 'rle', 'deflate']

    csv_file = os.path.join(tests_dir, "benchmark_results.csv")
    results = []

    print("=== Running Algorithms Benchmarks & Cross-Testing ===")
    print(f"{'Testcase':<28} | {'Algo':<7} | {'Orig Size':<10} | {'Comp Size':<10} | {'Comp (ms)':<9} | {'Decomp(ms)':<10} | {'Ratio':<6} | {'Savings':<8} | {'Match'}")
    print("-" * 115)

    for tf in test_files:
        filepath = os.path.join(tests_dir, tf)
        if not os.path.exists(filepath):
            print(f"[WARN] Test file {tf} not found. Skipping.")
            continue

        for algo in algorithms:
            res = run_test(executable, algo, filepath, temp_dir)
            if res:
                res['testcase'] = tf
                results.append(res)
                match_str = "PASS (100%)" if res['exact_match'] else "FAIL"
                print(f"{tf:<28} | {algo:<7} | {res['original_size']:<10,} | {res['compressed_size']:<10,} | {res['compress_time_ms']:<9.2f} | {res['decompress_time_ms']:<10.2f} | {res['ratio']:<6.2f} | {res['space_savings_percent']:<7.1f}% | {match_str}")

    # Write CSV
    with open(csv_file, 'w', newline='', encoding='utf-8') as f:
        writer = csv.DictWriter(f, fieldnames=[
            'testcase', 'algorithm', 'original_size', 'compressed_size',
            'compress_time_ms', 'decompress_time_ms', 'ratio', 'space_savings_percent', 'exact_match'
        ])
        writer.writeheader()
        writer.writerows(results)

    # Clean temp dir
    if os.path.exists(temp_dir):
        os.rmdir(temp_dir)

    print("-" * 115)
    print(f"\n[SUCCESS] Benchmark completed! Results saved to: {csv_file}")

if __name__ == '__main__':
    main()
