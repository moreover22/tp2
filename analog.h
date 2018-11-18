#ifndef ANALOG_H
#define ANALOG_H

#include "fechautil.h"
#include "hash.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct vuelos vuelos_t;

/*
 *
 */
vuelos_t *iniciar_vuelos();
/*
 *
 */
void finalizar_vuelos(vuelos_t *vuelos);
/*
 *
 */
bool agregar_archivo(vuelos_t *vuelos, char **args, size_t argc);

/*
 *
 */
bool ver_tablero(vuelos_t *vuelos, char **args, size_t argc);
/*
 *
 */
bool info_vuelo(vuelos_t *vuelos, char **args, size_t argc);
/*
 *
 */
bool prioridad_vuelos(vuelos_t *vuelos, char **args, size_t argc);
/*
 *
 */
bool borrar(vuelos_t *vuelos, char **args, size_t argc);
/*
 *
 */
void pruebas_tp2_alumno();
#endif // ANALOG_H
