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

    // mmap for reading
    char *mapped = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (mapped == MAP_FAILED) return NULL;

    // Allocate writable copy
    char *data = malloc(st.st_size + 1);
    if (!data) { munmap(mapped, st.st_size); return NULL; }
    memcpy(data, mapped, st.st_size);
    data[st.st_size] = '\0';  // Ensure null-terminated
    munmap(mapped, st.st_size);

    CSVParser *p = malloc(sizeof(CSVParser));
    if (!p) { free(data); return NULL; }

    p->data = data;
    p->size = st.st_size;
    p->pos = 0;
    p->delimiter = delimiter;
    p->quote = quote;
    p->current_row.fields = NULL;
    p->current_row.field_count = 0;
    p->current_row.capacity = 0;

    return p;
}

void csv_parser_free(CSVParser *parser) {
    if (!parser) return;
    if (parser->data) free(parser->data);  // ← free malloc'd copy
    if (parser->current_row.fields) free(parser->current_row.fields);
    free(parser);
}

CSVRow* csv_parser_next(CSVParser *parser) {
    if (parser->pos >= parser->size) return NULL;

    row_reset(&parser->current_row);
    char *p = parser->data + parser->pos;
    char *end = parser->data + parser->size;
    char *field_start = p;
    int in_quotes = 0;

    while (p < end) {
        char c = *p;

        if (c == parser->quote) {
            if (!in_quotes) {
                in_quotes = 1;
                field_start = p + 1;
            } else {
                if (p + 1 < end && *(p + 1) == parser->quote) {
                    p++;  // skip second " in ""
                } else {
                    in_quotes = 0;
                    *p = '\0';
                }
            }
        } else if (!in_quotes) {
            if (c == parser->delimiter) {
                *p = '\0';
                if (!ensure_capacity(&parser->current_row, parser->current_row.field_count + 1))
                    return NULL;
                parser->current_row.fields[parser->current_row.field_count++] = field_start;
                field_start = p + 1;
            } else if (c == '\n' || c == '\r') {
                *p = '\0';
                if (!ensure_capacity(&parser->current_row, parser->current_row.field_count + 1))
                    return NULL;
                parser->current_row.fields[parser->current_row.field_count++] = field_start;

                if (c == '\r' && p + 1 < end && *(p + 1) == '\n') p++;
                parser->pos = p - parser->data + 1;
                return &parser->current_row;
            }
        }
        p++;
    }

    // EOF: last field
    if (field_start < end) {
        char *field_end = end;
        if (in_quotes && field_end > field_start && *(field_end - 1) == parser->quote) {
            field_end--;
        }
        *field_end = '\0';

        if (!ensure_capacity(&parser->current_row, parser->current_row.field_count + 1))
            return NULL;
        parser->current_row.fields[parser->current_row.field_count++] = field_start;
    }

    parser->pos = parser->size;
    return parser->current_row.field_count > 0 ? &parser->current_row : NULL;
}