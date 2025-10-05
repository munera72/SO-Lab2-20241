#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

// Elimina espacios al inicio y fin de una cadena
static char *trim(char *str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str; // cadena vacía
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    return str;
}

// Parsea una línea completa en una lista de comandos
int parse_line(char *line, Command commands[], int *num_commands) {
    char *cmd_str;
    int count = 0;

    // Separar por '&' para detectar comandos paralelos
    while ((cmd_str = strsep(&line, "&")) != NULL) {
        cmd_str = trim(cmd_str);
        if (strlen(cmd_str) == 0) continue;

        Command cmd = { .redirect_file = NULL };
        char *redir_part = NULL;

        // Buscar redirección '>'
        if ((redir_part = strchr(cmd_str, '>')) != NULL) {
            *redir_part = '\0';          // separar la parte izquierda
            redir_part++;                // mover al nombre del archivo
            redir_part = trim(redir_part);

            // Si hay más de un archivo o formato inválido
            char *extra = strchr(redir_part, ' ');
            if (extra != NULL) {
                // solo permitimos 1 archivo
                fprintf(stderr, "An error has occurred\n");
                return -1;
            }
            cmd.redirect_file = redir_part;
        }

        // Tokenizar los argumentos
        char *token;
        int i = 0;
        while ((token = strsep(&cmd_str, " \t")) != NULL) {
            if (strlen(token) == 0) continue;
            cmd.argv[i++] = token;
            if (i >= MAX_ARGS - 1) break;
        }
        cmd.argv[i] = NULL;

        // Ignorar si no hay comando
        if (cmd.argv[0] == NULL)
            continue;

        commands[count++] = cmd;
    }

    *num_commands = count;
    return 0;
}
