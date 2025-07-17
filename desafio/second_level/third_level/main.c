#include <ctype.h>
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

typedef struct ProductQueueDelivery {
  char *nome;
  int cpf;
  int cep;
  char *nomeRua;
  int numCasa;
  char *complemento;

  int cod;
  products_enum type;
  char *descricao;
  float preco;

  struct ProductQueueDelivery *prox;
} ProductQueueDelivery;

typedef struct {
  ProductQueueDelivery *header;
  ProductQueueDelivery *tail;
  int tamanho;
} QueueDelivery;

QueueDelivery *entregas = NULL;

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
void remove_product_from_list(ProductLinkedList *lista,
                              Product *product_to_remove);

ProductQueueDelivery *constuctor_product_queue(char *nome, int cpf, int cep,
                                               char *nomeRua, int numCasa,
                                               char *complemento, int cod,
                                               products_enum type,
                                               char *descricao, float preco);
ProductQueueDelivery *
adicionar_na_lista_de_entregas(QueueDelivery *queue,
                               Product *product_purchased);
void remover_na_lista_de_entregas(QueueDelivery *queue);
void liberar_fila_entregas(QueueDelivery *queue);
void imprimir_fila_entregas(QueueDelivery *queue);
void handle_imprimir_lista_produtos(ProductLinkedList *lista);
products_enum get_product_type_from_menu();
void adicionar_produto_manualmente(ProductLinkedList *lista,
                                   ProductBox *pilhas[]);

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
      printf("  %d. COD: %d, TIPO: %s, DESC: %s, PRECO: %.2f (Caixa: %d)\n",
             count++, atual->cod, products_names[atual->type], atual->descricao,
             atual->preco, atual->cod_caixa);
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

void remove_product_from_list(ProductLinkedList *lista,
                              Product *product_to_remove) {
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
    if (temp_stack->prox == NULL) {
      break;
    }
    push_box(product_stack_head, pop_box(&temp_stack));
  }

  if (temp_stack != NULL && found_box != NULL) {
    ProductBoxItem *first_item = temp_stack->items_header;
    temp_stack->items_header = temp_stack->items_header->prox;
    temp_stack->tam--;
    first_item->prox = NULL;

    first_item->prox = found_box->items_header;
    found_box->items_header = first_item;
    found_box->tam++;
  }

  if (found_box != NULL) {
    if (found_box->tam == 0) {
      free(found_box);
    } else {
      push_box(product_stack_head, found_box);
    }
  } else {
    if (removed_box_item)
      free(removed_box_item);
    return NULL;
  }

  if (temp_stack->tam == 0) {
    free(temp_stack);
  } else {
    push_box(product_stack_head, pop_box(&temp_stack));
  }

  if (removed_box_item != NULL) {
    Product *purchased_copy = malloc(sizeof(Product));
    if (purchased_copy == NULL) {
      printf("Erro: Memória insuficiente para copiar o produto comprado.\n");
      free(removed_box_item);
      return NULL;
    }

    purchased_copy->cod = product_to_buy->cod;
    purchased_copy->type = product_to_buy->type;
    strncpy(purchased_copy->descricao, product_to_buy->descricao, MAX_DESC - 1);
    purchased_copy->descricao[MAX_DESC - 1] = '\0';
    purchased_copy->preco = product_to_buy->preco;
    purchased_copy->cod_caixa = product_to_buy->cod_caixa;

    free(removed_box_item);
    remove_product_from_list(lista, product_to_buy);

    return purchased_copy;
  } else {
    return NULL;
  }
}

ProductQueueDelivery *constuctor_product_queue(char *nome, int cpf, int cep,
                                               char *nomeRua, int numCasa,
                                               char *complemento, int cod,
                                               products_enum type,
                                               char *descricao, float preco) {
  ProductQueueDelivery *p = malloc(sizeof(ProductQueueDelivery));
  if (!p)
    return NULL;

  p->prox = NULL;

  p->nome = NULL;
  p->complemento = NULL;
  p->nomeRua = NULL;
  p->descricao = NULL;

  p->cpf = cpf;
  p->numCasa = numCasa;
  p->cep = cep;

  p->cod = cod;
  p->preco = preco;
  p->type = type;

  p->nome = strdup(nome);
  if (!p->nome) {
    free(p);
    return NULL;
  }

  p->complemento = strdup(complemento);
  if (!p->complemento) {
    free(p->nome);
    free(p);
    return NULL;
  }

  p->nomeRua = strdup(nomeRua);
  if (!p->nomeRua) {
    free(p->nome);
    free(p->complemento);
    free(p);
    return NULL;
  }

  p->descricao = strdup(descricao);
  if (!p->descricao) {
    free(p->nome);
    free(p->complemento);
    free(p->nomeRua);
    free(p);
    return NULL;
  }

  return p;
}

ProductQueueDelivery *
adicionar_na_lista_de_entregas(QueueDelivery *queue,
                               Product *product_purchased) {
  int cep, cpf, numCasa;
  char nomeRua[100], complemento[100], nome[100];

  printf("\n----------- Dados do Comprador -----------\n");

  printf("Digite o nome do comprador: ");
  fgets(nome, 100, stdin);
  nome[strcspn(nome, "\n")] = '\0';

  printf("Digite o cpf do comprador: ");
  scanf("%d", &cpf);
  while (getchar() != '\n' && !feof(stdin))
    ;

  printf("\n----------- Endereço -----------\n");

  printf("Digite o cep: ");
  scanf("%d", &cep);
  while (getchar() != '\n' && !feof(stdin))
    ;

  printf("Digite nome da rua: ");
  fgets(nomeRua, 100, stdin);
  nomeRua[strcspn(nomeRua, "\n")] = '\0';

  printf("Digite o número da casa: ");
  scanf("%d", &numCasa);
  while (getchar() != '\n' && !feof(stdin))
    ;

  printf("Digite o complemento: ");
  fgets(complemento, 100, stdin);
  complemento[strcspn(complemento, "\n")] = '\0';

  ProductQueueDelivery *pedido = constuctor_product_queue(
      nome, cpf, cep, nomeRua, numCasa, complemento, product_purchased->cod,
      product_purchased->type, product_purchased->descricao,
      product_purchased->preco);

  if (pedido == NULL) {
    printf("Erro ao criar pedido de entrega. Memória insuficiente.\n");
    free(product_purchased);
    return NULL;
  }

  if (queue->header == NULL) {
    queue->header = pedido;
    queue->tail = pedido;
  } else {
    queue->tail->prox = pedido;
    queue->tail = pedido;
  }
  queue->tamanho++;

  free(product_purchased);
  return pedido;
}

void remover_na_lista_de_entregas(QueueDelivery *queue) {
  if (queue == NULL || queue->header == NULL) {
    printf("A fila de entregas está vazia.\n");
    return;
  }

  ProductQueueDelivery *removido = queue->header;
  queue->header = removido->prox;
  if (queue->header == NULL) {
    queue->tail = NULL;
  }
  queue->tamanho--;

  printf("\n--- Pedido de Entrega Processado e Removido ---\n");
  printf("  Produto: COD %d, Tipo: %s, Descrição: %s, Preço: %.2f\n",
         removido->cod, products_names[removido->type], removido->descricao,
         removido->preco);
  printf("  Comprador: %s (CPF: %d)\n", removido->nome, removido->cpf);
  printf("  Endereço: %s, %d, %s (CEP: %d)\n", removido->nomeRua,
         removido->numCasa,
         removido->complemento && strlen(removido->complemento) > 0
             ? removido->complemento
             : "N/A",
         removido->cep);
  printf("--------------------------------------------------\n");

  free(removido->nome);
  free(removido->nomeRua);
  free(removido->complemento);
  free(removido->descricao);
  free(removido);
}

void imprimir_fila_entregas(QueueDelivery *queue) {
  printf("\n--- Fila de Entregas Pendentes ---\n");
  if (queue == NULL || queue->header == NULL) {
    printf("   A fila de entregas está vazia.\n");
    printf("-----------------------------------\n");
    return;
  }

  ProductQueueDelivery *atual = queue->header;
  int count = 1;
  while (atual != NULL) {
    printf("  %d. Pedido:\n", count++);
    printf("     Produto: COD %d, Tipo: %s, Descrição: %s, Preço: %.2f\n",
           atual->cod, products_names[atual->type], atual->descricao,
           atual->preco);
    printf("     Comprador: %s (CPF: %d)\n", atual->nome, atual->cpf);
    printf("     Endereço: %s, %d, %s (CEP: %d)\n", atual->nomeRua,
           atual->numCasa,
           atual->complemento && strlen(atual->complemento) > 0
               ? atual->complemento
               : "N/A",
           atual->cep);
    printf("     ---\n");
    atual = atual->prox;
  }
  printf("-----------------------------------\n");
}

void liberar_fila_entregas(QueueDelivery *queue) {
  if (queue == NULL)
    return;
  while (queue->header != NULL) {
    remover_na_lista_de_entregas(queue);
  }
  free(queue);
}

products_enum get_product_type_from_menu() {
  int choice;
  printf("\n--- Selecione o Tipo de Produto --- \n");
  for (int i = 0; i < NUM_PRODUCTS; i++) {
    printf("%d. %s\n", i + 1, products_names[i]);
  }
  printf("0. Todos os Tipos (ou Voltar)\n");
  printf("Escolha uma opção: ");
  scanf("%d", &choice);
  while (getchar() != '\n' && !feof(stdin))
    ;

  if (choice > 0 && choice <= NUM_PRODUCTS) {
    return (products_enum)(choice - 1);
  } else if (choice == 0) {
    return (products_enum)-1;
  } else {
    printf("Opção inválida. Seleção de tipo ignorada.\n");
    return (products_enum)-1;
  }
}

void handle_imprimir_lista_produtos(ProductLinkedList *lista) {
  float max_preco = -1.0f;
  float min_preco = -1.0f;
  int escolha_filtro;
  char *tipo_para_imprimir = NULL;

  printf("\nOpções de filtro para a lista de produtos:\n");
  printf("1. Imprimir todos os produtos\n");
  printf("2. Filtrar por tipo\n");
  printf("3. Filtrar por tipo e máximo\n");
  printf("4. Filtrar por tipo e mínimo\n");
  printf("5. Filtrar por preço máximo\n");
  printf("6. Filtrar por preço mínimo\n");
  printf("7. Filtrar por tipo e preço (máx/min)\n");
  printf("Escolha uma opção de filtro: ");
  scanf("%d", &escolha_filtro);
  while (getchar() != '\n' && !feof(stdin))
    ;

  switch (escolha_filtro) {
  case 1:
    break;
  case 2: {
    products_enum selected_type = get_product_type_from_menu();
    if (selected_type != (products_enum)-1) {
      tipo_para_imprimir = (char *)products_names[selected_type];
    }
    break;
  }
  case 3: {
    products_enum selected_type = get_product_type_from_menu();
    printf("Digite o preço MÁXIMO: ");
    scanf("%f", &max_preco);
    while (getchar() != '\n' && !feof(stdin))
      ;
    if (selected_type != (products_enum)-1) {
      tipo_para_imprimir = (char *)products_names[selected_type];
    }
    break;
  }
  case 4: {
    products_enum selected_type = get_product_type_from_menu();
    printf("Digite o preço MÍNIMO: ");
    scanf("%f", &min_preco);
    while (getchar() != '\n' && !feof(stdin))
      ;
    if (selected_type != (products_enum)-1) {
      tipo_para_imprimir = (char *)products_names[selected_type];
    }
    break;
  }
  case 5:
    printf("Digite o preço MÁXIMO: ");
    scanf("%f", &max_preco);
    while (getchar() != '\n' && !feof(stdin))
      ;
    break;
  case 6:
    printf("Digite o preço MÍNIMO: ");
    scanf("%f", &min_preco);
    while (getchar() != '\n' && !feof(stdin))
      ;
    break;
  case 7: {
    products_enum selected_type = get_product_type_from_menu();
    if (selected_type != (products_enum)-1) {
      tipo_para_imprimir = (char *)products_names[selected_type];
    }
    printf("Digite o preço MÁXIMO: ");
    scanf("%f", &max_preco);
    while (getchar() != '\n' && !feof(stdin))
      ;
    printf("Digite o preço MÍNIMO: ");
    scanf("%f", &min_preco);
    while (getchar() != '\n' && !feof(stdin))
      ;
    break;
  }
  default:
    printf("Opção de filtro inválida. Imprimindo todos os produtos.\n");
    break;
  }

  imprimir_lista_produtos(lista, tipo_para_imprimir, max_preco, min_preco);
}

void adicionar_produto_manualmente(ProductLinkedList *lista,
                                   ProductBox *pilhas[]) {
  int cod;
  products_enum type;
  char descricao[MAX_DESC];
  float preco;
  int tipo_valido = 0;

  printf("\n--- Adicionar Novo Produto Manualmente ---\n");

  printf("Digite o codigo do produto: ");
  scanf("%d", &cod);
  while (getchar() != '\n' && !feof(stdin))
    ;

  if (search_product(lista, cod) != NULL) {
    printf("Erro: Produto com codigo %d ja existe na lista.\n", cod);
    return;
  }

  type = get_product_type_from_menu();
  if (type == (products_enum)-1) {
    printf("Selecao de tipo invalida ou cancelada. Produto nao adicionado.\n");
    return;
  }

  printf("Digite a descricao do produto (max %d caracteres): ", MAX_DESC - 1);
  fgets(descricao, MAX_DESC, stdin);
  descricao[strcspn(descricao, "\n")] = '\0';

  printf("Digite o preco do produto: ");
  scanf("%f", &preco);
  while (getchar() != '\n' && !feof(stdin))
    ;

  adicionar_produto_na_lista(lista, pilhas, cod, type, descricao, preco);
  printf("Produto COD %d (%s) adicionado com sucesso!\n", cod,
         products_names[type]);
}

int main() {
  ProductLinkedList listaProdutos = {NULL, NULL, 0};
  ProductBox *pilhas[NUM_PRODUCTS] = {NULL};
  int opcao;
  int cod_produto_comprar;
  char nome_arquivo[50];
  int total_carregado = 0;

  entregas = (QueueDelivery *)malloc(sizeof(QueueDelivery));
  if (entregas == NULL) {
    printf("Erro ao alocar memoria para a fila de entregas.\n");
    return 1;
  }
  entregas->header = NULL;
  entregas->tail = NULL;
  entregas->tamanho = 0;

  do {
    printf("\n--- MENU ---\n");
    printf("1. Carregar produtos do CSV\n");
    printf("2. Adicionar produto manualmente\n");
    printf("3. Imprimir lista de produtos\n");
    printf("4. Imprimir pilhas de caixas\n");
    printf("5. Comprar produto\n");
    printf("6. Imprimir fila de entregas\n");
    printf("7. Processar proxima entrega\n");
    printf("0. Sair\n");
    printf("Escolha uma opcao: ");
    scanf("%d", &opcao);

    switch (opcao) {
    case 1:
      printf("Digite o nome do arquivo CSV (ex: products.csv): ");
      scanf("%s", nome_arquivo);
      while (getchar() != '\n' && !feof(stdin))
        ;
      total_carregado = carregar_csv(nome_arquivo, &listaProdutos, pilhas);
      if (total_carregado >= 0) {
        printf("Total de produtos carregados do CSV: %d\n", total_carregado);
      } else {
        printf("Falha ao carregar produtos do arquivo CSV.\n");
      }
      break;
    case 2:
      adicionar_produto_manualmente(&listaProdutos, pilhas);
      break;
    case 3:
      handle_imprimir_lista_produtos(&listaProdutos);
      break;
    case 4:
      for (int i = 0; i < NUM_PRODUCTS; i++) {
        imprimir_pilha_de_caixas(pilhas[i], products_names[i]);
      }
      break;
    case 5: {
      printf("Digite o codigo do produto a ser comprado: ");
      scanf("%d", &cod_produto_comprar);
      while (getchar() != '\n' && !feof(stdin))
        ;

      Product *purchased_product_ptr =
          buy_product(&listaProdutos, pilhas, cod_produto_comprar);

      if (purchased_product_ptr != NULL) {
        ProductQueueDelivery *aux =
            adicionar_na_lista_de_entregas(entregas, purchased_product_ptr);
        if (aux != NULL) {
          printf("Compra do produto COD %d (%s) realizada com sucesso e "
                 "adicionado a fila de entregas!\n",
                 aux->cod, products_names[aux->type]);
        } else {
          printf("Erro: Nao foi possivel adicionar o produto a fila de "
                 "entregas.\n");
        }
      } else {
        printf("Falha na compra do produto COD %d.\n", cod_produto_comprar);
      }
      break;
    }
    case 6:
      imprimir_fila_entregas(entregas);
      break;
    case 7:
      remover_na_lista_de_entregas(entregas);
      break;
    case 0:
      printf("Saindo do programa...\n");
      break;
    default:
      printf("Opcao invalida. Tente novamente.\n");
      break;
    }
  } while (opcao != 0);

  liberar_lista_produtos(&listaProdutos);
  for (int i = 0; i < NUM_PRODUCTS; i++) {
    pilhas[i] = liberar_pilha_de_caixas(pilhas[i]);
  }
  liberar_fila_entregas(entregas);

  printf("\nMemoria liberada. Programa encerrado.\n");

  return 0;
}