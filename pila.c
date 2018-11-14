#include "pila.h"
#include "testing.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define CAPACIDAD_INICIAL 10
#define FACTOR_REDIM_APILAR 2
#define FACTOR_REDIM_DESAPILAR 4
#define FACTOR_NUEVO_TAM (2 * FACTOR_REDIM_DESAPILAR)

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

// Cambia la capacidad de la pila
// Pre: la pila fue creada
// Post: la pila cambió de capacidad a nueva_capacidad y devuelve true
// o la pila queda intacta y devuelve false si no se pudo cambiar la 
// capacidad a nueva_capacidad o en caso de que la nueva capacidad 
// no pueda guardar los datos ya guardados
bool pila_redimensionar(pila_t* pila, size_t nueva_capacidad){
	if(pila->cantidad > nueva_capacidad) return false;

	void** nuevos_datos = realloc(pila->datos, nueva_capacidad * sizeof(void*));
	if(nueva_capacidad > 0 && !nuevos_datos) return false;

	pila->datos = nuevos_datos;
	pila->capacidad = nueva_capacidad;
	return true;
}

// Crea una pila.
// Post: devuelve una nueva pila vacía.
pila_t* pila_crear(void) {
	pila_t* pila = malloc(sizeof(pila_t));
	if(!pila) return NULL;

	pila->capacidad = CAPACIDAD_INICIAL;
	pila->cantidad = 0;

	pila->datos = malloc(pila->capacidad * sizeof(void*));

	if(pila->capacidad > 0 && !pila->datos) {
		free(pila);
		return NULL;
	}
	return pila;
}

// Destruye la pila.
// Pre: la pila fue creada.
// Post: se eliminaron todos los elementos de la pila.
void pila_destruir(pila_t *pila) {
	free(pila->datos);
	free(pila);
}

// Devuelve verdadero o falso, según si la pila tiene o no elementos apilados.
// Pre: la pila fue creada.
bool pila_esta_vacia(const pila_t *pila) {
	return pila->cantidad == 0; 
}

// Agrega un nuevo elemento a la pila. Devuelve falso en caso de error.
// Pre: la pila fue creada.
// Post: se agregó un nuevo elemento a la pila, valor es el nuevo tope.
bool pila_apilar(pila_t *pila, void* valor) {
	if(pila->capacidad <= pila->cantidad) {
		if(!pila_redimensionar(pila, pila->capacidad * FACTOR_REDIM_APILAR))
			return false;
	}
	pila->datos[pila->cantidad] = valor;
	pila->cantidad++;
	return true;
}

// Obtiene el valor del tope de la pila. Si la pila tiene elementos,
// se devuelve el valor del tope. Si está vacía devuelve NULL.
// Pre: la pila fue creada.
// Post: se devolvió el valor del tope de la pila, cuando la pila no está
// vacía, NULL en caso contrario.
void* pila_ver_tope(const pila_t *pila) {
	if(pila_esta_vacia(pila)) return NULL;
	return pila->datos[pila->cantidad - 1];
}



// Saca el elemento tope de la pila. Si la pila tiene elementos, se quita el
// tope de la pila, y se devuelve ese valor. Si la pila está vacía, devuelve
// NULL.
// Pre: la pila fue creada.
// Post: si la pila no estaba vacía, se devuelve el valor del tope anterior
// y la pila contiene un elemento menos.
void* pila_desapilar(pila_t *pila) {
	if(pila_esta_vacia(pila)) return NULL;

	void* pila_tope = pila_ver_tope(pila);
	
	pila->cantidad--;
	
	if(pila->cantidad <= pila->capacidad / FACTOR_REDIM_DESAPILAR && 
		pila->capacidad / FACTOR_NUEVO_TAM >= CAPACIDAD_INICIAL) {
		pila_redimensionar(pila, pila->capacidad / FACTOR_NUEVO_TAM);
	}
	return pila_tope;
}