#!/usr/bin/env python3
"""
Benchmark: Python csv.reader vs fast-csv-parser (C)
Author: Moibon Dereje
"""

import csv
import time
import os
import sys
from pathlib import Path

# === CONFIG ===
CSV_FILE = "large_benchmark.csv"  # Must exist in examples/
# Generate a large CSV if not exists
ROWS = 1_000_000  # 1 million rows
COLS = 5
# ==============

def generate_large_csv(path: Path, rows: int, cols: int):
    print(f"Generating {rows:,} rows x {cols} cols CSV... ", end="", flush=True)
    with open(path, "w", newline="") as f:
        writer = csv.writer(f)
        headers = [f"col{i}" for i in range(cols)]
        writer.writerow(headers)
        for i in range(rows):
            row = [f"value{i}_{j}" if j % 2 == 0 else i * j for j in range(cols)]
            writer.writerow(row)
    print("Done.")

def benchmark_python_csv(filepath: str):
    print("Benchmarking Python csv.reader...")
    start = time.perf_counter()
    with open(filepath, newline="") as f:
        reader = csv.reader(f)
        rows = sum(1 for _ in reader)  # Just count rows
    end = time.perf_counter()
    print(f"Python: {rows:,} rows in {(end - start):.3f}s")
    return end - start

def benchmark_c_parser(filepath: str):
    print("Benchmarking fast-csv-parser (C)...")
    binary = "../csvparse"
    if not os.path.exists(binary):
        print(f"Error: {binary} not found. Run 'make' first.")
        sys.exit(1)
    start = time.perf_counter()
    # Run without output
    result = os.system(f"{binary} {filepath} > /dev/null 2>&1")
    if result != 0:
        print("C parser failed.")
        sys.exit(1)
    end = time.perf_counter()
    print(f"C: parsed in {(end - start):.3f}s")
    return end - start
def main():
    csv_path = Path(__file__).parent / CSV_FILE
    if not csv_path.exists() or csv_path.stat().st_size < 10_000:
        generate_large_csv(csv_path, ROWS, COLS)

    print(f"Using CSV: {csv_path} ({csv_path.stat().st_size / 1e6:.1f} MB)\n")

    t_py = benchmark_python_csv(csv_path)
    t_c = benchmark_c_parser(csv_path)

    speedup = t_py / t_c if t_c > 0 else float('inf')
    print(f"\nSpeedup: {speedup:.1f}x faster")

if __name__ == "__main__":
    main()