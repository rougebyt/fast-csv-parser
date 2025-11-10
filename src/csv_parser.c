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
    for (int i = 0; i < row->field_count; i++) {
        free(row->fields[i]);
    }
    row->field_count = 0;
}

CSVParser* csv_parser_new(const char *filename, char delimiter, char quote) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) return NULL;

    struct stat st;
    if (fstat(fd, &st) < 0) { close(fd); return NULL; }

    char *mapped = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (mapped == MAP_FAILED) return NULL;

    char *data = malloc(st.st_size + 1);
    if (!data) { munmap(mapped, st.st_size); return NULL; }
    memcpy(data, mapped, st.st_size);
    data[st.st_size] = '\0';
    munmap(mapped, st.st_size);

    CSVParser *p = malloc(sizeof(CSVParser));
    if (!p) { free(data); return NULL; }

    p->data = data;
    p->size = st.st_size;
    p->pos = 0;
    p->delimiter = delimiter;
    p->quote = quote;
    p->current_row.fields = malloc(16 * sizeof(char*));
    p->current_row.field_count = 0;
    p->current_row.capacity = 16;

    return p;
}

void csv_parser_free(CSVParser *parser) {
    if (!parser) return;
    row_reset(&parser->current_row);
    free(parser->current_row.fields);
    free(parser->data);
    free(parser);
}

CSVRow* csv_parser_next(CSVParser *parser) {
    if (parser->pos >= parser->size) return NULL;

    row_reset(&parser->current_row);

    size_t pos = parser->pos;

    while (pos < parser->size) {
        if (!ensure_capacity(&parser->current_row, parser->current_row.field_count + 1)) return NULL;

        int is_quoted = 0;
        if (parser->data[pos] == parser->quote) {
            is_quoted = 1;
            pos++;
        }

        char *field = malloc(parser->size - pos + 1);
        if (!field) return NULL;

        int fpos = 0;

        while (pos < parser->size) {
            char c = parser->data[pos];

            if (is_quoted) {
                if (c == parser->quote) {
                    if (pos + 1 < parser->size && parser->data[pos + 1] == parser->quote) {
                        field[fpos++] = parser->quote;
                        pos++;
                    } else {
                        pos++;
                        break;
                    }
                } else {
                    field[fpos++] = c;
                }
            } else {
                if (c == parser->delimiter || c == '\n' || c == '\r') {
                    break;
                } else {
                    field[fpos++] = c;
                }
            }
            pos++;
        }

        field[fpos] = '\0';

        parser->current_row.fields[parser->current_row.field_count++] = field;

        if (pos < parser->size && parser->data[pos] == parser->delimiter) {
            pos++;
            continue;
        } else {
            break;
        }
    }

    if (pos < parser->size && (parser->data[pos] == '\n' || parser->data[pos] == '\r')) {
        if (parser->data[pos] == '\r' && pos + 1 < parser->size && parser->data[pos] + 1 == '\n') {
            pos += 2;
        } else {
            pos++;
        }
    }

    parser->pos = pos;

    return parser->current_row.field_count > 0 ? &parser->current_row : NULL;
}