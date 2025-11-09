#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../include/csv_parser.h"

static int ensure_capacity(CSVRow *row, int needed) {
    if (row->capacity < needed) {
        int new_cap = needed < 16 ? 16 : needed * 2;
        char **new_fields = realloc(row->fields, new_cap * sizeof(char*));
        if (!new_fields) return 0;
        row->fields = new_fields;
        row->capacity = new_cap;
    }
    return 1;
}

static void row_reset(CSVRow *row) {
    row->field_count = 0;
}

CSVParser* csv_parser_new(const char *filename, char delimiter, char quote) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) return NULL;

    struct stat st;
    if (fstat(fd, &st) < 0) { close(fd); return NULL; }

    char *data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (data == MAP_FAILED) return NULL;

    CSVParser *p = malloc(sizeof(CSVParser));
    p->data = data;
    p->size = st.st_size;
    p->pos = 0;
    p->delimiter = delimiter;
    p->quote = quote;
    p->current_row.fields = NULL;
    p->current_row.field_count = 0;
    p->current_row.capacity = 0;
    row_reset(&p->current_row);

    return p;
}

void csv_parser_free(CSVParser *parser) {
    if (!parser) return;
    munmap(parser->data, parser->size);
    if (parser->current_row.fields) free(parser->current_row.fields);
    free(parser);
}

CSVRow* csv_parser_next(CSVParser *parser) {
    if (parser->pos >= parser->size) return NULL;

    row_reset(&parser->current_row);
    char *start = parser->data + parser->pos;
    char *end = parser->data + parser->size;
    char *field_start = start;
    int in_quotes = 0;

    while (start < end) {
        char c = *start;

        if (c == parser->quote) {
            in_quotes = !in_quotes;
        } else if (!in_quotes && c == parser->delimiter) {
            *start = '\0';
            if (!ensure_capacity(&parser->current_row, parser->current_row.field_count + 1))
                return NULL;
            parser->current_row.fields[parser->current_row.field_count++] = field_start;
            field_start = start + 1;
        } else if (!in_quotes && (c == '\n' || c == '\r')) {
            *start = '\0';
            if (!ensure_capacity(&parser->current_row, parser->current_row.field_count + 1))
                return NULL;
            parser->current_row.fields[parser->current_row.field_count++] = field_start;

            if (c == '\r' && start + 1 < end && *(start + 1) == '\n') start++;
            parser->pos = start - parser->data + 1;
            return &parser->current_row;
        }
        start++;
    }

    if (field_start < end && field_start < start) {
        if (!ensure_capacity(&parser->current_row, parser->current_row.field_count + 1))
            return NULL;
        parser->current_row.fields[parser->current_row.field_count++] = field_start;
    }
    parser->pos = parser->size;
    return parser->current_row.field_count > 0 ? &parser->current_row : NULL;
}