#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <string_enhanced.h>

static char _buffer[32];
static int _digits;

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

void print_spacing_digits(int digits)
{
    _digits = digits;
}

void print_spacing_ull(ull number)
{
    int digits = _digits;
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

void print_spacing_int(int number)
{
    int digits = _digits;
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
    int digits = _digits - strlen(string);
    printf("%s", string);

    while(digits > 0) {
        printf(" ");
        digits--;
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
