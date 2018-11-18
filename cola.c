#include "cola.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct nodo {
	void* dato;
	struct nodo* prox;
}nodo_t;

/* La cola está planteada como una cola de punteros genéricos. */

struct cola {
	nodo_t* prim;
	nodo_t* ult;
};

/* ******************************************************************
 *                    PRIMITIVAS DE LA COLA
 * *****************************************************************/

// Crea un nodo con un respectivo valor.
// Post: devuelve un puntero a un nodo o NULL
// en caso de que no pueda crearlo.
nodo_t* crear_nodo(void* valor) {
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if(!nodo) return NULL;
	nodo->dato = valor;
	nodo->prox = NULL;
	return nodo;
}

/* ******************************************************************
 *                    PRIMITIVAS DE LA COLA
 * *****************************************************************/

// Crea una cola.
// Post: devuelve una nueva cola vacía.
cola_t* cola_crear(void) {
	cola_t* cola = malloc(sizeof(cola_t));
	if(!cola) return NULL;
	cola->prim = NULL;
	cola->ult = NULL;
	return cola;
}

// Destruye la cola. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la cola llama a destruir_dato.
// Pre: la cola fue creada. destruir_dato es una función capaz de destruir
// los datos de la cola, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la cola.
void cola_destruir(cola_t *cola, void destruir_dato(void*)) {
	while(!cola_esta_vacia(cola)){
		void* elemento = cola_desencolar(cola);
		if(!destruir_dato) continue;
		destruir_dato(elemento);
	}
	free(cola);
}

// Devuelve verdadero o falso, según si la cola tiene o no elementos encolados.
// Pre: la cola fue creada.
bool cola_esta_vacia(const cola_t *cola) {
	return !cola->prim && !cola->ult;
}

// Agrega un nuevo elemento a la cola. Devuelve falso en caso de error.
// Pre: la cola fue creada.
// Post: se agregó un nuevo elemento a la cola, valor se encuentra al final
// de la cola.
bool cola_encolar(cola_t *cola, void* valor) {
	nodo_t* nuevo_nodo = crear_nodo(valor);
	if(!nuevo_nodo) return false;
	if(cola_esta_vacia(cola))
		cola->prim = nuevo_nodo;
	else
		(cola->ult)->prox = nuevo_nodo;
	cola->ult = nuevo_nodo;
	return true;
}

// Obtiene el valor del primer elemento de la cola. Si la cola tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la cola fue creada.
// Post: se devolvió el primer elemento de la cola, cuando no está vacía.
void* cola_ver_primero(const cola_t *cola) {
	if(cola_esta_vacia(cola)) return NULL;
	return (cola->prim)->dato;
}

// Saca el primer elemento de la cola. Si la cola tiene elementos, se quita el
// primero de la cola, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la cola fue creada.
// Post: se devolvió el valor del primer elemento anterior, la cola
// contiene un elemento menos, si la cola no estaba vacía.
void* cola_desencolar(cola_t *cola) {
	if(cola_esta_vacia(cola)) return NULL;
	nodo_t* primer_nodo = cola->prim;
	void* nodo_dato = primer_nodo->dato;
	cola->prim = primer_nodo->prox;
	// En caso de que tenga un solo elemento.
	if(!primer_nodo->prox)
		cola->ult = NULL;
	free(primer_nodo);
	return nodo_dato;
}
