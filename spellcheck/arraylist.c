#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "arraylist.h"

#ifndef DEBUG
#define DEBUG 0
#endif

void al_init(arraylist_t *L, unsigned size)
{
    L->data = malloc(size * sizeof(char*));
    L->length = 0;
    L->capacity = size;
}

char* al_get(arraylist_t *L, unsigned index)
{
    if (index >= L->length) {
        fprintf(stderr, "Index out of bounds!\n");
        return NULL;
    }
    return L->data[index];
}

void al_destroy(arraylist_t *L)
{
	for (int i = 0; i < L->length; i++) {
        free(L->data[i]);  // free each string
    }
    free(L->data);
}

unsigned al_length(arraylist_t *L)
{
    return L->length;
}

void al_push(arraylist_t *L, char* item)
{
    if (L->length == L->capacity) {
	L->capacity *= 2;
	char **temp = realloc(L->data, L->capacity * sizeof(char*));
	if (!temp) { 
	    fprintf(stderr, "Out of memory!\n");
	    exit(EXIT_FAILURE);
	}

	L->data = temp;
	if (DEBUG) printf("Resized array to %u\n", L->capacity);
    }

    L->data[L->length] = (char *)malloc((strlen(item) + 1) * sizeof(char));
    strcpy(L->data[L->length], item);
    // L->data[L->length] = item;
    L->length++;
}

// int al_pop(arraylist_t *L, char **dest)
// {
//     if (L->length == 0) return 0;

//     L->length--;
//     *dest = L->data[L->length];


//     return 1;
// }

