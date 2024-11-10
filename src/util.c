#include "util.h"
#include <stdlib.h>
#include <stdio.h>

char *read_file(char *path)
{
    FILE *ptr;
    char ch;
    ptr = fopen(path, "r");
    if (ptr == NULL)
    {
        printf("Couldn't read file '%s'\n", path);
    }
    size_t len = 0;
    do
    {
        ch = fgetc(ptr);
        ++len;
    } while (ch != EOF);
    char *output = malloc(len);
    size_t i = 0;
    fclose(ptr);
    ptr = fopen(path, "r");
    while (i < len)
    {
        output[i] = fgetc(ptr);
        ++i;
    }
    output[len - 1] = 0;
    return output;
}

inline void **append(void **buffer, void *data, size_t len)
{
    buffer = realloc(buffer, len + 1);
    buffer[len] = data;
    return buffer;
}
void print_n_times(char *string, int n)
{
    for (int i = 0; i != n; ++i)
    {
        printf("%s", string);
    }
}