#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    // Validación: solo se permite 0 o 1 argumento (para batch mode luego)
    if (argc > 2) {
        fprintf(stderr, "An error has occurred\n");
        exit(1);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while (1) {
        if (argc == 1) {
            printf("wish> ");
        }

        nread = getline(&line, &len, stdin);
        if (nread == -1) {
            exit(0);
        }

        if (line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
        }

        if (strcmp(line, "exit") == 0) {
            exit(0);
        }

    }

    free(line);
    return 0;
}
