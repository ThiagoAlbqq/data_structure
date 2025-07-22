#include <stdio.h>

#define MAX_NOME 100
#define MAX_DADOS 200
#define MAX_EMAIL 100
#define MAX_TELEFONE 20

typedef struct {
  int id;
  char nome[MAX_NOME];
  char cpf[15];
  char telefone[MAX_TELEFONE];
  char email[MAX_EMAIL];
} Cliente;