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
  char *numero_cola;
  int prioridad;
  fecha_t *fecha;
  int retraso_salida;
  int tiempo_vuelo;
  int cancelado;
} vuelo_t;

// ESTA SIRVE PARA EL HEAP DE PRIORIDAD
int vueloscmp(void *a, void *b) {
  vuelo_t *v_a = (vuelo_t *)a;
  vuelo_t *v_b = (vuelo_t *)b;
  int fecha_comp = fechacmp(v_a->fecha, v_b->fecha);
  if (fecha_comp != 0)
    return fecha_comp;
  return v_a->numero - v_b->numero;
}

hash_t *iniciar_vuelos() {
  hash_t *vuelos = hash_crear(free);
  if (!vuelos)
    return NULL;
  return vuelos;
}

bool agregar_archivo(hash_t *vuelos, const char *nombre_archivo) {}
bool ver_tablero(hash_t *vuelos, int cant_vuelos, const char *modo,
                 fecha_t desde, fecha_t hasta) {}
bool info_vuelo(hash_t *vuelos, const char *cod_vuelo) {
  vuelo_t *vuelo = (vuelo_t *)hash_obtener(vuelos, cod_vuelo);
  if (!vuelo)
    return false;
  char *fecha_hora = fechaAStr(vuelo->fecha);
  printf("%d %s %s %s %d %s %d %d %d\n", vuelo->numero, vuelo->aerolinea,
         vuelo->origen, vuelo->destino, vuelo->numero_cola, vuelo->prioridad,
         fecha_hora, vuelo->retraso_salida, vuelo->tiempo_vuelo,
         vuelo->cancelado);

  free(fecha_hora);
  return false;
}

bool prioridad_vuelos(hash_t *vuelos, int cant_vuelos) {}

bool borrar(hash_t *vuelos, fecha_t desde, decha_t hasta) {}
