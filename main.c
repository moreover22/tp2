#define _POSIX_C_SOURCE 200809L
#include "analog.h"
#include "hash.h"
#include "strutil.h"
#include "testing.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CMD 0
#define CMD_SEP ' '
#define ERROR -1

typedef bool (*comando_t)(vuelos_t *, char **, size_t);

typedef struct func {
  comando_t *comando;
} func_t;
/* ******************************************************************
 *                        PROGRAMA PRINCIPAL
 * *****************************************************************/
comando_t obtener_comando(char *comando) {
  const char *NOMBRE_COMANDO[] = {"agregar_archivo", "ver_tablero",
                                  "info_vuelo", "prioridad_vuelos", "borrar"};
  comando_t FUNCION_COMANDO[] = {agregar_archivo, ver_tablero, info_vuelo,
                                 prioridad_vuelos, borrar};
  const size_t CANTIDAD_COMANDOS = 5;

  for (int i = 0; i < CANTIDAD_COMANDOS; i++) {
    if (strcmp(comando, NOMBRE_COMANDO[i]) != 0)
      continue;
    return FUNCION_COMANDO[i];
  }
  return NULL;
}

char **slice(char *arr[], int inicio, int fin) {
  char **resultado = calloc((fin - inicio + 1), sizeof(char *));
  for (int i = 0; i < (fin - inicio); i++) {
    resultado[i] = strdup(arr[inicio + i]);
  }
  return resultado;
}

size_t len_entrada(char **entrada) {
  size_t i;
  for (i = 0; entrada && entrada[i]; i++)
    ;
  return i++;
}

void free_args(char *args[]) {
  for (int i = 0; args[i]; i++)
    free(args[i]);
  free(args);
}

bool ejecutar(vuelos_t *vuelos, char *entrada[]) {
  char *comando = entrada[CMD];
  size_t len = len_entrada(entrada);
  char **args = slice(entrada, 1, (int)len);

  comando_t funcion = obtener_comando(comando);
  bool ok = false;
  if (funcion)
    ok = funcion(vuelos, args, len - 1);
  free_args(args);
  return ok;
}

int main(void) {
  vuelos_t *vuelos = iniciar_vuelos();
  if (!vuelos)
    return ERROR;
  char *linea = NULL;
  size_t tam = 0;
  ssize_t len = 0;

  while ((len = getline(&linea, &tam, stdin)) > 0) {
    linea[len - 1] = '\0';
    char **entrada = split(linea, CMD_SEP);
    if (ejecutar(vuelos, entrada))
      printf("OK\n");
    else if (*entrada[CMD]) // Evita saltos de linea
      fprintf(stderr, "Error en comando %s\n", entrada[CMD]);
    free_strv(entrada);
  }

  free(linea);

  finalizar_vuelos(vuelos);

  return 0;
}
