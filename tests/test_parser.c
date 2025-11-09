#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/csv_parser.h"

// Simple assert macro
#define ASSERT(cond, msg) \
    do { if (!(cond)) { \
        fprintf(stderr, "FAIL: %s (line %d)\n", msg, __LINE__); \
        exit(1); \
    } } while(0)

#define TEST(name) printf("Running: %s\n", name)

void test_basic_parsing() {
    TEST("test_basic_parsing");

    // Create temp CSV in memory
    const char *csv_data = "name,age,city\nMoibon,25,Addis\nAlice,30,NY\n";
    FILE *f = tmpfile();
    fwrite(csv_data, 1, strlen(csv_data), f);
    fseek(f, 0, SEEK_SET);

    char filename[256];
    snprintf(filename, sizeof(filename), "/proc/self/fd/%d", fileno(f));

    CSVParser *parser = csv_parser_new(filename, ',', '"');
    ASSERT(parser != NULL, "Parser should be created");

    CSVRow *row;
    int row_count = 0;

    // Header
    row = csv_parser_next(parser);
    ASSERT(row != NULL, "Should read header");
    ASSERT(row->field_count == 3, "Header should have 3 fields");
    ASSERT(strcmp(row->fields[0], "name") == 0, "Field 0 == name");
    row_count++;

    // First data row
    row = csv_parser_next(parser);
    ASSERT(row != NULL, "Should read row 1");
    ASSERT(strcmp(row->fields[1], "25") == 0, "Age == 25");
    row_count++;

    // Second data row
    row = csv_parser_next(parser);
    ASSERT(row != NULL, "Should read row 2");
    ASSERT(strcmp(row->fields[2], "NY") == 0, "City == NY");
    row_count++;

    // EOF
    row = csv_parser_next(parser);
    ASSERT(row == NULL, "Should be EOF");

    csv_parser_free(parser);
    fclose(f);
    printf("PASS: test_basic_parsing (%d rows)\n", row_count);
}

void test_quoted_fields() {
    TEST("test_quoted_fields");

    const char *csv_data = "id,data\n1,\"hello, world\"\n2,\"\"\"quoted\"\"\"\n";
    FILE *f = tmpfile();
    fwrite(csv_data, 1, strlen(csv_data), f);
    fseek(f, 0, SEEK_SET);

    char filename[256];
    snprintf(filename, sizeof(filename), "/proc/self/fd/%d", fileno(f));

    CSVParser *parser = csv_parser_new(filename, ',', '"');
    ASSERT(parser != NULL, "Parser creation");

    // Skip header
    csv_parser_next(parser);

    CSVRow *row = csv_parser_next(parser);
    ASSERT(strcmp(row->fields[1], "hello, world") == 0, "Quoted comma preserved");

    row = csv_parser_next(parser);
    ASSERT(strcmp(row->fields[1], "\"quoted\"") == 0, "Escaped quotes preserved");

    csv_parser_free(parser);
    fclose(f);
    printf("PASS: test_quoted_fields\n");
}

int main() {
    printf("=== fast-csv-parser Unit Tests ===\n");
    test_basic_parsing();
    test_quoted_fields();
    printf("All tests passed!\n");
    return 0;
}