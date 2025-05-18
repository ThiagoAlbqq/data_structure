#include <stdio.h>
#include <stdlib.h>

typedef struct NO
{
    char *nome;
    int salgado;
    int suco;
    int cafe;
    int brownie;
    float preco;

    struct NO *prox;
} NO;

NO *inicio = NULL;
NO *tail = NULL;
int tam = 0;

void add(char *nome, int salgado, int suco, int cafe, int brownie, int pos)
{
    if (pos >= 0 && pos <= tam)
    {
        NO *novo = malloc(sizeof(NO));
        novo->nome = nome;
        novo->salgado = salgado;
        novo->suco = suco;
        novo->cafe = cafe;
        novo->brownie = brownie;
        novo->preco = 0.0;
        novo->prox = NULL;

        if (salgado > 0)
        {
            novo->preco += 5 * salgado;
        }
        if (suco > 0)
        {
            novo->preco += 3 * suco;
        }
        if (cafe > 0)
        {
            novo->preco += 1.25 * cafe;
        }
        if (brownie > 0)
        {
            novo->preco += 6 * brownie;
        }

        if (inicio == NULL)
        {
            inicio = novo;
            tail = novo;
        }
        else
        {
            if (pos == 0)
            {
                novo->prox = inicio;
                inicio = novo;
            }
            else if (pos == tam)
            {
                tail->prox = novo;
                tail = novo;
            }
            else
            {
                NO *aux = inicio;
                for (int i = 0; i < pos - 1; i++)
                {
                    aux = aux->prox;
                }
                aux->prox = novo;
            }
        }
        tam++;
    }
    else
    {
        printf("Posicao invalida! \n");
    }
}

void imprimir()
{
    NO *aux = inicio;
    for (int i = 0; i < tam; i++)
    {

        printf("Pedido do(a) %s:\n", aux->nome);
        printf("Qtd de salgados: %i\n", aux->salgado);
        printf("Qtd de sucos: %i\n", aux->suco);
        printf("Qtd de cafes: %i\n", aux->cafe);
        printf("Qtd de brownies: %i\n", aux->brownie);
        printf("TOTAL: R$%f\n", aux->preco);

        aux = aux->prox;
    }
}

int main()
{

    add("Eduarda", 1, 1, 0, 1, 0);
    add("Yasmin", 0, 2, 0, 0, 0);
    add("Andre", 1, 1, 0, 0, 0);
    add("Pedro", 3, 1, 0, 1, 3);
    add("Wesley", 1, 0, 1, 0, 4);
    add("Thiago", 1, 0, 1, 0, 1);
    imprimir();
}
