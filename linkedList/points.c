#include <stdio.h>
#include <stdlib.h>

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
point *insert(linkedList *list, int index, int x, int y);
void print_list(const linkedList *list);
void free_list(linkedList *list);

int main()
{
    linkedList points = {NULL, NULL, 0};
    append_list(&points, 1, 2);
    append_list(&points, 3, 2);
    append_list(&points, 4, 7);
    insert(&points, 10, 5, 0);
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

point *insert(linkedList *list, int index, int x, int y)
{

    if (index < 0 || index > list->tam)
    {
        printf("Invalid Index\n");
        return NULL;
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
        int i = 0;
        for (int i = 0; i < index - 1; i++)
            current = current->next;

        p->next = current->next;
        current->next = p;

        if (p->next == NULL)
            list->tail = p;
    }
    list->tam += 1;
    return p;
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
