#include <stdio.h>
#include <stdlib.h>

typedef struct NO
{
    int value;
    struct NO *prox;
} NO;

float q2(NO *aux)
{
    float soma = 0;
    while (aux != NULL)
    {
        if (aux->value % 2 == 0)
            soma += aux->value;
        aux = aux->prox;
    }

    return soma * soma;
}

NO *constructor(int value, NO *prox)
{
    NO *p = (NO *)malloc(sizeof(NO));
    p->value = value;
    p->prox = prox;
    return p;
}

int main()
{
    NO *p1 = constructor(5, NULL);
    NO *p2 = constructor(4, p1);
    NO *p3 = constructor(3, p2);
    NO *p4 = constructor(2, p3);
    NO *p5 = constructor(1, p4);

    printf("%.2f", q2(p5));

    return 0;
}