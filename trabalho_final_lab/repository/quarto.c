#include "./quarto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 500

void get_rooms() {
  FILE *arq = fopen("./database/quarto.txt", "r");
  if (arq == NULL) {
    perror("Erro ao abrir o arquivo de Quartos");
    return;
  }

  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), arq)) {
    int id;
    int tipo_id;
    char nome[MAX_NOME];
    char descricao[MAX_DESC];
    float diaria_por_pessoa;

    if (sscanf(line, "%d;%d;%[^;];%[^;];%f", &id, &tipo_id, nome, descricao,
               &diaria_por_pessoa) == 5) {
      Quarto *quarto_encontrado = (Quarto *)malloc(sizeof(Quarto));
      quarto_encontrado->id = id;
      quarto_encontrado->tipo_id = tipo_id;
      strcpy(quarto_encontrado->nome, nome);
      strcpy(quarto_encontrado->descricao, descricao);
      quarto_encontrado->diaria_por_pessoa = diaria_por_pessoa;

      printf("ID: %d, Tipo: %d, Nome: %s, Descrição: %s, Diária: %.2f\n",
             quarto_encontrado->id, quarto_encontrado->tipo_id,
             quarto_encontrado->nome, quarto_encontrado->descricao,
             quarto_encontrado->diaria_por_pessoa);
      free(quarto_encontrado);
    }
  }
  fclose(arq);
}