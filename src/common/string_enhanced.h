#ifndef _STRING_ENHANCED_H_
#define _STRING_ENHANCED_H_

#include "types/generic.h"

// Adds commas to numbers. It is printed in internal buffer and a pointer to
// this buffer is returned. So do whatever you want before call again one
// of these functions.
char *add_point_ull(ull number);
char *add_point_ulong(ulong number);
char *add_point_uint(uint number);

// Prints a type splitter by columns. You have to call 'set_spacing_digits',
// every time you want to define the number of characters per column. You can
// define just once, or iteratively to use different column lengths.
void set_spacing_digits(uint digits);
void print_spacing_digits(uint digits);
void print_spacing_ull(ull number);
void print_spacing_ulong(ulong number);
void print_spacing_uint(uint number);
void print_spacing_int(int number);
void print_spacing_string(char* string);
void print_spacing_string_align_left(char* string, uint left_spaces);

// Cleans the character pointed by 'chr', adding an '\0' in its position.
char* strclean(char *string, char chr);

// Converts a string to upper case.
void strtoup(char *string);

#else
#endif
