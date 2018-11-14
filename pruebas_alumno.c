#include "abb.h"
#include "testing.h"
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* *****************************************************************
 *                  PRUEBAS UNITARIAS DEL ALUMNO
 * *****************************************************************/
bool mostrar_elemento(const char * clave, void * dato, void * extra) {
	printf(" %s -", clave);
	return true;
}

int comparar_claves(const char* cadena1,const char* cadena2){	
	return atoi(cadena1)-atoi(cadena2);
}


void pruebas_abb_basicas(){
	// ESTAS PRUEBAS SON TEMPORALES, NO SON UNITARIAS 
	// Y NO CONTEMPLAN TODOS LOS CASOS. Puede fallar.
	// Pruebo abb_crear
	abb_t * arbol = abb_crear(comparar_claves, free);
	print_test("~Arbol creado", arbol);
	print_test("~Cantidad de elementos de arbol recien creado igual a 0", abb_cantidad(arbol) == 0);
	print_test("~Obtener un elemento de arbol recien creado igual a NULL", !abb_obtener(arbol, "1"));
	print_test("~Pertenece un elemento de arbol recien creado igual a false", !abb_pertenece(arbol, "1"));

	abb_iter_t* abb_iter = abb_iter_in_crear(arbol);

	while(!abb_iter_in_al_final(abb_iter)) {
		const char* clave = abb_iter_in_ver_actual(abb_iter);
	    printf("%s - ", clave);
	    abb_iter_in_avanzar(abb_iter);
	}
	printf("\n");

	int *uno = malloc(sizeof(int));
	int *dos = malloc(sizeof(int));
	int *tres = malloc(sizeof(int));
	int *cuatro = malloc(sizeof(int));
	int *nuevo_uno = malloc(sizeof(int));

	*uno = 1;
	*dos = 2;
	*tres = 3;
	*cuatro = 4;
	*nuevo_uno = 100;
	// Pruebo guardar
	print_test("Inserto el 2", abb_guardar(arbol, "2", dos));
	print_test("~Cantidad de elementos de arbol igual a 1", abb_cantidad(arbol) == 1);
	print_test("~Obtengo clave 2", abb_obtener(arbol, "2") == dos);
	print_test("~Pertenece clave 2", abb_pertenece(arbol, "2"));



	print_test("Inserto el 1", abb_guardar(arbol, "1", uno));
	print_test("~Cantidad de elementos de arbol igual a 2", abb_cantidad(arbol) == 2);
	print_test("~Obtengo clave 1", abb_obtener(arbol, "1") == uno);
	print_test("~Pertenece clave 1", abb_pertenece(arbol, "1"));

	// Pruebo guardar un elemento que ya estaba
	print_test("Inserto el 1 con otro valor", abb_guardar(arbol, "1", nuevo_uno));
	print_test("~Cantidad de elementos de arbol igual a 2", abb_cantidad(arbol) == 2);
	print_test("~Obtengo clave 1, nuevo valor", abb_obtener(arbol, "1") == nuevo_uno);
	print_test("~Pertenece clave 1", abb_pertenece(arbol, "1"));

	print_test("Inserto el 3", abb_guardar(arbol, "3", tres));
	print_test("~Cantidad de elementos de arbol igual a 3", abb_cantidad(arbol) == 3);
	print_test("~Obtengo clave 3", abb_obtener(arbol, "3") == tres);
	print_test("~Pertenece clave 1", abb_pertenece(arbol, "1"));

	print_test("Inserto el 4", abb_guardar(arbol, "4", cuatro));
	print_test("~Cantidad de elementos de arbol igual a 4", abb_cantidad(arbol) == 4);
	print_test("~Obtengo clave 4", abb_obtener(arbol, "4") == cuatro);
	print_test("~Pertenece clave 4", abb_pertenece(arbol, "4"));

	// Pruebo el iterador externo
	/*
	abb_iter_t* abb_iter = abb_iter_in_crear(arbol);
	while(!abb_iter_in_al_final(abb_iter)) {
	    const char* clave = abb_iter_in_ver_actual(abb_iter);
	    printf("%s - ", clave);
	    abb_iter_in_avanzar(abb_iter);
	}
	printf("\n");
	*/

	abb_iter_in_destruir(abb_iter);

	// Pruebo el iterador interno
	abb_in_order(arbol, mostrar_elemento, NULL);
	printf("\n");


	free(abb_borrar(arbol, "1"));
	free(abb_borrar(arbol, "2"));
	free(abb_borrar(arbol, "3"));
	free(abb_borrar(arbol, "4"));

	// Pruebo Destruir con función destruir
	abb_destruir(arbol);
}
void prueba_con_muchos_nodos(){
	
	// ESTAS PRUEBAS SON TEMPORALES, NO SON UNITARIAS 
	// Y NO CONTEMPLAN TODOS LOS CASOS. Puede fallar.
	// Pruebo abb_crear
	abb_t * arbol = abb_crear(comparar_claves, free);
	print_test("~Arbol creado", arbol);
	print_test("~Cantidad de elementos de arbol recien creado igual a 0", abb_cantidad(arbol) == 0);


	int *ocho = malloc(sizeof(int));
	int *diez = malloc(sizeof(int));
	int *cinco = malloc(sizeof(int));
	int *veintitres = malloc(sizeof(int));
	int *diecinueve = malloc(sizeof(int));
	int *siete = malloc(sizeof(int));
	int *setentaidos = malloc(sizeof(int));
	int *cuarenta = malloc(sizeof(int));
	int *doce = malloc(sizeof(int));
	
	

	*ocho = 8;
	*diez = 10;
	*cinco = 5;
	*veintitres = 23;
	*diecinueve = 19;
	*siete = 7;
	*setentaidos = 72;
	*cuarenta = 40;
	*doce = 12;
	
	// Pruebo guardar
	print_test("Inserto el 8", abb_guardar(arbol, "8", ocho));
	print_test("Inserto el 10", abb_guardar(arbol, "10", diez));
	print_test("Inserto el 5", abb_guardar(arbol, "5", cinco));
	print_test("Inserto el 23", abb_guardar(arbol, "23", veintitres));
	print_test("Inserto el 19", abb_guardar(arbol, "19", diecinueve));
	print_test("Inserto el 7", abb_guardar(arbol, "7", siete));
	print_test("Inserto el 72", abb_guardar(arbol, "72", setentaidos));
	print_test("Inserto el 40", abb_guardar(arbol, "40", cuarenta));
	print_test("Inserto el 12", abb_guardar(arbol, "12", doce));

	abb_iter_t* abb_iter = abb_iter_in_crear(arbol);
	while(!abb_iter_in_al_final(abb_iter)) {
		const char* clave = abb_iter_in_ver_actual(abb_iter);
	    printf("%s - ", clave);
	    abb_iter_in_avanzar(abb_iter);
	}
	printf("\n");
	// Pruebo el iterador interno
	abb_in_order(arbol, mostrar_elemento, NULL);
	printf("\n");

	free(abb_borrar(arbol, "72"));
	free(abb_borrar(arbol, "19"));

	abb_in_order(arbol, mostrar_elemento, NULL);
	printf("\n");

	

	abb_iter_in_destruir(abb_iter);
	// Pruebo Destruir con función destruir
	abb_destruir(arbol);
}
void prueba_un_solo_elemento(){

	abb_t * arbol = abb_crear(comparar_claves, free);
	print_test("~Arbol creado", arbol);
	print_test("~Cantidad de elementos de arbol recien creado igual a 0", abb_cantidad(arbol) == 0);

	int *tres = malloc(sizeof(int));
	int *ocho = malloc(sizeof(int));
	int *dos = malloc(sizeof(int));

	*tres=3;
	*ocho=8;
	*dos=2;
	
	print_test("Inserto el 3", abb_guardar(arbol, "3", tres));
	print_test("Inserto el 8", abb_guardar(arbol, "8", ocho));
	print_test("Inserto el 10", abb_guardar(arbol, "2", dos));
	
	// Pruebo el iterador interno
	abb_in_order(arbol, mostrar_elemento, NULL);
	printf("\n");
	
	free(abb_borrar(arbol, "3"));
	
	// Pruebo el iterador interno
	abb_in_order(arbol, mostrar_elemento, NULL);
	printf("\n");
	
	// Pruebo Destruir con función destruir
	abb_destruir(arbol);
}

/*
 * Se ejecutan todas las pruebas.
 */
void pruebas_abb_alumno() {
	prueba_un_solo_elemento();
	pruebas_abb_basicas();
	prueba_con_muchos_nodos();
}
