#ifndef ANALOG_H
#define ANALOG_H

#include "fechautil.h"
#include "hash.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct vuelos vuelos_t;

/* Inicializa la estructura de tipo vuelo y la devuelve.
 * En caso de que no pueda generarla o allocar memoria,
 * la función devuelve NULL.
 */
vuelos_t *iniciar_vuelos();
/* Dada una estructura de vuelo ya inicializada,
 * libera la memoria correspondiente la estructura
 * de vuelo.
 */
void finalizar_vuelos(vuelos_t *vuelos);

/* Dada una estrucutra de vuelos y un array args donde contiene
 * el nombre del archivo a cargar, la función actualiza la estructura
 * vuelos con los datos del archivo indicado.
 * argc: longitud de args.
 */
bool agregar_archivo(vuelos_t *vuelos, char **args, size_t argc);

/* Muestra en pantalla la información de vuelo en base a:
 * K cantidad vuelos: cantidad K de vuelos a mostrar
 * modo: asc/desc : cadena con “asc” o “desc”, indican el orden a elegir
 * utilizando el campo fecha de despegue
 * desde: cadena en formato YYYY-MM-DDTHH:MM:SS que indica el tiempo desde que
 * se tienen que mostrar los vuelos, los vuelos con una fecha de despegue
 * anteriores al tiempo ingresado no se tienen que mostrar.
 * hasta: cadena en formato YYYY-MM-DDTHH:MM:SS que indica el tiempo hasta que
 * se tienen que mostrar los vuelos, los vuelos con una fecha de despegue
 * posteriores al tiempo ingresado no se tienen que mostrar.
 *
 * Almacenados en ese orden en args.
 * argc: longitud de args.
 */
bool ver_tablero(vuelos_t *vuelos, char **args, size_t argc);

/* Dado un numero de vuelo (almacenado en args),
 * devuelve toda la información de ese vuelo almacenado
 * en la estructura de vuelos.
 * argc: longitud de args.
 */
bool info_vuelo(vuelos_t *vuelos, char **args, size_t argc);
/* Muestra en pantalla los K vuelos con
 * mayor prioridad de vuelos (K almacenado en args),
 * argc: longitud de args.
 */
bool prioridad_vuelos(vuelos_t *vuelos, char **args, size_t argc);
/* Dada una estructura de vuelos ya inicializada, borra toda la información
 * comprendida entre las fechas desde, hasta (almacenadas en el array args)
 * argc: longitud de args.
 */
bool borrar(vuelos_t *vuelos, char **args, size_t argc);

#endif // ANALOG_H
