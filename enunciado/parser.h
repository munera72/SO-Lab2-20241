// parser.h
#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 32

typedef struct {
    char *argv[MAX_ARGS];   // lista de argumentos terminada en NULL
    char *redirect_file;    // archivo de redirección (NULL si no hay)
} Command;

// Funciones del parser
int parse_line(char *line, Command commands[], int *num_commands);

#endif
