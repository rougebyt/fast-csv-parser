
<h1 align="center">fast-csv-parser</h1>
<p align="center">
  <strong>Fast CSV parser in C — 3–5× faster than Python</strong><br>
  <i>Memory-mapped, zero-copy, lightweight</i>
</p>
<p align="center">
  <a href="#installation">Install</a> •
  <a href="#usage">Usage</a> •
  <a href="#benchmarks">Benchmarks</a> •
  <a href="#platform-support">Platform</a> •
  <a href="#contributing">Contribute</a>
</p>
<p align="center">
  <img src="https://img.shields.io/badge/language-C-DC143C?logo=c&logoColor=white" />
  <img src="https://img.shields.io/badge/platform-Linux%20%7C%20WSL2-blue" />
  <img src="https://img.shields.io/badge/WSL2-Recommended-4B8BBE" />
  <img src="https://github.com/rougebyt/fast-csv-parser/actions/workflows/ci.yml/badge.svg" />
  <img src="https://img.shields.io/github/stars/rougebyt/fast-csv-parser?style=social" />
</p>

---

## Why This Exists

Parsing large CSV files in Python is **slow** due to interpreter overhead.  
`fast-csv-parser` uses **memory mapping (`mmap`)** and **zero-copy parsing** to read millions of rows in **seconds**, not minutes.

- No per-field allocation  
- No Python overhead  
- CLI + embeddable library  
- Built for **Linux & WSL2**

---

## Features

| Feature                     | Status  |
|----------------------------|---------|
| Memory-mapped I/O          | Done |
| Zero-copy field extraction | Done |
| Custom delimiter & quote   | Done |
| CLI + library              | Done |
| Docker build               | Done |
| GitHub Actions CI          | Done |
| Unit tests                 | Done |

---

## Platform Support

| OS        | Status    | Notes |
|-----------|-----------|-------|
| **Linux** | Supported | Ubuntu 22.04+ |
| **WSL2**  | Supported | Recommended for Windows |
| **macOS** | Experimental | Not tested |
| **Windows**| Not Supported | Use **WSL2** |

> **Pro tip**: On Windows, use **WSL2** — full Linux environment, zero changes needed.

### Windows + WSL2 Setup

```bash
# 1. Install WSL2 (run in PowerShell as Admin)
wsl --install -d Ubuntu

# 2. Open WSL terminal
git clone https://github.com/rougebyt/fast-csv-parser.git
cd fast-csv-parser

# 3. Install dependencies
sudo apt update && sudo apt install build-essential python3 python3-pip -y

# 4. Build & run
make
./csvparse examples/sample.csv
```

---

## Installation

### Option 1: Direct (Linux / WSL2)

```bash
git clone https://github.com/rougebyt/fast-csv-parser.git
cd fast-csv-parser

# Install dependencies
sudo apt update
sudo apt install build-essential python3 python3-pip -y

# Build
make
```

> Binary: `./csvparse`

---

### Option 2: Docker

```bash
docker build -t fast-csv-parser .

# Run tests
docker run --rm fast-csv-parser make test

# Run CLI with local file
docker run --rm -v $(pwd)/examples:/data fast-csv-parser ./csvparse /data/sample.csv
```

---

## Usage

### CLI

```bash
# Basic
./csvparse examples/sample.csv

# Custom delimiter
./csvparse data.csv --delimiter ';'

# Custom quote
./csvparse data.csv --quote "'"

# Print only specific columns (0-indexed)
./csvparse data.csv --columns 0,2
```

### As Library

```c
#include "include/csv_parser.h"

int main() {
    CSVParser *parser = csv_parser_new("examples/sample.csv", ',', '"');
    CSVRow *row;
    while ((row = csv_parser_next(parser)) != NULL) {
        printf("Name: %s, Age: %s\n",
               row->fields[0], row->fields[1]);
    }
    csv_parser_free(parser);
    return 0;
}
```

---

## Benchmarks

```bash
$ make bench
```

**1M rows × 5 cols (~57 MB CSV)**

| Parser         | Time   | Speedup |
|----------------|--------|---------|
| Python `csv`   | 4.75s  | 1.0×    |
| **C Parser**   | **1.70s** | **2.8×** |

> Realistic speedup under fair I/O conditions  
> Zero-copy + `mmap` = still **massive memory savings**  
> See [`examples/benchmark.py`](./examples/benchmark.py)

---

## Project Structure

```
src/           → Core parser + CLI
include/       → Header for library use
examples/      → Sample data + benchmark
tests/         → Unit tests
Dockerfile     → Containerized build
.github/       → CI/CD
```

---

## Contributing

1. Fork it
2. Create your branch (`git checkout -b feature/fast`)
3. Commit (`git commit -m 'Add SIMD support'`)
4. Push (`git push origin feature/fast`)
5. Open a Pull Request

---

## Author

**Moibon Dereje**  
- GitHub: [@rougebyt](https://github.com/rougebyt)  
- X: [@rougeByt](https://x.com/rougeByt)  
- Email: moibonthebest@gmail.com

---

## License

MIT © Moibon Dereje

---

<p align="center">
  <img src="https://img.shields.io/github/forks/rougebyt/fast-csv-parser?style=social" />
  <br><br>
  <i>Built with performance in mind.</i>
</p>
