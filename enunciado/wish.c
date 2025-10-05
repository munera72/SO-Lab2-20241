#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // para write()
#include <sys/types.h>

#define MAX_LINE 1024

// Mensaje de error estándar (como exige el enunciado)
void print_error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

// Procesa una línea: por ahora solo reconoce "exit"
void process_command(char *line) {
    // Ignorar líneas vacías o solo espacios
    if (line == NULL || strlen(line) == 0)
        return;

    // Comando integrado: exit
    if (strcmp(line, "exit") == 0) {
        exit(0);
    }

    // Por ahora, solo mostrar lo leído (para depuración)
    printf("Comando leído: %s\n", line);
}

int main(int argc, char *argv[]) {
    FILE *input = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    // Validación de argumentos
    if (argc == 1) {
        // Modo interactivo
        input = stdin;
    } else if (argc == 2) {
        // Modo batch
        input = fopen(argv[1], "r");
        if (input == NULL) {
            print_error();
            exit(1);
        }
    } else {
        // Más de un argumento: error y salida
        print_error();
        exit(1);
    }

    // Bucle principal del shell
    while (1) {
        // Mostrar prompt solo en modo interactivo
        if (input == stdin) {
            printf("wish> ");
            fflush(stdout);
        }

        nread = getline(&line, &len, input);
        if (nread == -1) {
            // EOF → terminar normalmente
            break;
        }

        // Eliminar salto de línea final
        if (line[nread - 1] == '\n') {
            line[nread - 1] = '\0';
        }

        // Ignorar líneas vacías o con solo espacios
        if (strspn(line, " \t") == strlen(line)) {
            continue;
        }

        // Procesar la línea leída
        process_command(line);
    }

    // Limpieza final
    free(line);
    if (input != stdin) {
        fclose(input);
    }

    return 0;
}
