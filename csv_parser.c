#include "csv_parser.h"

int main(int argc, char** argv)
{
	if (argc > 3)
	{
		FILE* f = fopen(argv[1], "rb");
		total_columns = atoi(argv[2]);
		total_rows = atoi(argv[3]);

		if (f != NULL)
		{
			uint32_t i;
			printf("opened %s successfully\n", argv[1]);
			csv_record* data = (csv_record*)malloc(sizeof(csv_record) * total_rows);
			//alloc the new columns, link our next column
			for (i = 0; i < total_rows; i++){
				data[i].columns = (char**)malloc(sizeof(char**) * total_columns);
				data[i].next = &data[i+1];
			}

			//ensure our last record doesn't link to any other
			data[total_rows-1].next = NULL;

			fseek(f, 0, SEEK_END);
			uint32_t fs = ftell(f);
			fseek(f, 0, SEEK_SET);
			char* buffer = (char*)malloc(sizeof(char) * fs);
			fread(&buffer[0], fs, 1, f);
			csv_parse(data, buffer, fs);
			print_records("original", data);
			fclose(f);
			free(buffer);
			buffer = NULL;

			if (argc > 4)
			{
				sort_column = atoi(argv[4]);
				quick_sort(data, &data);
				//print_records("sorted data", data);
				write_records("sorted_csv_data.csv", data);
			}

			while (data != NULL)
			{
				free(data->columns);
				data->columns = NULL;
				data = data->next;
			}
		
			printf("finished csv parsing\n");
		}

		else
			printf("failed to open %s\n", argv[1]);
	}

	else
		printf("[csv name] [num columns] [num rows] [sort column]\n");

	return 0;
}

//traverse the list writing records to file
void write_records(char* fn, csv_record* cs)
{
	//x,y,z\n
	FILE* f = fopen(fn, "w");
	uint32_t i;
	while (cs != NULL){
		for (i = 0; i < total_columns; i++){
			if (i < total_columns - 1)
				fprintf(f, "%s,", cs->columns[i]);
			else
				fprintf(f, "%s", cs->columns[i]);
		}

		fprintf(f, "\n");
		cs = cs->next;
	}

	fclose(f);
	printf("written sorted data to %s\n", fn);
}

//traverse the list writing records to commandline
void print_records(char* txt, csv_record* cs)
{
	uint32_t i = 0, c = i;
	csv_record* current = &cs[0];
	while (current != NULL){
		printf("row %i: ", c++);
		for (i = 0; i < total_columns; i++)
			printf("%s ", current->columns[i]);
		printf("\n");
		current = current->next;
	}
}

void csv_parse(csv_record* d, char* b, uint32_t l)
{
	//32 bytes char length by default
	char csv_key[CSV_CELL_LENGTH];
	bool str_value = false;
	uint32_t buf_c = 0;
	int32_t key_c = 0, old_key_c = 0;
	while (buf_c < l)
	{
		//if our value has a comma in that isn't used to seperate fields
		//then treat this as a formatted value e.g. "13,000"
		if (b[buf_c] != 0x22)
		{
			//0x0D CR, 0x2C COMMA
			if (b[buf_c] == 0x2C || b[buf_c] == 0x0D)
			{
				if (!str_value)
				{
					if (curr_column >= total_columns){
						curr_record++;
						curr_column = 0;
						//increment to skip str formatting on new row
						old_key_c++;
					}

					//alloc the select column, format, null terminate and place data in it
					d[curr_record].columns[curr_column] = (char*)malloc((key_c - old_key_c) + 1);
					memcpy(&d[curr_record].columns[curr_column][0], &b[old_key_c], key_c - old_key_c);
					d[curr_record].columns[curr_column][key_c - old_key_c] = 0;
					old_key_c = key_c + 1; 
					curr_column++;
				}
			}
		}

		else
			str_value = !str_value;

		buf_c++; key_c++;
	}
}

//diffentiate between cell w/ value and cell with letters
uint32_t char_get_value(char* v)
{
	uint32_t t = 0, i = 0;
	while (v[i] != NULL){
		t += isdigit(v[i]) != 0 ? atoi(&v[i++]) : (uint8_t)v[i++];
	}

	return t;
}

//get element in linked list
csv_record* csv_get_element(csv_record* cs, uint32_t rid)
{
	uint32_t c = 0;
	while (cs != NULL){
		if (c == rid)
			return cs;
		c++;
		cs = cs->next;
	}

	return NULL;
}

//find the end of the linked list
csv_record* csv_find_tail(csv_record* cs)
{
	while (cs->next != NULL)
		cs = cs->next;
	return cs;
}

//find pivot point and swap based on being lower than the pivot point
csv_record* partition(csv_record* cs, csv_record* h, csv_record* e, csv_record** nh, csv_record** ne)
{
	csv_record* pivot = csv_find_tail(cs);
	csv_record* prev = NULL, *curr = &cs[0], *tail = pivot;
	while (curr != pivot)
	{
		//get raw values of text or value for comparison
		int32_t pivot_val = char_get_value(pivot->columns[sort_column]);
		int32_t curr_val = char_get_value(curr->columns[sort_column]);
		if (curr_val < pivot_val)
		{
			//first iteration of record that is lower than pivot
			//is the new head of the list
			if ((*nh) == NULL)
				*nh = curr;

			prev = curr;
			curr = curr->next;
		}

		//if the current record is bigger than pivot
		//make current record the next of tail, and change tail
		else
		{
			if (prev != NULL)
				prev->next = curr->next;
			csv_record* tmp = curr->next;
			curr->next = NULL;
			tail->next = curr;
			tail = curr;
			curr = tmp;
		}
	}

	//if pivot is the smallest, it becomes the first element
	if ((*nh) == NULL)
		*nh = pivot;

	//update new end to current last
	*ne = tail;
	return pivot;
}

//works from the left side of pivot, then the right side
csv_record* quick_sort_recursion(csv_record* cs, csv_record* h, csv_record* e)
{
	//no quick sort needed if the head is null and both the head and end are the same
	if (h == NULL || h == e)
		return h;

	csv_record* nh = NULL, *ne = NULL;
	//partition the list, new head and end will be updated 
	csv_record* pivot = partition(cs, h, e, &nh, &ne);
	//if the pivot is the smallest elem
	if (nh != pivot)
	{
		//set the record pre-pivot to NULL
		csv_record* tmp = nh;
		while (tmp->next != pivot)
			tmp = tmp->next;
		tmp->next = NULL;

		//run same procedure above again
		nh = quick_sort_recursion(cs, nh, tmp);
		tmp = csv_find_tail(cs);
		tmp->next = pivot;
	}

	pivot->next = quick_sort_recursion(cs, pivot->next, ne);
	return nh;
}

//wrapper for the quick sort recursion, we call this..
void quick_sort(csv_record* cs, csv_record** hr)
{
	*hr = quick_sort_recursion(cs, *hr, csv_find_tail(&cs[0]));
}
