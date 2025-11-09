#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "../include/csv_parser.h"

void print_usage(const char *prog) {
    fprintf(stderr, "Usage: %s <file.csv> [options]\n", prog);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -d, --delimiter CHAR    Field delimiter (default: ',')\n");
    fprintf(stderr, "  -q, --quote CHAR        Quote character (default: '\"')\n");
    fprintf(stderr, "  -c, --columns LIST      Print only these columns (e.g., 0,2)\n");
    fprintf(stderr, "  -h, --help              Show help\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) { print_usage(argv[0]); return 1; }

    char delimiter = ',', quote = '"';
    char *columns_str = NULL;

    struct option long_opts[] = {
        {"delimiter", required_argument, 0, 'd'},
        {"quote", required_argument, 0, 'q'},
        {"columns", required_argument, 0, 'c'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "d:q:c:h", long_opts, NULL)) != -1) {
        switch (opt) {
            case 'd': delimiter = optarg[0]; break;
            case 'q': quote = optarg[0]; break;
            case 'c': columns_str = optarg; break;
            case 'h': print_usage(argv[0]); return 0;
            default: return 1;
        }
    }

    if (optind >= argc) { print_usage(argv[0]); return 1; }

    CSVParser *parser = csv_parser_new(argv[optind], delimiter, quote);
    if (!parser) {
        fprintf(stderr, "Error: Cannot open file %s\n", argv[optind]);
        return 1;
    }

    int *cols = NULL, ncol = 0;
    if (columns_str) {
        char *tok = strtok(columns_str, ",");
        while (tok) {
            cols = realloc(cols, (ncol + 1) * sizeof(int));
            cols[ncol++] = atoi(tok);
            tok = strtok(NULL, ",");
        }
    }

    CSVRow *row;
    while ((row = csv_parser_next(parser)) != NULL) {
        if (ncol == 0) {
            for (int i = 0; i < row->field_count; i++) {
                printf("%s%s", row->fields[i], i < row->field_count - 1 ? "\t" : "\n");
            }
        } else {
            for (int i = 0; i < ncol; i++) {
                if (cols[i] < row->field_count)
                    printf("%s%s", row->fields[cols[i]], i < ncol - 1 ? "\t" : "\n");
            }
        }
    }

    if (cols) free(cols);
    csv_parser_free(parser);
    return 0;
}