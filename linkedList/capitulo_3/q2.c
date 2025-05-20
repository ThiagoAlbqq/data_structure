#include <stdio.h>
void main()
{
    printf("A complexidade constante possui tempo de execução invariável,\n"
           "não sendo afetada pelo tamanho da entrada. Já a complexidade linear\n"
           "tem tempo de execução que cresce proporcionalmente ao tamanho da entrada.\n");

    printf("De maneira geral, a complexidade constante é mais eficiente que a linear.\n"
           "Contudo, em certas situações, a linear pode ser mais vantajosa. Um exemplo\n"
           "é quando se trabalha com entradas pequenas, como em Tabelas Hash. Nesses casos,\n"
           "f(1) pode ser menos eficiente por realizar muitas operações para poucos elementos,\n"
           "enquanto f(n) se torna mais rápido devido à proporcionalidade entre elementos\n"
           "e operações realizadas.\n");
}