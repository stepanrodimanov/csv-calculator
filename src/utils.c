#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "table.h"

int is_number(char* s) {
    if (!s || !*s) return 0;
    int i = 0;
    if (s[i] == '-') i++;
    if (!isdigit(s[i])) return 0;
    while (s[i]) {
        if (!isdigit(s[i])) return 0;
        i++;
    }
    return 1;
}

char* read_line(FILE* file) {
    size_t size = 128;
    size_t len = 0;
    char* buffer = malloc(size);
    
    if (!buffer) return NULL;
    
    int c;
    while ((c = fgetc(file)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char* tmp = realloc(buffer, size);
            if (!tmp) { 
                free(buffer); 
                return NULL; 
            }
            buffer = tmp;
        }
        buffer[len++] = c;
    }
    
    if (len == 0 && c == EOF) {
        free(buffer);
        return NULL;
    }
    
    buffer[len] = '\0';
    return buffer;
}

void free_table(Table* table) {
    if (!table) return;
    
    for (int i = 0; i < table->col_count; i++) {
        free(table->headers[i]);
    }
    free(table->headers);
    
    free(table->row_numbers);
    
    for (int i = 0; i < table->row_count; i++) {
        for (int j = 0; j < table->col_count; j++) {
            free(table->cells[i][j].raw);
        }
        free(table->cells[i]);
    }
    free(table->cells);
    
    free(table);
}
