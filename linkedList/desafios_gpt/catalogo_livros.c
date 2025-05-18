#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Livro
{
    char *titulo;
    char *autor;
    int ano;
} livro;

// Funcionalidade para um arquivo TXT ser o nosso banco de dados '-'
livro *carregar_livros(const char *filename, int *tam);
void adicionar_livro(livro *biblioteca, int tam, char *filename);

livro create_livro(char *titulo, char *autor, int ano);
livro *add_book(livro *biblioteca, int *tam, livro book);

void print_menu();

int main()
{
    int tam = 0;
    livro *biblioteca = carregar_livros("biblioteca.txt", &tam);

    int sair = 0;
    while (!sair)
    {
        print_menu();
        int choice;
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            for (int i = 0; i < tam; i++)
            {
                printf("Livro %d:\n", i + 1);
                printf("  Título: %s\n", biblioteca[i].titulo);
                printf("  Autor: %s\n", biblioteca[i].autor);
                printf("  Ano: %d\n\n", biblioteca[i].ano);
            }
            break;

        case 2:
        { // Adicionar livro
            char titulo[100], autor[100];
            int ano;

            printf("\nAdicionando novo livro:\n");
            printf("  Digite o Título: ");
            fgets(titulo, sizeof(titulo), stdin);
            titulo[strcspn(titulo, "\n")] = '\0'; // Remove \n

            printf("  Digite o Autor: ");
            fgets(autor, sizeof(autor), stdin);
            autor[strcspn(autor, "\n")] = '\0';

            printf("  Digite o Ano: ");
            scanf("%d", &ano);
            getchar(); // Limpa o buffer

            livro novo = create_livro(titulo, autor, ano);
            biblioteca = add_book(biblioteca, &tam, novo);
            adicionar_livro(biblioteca, tam, "biblioteca.txt");
            break;
        }

        case 3:
            printf("SAINDUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU.............");
            break;

        default:
            printf("Tem que colocar um numero entre 1 e 3 pô");
            break;
        }
    }

    for (int i = 0; i < tam; i++)
    {
        free(biblioteca[i].titulo);
        free(biblioteca[i].autor);
    }
    free(biblioteca);

    return 0;
}

void print_menu()
{
    printf("--------------------------------------\n"
           "         Bilioteca da JULIA           \n"
           "--------------------------------------\n"
           "MENU: --------------------------------\n"
           "1. Listar livros\n"
           "2. Adicionar livro\n"
           "3. Sair\n"
           "--------------------------------------\n");
}

livro *add_book(livro *biblioteca, int *tam, livro book)
{
    livro *temp = realloc(biblioteca, sizeof(livro) * (*tam + 1));
    if (temp == NULL)
    {
        free(temp);
        exit(1);
    }
    biblioteca = temp;
    biblioteca[*tam] = create_livro(book.titulo, book.autor, book.ano);
    (*tam)++;

    return biblioteca;
}

livro create_livro(char *titulo, char *autor, int ano)
{
    livro l;
    l.titulo = malloc(strlen(titulo) + 1);
    strcpy(l.titulo, titulo);
    l.autor = malloc(strlen(autor) + 1);
    strcpy(l.autor, autor);
    l.ano = ano;
    return l;
}

// Funcao para acessar o "banco de dados", entre muitas aspas - final boss
livro *carregar_livros(const char *filename, int *tam)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Aquivo não encontrado!");
        return NULL;
    }

    livro *biblioteca = NULL;
    char linha[256];
    while (fgets(linha, sizeof(linha), file))
    {
        linha[strcspn(linha, "\n")] = '\0';

        char *titulo = strtok(linha, ",");
        char *autor = strtok(NULL, ",");
        char *ano_str = strtok(NULL, ",");

        if (titulo == NULL || autor == NULL || ano_str == NULL)
        {
            printf("Formato inválido no arquivo: %s\n", linha);
            continue; // Pula para a próxima linha
        }

        if (titulo && autor && ano_str)
        {
            int ano = atoi(ano_str);
            livro book = create_livro(titulo, autor, ano);
            biblioteca = add_book(biblioteca, tam, book);
        }
    }

    fclose(file);
    return biblioteca;
}

void adicionar_livro(livro *biblioteca, int tam, char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }

    for (int i = 0; i < tam; i++)
    {
        fprintf(file, "%s,%s,%d\n", biblioteca[i].titulo, biblioteca[i].autor, biblioteca[i].ano);
    }

    fclose(file);
}