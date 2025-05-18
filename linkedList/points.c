#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALLOW_OVERFLOW 1
#define STRICT_MODE 0

typedef struct point
{
    int x, y;
    struct point *next;
} point;

typedef struct linkedList
{
    point *head;
    point *tail;
    int tam;
} linkedList;

point *create_point(int x, int y);
point *append_list(linkedList *list, int x, int y);
point *insert(linkedList *list, int index, int x, int y, int mode);
int contain(linkedList *list, point *p);
int find(linkedList *list, point *p);
void remove_at(linkedList *list, int index);

void print_list(const linkedList *list);
void free_list(linkedList *list);

int main(int argc, char *argv[])
{
    linkedList points = {NULL, NULL, 0};

    int mode = STRICT_MODE;

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-p") == 0)
        {
            mode = ALLOW_OVERFLOW;
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            mode = STRICT_MODE;
        }
        else
        {
            fprintf(stderr, "Uso: %s [-p | -s]\n", argv[0]);
            fprintf(stderr, "  -p : modo permissivo\n");
            fprintf(stderr, "  -s : modo estrito (padrão)\n");
            return 1;
        }
    }

    append_list(&points, 1, 2);
    append_list(&points, 3, 2);
    append_list(&points, 4, 7);
    insert(&points, 10, 5, 0, mode);

    print_list(&points);

    point *test = create_point(3, 2);
    printf("%d\n", contain(&points, test));
    printf("%d\n", find(&points, test));

    remove_at(&points, 0);
    print_list(&points);

    free_list(&points);
    return 0;
}

point *create_point(int x, int y)
{
    point *p = malloc(sizeof(point));
    if (p == NULL)
    {
        fprintf(stderr, "Erro ao alocar memória!\n");
        exit(EXIT_FAILURE);
    }
    p->x = x;
    p->y = y;
    p->next = NULL;
    return p;
}

point *append_list(linkedList *list, int x, int y)
{
    point *p = create_point(x, y);

    if (list->head == NULL)
    {
        list->head = p;
        list->tail = p;
    }
    else
    {
        list->tail->next = p;
        list->tail = p;
    }

    list->tam += 1;
    return p;
}

point *insert(linkedList *list, int index, int x, int y, int flag)
{

    if (index < 0 || (index > list->tam && flag == STRICT_MODE))
    {
        printf("Invalid Index\n");
        return NULL;
    }

    if (index > list->tam)
    {
        return append_list(list, x, y);
    }

    point *p = create_point(x, y);
    if (index == 0)
    {
        p->next = list->head;
        list->head = p;
        if (list->tam == 0)
            list->tail = p;
    }
    else if (index == list->tam)
    {
        list->tail->next = p;
        list->tail = p;
    }
    else
    {
        point *current = list->head;
        for (int i = 0; i < index - 1; i++)
            current = current->next;

        p->next = current->next;
        current->next = p;
    }
    list->tam += 1;
    return p;
}

int contain(linkedList *list, point *p)
{
    point *current = list->head;
    while (current->next != NULL)
    {
        if (current->x == p->x && current->y == p->y)
        {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

int find(linkedList *list, point *p)
{
    point *current = list->head;
    int index = 0;

    while (current != NULL)
    {
        if (current->x == p->x && current->y == p->y)
        {
            return index;
        }
        current = current->next;
        index++;
    }
    return -1;
}

void remove_at(linkedList *list, int index)
{

    if (index < 0 || index >= list->tam || list->head == NULL)
    {
        printf("Índice inválido\n");
        return;
    }

    point *to_remove;

    if (index == 0)
    {
        to_remove = list->head;
        list->head = to_remove->next;
        if (list->tam == 1)
        {
            list->tail = NULL;
        }
    }
    else
    {
        point *current = list->head;
        for (int i = 0; i < index - 1; i++)
            current = current->next;

        to_remove = current->next;
        current->next = to_remove->next;
        if (to_remove == list->tail)
        {
            list->tail = current;
        }
    }

    free(to_remove);
    list->tam--;
}

void print_list(const linkedList *list)
{
    point *current = list->head;
    while (current != NULL)
    {
        printf("(%d, %d)\n", current->x, current->y);
        current = current->next;
    }
}

void free_list(linkedList *list)
{
    point *current = list->head;
    while (current != NULL)
    {
        point *temp = current;
        current = temp->next;
        free(temp);
    }
    list->head = NULL;
    list->tail = NULL;
    list->tam = 0;
}
