#ifndef UTIL_H

#define UTIL_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define true 1
#define false 0

char *read_file(char *path);

void **append(void **buffer, void *data, size_t len);

void print_n_times(char *string, int n);

#endif
