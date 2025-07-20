#include <stdio.h>

void show_menu(char *type) {
  printf("----------------- MENU -----------------\n");
  printf("1. Inserir %s\n", type);
  printf("2. Buscar %s\n", type);
  printf("3. Atualizar %s\n", type);
  printf("4. Excluir %s\n", type);
  printf("0. Voltar\n");
  printf("----------------------------------------\n");
}

int main() {
  int choice = 0;
  do {
    // Chamada da função
    show_menu("TESTANDO");
    scanf("%d", &choice);
    switch (choice) {
    case 1:
      // Função de Inserir
      break;
    case 2:
      // Função de Buscar
      break;
    case 3:
      // Função de Atualizar
      break;
    case 4:
      // Função de Excluir
      break;
    case 0:
      printf("Saindo do programa...\n");
      break;
    default:
      break;
    }
  } while (choice != 0);
  return 0;
}