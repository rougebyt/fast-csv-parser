#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/csv_parser.h"

// === Cross-platform temp file creator ===
char* create_temp_csv(const char *data, char *template) {
    // template must be like "/tmp/csvtest.XXXXXX"
    strcpy(template + strlen(template) - 6, "XXXXXX");
    int fd = mkstemp(template);
    if (fd == -1) return NULL;

    FILE *f = fdopen(fd, "w");
    if (!f) { close(fd); unlink(template); return NULL; }

    fwrite(data, 1, strlen(data), f);
    fclose(f);
    return template;
}

void test_basic_parsing() {
    printf("Running: test_basic_parsing\n");

    const char *csv_data = "name,age,city\nMoibon,25,Addis\nAlice,30,NY\n";
    char filename[] = "/tmp/csvtest.XXXXXX";

    if (!create_temp_csv(csv_data, filename)) {
        printf("FAIL: create_temp_csv\n");
        exit(1);
    }

    CSVParser *parser = csv_parser_new(filename, ',', '"');
    if (!parser) {
        printf("FAIL: csv_parser_new(%s)\n", filename);
        unlink(filename);
        exit(1);
    }

    CSVRow *row;
    int rows = 0;
    while ((row = csv_parser_next(parser)) != NULL) {
        rows++;
        if (rows == 1) {
            if (strcmp(row->fields[0], "name") != 0) {
                printf("FAIL: header[0] != name\n");
                csv_parser_free(parser);
                unlink(filename);
                exit(1);
            }
        }
    }

    csv_parser_free(parser);
    unlink(filename);

    if (rows != 3) {
        printf("FAIL: expected 3 rows, got %d\n", rows);
        exit(1);
    }
    printf("PASS: test_basic_parsing\n");
}

void test_quoted_fields() {
    printf("Running: test_quoted_fields\n");

    const char *csv_data = "id,data\n1,\"hello, world\"\n2,\"\"\"quoted\"\"\"\n";
    char filename[] = "/tmp/csvtest.XXXXXX";

    if (!create_temp_csv(csv_data, filename)) {
        printf("FAIL: create_temp_csv\n");
        exit(1);
    }

    CSVParser *parser = csv_parser_new(filename, ',', '"');
    if (!parser) { unlink(filename); exit(1); }

    csv_parser_next(parser);  // Skip header

    CSVRow *row = csv_parser_next(parser);
    if (!row || strcmp(row->fields[1], "hello, world") != 0) {
        printf("FAIL: quoted comma not preserved\n");
        csv_parser_free(parser);
        unlink(filename);
        exit(1);
    }

    row = csv_parser_next(parser);
    if (!row || strcmp(row->fields[1], "\"quoted\"") != 0) {
        printf("FAIL: escaped quotes not preserved\n");
        csv_parser_free(parser);
        unlink(filename);
        exit(1);
    }

    csv_parser_free(parser);
    unlink(filename);
    printf("PASS: test_quoted_fields\n");
}

int main() {
    printf("=== fast-csv-parser Unit Tests ===\n");
    test_basic_parsing();
    test_quoted_fields();
    printf("All tests passed!\n");
    return 0;
}