#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "evaluator.h"
#include "print.h"
#include "utils.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Error: usage %s csv_file\n", argv[0]);
        return 1;
    }
    
    Table* table = load_csv(argv[1]);
    if (!table) {
        return 1;
    }
    
    evaluate_all_cell(table);
    print_table(table);
    free_table(table);
    
    return 0;
}
