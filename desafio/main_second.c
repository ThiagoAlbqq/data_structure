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

// Variavel para criar o cod da caixa iniciando de 1
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
void imprimir_lista_produtos(ProductLinkedList *lista, char *tipo, float max,
                             float min);
void imprimir_pilha_de_caixas(ProductBox *pilha_topo,
                              const char *nome_tipo_produto);
void liberar_lista_produtos(ProductLinkedList *lista);
ProductBox *liberar_pilha_de_caixas(ProductBox *pilha_topo);
Product *comprar_produto(ProductLinkedList *lista, ProductBox **pilha,
                         int cod_produto);

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
void inserir_ordenado(ProductLinkedList *lista, Product *novo) {
  Product *atual_node;

  novo->prox = NULL;
  novo->ant = NULL;

  if (lista->header == NULL) {
    // Lista vazia
    lista->header = novo;
    lista->tail = novo;
  } else {
    atual_node = lista->header;
    while (atual_node != NULL && novo->preco >= atual_node->preco) {
      atual_node = atual_node->prox;
    }
    if (atual_node == NULL) {
      // Inserção no final da lista
      novo->ant = lista->tail;
      lista->tail->prox = novo;
      lista->tail = novo;
    } else if (atual_node->ant == NULL) {
      // Inserção no início da lista
      novo->prox = lista->header;
      lista->header->ant = novo;
      lista->header = novo;
    } else {
      // Inserção no meio da lista
      novo->prox = atual_node;
      novo->ant = atual_node->ant;
      atual_node->ant->prox = novo;
      atual_node->ant = novo;
    }
  }
}

Product *adicionar_produto_na_lista(ProductLinkedList *lista,
                                    ProductBox *pilhas[], int cod,
                                    products_enum tipo, const char *desc,
                                    float preco) {
  Product *novo = malloc(sizeof(Product));
  if (!novo) {
    printf("Erro ao alocar memoria para produto na lista");
    exit(1);
  }
  novo->cod = cod;
  novo->type = tipo;
  strncpy(novo->descricao, desc, MAX_DESC - 1);
  novo->descricao[MAX_DESC - 1] = '\0';
  novo->preco = preco;
  inserir_ordenado(lista, novo);

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

void imprimir_lista_produtos(ProductLinkedList *lista, char *tipo, float max,
                             float min) {
  int type_enum = tipo != NULL ? parse_type(tipo) : -1;
  printf("\n--- Lista de Produtos Disponiveis (Ordenada por Preco) ---\n");
  if (!lista->header) {
    printf("  Nenhum produto na lista.\n");
    return;
  }
  if (!lista->header) {
    // Caso lista esteja vazia
    printf("   Nenhum produto na lista.\n");
    return;
  }
  Product *atual = lista->header;
  int count = 1;
  while (atual != NULL) {
    // Se eu passar o tipo, só será verdadeiro se o tipo do atual for igual ao
    // que eu passei
    int passa_tipo = (type_enum == -1) || (atual->type == type_enum);
    // Se eu passar o max, só será verdadeiro se o preco do atual for igual ao
    // que eu passei
    int passa_max = (max < 0) || (atual->preco <= max);
    // Se eu passar o min, só será verdadeiro se o preco do atual for igual ao
    // que eu passei
    int passa_min = (min < 0) || (atual->preco >= min);
    if (passa_tipo && passa_max && passa_min) {
      printf("  %d. COD: %d, TIPO: %d, DESC: %s, PRECO: %.2f\n", count++,
             atual->cod, atual->type, atual->descricao, atual->preco);
    }
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

ProductBoxItem *remove_first_item_from_box(ProductBox *box) {
  if (box == NULL || box->items_header == NULL) {
    return NULL;
  }

  while (box->prox != NULL) {
    box = box->prox;
  }

  ProductBoxItem *item_removed = box->items_header;
  box->items_header = box->items_header->prox;
  box->tam--;
  item_removed->prox = NULL;

  return item_removed;
}

void add_item_to_box_front(ProductBox *box, ProductBoxItem *item) {
  if (box == NULL || item == NULL)
    return;
  item->prox = box->items_header;
  box->items_header = item;
  box->tam++;

  ProductBoxItem *aux = box->items_header;
  while (aux != NULL) {
    printf("%d ", aux->product_cod);
    aux = aux->prox;
  }
}

Product *comprar_produto(ProductLinkedList *lista, ProductBox **pilha,
                         int cod_produto) {
  ProductBox *caixas_desempilhadas = NULL;
  ProductBox *caixa_do_produto = NULL;
  ProductBoxItem *item_encontrado_na_caixa = NULL;
  ProductBox *ultima_caixa_desempilhada_nao_contem_produto = NULL;

  while (*pilha != NULL) {
    ProductBox *atual_box = *pilha;
    *pilha = (*pilha)->prox;

    ProductBoxItem *produto_atual_item = atual_box->items_header;
    ProductBoxItem *anterior_item = NULL;
    int found_in_this_box = 0;

    while (produto_atual_item != NULL) {
      if (produto_atual_item->product_cod == cod_produto) {
        found_in_this_box = 1;
        item_encontrado_na_caixa = produto_atual_item;
        caixa_do_produto = atual_box;

        if (anterior_item == NULL) {
          atual_box->items_header = produto_atual_item->prox;
        } else {
          anterior_item->prox = produto_atual_item->prox;
        }
        atual_box->tam--;
        item_encontrado_na_caixa->prox = NULL;
        break;
      }
      anterior_item = produto_atual_item;
      produto_atual_item = produto_atual_item->prox;
    }

    if (found_in_this_box) {
      if (caixas_desempilhadas != NULL) {
        ProductBoxItem *item_para_mover =
            remove_first_item_from_box(caixas_desempilhadas);
        if (item_para_mover != NULL) {
          add_item_to_box_front(caixa_do_produto, item_para_mover);
          printf("Item %d movido da caixa ID %d para a caixa ID %d.\n",
                 item_para_mover->product_cod, caixas_desempilhadas->cod_caixa,
                 caixa_do_produto->cod_caixa);
        } else {
          printf(
              "A última caixa desempilhada (%d) não tinha itens para mover.\n",
              caixas_desempilhadas->cod_caixa);
        }
      }

      ProductBoxItem *aux = caixa_do_produto->items_header;
      while (aux != NULL) {
        printf("%d ", aux->product_cod);
        aux = aux->prox;
      }

      break;
    } else {
      atual_box->prox = caixas_desempilhadas;
      caixas_desempilhadas = atual_box;
      ultima_caixa_desempilhada_nao_contem_produto = atual_box;
    }
  }

  if (caixa_do_produto == NULL) {
    printf("Produto %d não encontrado na pilha informada.\n", cod_produto);
    while (caixas_desempilhadas != NULL) {
      ProductBox *temp_box = caixas_desempilhadas;
      caixas_desempilhadas = caixas_desempilhadas->prox;
      temp_box->prox = *pilha;
      *pilha = temp_box;
    }
    return NULL;
  }

  printf("Produto %d comprado com sucesso da caixa ID %d.\n", cod_produto,
         caixa_do_produto->cod_caixa);

  ProductBox *first_box = caixas_desempilhadas;
  while (first_box != NULL) {
    printf("aaaaaaaa%d \n", first_box->cod_caixa);
    first_box = first_box->prox;
  }

  while (caixas_desempilhadas != NULL) {
    if (caixas_desempilhadas->prox != NULL &&
        caixas_desempilhadas->prox->prox == NULL) {
      caixas_desempilhadas->prox = NULL;
    }
    ProductBox *temp_box = caixas_desempilhadas;
    caixas_desempilhadas = caixas_desempilhadas->prox;
    temp_box->prox = *pilha;
    *pilha = temp_box;
  }

  caixa_do_produto->prox = *pilha;
  *pilha = caixa_do_produto;
  first_box->prox = *pilha;
  printf("MEIO");
  *pilha = first_box;

  ProductBox *aux = caixas_desempilhadas;

  while (aux != NULL) {
    printf("%d \n", aux->cod_caixa);
    aux = aux->prox;
  }

  Product *auxiliar_lista = lista->header;
  Product *retorno_produto = NULL;

  while (auxiliar_lista != NULL) {
    if (auxiliar_lista->cod == cod_produto) {
      retorno_produto = malloc(sizeof(Product));
      if (retorno_produto == NULL) {
        perror("Erro ao alocar memória para o produto de retorno");
        exit(1);
      }
      *retorno_produto = *auxiliar_lista;
      retorno_produto->prox = NULL;
      retorno_produto->ant = NULL;

      if (auxiliar_lista->ant != NULL) {
        auxiliar_lista->ant->prox = auxiliar_lista->prox;
      } else {
        lista->header = auxiliar_lista->prox;
      }

      if (auxiliar_lista->prox != NULL) {
        auxiliar_lista->prox->ant = auxiliar_lista->ant;
      }

      Product *removido_da_lista = auxiliar_lista;
      auxiliar_lista = auxiliar_lista->prox;
      free(removido_da_lista);
      lista->tamanho--;
      printf("Produto %d removido da lista geral.\n", cod_produto);
      return retorno_produto;
    } else {
      auxiliar_lista = auxiliar_lista->prox;
    }
  }

  printf("Atenção: Produto %d comprado da pilha, mas não encontrado na lista "
         "geral de produtos para remoção.\n",
         cod_produto);
  return NULL;
}

int main() {
  ProductLinkedList listaProdutos = {NULL, NULL, 0};
  ProductBox *pilhas[NUM_PRODUCTS] = {NULL};

  int total_carregado = carregar_csv("products.csv", &listaProdutos, pilhas);

  adicionar_produto_na_lista(&listaProdutos, pilhas, 23444, PARAFINA,
                             "TESTANDO MANUAL", 0.01f);

  comprar_produto(&listaProdutos, &pilhas[DECKS_ANTIDERRAPANTES], 28);

  if (total_carregado >= 0) {
    printf("Total de produtos carregados do CSV: %d\n", total_carregado);
    imprimir_lista_produtos(&listaProdutos, NULL, 200.0f, 150.0f);
    for (int i = 0; i < NUM_PRODUCTS; i++) {
      imprimir_pilha_de_caixas(pilhas[i], products_names[i]);
    }
  } else {
    printf("Falha ao carregar produtos do arquivo CSV.\n");
  }

  liberar_lista_produtos(&listaProdutos);
  for (int i = 0; i < NUM_PRODUCTS; i++) {
    pilhas[i] = liberar_pilha_de_caixas(pilhas[i]);
  }

  printf("\nMemoria liberada. Programa encerrado.\n");

  return 0;
}