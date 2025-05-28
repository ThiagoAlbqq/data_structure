#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DESC 100

typedef enum
{
    PARAFINA,
    LEASH,
    QUILHAS,
    DECKS_ANTIDERRAPANTES,
    NUM_PRODUCTS
} products_enum;

const int products_limits[NUM_PRODUCTS] = {
    [PARAFINA] = 50,
    [LEASH] = 25,
    [QUILHAS] = 10,
    [DECKS_ANTIDERRAPANTES] = 5};

typedef struct Product
{
    int cod;
    products_enum type;
    char descricao[MAX_DESC];
    float preco;

    struct Product *prox;
    struct Product *ant;
} Product;

typedef struct ProductBox
{
    products_enum type;
    int tam;
    Product *header;
    struct ProductBox *prox;
} ProductBox;

typedef struct
{
    Product *header;
    Product *tail;
    int tamanho;
} ProductLinkedList;

products_enum parse_type(const char *str)
{
    if (strcmp(str, "PARAFINA") == 0)
        return PARAFINA;
    if (strcmp(str, "LEASH") == 0)
        return LEASH;
    if (strcmp(str, "QUILHAS") == 0)
        return QUILHAS;
    if (strcmp(str, "DECKS_ANTIDERRAPANTES") == 0)
        return DECKS_ANTIDERRAPANTES;
    return -1; // erro
}

int carregar_csv(const char *nome_arquivo, ProductLinkedList *lista);
void inserir_ordenado(ProductLinkedList *lista, Product *novo);
void inserir_na_caixa(ProductBox *caixa, Product *novo);

void adicionar_produto_na_lista(ProductLinkedList *lista, int cod, products_enum tipo, const char *desc, float preco)
{
    Product *novo = malloc(sizeof(Product));
    novo->cod = cod;
    novo->type = tipo;
    strncpy(novo->descricao, desc, MAX_DESC);
    novo->preco = preco;
    novo->prox = NULL;
    novo->ant = NULL;

    inserir_ordenado(lista, novo);
    lista->tamanho++;
}

void inserir_ordenado(ProductLinkedList *lista, Product *novo)
{
    if (!lista->header)
    {
        // Lista vazia
        lista->header = lista->tail = novo;
        return;
    }

    Product *atual = lista->header;

    // Encontrar posição correta
    while (atual && atual->preco < novo->preco)
    {
        atual = atual->prox;
    }

    if (!atual)
    {
        // Inserir no final
        novo->ant = lista->tail;
        lista->tail->prox = novo;
        lista->tail = novo;
    }
    else if (atual == lista->header)
    {
        // Inserir no início
        novo->prox = atual;
        atual->ant = novo;
        lista->header = novo;
    }
    else
    {
        // Inserir no meio
        novo->prox = atual;
        novo->ant = atual->ant;
        atual->ant->prox = novo;
        atual->ant = novo;
    }
}

void inserir_na_caixa(ProductBox *caixa, Product *novo)
{
    if (caixa->type != novo->type)
        return;

    if (caixa->tam >= products_limits[caixa->type])
    {
        // Cria nova caixa se necessário
        if (caixa->prox == NULL)
        {
            caixa->prox = malloc(sizeof(ProductBox));
            caixa->prox->type = caixa->type;
            caixa->prox->tam = 0;
            caixa->prox->header = NULL;
            caixa->prox->prox = NULL;
        }
        inserir_na_caixa(caixa->prox, novo);
        return;
    }

    // Inserir na caixa atual
    Product *novoProd = malloc(sizeof(Product));
    *novoProd = *novo; // Copia os dados
    novoProd->prox = caixa->header;
    caixa->header = novoProd;
    caixa->tam++;
}

int carregar_csv(const char *nome_arquivo, ProductLinkedList *lista)
{
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo)
    {
        perror("Erro ao abrir o arquivo");
        return -1;
    }

    char linha[256];
    fgets(linha, sizeof(linha), arquivo); // pula cabeçalho

    while (fgets(linha, sizeof(linha), arquivo))
    {
        linha[strcspn(linha, "\n")] = 0;

        char *token = strtok(linha, ";");
        int cod = token ? atoi(token) : -1;

        token = strtok(NULL, ";");
        products_enum tipo = token ? parse_type(token) : -1;

        token = strtok(NULL, ";");
        char descricao[MAX_DESC] = "";
        if (token)
            strncpy(descricao, token, MAX_DESC);

        token = strtok(NULL, ";");
        float preco = token ? atof(token) : 0;

        if (tipo != -1)
        {
            adicionar_produto_na_lista(lista, cod, tipo, descricao, preco);
        }
    }

    fclose(arquivo);
    return lista->tamanho;
}

void imprimir_caixa(ProductBox *caixa)
{
    printf("Caixa de tipo %d (%d/%d itens):\n", caixa->type, caixa->tam, products_limits[caixa->type]);
    Product *atual = caixa->header;
    while (atual)
    {
        printf("  COD: %d, DESC: %s, PREÇO: %.2f\n", atual->cod, atual->descricao, atual->preco);
        atual = atual->prox;
    }
    if (caixa->prox)
    {
        imprimir_caixa(caixa->prox);
    }
}

int main()
{
    ProductLinkedList listaProdutos = {NULL, NULL, 0};

    // Inicializa caixas para cada tipo de produto
    ProductBox parafina = {PARAFINA, 0, NULL, NULL};
    ProductBox leash = {LEASH, 0, NULL, NULL};
    ProductBox quilhas = {QUILHAS, 0, NULL, NULL};
    ProductBox decks_antiderrapantes = {DECKS_ANTIDERRAPANTES, 0, NULL, NULL};

    int total = carregar_csv("products.csv", &listaProdutos);
    printf("Produtos carregados: %d\n\n", total);

    // Distribui os produtos nas caixas apropriadas
    Product *atual = listaProdutos.header;
    while (atual)
    {
        switch (atual->type)
        {
        case PARAFINA:
            inserir_na_caixa(&parafina, atual);
            break;
        case LEASH:
            inserir_na_caixa(&leash, atual);
            break;
        case QUILHAS:
            inserir_na_caixa(&quilhas, atual);
            break;
        case DECKS_ANTIDERRAPANTES:
            inserir_na_caixa(&decks_antiderrapantes, atual);
            break;
        }
        atual = atual->prox;
    }

    // Imprime as caixas
    printf("\nCaixas de Parafina:\n");
    imprimir_caixa(&parafina);

    printf("\nCaixas de Leash:\n");
    imprimir_caixa(&leash);

    printf("\nCaixas de Quilhas:\n");
    imprimir_caixa(&quilhas);

    printf("\nCaixas de Decks Antiderrapantes:\n");
    imprimir_caixa(&decks_antiderrapantes);

    return 0;
}