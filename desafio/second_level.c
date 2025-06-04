#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_DESC 100 // Qnt max de caracteres em descricao

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

typedef struct
{
    Product *header;
    Product *tail;
    int tamanho;
} ProductLinkedList;

typedef struct ProductBoxItem
{
    int product_cod;
    // Só estou armazenando o codigo, isso é correto?
    struct ProductBoxItem *prox;
} ProductBoxItem;

typedef struct ProductBox
{
    int cod_caixa;
    products_enum type;
    int tam;
    ProductBoxItem *items_header;
    struct ProductBox *prox;
} ProductBox;

int g_next_box_code = 1;

products_enum parse_type(const char *str);
Product *adicionar_produto_na_lista(ProductLinkedList *lista, int cod, products_enum tipo, const char *desc, float preco);
void inserir_ordenado(ProductLinkedList *lista, Product *novo);
void adicionar_produto_ao_estoque(ProductBox **pilha_topo_ref, products_enum tipo_produto, int cod_produto);
int carregar_csv(const char *nome_arquivo, ProductLinkedList *lista,
                 ProductBox **parafina_stack_top_ref,
                 ProductBox **leash_stack_top_ref,
                 ProductBox **quilhas_stack_top_ref,
                 ProductBox **decks_antiderrapantes_stack_top_ref);
void imprimir_lista_produtos(ProductLinkedList *lista, char *tipo, float max, float min);
void imprimir_pilha_de_caixas(ProductBox *pilha_topo, const char *nome_tipo_produto);
void liberar_lista_produtos(ProductLinkedList *lista);
void liberar_pilha_de_caixas(ProductBox **pilha_topo_ref);

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
    printf("AVISO: Tipo de produto desconhecido '%s'\n", str);
    return (products_enum)-1; // Retorna um valor inválido para indicar erro
}

void inserir_ordenado(ProductLinkedList *lista, Product *novo)
{
    Product *atual_node;

    novo->prox = NULL;
    novo->ant = NULL;

    if (lista->header == NULL)
    {
        lista->header = novo;
        lista->tail = novo;
    }
    else
    {
        atual_node = lista->header;
        while (atual_node != NULL && novo->preco >= atual_node->preco)
        {
            atual_node = atual_node->prox;
        }
        if (atual_node == NULL)
        {
            // Inserir no final
            novo->ant = lista->tail;
            lista->tail->prox = novo;
            lista->tail = novo;
        }
        else if (atual_node->ant == NULL)
        {
            // Inserir no início
            novo->prox = lista->header;
            lista->header->ant = novo;
            lista->header = novo;
        }
        else
        {
            // Inserir no meio
            novo->prox = atual_node;
            novo->ant = atual_node->ant;
            atual_node->ant->prox = novo;
            atual_node->ant = novo;
        }
    }
}

Product *adicionar_produto_na_lista(ProductLinkedList *lista, int cod, products_enum tipo, const char *desc, float preco)
{
    Product *novo = malloc(sizeof(Product));
    if (!novo)
    {
        printf("Erro ao alocar memoria para produto na lista");
        exit(1);
    }
    novo->cod = cod;
    novo->type = tipo;
    strncpy(novo->descricao, desc, MAX_DESC - 1);
    novo->descricao[MAX_DESC - 1] = '\0';
    novo->preco = preco;
    inserir_ordenado(lista, novo);
    lista->tamanho++;
    return novo;
}

void adicionar_produto_ao_estoque(ProductBox **pilha_topo_ref, products_enum tipo_produto, int cod_produto)
{
    ProductBox *caixa_do_topo = *pilha_topo_ref;
    if (caixa_do_topo == NULL || caixa_do_topo->tam >= products_limits[tipo_produto])
    {
        ProductBox *nova_caixa = malloc(sizeof(ProductBox));
        if (!nova_caixa)
        {
            printf("Erro ao alocar memoria para nova caixa de estoque");
            exit(1);
        }

        nova_caixa->cod_caixa = g_next_box_code++;
        nova_caixa->type = tipo_produto;
        nova_caixa->tam = 0;
        nova_caixa->items_header = NULL;
        nova_caixa->prox = caixa_do_topo;

        *pilha_topo_ref = nova_caixa;
        caixa_do_topo = nova_caixa;
    }

    ProductBoxItem *novo_item = malloc(sizeof(ProductBoxItem));
    if (!novo_item)
    {
        printf("Erro ao alocar memoria para nova caixa de estoque");
        exit(1);
    }
    novo_item->product_cod = cod_produto;
    novo_item->prox = caixa_do_topo->items_header;
    caixa_do_topo->items_header = novo_item;
    caixa_do_topo->tam++;
}

int carregar_csv(const char *nome_arquivo, ProductLinkedList *lista,
                 ProductBox **parafina_stack_top_ref,
                 ProductBox **leash_stack_top_ref,
                 ProductBox **quilhas_stack_top_ref,
                 ProductBox **decks_antiderrapantes_stack_top_ref)
{

    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo)
    {
        printf("Erro ao abrir o arquivo CSV");
        return -1;
    }

    char linha[256];
    if (fgets(linha, sizeof(linha), arquivo) == NULL && !feof(arquivo))
    {
        printf("Erro ao ler o cabeçalho do CSV");
        fclose(arquivo);
        return -1;
    }

    int produtos_carregados = 0;
    while (fgets(linha, sizeof(linha), arquivo))
    {
        linha[strcspn(linha, "\n\r")] = 0;

        char *token;
        int cod_val;
        products_enum tipo_val;
        char descricao_val[MAX_DESC];
        float preco_val;

        // COD
        token = strtok(linha, ";");
        if (!token)
            continue;
        cod_val = atoi(token);

        // TIPO
        token = strtok(NULL, ";");
        if (!token)
            continue;
        tipo_val = parse_type(token);
        if (tipo_val == (products_enum)-1)
        {
            fprintf(stderr, "AVISO: Produto com codigo %d ignorado devido a tipo invalido.\n", cod_val);
            continue;
        }

        // DESCRICAO
        token = strtok(NULL, ";");
        if (!token)
            continue;
        strncpy(descricao_val, token, MAX_DESC - 1);
        descricao_val[MAX_DESC - 1] = '\0';

        // PRECO
        token = strtok(NULL, ";");
        if (!token)
            continue;
        preco_val = atof(token);

        Product *produto_adicionado = adicionar_produto_na_lista(lista, cod_val, tipo_val, descricao_val, preco_val);

        if (produto_adicionado)
        {
            produtos_carregados++;
            switch (tipo_val)
            {
            case PARAFINA:
                adicionar_produto_ao_estoque(parafina_stack_top_ref, tipo_val, produto_adicionado->cod);
                break;
            case LEASH:
                adicionar_produto_ao_estoque(leash_stack_top_ref, tipo_val, produto_adicionado->cod);
                break;
            case QUILHAS:
                adicionar_produto_ao_estoque(quilhas_stack_top_ref, tipo_val, produto_adicionado->cod);
                break;
            case DECKS_ANTIDERRAPANTES:
                adicionar_produto_ao_estoque(decks_antiderrapantes_stack_top_ref, tipo_val, produto_adicionado->cod);
                break;
            default:
                printf("ERRO INTERNO: Tipo de produto inesperado %d\n", tipo_val);
                break;
            }
        }
    }

    fclose(arquivo);
    return produtos_carregados;
}

// Como C não tem suporte para parametros opcionais, iremos adicionar um parametro e se for NULL imprime tudo
void imprimir_lista_produtos(ProductLinkedList *lista, char *tipo, float max, float min)
{
    int type_enum = tipo != NULL ? parse_type(tipo) : -1;
    printf("\n--- Lista de Produtos Disponiveis (Ordenada por Preco) ---\n");
    if (!lista->header)
    {
        printf("  Nenhum produto na lista.\n");
        return;
    }
    Product *atual = lista->header;
    int count = 1;
    while (atual != NULL)
    {
        // Se eu passar o tipo, só será verdadeiro se o tipo do atual for igual ao que eu passei
        int passa_tipo = (type_enum == -1) || (atual->type == type_enum);
        // Se eu passar o max, só será verdadeiro se o preco do atual for igual ao que eu passei
        int passa_max = (max < 0) || (atual->preco <= max);
        // Se eu passar o min, só será verdadeiro se o preco do atual for igual ao que eu passei
        int passa_min = (min < 0) || (atual->preco >= min);
        if (passa_tipo && passa_max && passa_min)
        {
            printf("  %d. COD: %d, TIPO: %d, DESC: %s, PRECO: %.2f\n",
                   count++, atual->cod, atual->type, atual->descricao, atual->preco);
        }
        atual = atual->prox;
    }
    printf("---------------------------------------------------------\n");
}

void imprimir_pilha_de_caixas(ProductBox *pilha_topo, const char *nome_tipo_produto)
{
    printf("\n--- Caixas de %s (Pilha de Estoque) ---\n", nome_tipo_produto);
    if (!pilha_topo)
    {
        printf("  Nenhuma caixa no estoque para %s.\n", nome_tipo_produto);
        printf("---------------------------------------------------------\n");
        return;
    }

    ProductBox *caixa_atual = pilha_topo;
    int num_caixa_impressao = 1;
    while (caixa_atual != NULL)
    {
        printf("  Caixa #%d (Codigo Real: %d, Tipo Enum: %d) [%d/%d itens]:\n",
               num_caixa_impressao++, caixa_atual->cod_caixa, caixa_atual->type,
               caixa_atual->tam, products_limits[caixa_atual->type]);

        ProductBoxItem *item_atual = caixa_atual->items_header;
        if (!item_atual)
        {
            printf("    -> Caixa vazia.\n");
        }
        else
        {
            int item_count = 1;
            while (item_atual != NULL)
            {
                printf("    -> Item %d: COD Produto: %d\n", item_count++, item_atual->product_cod);
                item_atual = item_atual->prox;
            }
        }
        caixa_atual = caixa_atual->prox;
    }
    printf("---------------------------------------------------------\n");
}

void liberar_lista_produtos(ProductLinkedList *lista)
{
    Product *atual = lista->header;
    Product *proximo_produto;
    while (atual != NULL)
    {
        proximo_produto = atual->prox;
        free(atual);
        atual = proximo_produto;
    }
    lista->header = NULL;
    lista->tail = NULL;
    lista->tamanho = 0;
}

void liberar_pilha_de_caixas(ProductBox **pilha_topo_ref)
{
    ProductBox *caixa_atual = *pilha_topo_ref;
    ProductBox *proxima_caixa;
    while (caixa_atual != NULL)
    {
        proxima_caixa = caixa_atual->prox;

        ProductBoxItem *item_atual = caixa_atual->items_header;
        ProductBoxItem *proximo_item;
        while (item_atual != NULL)
        {
            proximo_item = item_atual->prox;
            free(item_atual);
            item_atual = proximo_item;
        }

        free(caixa_atual);
        caixa_atual = proxima_caixa;
    }
    *pilha_topo_ref = NULL;
}

int main()
{
    ProductLinkedList listaProdutos = {NULL, NULL, 0};

    ProductBox *parafina_stack_top = NULL;
    ProductBox *leash_stack_top = NULL;
    ProductBox *quilhas_stack_top = NULL;
    ProductBox *decks_antiderrapantes_stack_top = NULL;

    int total_carregado = carregar_csv("products.csv", &listaProdutos,
                                       &parafina_stack_top, &leash_stack_top,
                                       &quilhas_stack_top, &decks_antiderrapantes_stack_top);

    if (total_carregado >= 0)
    {
        printf("Total de produtos carregados do CSV: %d\n", total_carregado);

        imprimir_lista_produtos(&listaProdutos, NULL, 200.0f, 100.0f);
        imprimir_pilha_de_caixas(parafina_stack_top, "Parafina");
        imprimir_pilha_de_caixas(leash_stack_top, "Leash");
        imprimir_pilha_de_caixas(quilhas_stack_top, "Quilhas");
        imprimir_pilha_de_caixas(decks_antiderrapantes_stack_top, "Decks Antiderrapantes");
    }
    else
    {
        printf("Falha ao carregar produtos do arquivo CSV.\n");
    }

    liberar_lista_produtos(&listaProdutos);
    liberar_pilha_de_caixas(&parafina_stack_top);
    liberar_pilha_de_caixas(&leash_stack_top);
    liberar_pilha_de_caixas(&quilhas_stack_top);
    liberar_pilha_de_caixas(&decks_antiderrapantes_stack_top);

    printf("\nMemoria liberada. Programa encerrado.\n");

    return 0;
}