#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <string_enhanced.h>

static char _buffer[32];
static uint _digits;

static void _add_comma_ull (ull number)
{
    if (number < 0) {
        printf ("-");
        _add_comma_ull(-number);
        return;
    }

    if (number < 1000) {
        sprintf(_buffer, "%s%llu", _buffer, number);
        return;
    }

    _add_comma_ull(number / 1000);
    sprintf(_buffer, "%s,%03llu", _buffer, (number % 1000));
}

char* add_comma_ull(ull number)
{
    _buffer[0] = '\0';
    _add_comma_ull(number);
    return _buffer;
}

static void _add_point_ulong (ulong number)
{
    if (number < 0) {
        printf ("-");
        _add_point_ulong(-number);
        return;
    }

    if (number < 1000) {
        sprintf(_buffer, "%s%lu", _buffer, number);
        return;
    }

    _add_point_ulong(number / 1000);
    sprintf(_buffer, "%s.%03lu", _buffer, (number % 1000));
}

char* add_point_ulong(ulong number)
{
    _buffer[0] = '\0';
    _add_point_ulong(number);
    return _buffer;
}

static void _add_point_uint (uint number)
{
    if (number < 0) {
        printf ("-");
        _add_point_uint(-number);
        return;
    }

    if (number < 1000) {
        sprintf(_buffer, "%s%u", _buffer, number);
        return;
    }

    _add_point_uint(number / 1000);
    sprintf(_buffer, "%s.%03u", _buffer, (number % 1000));
}

char* add_point_uint(uint number)
{
    _buffer[0] = '\0';
    _add_point_uint(number);
    return _buffer;
}

//TODO: remove
void print_spacing_digits(uint digits)
{
    _digits = digits;
}

void set_spacing_digits(uint digits)
{
    _digits = digits;
}

void print_spacing_ull(ull number)
{
    uint digits = _digits;
    printf("%llu", number);

    while(number > 9) {
        number = number / 10;
        digits--;
    }
    while(digits > 1) {
        printf( " ");
        digits--;
    }
}

void print_spacing_ulong(ulong number)
{
    uint digits = _digits;
    printf("%lu", number);

    while(number > 9) {
        number = number / 10;
        digits--;
    }
    while(digits > 1) {
        printf( " ");
        digits--;
    }
}

void print_spacing_uint(uint number)
{
    uint digits = _digits;
    printf("%u", number);

    while(number > 9) {
        number = number / 10;
        digits--;
    }
    while(digits > 1) {
        printf(" ");
        digits--;
    }
}

void print_spacing_int(int number)
{
    uint digits = _digits;
    printf("%d", number);

    while(number > 9) {
        number = number / 10;
        digits--;
    }
    while(digits > 1) {
        printf(" ");
        digits--;
    }
}

void print_spacing_string(char* string)
{
    uint digits = _digits - strlen(string);
    printf("%s", string);

    while(digits > 0) {
        printf(" ");
        digits--;
    }
}

void print_spacing_string_align_left(char* string, uint left_spaces)
{
    uint digits = _digits - strlen(string);

    while(digits > 0) {
        printf(" ");
        digits--;
    }

    printf("%s", string);

    while(left_spaces > 0) {
        printf(" ");
        left_spaces--;
    }
}

void strtoup(char *string)
{
    while (*string) {
        *string = toupper((unsigned char) *string);
        string++;
    }
}

char* strclean(char *string, char chr)
{
    char *index = strchr(string, chr);
    if (index == NULL) return NULL;
    string[index - string] = '\0';
    return index;
}
