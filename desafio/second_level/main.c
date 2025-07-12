#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DESC 100

typedef enum {
  PARAFINA,
  LEASH,
  QUILHAS,
  DECKS_ANTIDERRAPANTES,
  NUM_PRODUCTS
} products_enum;

const char *products_names[NUM_PRODUCTS] = {"PARAFINA", "LEASH", "QUILHAS",
                                            "DECKS_ANTIDERRAPANTES"};

const int products_limits[NUM_PRODUCTS] = {
    [PARAFINA] = 50, [LEASH] = 25, [QUILHAS] = 10, [DECKS_ANTIDERRAPANTES] = 5};

typedef struct Product {
  int cod;
  products_enum type;
  char descricao[MAX_DESC];
  float preco;
  int cod_caixa;

  struct Product *prox;
  struct Product *ant;
} Product;

typedef struct {
  Product *header;
  Product *tail;
  int tamanho;
} ProductLinkedList;

typedef struct ProductBoxItem {
  int product_cod;
  struct ProductBoxItem *prox;
} ProductBoxItem;

typedef struct ProductBox {
  int cod_caixa;
  products_enum type;
  int tam;
  ProductBoxItem *items_header;
  struct ProductBox *prox;
} ProductBox;

int g_next_box_code = 1;

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
Product *buy_product(ProductLinkedList *lista, ProductBox *pilhas[],
                     int cod_produto);
Product *search_product(ProductLinkedList *list, int cod_produto);

void push_box(ProductBox **head, ProductBox *new_box);
ProductBox *pop_box(ProductBox **head);
ProductBoxItem *remove_item_from_box(ProductBox *box, int product_code);
void remove_product_from_list(ProductLinkedList *lista, Product *product_to_remove);


products_enum parse_type(const char *str) {
  if (strcmp(str, "PARAFINA") == 0)
    return PARAFINA;
  if (strcmp(str, "LEASH") == 0)
    return LEASH;
  if (strcmp(str, "QUILHAS") == 0)
    return QUILHAS;
  if (strcmp(str, "DECKS_ANTIDERRAPANTES") == 0)
    return DECKS_ANTIDERRAPANTES;
  return (products_enum)-1;
}

void inserir_ordenado(ProductLinkedList *lista, Product *novo) {
  Product *atual_node;

  novo->prox = NULL;
  novo->ant = NULL;

  if (lista->header == NULL) {
    lista->header = novo;
    lista->tail = novo;
  } else {
    atual_node = lista->header;
    while (atual_node != NULL && novo->preco >= atual_node->preco) {
      atual_node = atual_node->prox;
    }
    if (atual_node == NULL) {
      novo->ant = lista->tail;
      lista->tail->prox = novo;
      lista->tail = novo;
    } else if (atual_node->ant == NULL) {
      novo->prox = lista->header;
      lista->header->ant = novo;
      lista->header = novo;
    } else {
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
  novo->cod_caixa = -1;

  inserir_ordenado(lista, novo);

  pilhas[tipo] = adicionar_produto_ao_estoque(pilhas[tipo], tipo, cod, novo);

  lista->tamanho++;
  return novo;
}

ProductBox *adicionar_produto_ao_estoque(ProductBox *pilha_topo,
                                         products_enum tipo_produto,
                                         int cod_produto, Product *produto) {
  if (pilha_topo == NULL || pilha_topo->tam >= products_limits[tipo_produto]) {
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
    printf("Erro ao alocar memoria para novo item na caixa");
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
  printf("\n--- Lista de Produtos Disponíveis (Ordenada por Preço) ---\n");
  if (!lista->header) {
    printf("   Nenhum produto na lista.\n");
    return;
  }
  Product *atual = lista->header;
  int count = 1;
  while (atual != NULL) {
    int passa_tipo = (type_enum == -1) || (atual->type == type_enum);
    int passa_max = (max < 0) || (atual->preco <= max);
    int passa_min = (min < 0) || (atual->preco >= min);
    if (passa_tipo && passa_max && passa_min) {
      printf("  %d. COD: %d, TIPO: %s, DESC: %s, PRECO: %.2f (Caixa: %d)\n", count++,
             atual->cod, products_names[atual->type], atual->descricao, atual->preco, atual->cod_caixa);
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
    printf("   Caixa #%d (Código Real: %d, Tipo: %s) [%d/%d itens]:\n",
           num_caixa_impressao++, caixa_atual->cod_caixa,
           products_names[caixa_atual->type], caixa_atual->tam,
           products_limits[caixa_atual->type]);

    ProductBoxItem *item_atual = caixa_atual->items_header;
    if (!item_atual) {
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

Product *search_product(ProductLinkedList *list, int cod_produto) {
  Product *aux = list->header;
  while (aux != NULL) {
    if (aux->cod == cod_produto) {
      return aux;
    }
    aux = aux->prox;
  }
  return NULL;
}

void push_box(ProductBox **head, ProductBox *new_box) {
  if (new_box == NULL)
    return;
  new_box->prox = *head;
  *head = new_box;
}
ProductBox *pop_box(ProductBox **head) {
  if (*head == NULL)
    return NULL;
  ProductBox *popped_box = *head;
  *head = (*head)->prox;
  popped_box->prox = NULL;
  return popped_box;
}

ProductBoxItem *remove_item_from_box(ProductBox *box, int product_code) {
  if (box == NULL || box->items_header == NULL) {
    return NULL;
  }

  ProductBoxItem *current_item = box->items_header;
  ProductBoxItem *prev_item = NULL;

  while (current_item != NULL) {
    if (current_item->product_cod == product_code) {
      if (prev_item == NULL) {
        box->items_header = current_item->prox;
      } else {
        prev_item->prox = current_item->prox;
      }
      current_item->prox = NULL;
      box->tam--;
      return current_item;
    }
    prev_item = current_item;
    current_item = current_item->prox;
  }
  return NULL;
}

void remove_product_from_list(ProductLinkedList *lista, Product *product_to_remove) {
    if (lista == NULL || product_to_remove == NULL) {
        return;
    }

    if (product_to_remove->ant == NULL) {
        lista->header = product_to_remove->prox;
    } else {
        product_to_remove->ant->prox = product_to_remove->prox;
    }

    if (product_to_remove->prox == NULL) {
        lista->tail = product_to_remove->ant;
    } else {
        product_to_remove->prox->ant = product_to_remove->ant;
    }

    free(product_to_remove);
    lista->tamanho--;
}


Product *buy_product(ProductLinkedList *lista, ProductBox *pilhas[],
                     int cod_produto) {
  Product *product_to_buy = search_product(lista, cod_produto);
  if (product_to_buy == NULL) {
    printf("Erro: Produto %d não encontrado na lista geral.\n", cod_produto);
    return NULL;
  }

  ProductBox **product_stack_head = &pilhas[product_to_buy->type];
  ProductBox *temp_stack = NULL;
  ProductBox *found_box = NULL;
  ProductBoxItem *removed_box_item = NULL;

  while (*product_stack_head != NULL) {
    ProductBox *current_box = pop_box(product_stack_head);
    if (current_box->cod_caixa == product_to_buy->cod_caixa) {
      removed_box_item = remove_item_from_box(current_box, cod_produto);
      if (removed_box_item != NULL) {
        found_box = current_box;
        break;
      } else {
        push_box(&temp_stack, current_box);
      }
    } else {
      push_box(&temp_stack, current_box);
    }
  }

  while (temp_stack != NULL) {
    push_box(product_stack_head, pop_box(&temp_stack));
  }

  if (found_box != NULL) {
    if (found_box->tam == 0) {
        free(found_box);
    } else {
        push_box(product_stack_head, found_box);
    }
  } else {
      if (removed_box_item) free(removed_box_item);
      return NULL;
  }

  if (removed_box_item != NULL) {
    free(removed_box_item);
    remove_product_from_list(lista, product_to_buy);
    return product_to_buy;
  } else {
    return NULL;
  }
}

int main() {
  ProductLinkedList listaProdutos = {NULL, NULL, 0};
  ProductBox *pilhas[NUM_PRODUCTS] = {NULL};
  int opcao;
  int cod_produto_comprar;
  char nome_arquivo[50];
  int total_carregado = 0;

  do {
    printf("\n--- MENU ---\n");
    printf("1. Carregar produtos do CSV\n");
    printf("2. Imprimir lista de produtos\n");
    printf("3. Imprimir pilhas de caixas\n");
    printf("4. Comprar produto\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcao);

    switch (opcao) {
      case 1:
        printf("Digite o nome do arquivo CSV (ex: products.csv): ");
        scanf("%s", nome_arquivo);
        total_carregado = carregar_csv(nome_arquivo, &listaProdutos, pilhas);
        if (total_carregado >= 0) {
          printf("Total de produtos carregados do CSV: %d\n", total_carregado);
        } else {
          printf("Falha ao carregar produtos do arquivo CSV.\n");
        }
        break;
      case 2:
        imprimir_lista_produtos(&listaProdutos, NULL, -1.0f, -1.0f);
        break;
      case 3:
        for (int i = 0; i < NUM_PRODUCTS; i++) {
          imprimir_pilha_de_caixas(pilhas[i], products_names[i]);
        }
        break;
      case 4:
        printf("Digite o código do produto a ser comprado: ");
        scanf("%d", &cod_produto_comprar);
        Product *purchased = buy_product(&listaProdutos, pilhas, cod_produto_comprar);
        if (purchased) {
            printf("Compra do produto COD %d (%s) realizada com sucesso!\n", cod_produto_comprar, "Produto Removido");
        } else {
            printf("Falha na compra do produto COD %d.\n", cod_produto_comprar);
        }
        break;
      case 0:
        printf("Saindo do programa...\n");
        break;
      default:
        printf("Opção inválida. Tente novamente.\n");
        break;
    }
  } while (opcao != 0);

  liberar_lista_produtos(&listaProdutos);
  for (int i = 0; i < NUM_PRODUCTS; i++) {
    pilhas[i] = liberar_pilha_de_caixas(pilhas[i]);
  }

  printf("\nMemória liberada. Programa encerrado.\n");

  return 0;
}