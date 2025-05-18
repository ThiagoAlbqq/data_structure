#include <stdio.h>
#include <stdlib.h>

typedef struct No
{
    int value;
    struct No *prox;
} No;

No *header;
No *tail;
int tam;

void add_node(int value, int pos);
void print_newList(No *header);
void printList_recursive(No *header);
int find(No *header);

int main()
{
    add_node(10, 0);
    add_node(10, 0);
    // add_node(30, 1);
    // add_node(40, 3);

    int search = find(header);

    if (search != -1)
    {
        printf("%d", search);
    }
    else
    {
        printf("Valor não é duplicado!");
    }
    return 0;
}

void print_newList(No *node)
{
    if (node == NULL)
        printf("LISTA VAZIA!");
    else
    {
        printf("LISTA ATUALIZADA: ");
        printList_recursive(node);
    }
}

void printList_recursive(No *node)
{
    if (node->prox == NULL)
    {
        printf("\n");
        return;
    }
    printf("%d", node->value);
    if (node->prox->prox != NULL)
    {
        printf(" -> ");
    }
    printList_recursive(node->prox);
}

void add_node(int value, int pos)
{
    No *newNode = malloc(sizeof(No));
    newNode->value = value;
    newNode->prox = NULL;

    if (pos > tam)
    {
        return;
    }
    else
    {
        if (tam == 0)
        {
            header = newNode;
            tail = newNode;
        }
        else
        {
            if (pos == tam)
            {
                // Adicionar no final
                tail->prox = newNode;
                tail = newNode;
            }
            else if (pos == 0)
            {
                // Adicionar no inicio
                newNode->prox = header;
                header = newNode;
            }
            else
            {
                // Adicionar no meio
                No *aux = header;
                for (int i = 0; i < pos - 1; i++)
                {
                    aux = aux->prox;
                }
                newNode->prox = aux->prox;
                aux->prox = newNode;
            }
        }
        tam++;
    }

    int soma = 0;
    No *aux = header;
    for (int i = 0; i < tam; i++)
    {
        soma += aux->value;
        aux = aux->prox;
    }

    tail->value = soma / tam;

    printf("TAIL: %d\n", tail->value);

    print_newList(header);
}

int find(No *header)
{
    No *aux = header;
    for (int i = 0; i < tam - 1; i++)
    {
        if (aux->value == tail->value)
            return tail->value;
        aux = aux->prox;
    }

    return -1;
}