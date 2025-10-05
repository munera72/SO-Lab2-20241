#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   // para write()
#include <sys/types.h>
#include "parser.h"
#include <fcntl.h>   // para open()
#include <sys/wait.h> // para wait()
#include <sys/stat.h>


#define MAX_LINE 1024
#define MAX_PATHS 64

char *paths[MAX_PATHS];
int num_paths = 0;

// Mensaje de error estándar (como exige el enunciado)
void print_error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

// Busca un ejecutable en los paths actuales
char* find_executable(char *command) {
    static char fullpath[1024];
    for (int i = 0; i < num_paths; i++) {
        snprintf(fullpath, sizeof(fullpath), "%s/%s", paths[i], command);
        if (access(fullpath, X_OK) == 0) {
            return fullpath;
        }
    }
    return NULL;
}

// Procesa una línea: por ahora solo reconoce "exit"
void process_command(char *line) {
    Command commands[32];
    int num_commands = 0;

    // Usar el parser
    if (parse_line(line, commands, &num_commands) == -1) {
        // Error ya mostrado
        return;
    } 

    for (int i = 0; i < num_commands; i++) {
        Command *cmd = &commands[i];

        // Si no hay comando, saltar
        if (cmd->argv[0] == NULL) continue;

        // Built-in: exit
        if (strcmp(cmd->argv[0], "exit") == 0) {
            if (cmd->argv[1] != NULL) {
                print_error(); // exit no acepta argumentos
                continue;
            }
            exit(0);
        }

        // Built-in: cd
        else if (strcmp(cmd->argv[0], "cd") == 0) {
            if (cmd->argv[1] == NULL || cmd->argv[2] != NULL) {
                print_error(); // cd requiere exactamente 1 argumento
            } else if (chdir(cmd->argv[1]) != 0) {
                print_error();
            }
        }
        
        // Built-in: path
        else if (strcmp(cmd->argv[0], "path") == 0) {
            // liberar paths anteriores
            for (int j = 0; j < num_paths; j++) free(paths[j]);
            num_paths = 0;

            for (int j = 1; cmd->argv[j] != NULL; j++) {
                if (num_paths < MAX_PATHS) {
                    paths[num_paths++] = strdup(cmd->argv[j]);
                }
            }
        }

        // Externos (todavía no ejecutamos, solo mostramos)
        else {
            char *exec_path = find_executable(cmd->argv[0]);
            if (exec_path == NULL) {
                print_error();
                continue;
            }

            pid_t pid = fork();
            if (pid < 0) {
                print_error();
                continue;
            }

            if (pid == 0) {  // Hijo
                // Si hay redirección
                if (cmd->redirect_file != NULL) {
                    int fd = open(cmd->redirect_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
                    if (fd < 0) {
                        print_error();
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    dup2(fd, STDERR_FILENO);
                    close(fd);
                }

                execv(exec_path, cmd->argv);
                // Si llega aquí, execv falló
                print_error();
                exit(1);
            } else {
                // Padre: esperar al hijo
                int status;
                waitpid(pid, &status, 0);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    FILE *input = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    paths[0] = strdup("/bin");
    num_paths = 1;

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
