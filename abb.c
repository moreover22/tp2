#define _POSIX_C_SOURCE 200809L
#include "abb.h"
#include "pila.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct abb_nodo abb_nodo_t;
struct abb_nodo {
  abb_nodo_t *izq;
  abb_nodo_t *der;
  char *clave;
  void *dato;
};

struct abb {
  abb_nodo_t *raiz;
  abb_comparar_clave_t cmp;
  abb_destruir_dato_t destruir_dato;
  size_t cantidad;
};

/* Devuelve el nodo el cual sería el padre del elemento clave si existiera.
 * En caso de que el nodo ya existe, devuelve ese mismo nodo.
 * Devuelve NULL si el arbol está vacio.
 * Pre: arbol fue previamente creado.
 */
abb_nodo_t *abb_obtener_candidato_padre(const abb_t *arbol, const char *clave);

/* Devuelve el nodo padre del elemento clave.
 * Devuelve NULL si el arbol está vacio o la clave no está o la clave
 * se encuentra en la raíz (No tiene padre).
 * Pre: arbol fue previamente creado.
 */
abb_nodo_t *abb_obtener_padre(const abb_t *arbol, const char *clave);

/* Devuelve el nodo hijo del elemento clave.
 * Devuelve NULL si la clave no está.
 * Pre: arbol fue previamente creado.
 */
abb_nodo_t *abb_obtener_nodo_con_padre(const abb_t *arbol,
                                       const abb_nodo_t *padre,
                                       const char *clave);

/* Crea un nodo con una clave, dato y le asigna el hijo izq y der.
 * Post: devuelve el nodo creado e inicializado en caso de éxito
 * o bien NULL en caso contrario.
 */
abb_nodo_t *crear_abb_nodo(const char *clave, void *dato, abb_nodo_t *izq,
                           abb_nodo_t *der);

void *_abb_nodo_borrar(abb_nodo_t *nodo);

void _abb_swap_nodos(abb_nodo_t *nodo1, abb_nodo_t *nodo2);

void *abb_borrar_hoja(abb_nodo_t *nodo, abb_nodo_t *padre);

void *abb_borrar_nodo_con_hijo_unico(abb_t *arbol, abb_nodo_t *nodo,
                                     abb_nodo_t *padre,
                                     abb_nodo_t *nodo_reemplazante);

void *abb_borrar_nodo_con_dos_hijos(abb_nodo_t *nodo);

/*******************************************************************
 *                    PRIMITIVAS DEL ABB
 ******************************************************************/

abb_t *abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato) {
  abb_t *arbol = malloc(sizeof(abb_t));
  if (!arbol)
    return NULL;
  arbol->raiz = NULL;
  arbol->cmp = cmp;
  arbol->destruir_dato = destruir_dato;
  arbol->cantidad = 0;
  return arbol;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato) {
  abb_nodo_t *candidato_padre = abb_obtener_candidato_padre(arbol, clave);

  // La clave ya está en el arbol
  if (candidato_padre && arbol->cmp(candidato_padre->clave, clave) == 0) {
    if (arbol->destruir_dato)
      arbol->destruir_dato(candidato_padre->dato);
    candidato_padre->dato = dato;
    return true;
  }

  abb_nodo_t *nodo = crear_abb_nodo(clave, dato, NULL, NULL);
  if (!nodo)
    return false;
  arbol->cantidad++;
  // Arbol vacio.
  if (!candidato_padre) {
    arbol->raiz = nodo;
    return true;
  }

  // La clave no está en el arbol
  if (arbol->cmp(candidato_padre->clave, clave) > 0)
    candidato_padre->izq = nodo;
  else
    candidato_padre->der = nodo;

  return true;
}

void *abb_borrar(abb_t *arbol, const char *clave) {
  if (!abb_pertenece(arbol, clave))
    return NULL;

  abb_nodo_t *padre = abb_obtener_padre(arbol, clave);
  abb_nodo_t *nodo = NULL;

  if (!padre)
    nodo = arbol->raiz;
  else
    nodo = abb_obtener_nodo_con_padre(arbol, padre, clave);

  arbol->cantidad--;

  if (!arbol->cantidad)
    arbol->raiz = NULL;

  if (!nodo->izq && !nodo->der)
    return abb_borrar_hoja(nodo, padre);

  if (nodo->izq && !nodo->der)
    return abb_borrar_nodo_con_hijo_unico(arbol, nodo, padre, nodo->izq);

  if (!nodo->izq && nodo->der)
    return abb_borrar_nodo_con_hijo_unico(arbol, nodo, padre, nodo->der);

  return abb_borrar_nodo_con_dos_hijos(nodo);
}

void *abb_obtener(const abb_t *arbol, const char *clave) {
  abb_nodo_t *candidato_padre = abb_obtener_candidato_padre(arbol, clave);
  if (!candidato_padre)
    return NULL;

  if (arbol->cmp(candidato_padre->clave, clave) == 0)
    return candidato_padre->dato;
  return NULL;
}

bool abb_pertenece(const abb_t *arbol, const char *clave) {
  abb_nodo_t *candidato_padre = abb_obtener_candidato_padre(arbol, clave);
  if (!candidato_padre)
    return false;
  return (arbol->cmp(candidato_padre->clave, clave) == 0);
}

size_t abb_cantidad(abb_t *arbol) { return arbol->cantidad; }

void _abb_destruir(abb_nodo_t *nodo, abb_destruir_dato_t destruir_dato) {
  if (!nodo)
    return;
  _abb_destruir(nodo->izq, destruir_dato);
  _abb_destruir(nodo->der, destruir_dato);
  if (destruir_dato)
    destruir_dato(nodo->dato);
  _abb_nodo_borrar(nodo);
}

void abb_destruir(abb_t *arbol) {
  _abb_destruir(arbol->raiz, arbol->destruir_dato);
  free(arbol);
}

/* ******************************************************************
 *                    ITERADOR INTERNO
 * *****************************************************************/
bool _abb_in_order(abb_nodo_t *nodo, bool visitar(const char *, void *, void *),
                   void *extra) {
  if (!nodo)
    return true;
  bool iterar = _abb_in_order(nodo->izq, visitar, extra);
  iterar &= visitar(nodo->clave, nodo->dato, extra);
  if (iterar)
    _abb_in_order(nodo->der, visitar, extra);
  return true;
}
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *),
                  void *extra) {
  _abb_in_order(arbol->raiz, visitar, extra);
}

/* ******************************************************************
 *                    ITERADOR EXTERNO
 * *****************************************************************/

struct abb_iter {
  pila_t *pila;
};
/*
 *
 */
abb_iter_t *_abb_iter_in_crear_vacio(const abb_t *arbol) {
  abb_iter_t *iter = malloc(sizeof(abb_iter_t));
  if (!iter)
    return NULL;
  pila_t *pila = pila_crear();
  if (!pila) {
    free(iter);
    return NULL;
  }
  iter->pila = pila;
  return iter;
}
abb_iter_t *abb_iter_in_crear(const abb_t *arbol) {
  abb_iter_t *iter = _abb_iter_in_crear_vacio(arbol);
  if (!iter)
    return NULL;

  abb_nodo_t *actual = arbol->raiz;
  printf("CLAVEEEEE: %s\n", actual->clave);
  while (actual) {
    pila_apilar(iter->pila, actual);
    actual = actual->izq;
  }
  return iter;
}
abb_iter_t *abb_iter_in_crear_desde(const abb_t *arbol, const char *clave) {
  abb_iter_t *iter = _abb_iter_in_crear_vacio(arbol);
  if (!iter)
    return NULL;
  abb_nodo_t *actual = arbol->raiz;
  while (actual) {
    int comp = arbol->cmp(actual->clave, clave);
    if (comp >= 0)
      pila_apilar(iter->pila, actual);

    if (comp > 0)
      actual = actual->izq;
    else if (comp < 0)
      actual = actual->der;
    else
      // En las ramas izq no voy a encontrar otro valor mayor a clave
      break;
  }
  return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter) {
  if (pila_esta_vacia(iter->pila))
    return false;

  abb_nodo_t *desapilado = pila_desapilar(iter->pila);
  abb_nodo_t *actual = desapilado->der;
  while (actual) {
    pila_apilar(iter->pila, actual);
    actual = actual->izq;
  }
  return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter) {
  abb_nodo_t *tope = pila_ver_tope(iter->pila);
  if (!tope)
    return NULL;
  return (const char *)tope->clave;
}

bool abb_iter_in_al_final(const abb_iter_t *iter) {
  return pila_esta_vacia(iter->pila);
}

void abb_iter_in_destruir(abb_iter_t *iter) {
  pila_destruir(iter->pila);
  free(iter);
}

/* ******************************************************************
 *                     Extras
 * *****************************************************************/

abb_nodo_t *_abb_obtener_padre(abb_nodo_t *nodo, const char *clave,
                               abb_comparar_clave_t cmp, abb_nodo_t *padre) {
  if (!nodo)
    return NULL;

  if (cmp(clave, nodo->clave) == 0)
    return padre;

  if (cmp(clave, nodo->clave) < 0)
    return _abb_obtener_padre(nodo->izq, clave, cmp, nodo);

  return _abb_obtener_padre(nodo->der, clave, cmp, nodo);
}

abb_nodo_t *abb_obtener_padre(const abb_t *arbol, const char *clave) {
  return _abb_obtener_padre(arbol->raiz, clave, arbol->cmp, NULL);
}

abb_nodo_t *_abb_obtener_candidato_padre(abb_nodo_t *nodo, const char *clave,
                                         abb_comparar_clave_t cmp) {
  if (!nodo)
    return NULL;
  // Si tiene la clave, es el nodo
  if (cmp(clave, nodo->clave) == 0)
    return nodo;

  abb_nodo_t *candidato = NULL;

  if (cmp(clave, nodo->clave) < 0)
    candidato = _abb_obtener_candidato_padre(nodo->izq, clave, cmp);
  else
    candidato = _abb_obtener_candidato_padre(nodo->der, clave, cmp);

  if (!candidato)
    return nodo;
  return candidato;
}

abb_nodo_t *abb_obtener_candidato_padre(const abb_t *arbol, const char *clave) {
  return _abb_obtener_candidato_padre(arbol->raiz, clave, arbol->cmp);
}

abb_nodo_t *crear_abb_nodo(const char *clave, void *dato, abb_nodo_t *izq,
                           abb_nodo_t *der) {
  abb_nodo_t *nodo = malloc(sizeof(abb_nodo_t));
  if (!nodo)
    return NULL;
  nodo->clave = strdup(clave);
  nodo->dato = dato;
  nodo->izq = izq;
  nodo->der = der;
  return nodo;
}

abb_nodo_t *abb_obtener_nodo_con_padre(const abb_t *arbol,
                                       const abb_nodo_t *padre,
                                       const char *clave) {
  if (arbol->cmp(padre->clave, clave) > 0)
    return padre->izq;
  else
    return padre->der;
}

/*******************************************************************
 *                FUNCIONES AUX. PARA ABB_BORRAR
 ******************************************************************/

void *_abb_nodo_borrar(abb_nodo_t *nodo) {
  void *dato = nodo->dato;
  free((char *)nodo->clave);
  free(nodo);
  return dato;
}

void _abb_swap_nodos(abb_nodo_t *nodo1, abb_nodo_t *nodo2) {
  void *dato = nodo1->dato;
  char *clave = nodo1->clave;

  nodo1->clave = nodo2->clave;
  nodo1->dato = nodo2->dato;
  nodo2->clave = clave;
  nodo2->dato = dato;
}

void *abb_borrar_hoja(abb_nodo_t *nodo, abb_nodo_t *padre) {
  if (!padre)
    return _abb_nodo_borrar(nodo);

  if (padre->izq == nodo)
    padre->izq = NULL;
  else
    padre->der = NULL;
  return _abb_nodo_borrar(nodo);
}

void *abb_borrar_nodo_con_hijo_unico(abb_t *arbol, abb_nodo_t *nodo,
                                     abb_nodo_t *padre,
                                     abb_nodo_t *nodo_reemplazante) {
  if (!padre)
    arbol->raiz = nodo_reemplazante;
  else if (padre->izq == nodo)
    padre->izq = nodo_reemplazante;
  else
    padre->der = nodo_reemplazante;

  return _abb_nodo_borrar(nodo);
}

void *abb_borrar_nodo_con_dos_hijos(abb_nodo_t *nodo) {
  abb_nodo_t *padre_reemplazante = nodo;
  abb_nodo_t *reemplazante = nodo->der;

  while (reemplazante->izq) {
    padre_reemplazante = reemplazante;
    reemplazante = reemplazante->izq;
  }
  _abb_swap_nodos(reemplazante, nodo);

  if (reemplazante->der)
    return abb_borrar_nodo_con_hijo_unico(
        NULL, reemplazante, padre_reemplazante, reemplazante->der);

  return abb_borrar_hoja(reemplazante, padre_reemplazante);
}
