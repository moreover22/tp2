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
/* Dado una cadena de caracteres, la funcion devuelve un puntero
 * a la función correspondiente con ese comando.
 * En caso de que el comando sea inválido, devuelve NULL
 */
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
/* Dado un array de cadenas de caracteres, devuelve
 * un nuevo array que tiene los mismos elementos del
 * array original desde inicio hasta final.
 * El array debe ser liberado manualmente.
 */
char **slice(char *arr[], int inicio, int fin) {
  char **resultado = calloc((fin - inicio + 1), sizeof(char *));
  for (int i = 0; i < (fin - inicio); i++) {
    resultado[i] = strdup(arr[inicio + i]);
  }
  return resultado;
}
/* Dado un vector de cadenas de caracteres, con el último elemento NULL
 * la función devuelve la longitud del vector hasta NULL.
 */
size_t len_entrada(char **entrada) {
  size_t i;
  for (i = 0; entrada[i]; i++)
    if (strlen(entrada[i]) == 0)
      return 0;
  return i++;
}
/* Dado un array de cadenas, libera la memoria de
 * cada una de las cadenas y del array en sí.
 */
void free_args(char *args[]) {
  for (int i = 0; args[i]; i++)
    free(args[i]);
  free(args);
}

/* Dado un puntero a la estructura de vuelos,
 * previamente inicializado, y una entrada de
 * usuario (vector de cadena de caracteres cuyo
 * primer elemento es el comando a ejecutar)
 * Ejecuta el comando y devuelve True si pudo
 * ejecutar el comando correctamente.
 */
bool ejecutar(vuelos_t *vuelos, char *entrada[]) {
  char *comando = entrada[CMD];
  size_t len = len_entrada(entrada);
  if (len == 0)
    return false;
  char **args = slice(entrada, 1, (int)len);

  comando_t funcion = obtener_comando(comando);
  bool ok = false;
  if (funcion)
    ok = funcion(vuelos, args, len - 1);
  free_args(args);
  return ok;
}

/* Función principal del programa.
 */
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
