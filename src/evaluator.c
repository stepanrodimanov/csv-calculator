#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include "evaluator.h"
#include "utils.h"

int get_cell_value(Table* table, Cell* cell, char* col_index, int row_index) {
    int pos_col = -1;
    for(int i = 0; i < table->col_count; i++){
        if(strcmp(col_index, table->headers[i]) == 0) {
            pos_col = i;
            break;
        }
    }
    
    if (pos_col == -1) {
        cell->error = 1;
        return 0;
    }
    
    int pos_row = -1;
    for(int i = 0; i < table->row_count; i++){
        if(row_index == table->row_numbers[i]) {
            pos_row = i;
            break;
        }
    }
    
    if (pos_row == -1) {
        cell->error = 1;
        return 0;
    }
    
    Cell* target = &table->cells[pos_row][pos_col];
    
    if (target->is_formula && target->visited == 0) {
        evaluate_cell(table, pos_row, pos_col);
    }
    
    if (target->visited == 1) {
        cell->error = 1;
        return 0;
    }
    
    if (target->error) {
        cell->error = 1;
        return 0;
    }
    
    return target->value;
}

Formula* parse_formula(Cell* cell) {
    Formula* f = NULL;
    if(cell->is_formula){
        int len = strlen(cell->raw);
        int pos_op = -1;
        for(int i = 1; i < len; i++){
            if(cell->raw[i] == '+' || cell->raw[i] == '-' || cell->raw[i] == '*' || cell->raw[i] == '/') {
                pos_op = i;
            }
        }
        if (pos_op == -1) {
            return NULL;
        }
        
        f = malloc(sizeof(Formula));
        if(!f){
            fprintf(stderr, "Error: memory allocation failed\n");
            return NULL;
        }
        f->op = cell->raw[pos_op];
        f->left = malloc((pos_op) * sizeof(char));
        if(!f->left){
            free(f);
            return NULL;
        }
        f->right = malloc((len - pos_op + 1) * sizeof(char));
        if(!f->right){
            free(f->left);
            free(f);
            return NULL;
        }
        
        int left_len = 0;
        for(int i = 1; i < pos_op; i++) {
            if(cell->raw[i] != ' '){
                f->left[left_len++] = cell->raw[i];
            }
        }
        f->left[left_len] = '\0';
        
        int right_len = 0;
        for(int i = pos_op + 1; i < len; i++) {
            if(cell->raw[i] != ' '){
                f->right[right_len++] = cell->raw[i];
            }
        }
        f->right[right_len] = '\0';
    }
    return f;
}

int parse_address(const char* str, char* col_name, int* row_num) {
    int i = 0;
    while (str[i] && !isdigit(str[i])) {
        col_name[i] = str[i];
        i++;
    }
    col_name[i] = '\0';
    
    int j = i;
    while (str[j]) {
        if (!isdigit(str[j])) {
            return 1;
        }
        j++;
    }
    
    char* endptr;
    errno = 0;
    long num = strtol(str + i, &endptr, 10);
    if (*endptr != '\0' || errno == ERANGE || num <= 0 || num > INT_MAX) {
        return 1;
    }
    *row_num = (int)num;
    return 0;
}

int apply_operation(Formula* f, int left, int right, Cell* cell) {
    long long result; 
    
    switch (f->op) {
        case '+':
            result = (long long)left + (long long)right;
            if (result > INT_MAX || result < INT_MIN) {
                cell->error = 1;
                fprintf(stderr, "Error: integer overflow in addition\n");
                return 0;
            }
            return (int)result;
            
        case '-':
            result = (long long)left - (long long)right;
            if (result > INT_MAX || result < INT_MIN) {
                cell->error = 1;
                fprintf(stderr, "Error: integer overflow in subtraction\n");
                return 0;
            }
            return (int)result;
            
        case '*':
            result = (long long)left * (long long)right;
            if (result > INT_MAX || result < INT_MIN) {
                cell->error = 1;
                fprintf(stderr, "Error: integer overflow in multiplication\n");
                return 0;
            }
            return (int)result;
            
        case '/':
            if (right == 0) {
                cell->error = 1;
                fprintf(stderr, "Error: division by zero\n");
                return 0;
            }
            return left / right;
            
        default:
            cell->error = 1;
            return 0;
    }
}

int get_operand_value(Table* table, Cell* cell, const char* operand) {
    int is_num = 1;
    for (int i = 0; operand[i]; i++) {
        if (i == 0 && operand[i] == '-') continue;
        if (!isdigit(operand[i])) {
            is_num = 0;
            break;
        }
    }
    
    if (is_num) {
        char* endptr;
        errno = 0;
        long num = strtol(operand, &endptr, 10);
        if (*endptr != '\0' || errno == ERANGE || num < INT_MIN || num > INT_MAX) {
            cell->error = 1;
            return 0;
        }
        return (int)num;
    }
    
    int name_len = 0;
    while (operand[name_len] && !isdigit(operand[name_len])) {
        name_len++;
    }
    
    char* col_name = malloc(name_len + 1);
    if (!col_name) {
        cell->error = 1;
        return 0;
    }
    
    int row_num;
    if (parse_address(operand, col_name, &row_num) != 0) {
        free(col_name);
        cell->error = 1;
        return 0;
    }
    
    int result = get_cell_value(table, cell, col_name, row_num);
    free(col_name);
    return result;
}

int evaluate_cell(Table* table, int row_idx, int col_idx) {
    Cell* cell = &table->cells[row_idx][col_idx];
    
    if (cell->visited == 2 && !cell->error) return cell->value;
    if (cell->visited == 1) {
        cell->error = 1;
        fprintf(stderr, "Error: cyclic dependency at cell [%d][%d]\n", row_idx, col_idx);
        return 0;
    }
    
    cell->visited = 1;
    
    if (!cell->is_formula) {
        cell->visited = 2;
        return cell->value;
    }
    
    Formula* f = parse_formula(cell);
    if (!f) {
        cell->error = 1;
        cell->value = 0;
        cell->visited = 2;
        return 0;
    }
    
    int left_val = get_operand_value(table, cell, f->left);
    int right_val = get_operand_value(table, cell, f->right);
    
    if (!cell->error) {
        cell->value = apply_operation(f, left_val, right_val, cell);
    }
    
    free(f->left);
    free(f->right);
    free(f);
    
    cell->visited = 2;
    return cell->value;
}

void evaluate_all_cell(Table* table) {
    for (int i = 0; i < table->row_count; i++) {
        for (int j = 0; j < table->col_count; j++) {
            evaluate_cell(table, i, j);
        }
    }
}
