#ifndef FECHAUTIL_H
#define FECHAUTIL_H

#include <stddef.h>
typedef struct fecha fecha_t;

fecha_t *crearFecha(const char *str);
int fechacmp(const fecha_t a, const fecha_t b);
char *fechaAStr(fecha_t *fecha);

#endif // FECHAUTIL_H
