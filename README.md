
<h1 align="center">fast-csv-parser</h1>

<p align="center">
  <strong>Blazing-fast CSV parser in C — 10× faster than Python</strong><br>
  <i>Memory-mapped, zero-copy, SIMD-ready</i>
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
  <img src="https://img.shields.io/badge/platform-Linux%20%7C%20macOS-blue" />
  <img src="https://img.shields.io/badge/WSL2-Recommended-4B8BBE" />
  <img src="https://github.com/rougebyt/fast-csv-parser/actions/workflows/ci.yml/badge.svg" />
  <img src="https://img.shields.io/github/stars/rougebyt/fast-csv-parser?style=social" />
</p>

---

## Why This Exists

Parsing large CSV files in Python is **slow** due to interpreter overhead.  
`fast-csv-parser` uses **memory mapping (`mmap`)** and **zero-copy parsing** to read gigabytes of CSV in **milliseconds**.

- **No dynamic allocation per field**  
- **SIMD-ready** (future extensions)  
- **Single header + source**  
- **CLI + embeddable library**

---

## Features

| Feature | Status |
|-------|--------|
| Memory-mapped I/O | Done |
| Zero-copy field extraction | Done |
| Quoted fields, escaped quotes | Done |
| Custom delimiter & quote char | Done |
| CLI + embeddable library | Done |
| Docker build | Done |
| GitHub Actions CI | Done |
| Unit tests | Done |

---

## Platform Support

| OS | Status | Notes |
|----|--------|-------|
| **Linux** | Supported | Tested on Ubuntu 22.04 |
| **macOS** | Supported | Tested on macOS 14 |
| **Windows** | Not Supported | Use **WSL2** (see below) |

> **Pro tip**: On Windows, use **WSL2** — full Linux environment, zero changes needed.

### Windows + WSL2 Setup

```bash
# 1. Install WSL2
wsl --install -d Ubuntu

# 2. Open WSL terminal
git clone https://github.com/rougebyt/fast-csv-parser.git
cd fast-csv-parser
make
./csvparse examples/sample.csv
```

---

## Installation

### From Source

```bash
git clone https://github.com/rougebyt/fast-csv-parser.git
cd fast-csv-parser
make
```

Binary will be at `./csvparse`

### Docker

```bash
docker build -t csvparser .
docker run --rm -v $(pwd)/examples:/data csvparser /data/sample.csv
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

# Print only specific columns
./csvparse data.csv --columns 0,2,4
```

### As Library

```c
#include "csv_parser.h"

int main() {
    CSVParser *parser = csv_parser_new("examples/sample.csv", ',', '"');
    CSVRow *row;

    while ((row = csv_parser_next(parser)) != NULL) {
        printf("Name: %s, Age: %s\n", 
               csv_row_get(row, 0), csv_row_get(row, 1));
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

**1M rows × 5 cols (~87 MB CSV)**

| Parser       | Time    | Speedup |
|--------------|---------|---------|
| Python `csv` | 8.72s   | 1.0×    |
| **C Parser** | **0.89s** | **9.8×** |

> Zero-copy + `mmap` = massive wins  
> See [`examples/benchmark.py`](./examples/benchmark.py)

---

## Project Structure

```
src/          → Core parser + CLI
include/      → Header for library use
examples/     → Sample data + benchmark
tests/        → Unit tests
Dockerfile    → Containerized build
.github/      → CI/CD
```

---

## Contributing

1. Fork it
2. Create your feature branch (`git checkout -b feature/amazing`)
3. Commit (`git commit -m 'Add amazing feature'`)
4. Push (`git push origin feature/amazing`)
5. Open a Pull Request

---

## Author

**Moibon Dereje**  
- GitHub: [@rougebyt](https://github.com/rougebyt)  
- X: [@rougebyt](https://x.com/rougebyt)  
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
```

---

