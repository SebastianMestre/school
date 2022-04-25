#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_LINE 4096
#define MAX_ARGS 2048
#define MAX_COMS 64

char* alloc_string(char const* src, char const* end) {
  int const len = end - src;
  char *result = malloc(len + 1);
  memcpy(result, src, len);
  result[len] = '\0';
  return result;
}

char const* skip_spaces(char const* p) {
  while (*p == ' ')
    p++;
  return p;
}

bool is_command_terminator(char c) {
  return c == '\n' || c == '\0';
}

bool is_command_delimiter(char c) {
  return c == ' ' || c == '\n' || c == '\0';
}

char** parse_command(char const* input) {
  char** result = malloc(sizeof(char *) * MAX_ARGS);
  char** it = result;

  for (char const* l = skip_spaces(input); !is_command_terminator(*l); l = skip_spaces(l)) {
    assert(it - result < MAX_ARGS);

    char const* f = l;

    while (!is_command_delimiter(*l))
      l++;

    *it++ = alloc_string(f, l);
  }
  *it = NULL;

  return result;
}

char*** parse_command_list(char* input) {
  char*** result = malloc(sizeof(*result) * MAX_COMS);
  char*** it = result;

  strtok(input, "|");
  char* tok = input;
  do {
    *it++ = parse_command(tok);
  } while ((tok = strtok(NULL, "|")));
  *it = NULL;

  return result;
}

void free_command_list(char*** commands) {
  for (int c = 0; commands[c] != NULL; c++) {
    char** args = commands[c];
    for (int i = 0; args[i] != NULL; i++) {
      free(args[i]);
    }
    free(args);
  }
  free(commands);
}

int main(int argc, char **argv) {

  char input[MAX_LINE];
  int wstatus;

  while (1) {
    printf(">> ");
    // fflush(stdin);
    fgets(input, MAX_LINE, stdin);

    if (strcmp(input, "exit\n") == 0 || strcmp(input, "exit") == 0)
      break;

    char*** commands = parse_command_list(input);

    int command_count = 0;
    while (commands[command_count] != NULL) command_count++;

    int pn[2] = {-1, -1}; // pipe nuevo
    int pv[2] = {-1, -1}; // pipe viejo

    for (int c = 0; c < command_count; c++) {

      // si no soy el ultimo comando, armo un pipe para poder hablar con el comando siguiente
      if (c < command_count-1) {
        pipe(pn);
      }

      pid_t pid = fork();

      if (pid < 0) {
        perror("Fork has failed\n");
      } else if (pid == 0) {
        // soy el proceso hijo

        // si no soy el ultimo, tengo que escribir en un pipe, para que el siguiente proceso lea
        if (c < command_count-1) {
          close(1); // cerrar el stdout
          int r = dup(pn[1]);
          assert(r == 1);
          close(pn[0]); // no leo de ese pipe
        }

        // si no soy el primero, tengo que leer del pipe en el que el proceso anterior escribe
        if (c > 0) {
          close(0); // cerrar el stdin
          int r = dup(pv[0]);
          assert(r == 0);
          close(pv[1]); // no escribo en ese pipe
        }

        execv(commands[c][0], commands[c]);
        exit(0);
      }

      if (c > 0) {
        close(pv[0]);
        close(pv[1]);
      }
      pv[0] = pn[0];
      pv[1] = pn[1];
    }

    for (int c = 0; c < command_count; ++c)
      wait(&wstatus);

    free_command_list(commands);
  }

  return 0;
}

