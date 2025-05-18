#include <stdio.h>
#include <stdlib.h>

typedef struct NO
{
    int value;
    struct NO *prox;
} NO;

void reverse_print(NO *node)
{
    if (node == NULL)
    {
        return;
    }
    reverse_print(node->prox);
    printf("%d\n", node->value);
}

NO *constructor(int value, NO *prox)
{
    NO *p = (NO *)malloc(sizeof(NO));
    p->value = value;
    p->prox = prox;
    return p;
}

void free_list(NO *node)
{
    NO *current = node;
    while (current != NULL)
    {
        NO *next = current->prox;
        free(current);
        current = next;
    }
}

int main()
{
    NO *p1 = constructor(5, NULL);
    NO *p2 = constructor(4, p1);
    NO *p3 = constructor(3, p2);
    NO *p4 = constructor(2, p3);
    NO *p5 = constructor(1, p4);

    reverse_print(p5);

    free_list(p5);

    return 0;
}