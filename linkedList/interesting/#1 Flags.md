Caso quisermos dar a inicialização com : -m : -p ou - m : -s ou casos relacionados :

```c
if (strncmp(argv[i], "-m:", 3) == 0) {
    char *valor = argv[i] + 3;
    if (strcmp(valor, "-p") == 0)
    {
        mode = ALLOW_OVERFLOW;
    }
    else if (strcmp(valor, "-s") == 0)
    {
        mode = STRICT_MODE;
    }
    else
    {
        fprintf(stderr, "Invalid mode: %s. Use '-p' ou '-s'.\n", valor);
        return 1;
    }
}
```
