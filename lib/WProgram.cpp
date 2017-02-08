#include <WProgram.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STR_LEN 1024

size_t strlen(const char *s)
{
    for(int i=0; i<MAX_STR_LEN; ++i)
    {
	if(0==s[i]) return i;
    }
    return MAX_STR_LEN;
}


void *malloc(size_t)
{
    abort();
}
void free(void *)
{
    abort();
}

size_t Print::print(const char*)
{
    abort();
}

