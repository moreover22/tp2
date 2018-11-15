#ifndef ANALOG_H
#define ANALOG_H

#include "fechautil.h"
#include "hash.h"
#include <stdbool.h>
#include <stddef.h>
/*
 *
 */
hash_t *iniciar_vuelos();
/*
 *
 */
bool agregar_archivo(hash_t *vuelos, const char *nombre_archivo);

/*
 *
 */
// Creo que tiene que recibir el hash de vuelos también.
bool ver_tablero(hash_t *vuelos, int cant_vuelos, const char *modo,
                 fecha_t desde, fecha_t hasta);
/*
 *
 */
// idem ver_tablero
bool info_vuelo(hash_t *vuelos, const char *cod_vuelo);
/*
 *
 */
bool prioridad_vuelos(hash_t *vuelos, int cant_vuelos);
/*
 *
 */
bool borrar(hash_t *vuelos, fecha_t desde, decha_t hasta);
/*
 *
 */
void pruebas_tp2_alumno();
#endif // ANALOG_H
