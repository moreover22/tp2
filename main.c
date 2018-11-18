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
/* ******************************************************************
 *                        PROGRAMA PRINCIPAL
 * *****************************************************************/

hash_t *iniciar_operaciones(const char *nombre_comando[],
                            const comando_t *funcion_comando,
                            size_t cantidad_comandos) {
  hash_t *operaciones = hash_crear(NULL);
  if (!operaciones)
    return NULL;
  for (int i = 0; i < cantidad_comandos; i++)
    hash_guardar(operaciones, nombre_comando[i], funcion_comando[i]);
  return operaciones;
}

char **slice(char *arr[], int inicio, int fin) {
  char **resultado = malloc(sizeof(char *) * (fin - inicio));
  for (int i = 0; i < (fin - inicio); i++) {
    resultado[i] = strdup(arr[inicio + i]);
  }
  return resultado;
}

size_t len_entrada(char **entrada) {
  size_t i;
  for (i = 0; entrada && entrada[i]; i++)
    ;
  return i;
}

void free_args(char *args[]) {
  for (int i = 0; args && args[i]; i++)
    free(args[i]);
  free(args);
}

bool ejecutar(vuelos_t *vuelos, hash_t *operaciones, char *entrada[]) {
  char *comando = entrada[CMD];
  size_t len = len_entrada(entrada);
  char **args = slice(entrada, 1, (int)len);

  comando_t funcion = hash_obtener(operaciones, comando);
  if (!funcion)
    return false;
  return funcion(vuelos, args, len - 1);
  free_args(args);
}

int main(void) {

  const char *NOMBRE_COMANDO[] = {"agregar_archivo", "ver_tablero",
                                  "info_vuelo", "prioridad_vuelos", "borrar"};
  const comando_t FUNCION_COMANDO[] = {agregar_archivo, ver_tablero, info_vuelo,
                                       prioridad_vuelos, borrar};
  const size_t CANTIDAD_COMANDOS = 5;

  hash_t *operaciones =
      iniciar_operaciones(NOMBRE_COMANDO, FUNCION_COMANDO, CANTIDAD_COMANDOS);

  if (!operaciones)
    return ERROR;
  vuelos_t *vuelos = iniciar_vuelos();
  if (!vuelos) {
    hash_destruir(operaciones);
    return ERROR;
  }

  char *linea = NULL;
  size_t tam = 0;
  ssize_t len = 0;

  while ((len = getline(&linea, &tam, stdin)) > 0) {
    linea[len - 1] = '\0';
    char **entrada = split(linea, CMD_SEP);
    if (ejecutar(vuelos, operaciones, entrada))
      printf("OK\n");
    else
      fprintf(stderr, "Error en comando %s\n", entrada[CMD]);
    free_strv(entrada);
  }

  free(linea);

  hash_destruir(operaciones);
  finalizar_vuelos(vuelos);

  return 0;
}
