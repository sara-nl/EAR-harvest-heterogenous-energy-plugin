#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <common/types/projection.h>
#include <common/states.h>

#define PERMISSION S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define OPTIONS O_WRONLY | O_CREAT | O_TRUNC | O_APPEND

int read_coefficients_file(char *path, coefficient_t **coeffs, int size)
{
    coefficient_t *coeffs_aux;
    int ret, fd;

    if ((fd = open(path, O_RDONLY)) < 0) {
        return EAR_FILE_NOT_FOUND;
    }

    if (size <= 0) {
        size = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);
    }

    // Allocating memory
    coeffs_aux = (coefficient_t *) malloc(size);

    if (coeffs_aux == NULL)
    {
        close(fd);
        return EAR_ALLOC_ERROR;
    }

    // Reset the memory to zeroes
    memset(coeffs_aux, 0, sizeof(coefficient_t));

    if ((ret = read(fd, coeffs_aux, size)) != size)
    {
        close(fd);
        free(coeffs_aux);
        return EAR_READ_ERROR;
    }
    close(fd);

    *coeffs = coeffs_aux;
    return (size / sizeof(coefficient_t));
}
