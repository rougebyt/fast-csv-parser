#ifndef CSV_PARSER_H
#define CSV_PARSER_H

typedef struct {
    char **fields;
    int field_count;
    int capacity;
} CSVRow;

typedef struct {
    char *data;
    size_t size;
    size_t pos;
    char delimiter;
    char quote;
    CSVRow current_row;
} CSVParser;

// Public functions
CSVParser* csv_parser_new(const char *filename, char delimiter, char quote);
void csv_parser_free(CSVParser *parser);
CSVRow* csv_parser_next(CSVParser *parser);

#endif