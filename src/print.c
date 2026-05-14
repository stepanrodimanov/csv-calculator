#include <stdio.h>
#include "print.h"

void print_table(Table* table) {
    for (int j = 0; j < table->col_count; j++) {
        printf(",%s", table->headers[j]);
    }
    printf("\n");
    
    for (int i = 0; i < table->row_count; i++) {
        printf("%d", table->row_numbers[i]);
        
        for (int j = 0; j < table->col_count; j++) {
            Cell* cell = &table->cells[i][j];
            if (cell->error) {
                printf(",ERR");
            } else {
                printf(",%d", cell->value);
            }
        }
        printf("\n");
    }
}
