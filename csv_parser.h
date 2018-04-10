#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <stdio.h>
#include <stdint.h>
//gcc
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>

//vs
//#include <cctype>
//#include <memory>

typedef struct csv_record
{
	char** columns;
	struct csv_record* next;
} csv_record;

#define CSV_CELL_LENGTH 32

void csv_parse(csv_record* d, char* b, uint32_t l);
csv_record* partition(csv_record* cs, csv_record* h, csv_record* e, csv_record** nh, csv_record** ne);
csv_record* quick_sort_recursion(csv_record* cs, csv_record* h, csv_record* e);
void quick_sort(csv_record* cs, csv_record** hr);
void print_records(char* txt, csv_record* cs);
uint32_t char_get_value(char* v);
csv_record* csv_get_element(csv_record* cs, uint32_t rid);
csv_record* csv_find_tail(csv_record* cs);
void write_records(char* fn, csv_record* cs);

int32_t curr_record = 0, curr_column = 0;
uint32_t total_rows = 0, total_columns = 0, sort_column = 0;

#endif