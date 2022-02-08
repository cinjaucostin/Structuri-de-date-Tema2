#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hierarchy.h"
#include "utils.h"

Tree search_node(Tree tree, char *manager_name);
Tree sorteaza_echipa(Tree tree);
int level(Tree tree);
int search_max(Tree tree);
void reorganizare_aux(Tree tree, Tree new_manager);
void preorder_traversal_auxiliar(FILE *f, Tree tree);

/* Adauga un angajat nou in ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele noului angajat
 * manager_name: numele sefului noului angajat
 *
 * return: intoarce ierarhia modificata. Daca tree si manager_name sunt NULL, 
           atunci employee_name e primul om din ierarhie.
 */

Tree hire(Tree tree, char *employee_name, char *manager_name)
{
    Tree mng = search_node(tree, manager_name);
    if (mng == NULL) //inseamna ca ierarhia este goala si trebuie sa introducem noi radacina
    {
        tree = malloc(sizeof(TreeNode));                                 //alocam spatiu pentru nod
        tree->name = malloc((strlen(employee_name) + 1) * sizeof(char)); //alocam spatiu pentru nume
        strcpy(tree->name, employee_name);                               //copiem numele angajatului
        tree->direct_employees_no = 0;                                   //actualizam campul cu numarul des angajati ai noului angajat
        tree->manager = NULL;                                            //inseamna ca este radacina(seful companiei)
        return tree;                                                     //returnam noul arbore
    }
    else if (mng != NULL) //inseamna ca arborele nu este gol si am gasit managerul caruia trebuie sa ii marim echipa
    {
        if (mng->direct_employees_no == 0) //Cazul in care angajatul nu este manager si trebuie sa ii alocam memorie pentru noua sa echipa
        {
            //Operatia de creare e noului angajat
            Tree new_employee;
            new_employee = malloc(sizeof(TreeNode));
            new_employee->name = malloc((strlen(employee_name) + 1) * sizeof(char));
            strcpy(new_employee->name, employee_name);
            new_employee->direct_employees_no = 0;
            mng->team = malloc(sizeof(Tree));                   //alocam memorie pentru echipa sa
            mng->team[mng->direct_employees_no] = new_employee; //Introducem noul angajat pe singura pozitie din echipa(legatura manager->angajat)
            new_employee->manager = mng;                        //Cream legatura angajat->manager
            mng->direct_employees_no++;                         //Marim echipa managerului
            return tree;
        }
        else if (mng->direct_employees_no != 0)
        {
            //Operatie de creare a noului angajat
            Tree new_employee;
            new_employee = malloc(sizeof(TreeNode));
            new_employee->name = malloc((strlen(employee_name) + 1) * sizeof(char));
            strcpy(new_employee->name, employee_name);
            new_employee->direct_employees_no = 0;
            mng->team = realloc(mng->team, (mng->direct_employees_no + 1) * sizeof(Tree)); //realocam memorie pentru echipa managerului
            new_employee->manager = mng;                                                   //legatura angajat->manager
            mng->team[mng->direct_employees_no] = new_employee;                            //Introducem legatura manager->angajat pe ultima pozitie din vectorul ehipa
            mng->direct_employees_no++;                                                    //Marim echipa managerului
            mng = sorteaza_echipa(mng);                                                    //Sortam echipa pentru a avea angajatii ordonati lexicografic
            return tree;
        }
    }
}
//Functie de cautare a unui nod cu numele dat ca parametru al functiei
Tree search_node(Tree tree, char *name)
{
    if (tree == NULL) //Inseamna ca arborele de ierarhie este gol
        return NULL;
    else if (strcmp(tree->name, name) == 0) //Inseamna ca am ajuns in nodul cu numele cerut si returnam pointerul acestuia
        return tree;
    else
    {
        for (int i = 0; i < tree->direct_employees_no; i++) //Parcurgem in continuare arborele
        {
            if (search_node(tree->team[i], name) != NULL)
                return search_node(tree->team[i], name);
        }
    }
    return NULL; //In caz ca nu s-a gasit nimic(angajatul cu numele dat nu se afla in ierarhie)
}
//Functie de sortare a echipei unui angajat
Tree sorteaza_echipa(Tree tree)
{
    for (int i = 0; i < tree->direct_employees_no; i++)
    {
        for (int j = 0; j < tree->direct_employees_no; j++)
        {
            if (strcmp(tree->team[i]->name, tree->team[j]->name) < 0)
            {
                //Interschimbam pointerii
                Tree temp = tree->team[j];
                tree->team[j] = tree->team[i];
                tree->team[i] = temp;
            }
        }
    }
    return tree;
}

/* Sterge un angajat din ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului concediat
 *
 * return: intoarce ierarhia modificata.
 */
Tree fire(Tree tree, char *employee_name)
{
    Tree eliminat = search_node(tree, employee_name);
    if (eliminat != NULL)
    {
        if (eliminat->manager == NULL) //inseamna ca nodul ce trebuie eliminat este radacina(nu se poate acest lucru)
            return tree;
        if (eliminat->direct_employees_no == 0) //inseamna ca este frunza
        {
            Tree manager = eliminat->manager;
            if (manager->direct_employees_no == 1)
            {
                manager->team = NULL;
                manager->direct_employees_no = 0;
            }
            else
            {
                for (int i = 0; i < manager->direct_employees_no; i++)
                {
                    if (strcmp(manager->team[i]->name, employee_name) >= 0) //mutam la stanga angajatii care se afla la dreapta celui pe care trebuie sa il eliminam
                    {
                        manager->team[i] = manager->team[i + 1];
                    }
                }
                free(eliminat->name);
                free(eliminat);
                manager->direct_employees_no--;
            }
        }
        else if (eliminat->direct_employees_no > 0) //inseamna ca nodul pe care trebuie sa il elimin este si el manager la randul lui => trebuie sa ii mutam echipa la managerul sau
        {
            Tree manager = eliminat->manager;
            //Introducem echipa angajatului concediat in echipa managerului sau
            manager->team = realloc(manager->team, (manager->direct_employees_no + eliminat->direct_employees_no) * sizeof(Tree)); //realocam memorie pentru echipa managerului
            for (int i = 0; i < eliminat->direct_employees_no; i++)
            {
                manager->team[manager->direct_employees_no + i] = eliminat->team[i];
                eliminat->team[i]->manager = manager;
            }
            manager->direct_employees_no += eliminat->direct_employees_no;
            manager = sorteaza_echipa(manager);
            for (int i = 0; i < manager->direct_employees_no; i++)
            {
                if (strcmp(manager->team[i]->name, employee_name) >= 0) //mutam la stanga angajatii care se afla la dreapta celui pe care trebuie sa il eliminam
                {
                    manager->team[i] = manager->team[i + 1];
                }
            }
            free(eliminat->name);
            free(eliminat);
            manager->direct_employees_no--;
        }
    }
    return tree;
}

/* Promoveaza un angajat in ierarhie. Verifica faptul ca angajatul e cel putin 
 * pe nivelul 2 pentru a putea efectua operatia.
 * 
 * tree: ierarhia existenta
 * employee_name: numele noului angajat
 *
 * return: intoarce ierarhia modificata.
 */

Tree promote(Tree tree, char *employee_name)
{
    Tree angajat = search_node(tree, employee_name);
    int nivel_angajat = level(angajat);
    char *nume_angajat;
    nume_angajat = malloc(10 * sizeof(char));
    if (angajat != NULL)
    {
        if (nivel_angajat >= 2)
        {
            Tree backup = angajat->manager->manager;
            //Mutam echipa angajatului ce trebuie promovat in echipa managerului sau
            angajat->manager->team = realloc(angajat->manager->team, (angajat->manager->direct_employees_no + angajat->direct_employees_no) * sizeof(Tree));
            //Introducem de fiecare data la sfarsitul echipei noul membru
            for (int i = 0; i < angajat->direct_employees_no; i++)
            {
                angajat->manager->team[angajat->manager->direct_employees_no + i] = angajat->team[i];
                angajat->team[i]->manager = angajat->manager;
            }
            angajat->manager->direct_employees_no += angajat->direct_employees_no; //Marim dimensiunea echipei managerului
            angajat->manager = sorteaza_echipa(angajat->manager);                  //sortam echipa managerului pentru ca angajatii sa fie ordonati lexicografic
            //Concediem angajatul si il reangajam ca membru al echipei managerului managerului sau initial
            strcpy(nume_angajat, angajat->name);
            angajat->direct_employees_no = 0;
            angajat->team = NULL;
            tree = fire(tree, nume_angajat);
            tree = hire(tree, nume_angajat, backup->name);
            free(nume_angajat);
            return tree;
        }
        else
            return tree;
    }
    else
        return tree;
}
//Functie de calculare a nivelului unui nod din ierarhie
int level(Tree tree)
{
    if (tree->manager == NULL)
        return 0;
    else
        return 1 + level(tree->manager);
}

/* Muta un angajat in ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului
 * new_manager_name: numele noului sef al angajatului
 *
 * return: intoarce ierarhia modificata.
 */
Tree move_employee(Tree tree, char *employee_name, char *new_manager_name)
{
    Tree new_manager = search_node(tree, new_manager_name);
    Tree angajat_mutat = search_node(tree, employee_name);
    if (angajat_mutat != NULL && new_manager != NULL)
    {
        for (int i = 0; i < new_manager->direct_employees_no; i++) //cautam intai sa vedem daca angajatul nu se afla deja in echipa noului manager
        {
            if (strcmp(new_manager->team[i]->name, employee_name) == 0)
                return tree;
        }
        if (angajat_mutat->manager != NULL) //Primul om din ierarhie nu poate fi mutat
        {
            //Ii adaugam echipa la echipa managerului sau
            angajat_mutat->manager->team = realloc(angajat_mutat->manager->team, (angajat_mutat->manager->direct_employees_no + angajat_mutat->direct_employees_no) * sizeof(Tree));
            for (int i = 0; i < angajat_mutat->direct_employees_no; i++)
            {
                angajat_mutat->manager->team[angajat_mutat->manager->direct_employees_no + i] = angajat_mutat->team[i];
                angajat_mutat->team[i]->manager = angajat_mutat->manager;
            }
            angajat_mutat->manager->direct_employees_no += angajat_mutat->direct_employees_no;
            angajat_mutat->manager = sorteaza_echipa(angajat_mutat->manager);
            //Concediem angajatul si il angajam in echipa noului managers
            angajat_mutat->direct_employees_no = 0;
            angajat_mutat->team = NULL;
            tree = fire(tree, employee_name);
            tree = hire(tree, employee_name, new_manager_name);
            return tree;
        }
    }
    return tree;
}

/* Muta o echipa in ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului din varful echipei mutate
 * new_manager_name: numele noului sef al angajatului
 *
 * return: intoarce ierarhia modificata.
 */
Tree move_team(Tree tree, char *employee_name, char *new_manager_name)
{
    Tree new_manager = search_node(tree, new_manager_name);
    Tree angajat_mutat = search_node(tree, employee_name);
    if (new_manager != NULL && angajat_mutat != NULL) //Verificam daca cei doi se afla in ierarhia de angajati
    {
        if (angajat_mutat->manager != NULL) //Verificam daca nu cumva echipa pe care trebuie sa o mutam este echipa nodului radacina
        {
            for (int i = 0; i < angajat_mutat->manager->direct_employees_no; i++) //Eliminam angajatul din echipa managerului sau prin shiftari in vectorul team al managerului sau
            {
                if (strcmp(angajat_mutat->manager->team[i]->name, employee_name) >= 0)
                {
                    angajat_mutat->manager->team[i] = angajat_mutat->manager->team[i + 1];
                }
            }
            angajat_mutat->manager->direct_employees_no--;
            if (new_manager->direct_employees_no != 0) //Cazul in care echipa noului manager nu este goala
            {
                new_manager->team = realloc(new_manager->team, (new_manager->direct_employees_no + 1) * sizeof(Tree)); //Realocam echipa managerului cu mai multa memorie
                new_manager->team[new_manager->direct_employees_no] = angajat_mutat;                                   //Introducem angajatul al carei echipe va fi mutate in echipa noului sau manager
                new_manager->direct_employees_no++;                                                                    //Marim dimensiunea echipei
                new_manager = sorteaza_echipa(new_manager);                                                            //Sortam echipa
                angajat_mutat->manager = new_manager;                                                                  //Restabilim legatura angajat_mutat->manager_nou
                return tree;
            }
            else //Cazul in care echipa noului manager este goala
            {
                new_manager->team = malloc(sizeof(Tree));                            //Alocam memorie pentru echipa acestuia
                new_manager->team[new_manager->direct_employees_no] = angajat_mutat; //Introducem angajatul in noua echipa
                new_manager->direct_employees_no++;                                  //Marim dimensiunea echipei
                angajat_mutat->manager = new_manager;                                //Restabilim legatura la noul manager a angajatului mutat
                return tree;
            }
        }
    }
    return tree;
}

/* Concediaza o echipa din ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului din varful echipei concediate
 *
 * return: intoarce ierarhia modificata.
 */
Tree fire_team(Tree tree, char *employee_name)
{
    Tree mng = search_node(tree, employee_name); //Cautam pointerul al carui camp name coincide cu employee_name
    if (mng != NULL)                             //Cazul in care exista angajatul cu numele dat in ierarhie
    {
        if (mng->manager != NULL)
        {
            if (mng->manager->direct_employees_no == 1) //Cazul in care echipa managerului contine doar angajatul a carui echipa trebuie concediata
            {
                mng->manager->team = NULL;             //Echipa managerului va pointa catre NULL
                mng->manager->direct_employees_no = 0; //Dimensiunea acesteia va fi 0
                return tree;
            }
            else //Cazul in care echipa managerului contine mai multi angajati
            {
                for (int i = 0; i < mng->manager->direct_employees_no; i++) //Eliminam prin shiftare stanga angajatul a carui echipa trebuie sa o concediem
                {
                    if (strcmp(mng->manager->team[i]->name, employee_name) >= 0)
                    {
                        mng->manager->team[i] = mng->manager->team[i + 1];
                    }
                }
                mng->manager->direct_employees_no--; //Micsoram dimensiunea echipei managerului
                mng->team = NULL;                    //Echipa angajatului eliminat va fi NULL
                mng->direct_employees_no = 0;        //Dimensiunea echipei acestuia va fi 0
                return tree;
            }
        }
        else
            return tree;
    }
    return tree;
}

/* Afiseaza toti angajatii sub conducerea unui angajat.
 * 
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 * employee_name: numele angajatului din varful echipei
 */

void get_employees_by_manager(FILE *f, Tree tree, char *employee_name)
{
    Tree manager = search_node(tree, employee_name); //Cautam nodul(managerul) de unde trebuie sa facem afisarea
    if (manager == NULL)                             //Inseamna ca managerul nu se afla in ierarhie si atunci printam doar new-line
        fprintf(f, "\n");
    else
    {
        Vector v = init(2);                             //Initializam un element de tip Vector
        get_by_manager(manager, &v);                    //Aplicam functia auxiliara care ne intoarce toate numele angajatilor de sub managerul dat
        qsort(v.array, v.size, sizeof(char *), cmpstr); //Ne folosim de functia qsort pentru a ordona alfabetic numele tuturor angajatilor ce apartin de managerul dat
        for (int i = 0; i < v.size; i++)                //Afisam campul array(campul unde se afla de fapt numele angajatilor)
            fprintf(f, "%s ", v.array[i]);
        fprintf(f, "\n");
        //Eliberam memoria ocupata de campul array(doar pentru el am alocat memorie dinamic)
        for (int i = 0; i < v.size; i++)
            free(v.array[i]);
        free(v.array);
    }
}

/* Afiseaza toti angajatii de pe un nivel din ierarhie.
 * 
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 * level: nivelul din ierarhie
 */
void get_employees_by_level(FILE *f, Tree tree, int level)
{
    if (level < 0) //Inseamna ca nivelul este negativ(ceea ce nu se poate) si in acest caz printam doar new-line
        fprintf(f, "\n");
    else
    {
        //Efectuam aceleasi operatii ca la get_employees_by_manager doar ca de aceasta data difera functia care ne intoarce un vector cu numele angajatilor ce se afla doar pe un anumit nivel
        Vector v = init(2);
        get_by_level(tree, level, &v);
        qsort(v.array, v.size, sizeof(char *), cmpstr);
        for (int i = 0; i < v.size; i++)
            fprintf(f, "%s ", v.array[i]);
        fprintf(f, "\n");
        for (int i = 0; i < v.size; i++)
            free(v.array[i]);
        free(v.array);
    }
}

/* Afiseaza angajatul cu cei mai multi oameni din echipa.
 * 
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 */

int search_max(Tree tree) //Functie pentru a calcula dimensiunea maxima a unei echipe
{
    if (tree == NULL)
        return 0;
    int res = tree->direct_employees_no; //Retinem valoarea din campul direct_employees_no
    for (int i = 0; i < tree->direct_employees_no; i++)
    {
        if (tree->team[i] != NULL)
        {
            int res1 = search_max(tree->team[i]);
            if (res1 > res) //In caz ca gasim o valoare mai mare decat cea retinuta in variabila res
                res = res1;
            return res;
        }
    }
}
void get_best_manager(FILE *f, Tree tree)
{
    int max = search_max(tree); //Cautam maximul dimensiunii unei echipe in ierarhie
    Vector v = init(2);
    best_manager_function(tree, max, &v);           //Apelam functia definita pentru a ne selecta numele tuturor angajatilor al caror camp direct_employees_no coincide cu maximul calculat
    qsort(v.array, v.size, sizeof(char *), cmpstr); //Sortam campul array al structurii de tip vector
    for (int i = 0; i < v.size; i++)                //Afisarea numelor angajatilor
        fprintf(f, "%s ", v.array[i]);
    fprintf(f, "\n");
    //Eliberam memorie ocupata de array
    for (int i = 0; i < v.size; i++)
        free(v.array[i]);
    free(v.array);
}

/* Reorganizarea ierarhiei cu un alt angajat in varful ierarhiei.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului care trece in varful ierarhiei
 */

void reorganizare_aux(Tree tree, Tree mng)
{
    if (tree->manager == NULL) //Cazul de baza(ne oprim atunci cand ajungem cu tree->manager==NULL, adica la nodul radacina)
        return;
    reorganizare_aux(tree->manager, tree);                         //Aplicam functia recursiv(pentru un nod mai sus in ierarhie care va avea drept parinte nodul curent-managerul va fi sub conducerea angajatului sau)
    for (int i = 0; i < tree->manager->direct_employees_no; i++)   //Stergem angajatul curent din echipa managerului sau
        if (strcmp(tree->manager->team[i]->name, tree->name) >= 0) //Shiftam stanga pentru a sterge angajatul curent din echipa managerului
            tree->manager->team[i] = tree->manager->team[i + 1];
    tree->manager->direct_employees_no--; //Micsoram dimensiunea echipei managerului deoarece am eliminat angajatul curent din ea
    //In continuare introducem managerul angajatului curent in echipa angajatului curent
    if (tree->direct_employees_no == 0) //Cazul in care angajatul curent nu are deja echipa ceea ce inseamna ca trebuie sa alocam memorie pentru ea
    {
        tree->team = malloc(sizeof(Tree));
        tree->team[0] = tree->manager; //Introducem managerul in echipa
        tree->direct_employees_no++;
    }
    else //Cazul in care are deja echipa -> realocam, introducem pe ultima pozitie si dupa aplicam sortarea pe echipa angajatului curent
    {
        tree->team = realloc(tree->team, (tree->direct_employees_no + 1) * sizeof(Tree));
        tree->team[tree->direct_employees_no] = tree->manager;
        tree->direct_employees_no++;
        tree = sorteaza_echipa(tree);
    }
    tree->manager->manager = tree; //Modificam legatura manager a managerului curent care va deveni fix angajatul(doarece managerul angajatului devine copilul sau)
    tree->manager = mng;           //Managerul nodului curent va devenit managerul primit ca argument in functie
}

Tree reorganize(Tree tree, char *employee_name)
{
    Tree angajat = search_node(tree, employee_name);
    if (angajat != NULL) //Cazul in care gasim angajatul in ierarhie
    {
        if (angajat->manager == NULL) //Nu putem reorganiza ierarhia pentru nodul radacina
            return tree;
        else
        {
            reorganizare_aux(angajat, NULL); //Deoarece nodul gasit va deveni radacina deci va avea ca manager NULL
            return angajat;
        }
    }
    else
        return tree;
}

/* Parcurge ierarhia conform parcurgerii preordine.s
 *
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 */
void preorder_traversal_auxiliar(FILE *f, Tree tree)
{
    if (tree != NULL)
    {
        if (tree != NULL && tree->manager == NULL) //Verificam daca nodul pe care il afisam este radacina
        {
            fprintf(f, "%s ", tree->name);
        }
        else if (tree != NULL && tree->manager != NULL) //Inseamna ca nodul nu este radacina si trebuie sa facem afisare de tip nod-manager
        {
            fprintf(f, "%s-%s ", tree->name, tree->manager->name);
        }
        for (int i = 0; i < tree->direct_employees_no; i++) //Avansam in arbore
            preorder_traversal_auxiliar(f, tree->team[i]);
    }
}
void preorder_traversal(FILE *f, Tree tree)
{
    preorder_traversal_auxiliar(f, tree); //A fost nevoie de functie auxiliara din cauza ca trebuia printat un new_line dupa fiecare apel
    fprintf(f, "\n");
}

/* Elibereaza memoria alocata nodurilor din arbore
 *
 * tree: ierarhia existenta
 */
void destroy_tree(Tree tree)
{
    return;
}
