#ifndef _STRING_ENHANCED_H_
#define _STRING_ENHANCED_H_

#include <types.h>

//
char* add_comma_ull(ull number);
char* add_point_ulong(ulong number);
char* add_point_uint(uint number);

//
void set_spacing_digits(uint digits);
void print_spacing_digits(uint digits);
void print_spacing_ull(ull number);
void print_spacing_ulong(ulong number);
void print_spacing_uint(uint number);
void print_spacing_int(int number);
void print_spacing_string(char* string);
void print_spacing_string_align_left(char* string, uint left_spaces);

//
char* strclean(char *string, char chr);

//
void strtoup(char *string);

#else
#endif