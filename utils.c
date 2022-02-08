#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "hierarchy.h"

//Functie de comparare pentru a putea aplica qsort
int cmpstr(const void *name1, const void *name2)
{
    const char *name11 = *(const char **)name1;
    const char *name22 = *(const char **)name2;
    return strcmp(name11, name22);
}

//Functie de initiere a unui nod de tip Vector
Vector init(int capacity)
{
    Vector v;
    v.size = 0;
    v.capacity = capacity;
    v.array = malloc(capacity * sizeof(char *));
    return v;
}

Vector push(Vector v, char *value)
{
    if (v.capacity == v.size) //Cazul in care dimensiunea a ajuns la capacitatea maxima a structurii
    {
        v.capacity *= 2;                                         //Dublam capacitatea
        v.array = realloc(v.array, v.capacity * sizeof(char *)); //Realocam memorie pentru campul array cu dimensiunea noii capacitati
    }
    v.array[v.size++] = strdup(value); //Introducem pe ultima pozitie a campului array sirul de caractere dat ca parametru(numele angajatului in cazul nostru)
    return v;
}

//Functie auxiliara pentru best_manager
void best_manager_function(Tree root, int value, Vector *v)
{
    if (root == NULL)
        return;
    if (root->direct_employees_no == value) //Inseamna ca am gasit un angajat care indeplineste conditia ceruta(numarul de angajati ai echipei sale este maxim)
    {
        *v = push(*v, root->name);                          //Il introducem in structura noastra
        for (int i = 0; i < root->direct_employees_no; i++) //Avansam in ierarhie
            if (root->team[i] != NULL)
                best_manager_function(root->team[i], value, v);
    }
    else //In acest caz doar avansam in ierarhie
    {
        for (int i = 0; i < root->direct_employees_no; i++)
            if (root->team[i] != NULL)
                best_manager_function(root->team[i], value, v);
    }
}

//Functie auxiliara pentru employees_by_manager
void get_by_manager(Tree root, Vector *v)
{
    if (root == NULL)
        return;
    *v = push(*v, root->name);                          //Introducem numele angajatului in vector
    for (int i = 0; i < root->direct_employees_no; i++) //Avansam in ierarhie
        if (root->team[i] != NULL)
            get_by_manager(root->team[i], v);
}

//Functie auxiliara pentru employees_by_level
void get_by_level(Tree root, int nivel, Vector *v)
{
    if (root == NULL)
        return;
    if (level(root) == nivel) //Verificam conditia de introducere in vector(Daca nivelul angajatului este egal cu nivelul pe care noi trebuie sa il afisam)
    {
        *v = push(*v, root->name);                          //Introducem numele angajatului in vector
        for (int i = 0; i < root->direct_employees_no; i++) //Avansam in ierarhie
            if (root->team[i] != NULL)
                get_by_level(root->team[i], nivel, v);
    }
    else //Conditia nu este indeplinita si trebuie doar sa avansam la alt angajat in ierarhie
    {
        for (int i = 0; i < root->direct_employees_no; i++)
            if (root->team[i] != NULL)
                get_by_level(root->team[i], nivel, v);
    }
}