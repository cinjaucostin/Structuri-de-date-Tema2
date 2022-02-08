#ifndef _UTILS_H_
#define _UTILS_H_

#include "hierarchy.h"
typedef struct vector
{
    char **array;
    int size;
    int capacity;
} Vector;

int cmpstr(const void *name1, const void *name2);
Vector init(int capacity);
Vector push(Vector v, char *value);
void best_manager_function(Tree root, int value, Vector *v);
void get_by_manager(Tree root, Vector *v);
void get_by_level(Tree root, int nivel, Vector *v);
int level(Tree tree);

#endif