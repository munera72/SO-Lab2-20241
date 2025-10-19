cat > README.md <<'EOF'
README - Explicación del código y funcionamiento de la shell

Descripción general:
Este repositorio implementa una shell simple. El flujo principal es: inicialización → bucle de lectura → parseo → ejecución → limpieza.

Estructura y funciones clave:
- main(): inicia la shell, configura señales y entra en el bucle principal.
- prompt(): muestra el prompt y prepara la entrada del usuario.
- read_input(): lee una línea desde stdin (maneja EOF).
- tokenize()/lex(): divide la línea en tokens (palabras, operadores como |, >, <, &).
- parse(): construye una estructura de comandos a partir de los tokens (comando simple, tuberías, redirecciones, ejecución en segundo plano).
- expand_variables(): sustituye variables de entorno y manejo de comillas.
- is_builtin()/run_builtin(): detecta y ejecuta comandos internos (ej.: cd, exit, export).
- launch_process()/exec_command(): realiza fork() y execvp() para comandos externos; gestiona redirecciones antes de exec.
- handle_pipes(): crea pipes para conectar procesos en una tubería.
- handle_redirection(): redirige stdin/stdout/stderr según los operadores (<, >, >>).
- wait_children()/reap_zombies(): espera procesos en primer plano y limpia procesos terminados.
- signal_handlers(): captura señales (SIGINT, SIGCHLD) para control de interrupciones y recolección de hijos.

Cómo funciona la shell (resumen operativo):
1. El usuario introduce una línea; la shell la tokeniza.
2. Se parsea para identificar comandos, argumentos, redirecciones y tuberías.
3. Para comandos internos, se ejecutan en el mismo proceso sin fork en la mayoría de los casos.
4. Para comandos externos, la shell hace fork(); en el hijo aplica redirecciones y pipes, y llama a exec.
5. La shell conecta múltiples procesos con pipes cuando hay '|'.
6. Si el comando termina con '&', se ejecuta en segundo plano y la shell no espera su finalización inmediata.
7. Se manejan variables de entorno, expansión de tildes y comillas simples/dobles según reglas de shell.
8. Las señales interruptoras (ej. Ctrl+C) son controladas para no terminar la shell principal inadvertidamente; SIGCHLD se usa para reaper procesos hijos.

Notas de implementación:
- Mantener separación clara entre parseo y ejecución facilita añadir características (alias, historial, scripting).
- Probar casos: redirecciones encadenadas, pipes múltiples, ejecuciones en background y manejo de errores del sistema.
