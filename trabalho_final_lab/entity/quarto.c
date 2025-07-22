#include <stdio.h>

#define MAX_NOME 100
#define MAX_DESC 200

typedef struct {
  int id;
  int tipo_id;
  char nome[MAX_NOME];
  char descricao[MAX_DESC];
  float diaria_por_pessoa;
} Quarto;