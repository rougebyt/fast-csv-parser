CC = gcc
CFLAGS = -O3 -Wall -Wextra -std=c11 -Iinclude
TARGET = csvparse
TEST_TARGET = test_parser
SRC = src/main.c src/csv_parser.c
TEST_SRC = tests/test_parser.c src/csv_parser.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

$(TEST_TARGET): $(TEST_SRC)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SRC)

test: $(TEST_TARGET)
	@echo "=== Running Unit Tests ==="
	@./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET) $(OBJ)
	rm -f /tmp/csvtest.*
bench: $(TARGET)
	@echo "=== C Parser ==="
	@time ./$(TARGET) examples/sample.csv > /dev/null
	@echo "=== Python csv ==="
	@time python3 examples/benchmark.py > /dev/null



.PHONY: all clean test bench