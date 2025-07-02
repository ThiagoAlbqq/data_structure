#include <stdio.h>
#include <stdlib.h>

typedef struct NO {
  int chave;
  struct NO *dir;
  struct NO *esq;
} NO;

NO *raiz = NULL;
int tam = 0;

NO *buscar(NO *aux, int chave) {
  if (aux == NULL) {
    // arvore esta vazia
    return NULL;
  } else if (chave == aux->chave) {
    // encontrei
    return aux;
  } else if (chave < aux->chave) {
    // descer para esq?
    if (aux->esq != NULL) {
      return buscar(aux->esq, chave);
    } else {
      return aux;
    }
  } else {
    // descer para dir
    if (aux->dir != NULL) {
      return buscar(aux->dir, chave);
    } else {
      return aux;
    }
  }
}

void add(int chave) {
  NO *novo = malloc(sizeof(NO));
  novo->chave = chave;
  novo->esq = NULL;
  novo->dir = NULL;
  NO *aux = buscar(raiz, chave);
  if (aux == NULL) {
    // arvore esta vazia
    raiz = novo;
  } else if (aux->chave == chave) {
    printf("Adição nao permitida. Chave duplicada\n");
    free(novo);
  } else {
    // pai do no que eu quero adicionar
    if (chave < aux->chave) {
      aux->esq = novo;
    } else {
      aux->dir = novo;
    }
  }
}

void imprimir_in(NO *aux) {
  if (aux != NULL) {
    imprimir_in(aux->esq);
    printf("%d\n", aux->chave);
    imprimir_in(aux->dir);
  }
}

NO *remover(NO *aux, int chave) {
  NO *atual = aux, *pai = NULL;

  // 1. Buscar o nó a ser removido e seu pai
  while (atual != NULL && atual->chave != chave) {
    pai = atual;
    if (chave < atual->chave)
      atual = atual->esq;
    else
      atual = atual->dir;
  }

  // 2. Caso a chave não exista
  if (atual == NULL)
    return aux;

  // 3. Caso com 0 ou 1 filho
  if (atual->esq == NULL || atual->dir == NULL) {
    NO *filho = (atual->esq != NULL) ? atual->esq : atual->dir;

    // Se o nó a remover é a raiz
    if (pai == NULL) {
      free(atual);
      return filho; // nova raiz
    }

    // Substituir o filho na posição correta do pai
    if (pai->esq == atual)
      pai->esq = filho;
    else
      pai->dir = filho;

    free(atual);
  }

  // 4. Caso com 2 filhos
  else {
    // Maior da subárvore esquerda (antecessor)
    NO *sub = atual->esq;
    NO *pai_sub = atual;

    while (sub->dir != NULL) {
      pai_sub = sub;
      sub = sub->dir;
    }

    // Substitui valor
    atual->chave = sub->chave;

    // Remove o nó substituto
    if (pai_sub->dir == sub)
      pai_sub->dir = sub->esq;
    else
      pai_sub->esq = sub->esq;

    free(sub);
  }

  return aux; // raiz não mudou
}

int main() {
  add(52);
  add(30);
  add(12);
  add(25);
  add(58);
  remover(raiz, 12);
  imprimir_in(raiz);
  return 0;
}