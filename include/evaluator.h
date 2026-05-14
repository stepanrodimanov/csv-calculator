#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "table.h"
#include "formula.h"

int evaluate_cell(Table* table, int row_idx, int col_idx);
int get_cell_value(Table* table, Cell* cell, char* col_index, int row_index);
int get_operand_value(Table* table, Cell* cell, const char* operand);
Formula* parse_formula(Cell* cell);
int parse_address(const char* str, char* col_name, int* row_num);
int apply_operation(Formula* f, int left, int right, Cell* cell);
void evaluate_all_cell(Table* table);

#endif
