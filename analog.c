#include "analog.h"
#include "fechautil.h"
#include "hash.h"
#include "strutil.h"
#include <string.h>
typedef struct vuelo {
  int numero;
  char *aerolinea;
  char *origen;
  char *destino;
  char *tail_number;
  int prioridad;
  fecha_t *fecha;
  int departure_delay;
  int tiempo_vuelo;
  int cancelado;
} vuelo_t;

int vueloscmp(void *a, void *b) {
  vuelo_t *v_a = (vuelo_t *)a;
  vuelo_t *v_b = (vuelo_t *)b;
  return v_a->numero - v_b->numero;
}

// ESTA SIRVE PARA EL HEAP DE PRIORIDAD
// int vueloscmp(void *a, void *b) {
//   vuelo_t *v_a = (vuelo_t *)a;
//   vuelo_t *v_b = (vuelo_t *)b;
//   int fecha_comp = fechacmp(v_a->fecha, v_b->fecha);
//   if (fecha_comp != 0)
//     return fecha_comp;
//   return v_a->numero - v_b->numero;
// }

hash_t *iniciar_vuelos() {
  hash_t *vuelos = hash_crear(vueloscmp);
  if (!vuelos)
    return NULL;
  return vuelos;
}

bool agregar_archivo(hash_t *vuelos, const char *nombre_archivo) {}
bool ver_tablero(hash_t *vuelos, int cant_vuelos, const char *modo,
                 fecha_t desde, fecha_t hasta) {}
bool info_vuelo(hash_t *vuelos, const char *cod_vuelo) {}

bool prioridad_vuelos(hash_t *vuelos, int cant_vuelos) {}

bool borrar(hash_t *vuelos, fecha_t desde, decha_t hasta) {}
