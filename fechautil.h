#ifndef FECHAUTIL_H
#define FECHAUTIL_H

#include <stdbool.h>
#include <stddef.h>
typedef struct fecha fecha_t;

/* Dada una cadena de caracteres en el formato
 * "YYYY-MM-DDTHH:MM:SS" devuelve una estrucutra
 * de fecha. En caso de no poder allocar memoria
 * devuelve NULL.
 */
fecha_t *fecha_crear(const char *str);

/* Dada una estructura de fecha, devuelve una cadena
 * con el formato "YYYY-MM-DDTHH:MM:SS".
 * Se debe liberar la memoria de la cadena manualmente.
 */
char *fecha_a_str(fecha_t *fecha);

/* Dada dos fechas a, b, devolver:
 *   menor a 0  si  a < b
 *       0      si  a == b
 *   mayor a 0  si  a > b
 */
int fechacmp(const fecha_t *a, const fecha_t *b);

/*
 * Devuelve true si el string cumple con el formato
 * "YYYY-MM-DDTHH:MM:SS". False caso contrario.
 */
bool fecha_valida(const char *str);

/* Sirve para incrementar una fecha.
 * No se asegura que la fecha sea válida.
 */
void fecha_sumar_segundos(fecha_t *fecha, int segundos);

#endif // FECHAUTIL_H
