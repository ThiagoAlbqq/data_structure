#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DESC 100 // Qnt max de caracteres em descricao

// Tipos de produtos
typedef enum {
  PARAFINA,
  LEASH,
  QUILHAS,
  DECKS_ANTIDERRAPANTES,
  NUM_PRODUCTS
} products_enum;

// Array com o nome dos produtos
const char *products_names[NUM_PRODUCTS] = {"PARAFINA", "LEASH", "QUILHAS",
                                            "DECKS_ANTIDERRAPANTES"};

// Array com os limites de cada produto
const int products_limits[NUM_PRODUCTS] = {
    [PARAFINA] = 50, [LEASH] = 25, [QUILHAS] = 10, [DECKS_ANTIDERRAPANTES] = 5};

// Lista duplamente encadeada de produtos
typedef struct Product {
  int cod;
  products_enum type;
  char descricao[MAX_DESC];
  float preco;
  int cod_caixa;

  struct Product *prox;
  struct Product *ant;
} Product;

// STRUCT - LinkedList
typedef struct {
  Product *header;
  Product *tail;
  int tamanho;
} ProductLinkedList;

// Lista encadeada dos produtos da caixa
typedef struct ProductBoxItem {
  int product_cod;
  struct ProductBoxItem *prox;
} ProductBoxItem;

// STRUCT - Caixa
typedef struct ProductBox {
  int cod_caixa;
  products_enum type;
  int tam;
  ProductBoxItem *items_header;
  struct ProductBox *prox;
} ProductBox;

// Variavel para criar o cod da caixa (iniciando de 1)
int g_next_box_code = 1;

// Declaração das funções
products_enum parse_type(const char *str);
Product *adicionar_produto_na_lista(ProductLinkedList *lista,
                                    ProductBox *pilhas[], int cod,
                                    products_enum tipo, const char *desc,
                                    float preco);
void inserir_ordenado(ProductLinkedList *lista, Product *novo);
ProductBox *adicionar_produto_ao_estoque(ProductBox *pilha_topo,
                                         products_enum tipo_produto,
                                         int cod_produto, Product *produto);
int carregar_csv(const char *nome_arquivo, ProductLinkedList *lista,
                 ProductBox *pilhas[]);
void imprimir_lista_produtos(ProductLinkedList *lista);
void imprimir_pilha_de_caixas(ProductBox *pilha_topo,
                              const char *nome_tipo_produto);
void liberar_lista_produtos(ProductLinkedList *lista);
ProductBox *liberar_pilha_de_caixas(ProductBox *pilha_topo);

products_enum parse_type(const char *str) {
  if (strcmp(str, "PARAFINA") == 0)
    return PARAFINA;
  if (strcmp(str, "LEASH") == 0)
    return LEASH;
  if (strcmp(str, "QUILHAS") == 0)
    return QUILHAS;
  if (strcmp(str, "DECKS_ANTIDERRAPANTES") == 0)
    return DECKS_ANTIDERRAPANTES;
  printf("AVISO: Tipo de produto desconhecido '%s'\n", str);
  return (products_enum)-1;
}

// Função para inserir o produto na lista de forma ordenada por preço
// (crescente)
// De inicio fiz algo ruim (estava percorrendo do inicio até o final, sendo que
// se o valor do final fosse menor do que o do que eu estou criando, eu poderia
// só chamar o adicionar no final)
void inserir_ordenado(ProductLinkedList *lista, Product *novo) {
  Product *atual_node;

  novo->prox = NULL;
  novo->ant = NULL;

  if (lista->header == NULL) {
    // Lista vazia
    lista->header = novo;
    lista->tail = novo;
  } else if (lista->header->preco > novo->preco) {
    // Inserção no início da lista
    novo->prox = lista->header;
    lista->header->ant = novo;
    lista->header = novo;
  } else if (lista->tail->preco <= novo->preco) {
    // Inserção no final da lista
    novo->ant = lista->tail;
    lista->tail->prox = novo;
    lista->tail = novo;
  } else {
    atual_node = lista->header;
    while (atual_node != NULL && novo->preco >= atual_node->preco) {
      atual_node = atual_node->prox;
    }
    // Inserção no meio da lista
    novo->prox = atual_node;
    novo->ant = atual_node->ant;
    atual_node->ant->prox = novo;
    atual_node->ant = novo;
  }
}

Product *adicionar_produto_na_lista(ProductLinkedList *lista,
                                    ProductBox *pilhas[], int cod,
                                    products_enum tipo, const char *desc,
                                    float preco) {
  Product *novo = malloc(sizeof(Product));
  // Apenas um caso de boas praticas
  if (!novo) {
    printf("Erro ao alocar memoria para produto na lista");
    exit(1);
  }
  novo->cod = cod;
  novo->type = tipo;
  strncpy(novo->descricao, desc, MAX_DESC - 1);
  novo->descricao[MAX_DESC - 1] = '\0';
  novo->preco = preco;

  // A ideia aqui é chamr o inserir_ordenado e modificar a lista inserindo o
  // novo
  inserir_ordenado(lista, novo);

  // Modificar a pilha com o novo produto (e já cria uma caixa se precisar)
  pilhas[tipo] = adicionar_produto_ao_estoque(pilhas[tipo], tipo, cod, novo);

  lista->tamanho++;
  return novo;
}

ProductBox *adicionar_produto_ao_estoque(ProductBox *pilha_topo,
                                         products_enum tipo_produto,
                                         int cod_produto, Product *produto) {
  if (pilha_topo == NULL || pilha_topo->tam >= products_limits[tipo_produto]) {
    // Caso a pilha esteja vazia ou a caixa no topo esteja cheia, cria nova
    // caixa
    ProductBox *nova_caixa = malloc(sizeof(ProductBox));
    if (!nova_caixa) {
      printf("Erro ao alocar memoria para nova caixa de estoque");
      exit(1);
    }

    nova_caixa->cod_caixa = g_next_box_code++;
    nova_caixa->type = tipo_produto;
    nova_caixa->tam = 0;
    nova_caixa->items_header = NULL;
    nova_caixa->prox = pilha_topo;

    pilha_topo = nova_caixa;
  }

  ProductBoxItem *novo_item = malloc(sizeof(ProductBoxItem));
  if (!novo_item) {
    printf("Erro ao alocar memoria para nova caixa de estoque");
    exit(1);
  }

  novo_item->product_cod = cod_produto;
  novo_item->prox = pilha_topo->items_header;
  pilha_topo->items_header = novo_item;
  pilha_topo->tam++;
  produto->cod_caixa = pilha_topo->cod_caixa;

  return pilha_topo;
}

// Chegada dos 100 produtos via CSV (arquivo externo)
int carregar_csv(const char *nome_arquivo, ProductLinkedList *lista,
                 ProductBox *pilhas[]) {
  FILE *arquivo = fopen(nome_arquivo, "r");
  if (!arquivo) {
    printf("Erro ao abrir o arquivo CSV");
    return -1;
  }

  char linha[256];
  if (fgets(linha, sizeof(linha), arquivo) == NULL && !feof(arquivo)) {
    printf("Erro ao ler o cabeçalho do CSV");
    fclose(arquivo);
    return -1;
  }

  int produtos_carregados = 0;
  while (fgets(linha, sizeof(linha), arquivo)) {
    linha[strcspn(linha, "\n\r")] = 0;

    char *token;
    int cod_val;
    products_enum tipo_val;
    char descricao_val[MAX_DESC];
    float preco_val;

    token = strtok(linha, ";");
    if (!token)
      continue;
    cod_val = atoi(token);

    token = strtok(NULL, ";");
    if (!token)
      continue;
    tipo_val = parse_type(token);
    if (tipo_val == (products_enum)-1) {
      printf("AVISO: Produto com codigo %d ignorado devido a tipo invalido.\n",
             cod_val);
      continue;
    }

    token = strtok(NULL, ";");
    if (!token)
      continue;
    strncpy(descricao_val, token, MAX_DESC - 1);
    descricao_val[MAX_DESC - 1] = '\0';

    token = strtok(NULL, ";");
    if (!token)
      continue;
    preco_val = atof(token);

    adicionar_produto_na_lista(lista, pilhas, cod_val, tipo_val, descricao_val,
                               preco_val);
    produtos_carregados++;
  }

  fclose(arquivo);
  return produtos_carregados;
}

void imprimir_lista_produtos(ProductLinkedList *lista) {
  printf("\n--- Lista de Produtos Disponiveis (Ordenada por Preco) ---\n");
  if (!lista->header) {
    // Caso lista esteja vazia
    printf("   Nenhum produto na lista.\n");
    return;
  }
  Product *atual = lista->header;
  int count = 1;
  while (atual != NULL) {
    // Percorre e imprime cada produto da lista
    printf("   %d. COD: %d, CAIXA: %d, TIPO: %s, DESC: %s, PRECO: %.2f\n",
           count++, atual->cod, atual->cod_caixa, products_names[atual->type],
           atual->descricao, atual->preco);
    atual = atual->prox;
  }
  printf("---------------------------------------------------------\n");
}

void imprimir_pilha_de_caixas(ProductBox *pilha_topo,
                              const char *nome_tipo_produto) {
  printf("\n--- Caixas de %s (Pilha de Estoque) ---\n", nome_tipo_produto);
  if (!pilha_topo) {
    printf("   Nenhuma caixa no estoque para %s.\n", nome_tipo_produto);
    printf("---------------------------------------------------------\n");
    return;
  }

  ProductBox *caixa_atual = pilha_topo;
  int num_caixa_impressao = 1;
  while (caixa_atual != NULL) {
    printf("   Caixa #%d (Codigo Real: %d, Tipo: %s) [%d/%d itens]:\n",
           num_caixa_impressao++, caixa_atual->cod_caixa,
           products_names[caixa_atual->type], caixa_atual->tam,
           products_limits[caixa_atual->type]);

    ProductBoxItem *item_atual = caixa_atual->items_header;
    if (!item_atual) {
      // Caixa está vazia
      printf("     -> Caixa vazia.\n");
    } else {
      int item_count = 1;
      while (item_atual != NULL) {
        printf("     -> Item %d: COD Produto: %d\n", item_count++,
               item_atual->product_cod);
        item_atual = item_atual->prox;
      }
    }
    caixa_atual = caixa_atual->prox;
  }
  printf("---------------------------------------------------------\n");
}

void liberar_lista_produtos(ProductLinkedList *lista) {
  Product *atual = lista->header;
  Product *proximo_produto;
  while (atual != NULL) {
    proximo_produto = atual->prox;
    free(atual);
    atual = proximo_produto;
  }
  lista->header = NULL;
  lista->tail = NULL;
  lista->tamanho = 0;
}

ProductBox *liberar_pilha_de_caixas(ProductBox *pilha_topo) {
  ProductBox *caixa_atual = pilha_topo;
  ProductBox *proxima_caixa;
  while (caixa_atual != NULL) {
    proxima_caixa = caixa_atual->prox;

    ProductBoxItem *item_atual = caixa_atual->items_header;
    ProductBoxItem *proximo_item;
    while (item_atual != NULL) {
      proximo_item = item_atual->prox;
      free(item_atual);
      item_atual = proximo_item;
    }

    free(caixa_atual);
    caixa_atual = proxima_caixa;
  }
  return NULL;
}

int main() {
  ProductLinkedList listaProdutos = {NULL, NULL, 0};
  ProductBox *pilhas[NUM_PRODUCTS] = {NULL};

  int total_carregado = carregar_csv("products.csv", &listaProdutos, pilhas);

  adicionar_produto_na_lista(&listaProdutos, pilhas, 23444, PARAFINA,
                             "TESTANDO MANUAL", 0.01f);

  if (total_carregado >= 0) {
    printf("Total de produtos carregados do CSV: %d\n", total_carregado);
    imprimir_lista_produtos(&listaProdutos);
    for (int i = 0; i < NUM_PRODUCTS; i++) {
      imprimir_pilha_de_caixas(pilhas[i], products_names[i]);
    }
  } else {
    printf("Falha ao carregar produtos do arquivo CSV.\n");
  }

  // Já que criei as pilhas e as listas usando MALLOC (manualmente) devo
  // libera-las manualmente também
  liberar_lista_produtos(&listaProdutos);
  for (int i = 0; i < NUM_PRODUCTS; i++) {
    pilhas[i] = liberar_pilha_de_caixas(pilhas[i]);
  }

  printf("\nMemoria liberada. Programa encerrado.\n");

  return 0;
}