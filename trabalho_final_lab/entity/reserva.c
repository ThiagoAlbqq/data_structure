#include <stdio.h>

typedef struct {
  int id;
  int quarto_id;      // Refere ao Quarto
  int cliente_id;     // Refere ao Cliente
  char check_in[11];  // "YYYY-MM-DD"
  char check_out[11]; // "YYYY-MM-DD"
  int quantidade_pessoas;
  float preco_total;
} Reserva;