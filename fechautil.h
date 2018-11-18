#ifndef FECHAUTIL_H
#define FECHAUTIL_H

#include <stdbool.h>
#include <stddef.h>
typedef struct fecha fecha_t;

/*
 *
 */
fecha_t *fecha_crear(const char *str);

/*
 *
 */
char *fecha_a_str(fecha_t *fecha);

/*
 *
 */
int fechacmp(const fecha_t *a, const fecha_t *b);

/*
 *
 */
bool fecha_valida(const char *str);

/* Sirve para incrementar una fecha.
 * No se asegura que la fecha sea válida.
 */
void fecha_sumar_segundos(fecha_t *fecha, int segundos);

#endif // FECHAUTIL_H
