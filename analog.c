#include "analog.h"
#include "fechautil.h"
#include "hash.h"
#include "strutil.h"
typedef struct vuelo {
  int numero;
  char *aerolinea;
  char *fecha_t *fecha;

} vuelo_t;
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
