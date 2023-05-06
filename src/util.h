#ifndef UTIL_H

#define UTIL_H

#include<stdlib.h>
#include<stdint.h>
#include<stddef.h>

typedef uint8_t bool;
#define true 1
#define false 0

char* read_file(char* path);

void** append(void** buffer, void* data, size_t len);

#endif