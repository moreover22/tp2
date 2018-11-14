#define _POSIX_C_SOURCE 200809L
#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TAM_INICIAL 101
#define FACTOR_CARGA 0.5

typedef enum estado { OCUPADO, VACIO, BORRADO } estado_t;

typedef struct elemento {
  void *dato;
  char *clave;
  estado_t estado;
} elemento_t;

typedef struct hash {
  size_t cantidad;
  size_t capacidad;
  size_t borrados;
  elemento_t *tabla;
  hash_destruir_dato_t funcion_destruir;
} hash_t;

typedef struct hash_iter {
  const hash_t *hash;
  elemento_t *actual;
  size_t posicion;
} hash_iter_t;

/*
 * Dado una cadena de caracteres,
 * devuelve un numero de hash
 * generado a partir de la cadena
 * con el algoritmo djb2.
 */
unsigned long hashing(const char *str);

/*
 * Dado una clave y la capacidad
 * devuelve una posición entre
 * 0 y capacidad.
 */
size_t hash_posicion(size_t capacidad, const char *clave);

/*
 * Dado un hash y una clave, la
 * función devuelve el elemento
 * que corresponda a esa clave
 * o NULL en caso de que no exista
 * dicha clave.
 * Pre: hash fue creado.
 */
elemento_t *hash_obtener_elemento(const hash_t *hash, const char *clave);

/*
 * Dada una tabla de elementos y su
 * capacidad, la función inicializa
 * sus valores.
 * Pre: la memoria para la tabla
 * fue previamente reservada.
 * Post: todos los valores de la
 * tabla van a ser NULL/VACIO según
 * corresponda.
 */
void tabla_valores_iniciales(elemento_t *tabla, size_t capacidad);

/*
 * Dado un hash y una nueva_capacidad
 * se redimensiona la tabla de hash
 * con la nueva dimensión y se reubican
 * todos sus elementos.
 * Pre: hash fue creado.
 * Post: la tabla de hash pasará
 * a tener la nueva capacidad.
 */
bool hash_redimensionar(hash_t *hash, size_t nueva_capacidad);

/*
 * Dado un hash y una posicion
 * devuelve la proxima posición
 * vacia del hash. Devuelve -1
 * en caso de que no haya podido
 * encontrarlo.
 * Pre: hash fue creado posicion es menor
 * la capacidad del hash.
 */
int encontrar_prox_vacio(hash_t *hash, size_t posicion);

void pasar_tabla(hash_t *hash, elemento_t *vieja_tabla, size_t capacidad_vieja);

/* ******************************************************************
 *                        PRIMITIVAS HASH
 * *****************************************************************/

hash_t *hash_crear(hash_destruir_dato_t destruir_dato) {

  hash_t *hash = malloc(sizeof(hash_t));
  if (!hash)
    return NULL;

  hash->funcion_destruir = destruir_dato;
  hash->tabla = malloc(TAM_INICIAL * sizeof(elemento_t));

  if (!hash->tabla) {
    free(hash);
    return NULL;
  }

  hash->capacidad = TAM_INICIAL;
  hash->cantidad = 0;
  hash->borrados = 0;
  tabla_valores_iniciales(hash->tabla, hash->capacidad);

  return hash;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato) {

  if (((float)hash->cantidad / (float)hash->capacidad) >= FACTOR_CARGA)
    if (!hash_redimensionar(hash, hash->capacidad * 2))
      return false;

  elemento_t *elemento = hash_obtener_elemento(hash, clave);
  size_t posicion = hash_posicion(hash->capacidad, clave);

  if (elemento) {
    if (hash->funcion_destruir && hash->tabla[posicion].dato) {
      hash->funcion_destruir(hash->tabla[posicion].dato);
      hash->tabla[posicion].dato = NULL;
    }
    hash->tabla[posicion].dato = dato;
    return true;
  }

  int nueva_posicion = encontrar_prox_vacio(hash, posicion);
  if (nueva_posicion < 0)
    return false;
  hash->tabla[nueva_posicion].clave = strdup(clave);
  hash->tabla[nueva_posicion].dato = dato;
  hash->tabla[nueva_posicion].estado = OCUPADO;

  hash->cantidad++;
  return true;
}

void *hash_borrar(hash_t *hash, const char *clave) {

  elemento_t *elemento = hash_obtener_elemento(hash, clave);
  if (!elemento)
    return NULL;
  elemento->estado = BORRADO;
  free(elemento->clave);
  elemento->clave = NULL;
  hash->borrados++;
  return elemento->dato;
}

void *hash_obtener(const hash_t *hash, const char *clave) {
  elemento_t *elemento = hash_obtener_elemento(hash, clave);
  if (!elemento)
    return NULL;
  return elemento->dato;
}

bool hash_pertenece(const hash_t *hash, const char *clave) {
  return hash_obtener_elemento(hash, clave) != NULL;
}

size_t hash_cantidad(const hash_t *hash) {
  return hash->cantidad - hash->borrados;
}

void hash_destruir(hash_t *hash) {
  for (size_t i = 0; i < hash->capacidad; i++) {
    if (hash->tabla[i].estado != OCUPADO)
      continue;
    if (hash->funcion_destruir)
      hash->funcion_destruir(hash->tabla[i].dato);
    free(hash->tabla[i].clave);
  }
  free(hash->tabla);
  free(hash);
}

/* ******************************************************************
 *                        PRIMITIVAS ITERADOR HASH
 * *****************************************************************/

hash_iter_t *hash_iter_crear(const hash_t *hash) {
  hash_iter_t *iter = malloc(sizeof(hash_iter_t));
  if (!iter)
    return NULL;

  iter->hash = hash;
  iter->posicion = 0;
  iter->actual = &(hash->tabla[0]);

  if (hash->tabla[0].estado != OCUPADO) {
    hash_iter_avanzar(iter);
  }
  return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter) {
  if (hash_iter_al_final(iter))
    return false;
  for (size_t i = iter->posicion + 1; i < iter->hash->capacidad; i++) {
    if (iter->hash->tabla[i].estado == OCUPADO) {
      iter->posicion = i;
      iter->actual = &(iter->hash->tabla[i]);
      return true;
    }
  }
  iter->actual = NULL;
  return false;
}
const char *hash_iter_ver_actual(const hash_iter_t *iter) {
  if (!iter->actual)
    return NULL;
  return iter->actual->clave;
}
bool hash_iter_al_final(const hash_iter_t *iter) { return !iter->actual; }
void hash_iter_destruir(hash_iter_t *iter) { free(iter); }

/* ******************************************************************
 *                        FUNCIONES EXTRA
 * *****************************************************************/

unsigned long hashing(const char *str) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}

size_t hash_posicion(size_t capacidad, const char *clave) {
  return (size_t)hashing(clave) % capacidad;
}

elemento_t *hash_obtener_elemento(const hash_t *hash, const char *clave) {
  size_t posicion = hash_posicion(hash->capacidad, clave);

  for (size_t i = 0; i < hash->capacidad; i++) {
    size_t nueva_posicion = (posicion + i) % hash->capacidad;

    if (hash->tabla[nueva_posicion].estado == BORRADO)
      continue;
    if (hash->tabla[nueva_posicion].estado == VACIO)
      return NULL;
    if (strcmp(hash->tabla[nueva_posicion].clave, clave) == 0)
      return &hash->tabla[nueva_posicion];
  }
  return NULL;
}

void tabla_valores_iniciales(elemento_t *tabla, size_t capacidad) {
  for (size_t i = 0; i < capacidad; i++) {
    tabla[i].clave = NULL;
    tabla[i].dato = NULL;
    tabla[i].estado = VACIO;
  }
}

bool hash_redimensionar(hash_t *hash, size_t nueva_capacidad) {
  elemento_t *nueva_tabla = malloc(sizeof(elemento_t) * nueva_capacidad);
  if (!nueva_tabla)
    return false;
  elemento_t *vieja_tabla = hash->tabla;

  size_t capacidad_vieja = hash->capacidad;
  hash->capacidad = nueva_capacidad;
  hash->tabla = nueva_tabla;
  hash->cantidad = 0;
  hash->borrados = 0;
  pasar_tabla(hash, vieja_tabla, capacidad_vieja);
  free(vieja_tabla);
  return true;
}

void pasar_tabla(hash_t *hash, elemento_t *vieja_tabla,
                 size_t capacidad_vieja) {
  tabla_valores_iniciales(hash->tabla, hash->capacidad);
  for (size_t i = 0; i < capacidad_vieja; i++) {
    if (vieja_tabla[i].estado != OCUPADO)
      continue;
    hash_guardar(hash, vieja_tabla[i].clave, vieja_tabla[i].dato);
    free(vieja_tabla[i].clave);
  }
}
int encontrar_prox_vacio(hash_t *hash, size_t posicion) {
  for (size_t i = 0; i < hash->capacidad; i++) {
    int nueva_posicion = (int)((i + posicion) % hash->capacidad);
    if (hash->tabla[nueva_posicion].estado == VACIO)
      return nueva_posicion;
  }
  return -1;
}
