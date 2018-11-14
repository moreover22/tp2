#ifndef ABB_H
#define ABB_H

#include <unistd.h>
#include <stdbool.h>
/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/* El ABB está planteado como un ABB de punteros genéricos. */
typedef struct abb abb_t;
typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);

/* ******************************************************************
 *                    PRIMITIVAS DEL ABB
 * *****************************************************************/

/* Crea un arbol binario de búsqueda.
 * Pre: cmp es una función que permite comparar las claves del ABB. La
 * función destruir_dato se encarga de destruir el tipo de dato que
 * se guarda en el ABB.
 * Post: devuelve un nuevo ABB vacío.
 */
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

/* Agrega un elemento al arbol. Devuelve false en caso de error.
 * En caso que ya exista la clave, se actualiza el dato.
 * Pre: El arbol fue creado.
 * Post: El arbol tiene el elemento con clave y dato.
 */
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

/* Borra el elemento de la clave y lo devuelve.
 * En caso de que no exista el elemento con esa clave, la función
 * devuelve NULL.
 * Pre: El arbol fue creado.
 * Post: Se devuelve el dato de la clave borrada en caso de que
 * existiese, NULL caso contrario.
 */
void *abb_borrar(abb_t *arbol, const char *clave);

/* Devuelve el dato de la clave si es que existe, NULL en caso 
 * contrario.
 * Pre: arbol fue creado.
 * Post: se devolvió el dato de la clave o NULL si la clave no 
 * está en el arbol.
 */
void *abb_obtener(const abb_t *arbol, const char *clave);

/* Devuelve true en caso de que la clave esté en el arbol.
 * Pre: arbol fue creado.
 * Post: Devuelve true si hay un elemento en el arbol con 
 * la clave indicada, false caso contrario.
 */
bool abb_pertenece(const abb_t *arbol, const char *clave);

/* Devuelve la cantidad de elementos que tiene el arbol
 * Pre: el arbol fue creado.
 */
size_t abb_cantidad(abb_t *arbol);

/* Se destruyen todos los datos de loselementos del arbol y el arbol.
 * Pre: el arbol fue creado.
 * Post: Se eliminaron los datos del arbol y el arbol.
 */
void abb_destruir(abb_t *arbol);

/* ******************************************************************
 *                DEFINICION DE LOS ITERADORES
 * *****************************************************************/

/* ******************************************************************
 *                    ITERADOR INTERNO
 * *****************************************************************/

/* Permite recorrer el arbol en orden y aplica la función
 * visitar a cada elemento.
 * Pre: arbol fue creado.
 */
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);

/* ******************************************************************
 *                    ITERADOR EXTERNO
 * *****************************************************************/

typedef struct abb_iter abb_iter_t;

/* Crea un iterador externo del TDA ABB.
 * Pre: arbol fue creado.
 * Post: Devuelve un iterador de arbol apuntando al nodo con menor valor 
 * que se encuentra en el arbol.
 */
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

/* El iterador avanza al elemento con clave mayor próxima.
 * Devuelve true si pudo avanzar, false caso contrario.
 * Pre: iter fue creado.
 * Post: avanza al siguiente elemento.
 */
bool abb_iter_in_avanzar(abb_iter_t *iter);

/* Devuelve el valor de la clave del nodo actual. Devuelve 
 * NULL en caso de que actual lo sea.
 * Pre: iter fue creado.
 */
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

/* Devuelve true si el iterador está al final del arbol,
 * false en caso contrario.
 * Pre: iter fue creado
 */
bool abb_iter_in_al_final(const abb_iter_t *iter);

/* Destruye el iterador.
 * Pre: iter fue creado.
 */
void abb_iter_in_destruir(abb_iter_t* iter);

/* *****************************************************************
 *                      PRUEBAS UNITARIAS
 * *****************************************************************/

// Realiza pruebas sobre la implementación del alumno.
//
// Las pruebas deben emplazarse en el archivo ‘pruebas_alumno.c’, y
// solamente pueden emplear la interfaz pública tal y como aparece en cola.h
// (esto es, las pruebas no pueden acceder a los miembros del struct cola).
//
// Para la implementación de las pruebas se debe emplear la función
// print_test(), como se ha visto en TPs anteriores.
void pruebas_abb_alumno(void);

#endif // ABB_H
