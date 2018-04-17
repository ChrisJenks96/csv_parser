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

csv_record* csv_init(uint32_t total_rows, uint32_t total_columns);
bool csv_parse(char* fn, csv_record* d);
csv_record* partition(csv_record* cs, csv_record* h, csv_record* e, csv_record** nh, csv_record** ne);
csv_record* quick_sort_recursion(csv_record* cs, csv_record* h, csv_record* e);
void quick_sort(csv_record* cs, csv_record** hr);
void csv_print_records(char* txt, csv_record* cs);
uint32_t char_get_value(char* v);
csv_record* csv_get_element(csv_record* cs, uint32_t rid);
csv_record* csv_find_tail(csv_record* cs);
void csv_write_records(char* fn, csv_record* cs);
void csv_write_column_header(char* fn, csv_record* cs, uint32_t cid, int32_t caid);
void csv_free(csv_record* d);
uint32_t csv_find_record_id(csv_record* cs, uint32_t rid, uint32_t cid, char* r);
uint32_t csv_get_record_column(csv_record* cs, uint32_t cid, char* r);
char** csv_get_row(csv_record* cs, uint32_t rid);

static int32_t curr_record, curr_column;
static uint32_t total_rows, total_columns, sort_column;

#endif