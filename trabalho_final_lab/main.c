#include "./interface/menu.h"
#include "./repository/quarto.h"
#include <stdio.h>

int main() {
  int choice = 0;
  do {
    // Chamada da função
    show_menu("TESTANDO");
    scanf("%d", &choice);
    while (getchar() != '\n')
      ;
    switch (choice) {
    case 1:
      // Função de Inserir
      break;
    case 2:
      // Função de Buscar
      get_rooms();
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
      printf("Opção inválida! Tente novamente.\n");
      break;
    }
  } while (choice != 0);
  return 0;
}