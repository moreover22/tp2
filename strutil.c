#include "strutil.h"
#include <stdlib.h>
#include <string.h>

void inicializar_vector(char* strv, size_t len) {
	if(!strv) return;
	for(size_t i = 0; i < len; i++)
		strv[i] = '\0';
}

char** split(const char* str, char sep) {
	// Cantidad de elementos que va a tener el resultado
	// Va tener al menos 2, una cadena vacía y NULL indi-
	// cando final de vector.
	size_t cantidad = 2;

	// Cuenta las apariciones del separador
	size_t len = strlen(str);
	for(size_t i = 0; i < len; i++){
		if (str[i] == sep)
			cantidad++;
	}

	char ** resultado = malloc(sizeof(char *) * cantidad);
	if (!resultado) return NULL;
	const char * ult_apar = str;

	size_t elementos = 0;
	size_t contador = 0;

	for (int i = 0; i <= len; i++){
		if(str[i] != sep && str[i] != '\0') { 
			contador++;
			continue;
		}
		// Guardo un espacio más para el '\0'
		char * elemento = malloc(sizeof(char) * (contador + 1));
		
		memcpy(elemento, ult_apar, contador);
		elemento[contador] = '\0';
		resultado[elementos] = elemento;

		elementos++;
		// Guardo la ultima aparicion del siguiente del separador
		ult_apar += contador + 1;
		contador = 0;	
	}
	resultado[cantidad - 1] = NULL;
	return resultado;
}
char* join(char** strv, char sep) {
	// Calculo la longitud del nuevo vector
	size_t contador = 0;
	for(size_t i = 0; strv && strv[i]; i++){
		contador += strlen(strv[i]) + 1;
	}

	if(contador == 0) contador++;
	
	char * resultado = malloc(sizeof(char) * contador);
	if(!resultado) return NULL;
	inicializar_vector(resultado, contador);
	// i : Indice de strv (una palabra en particular)
	// i_r : Indice en el vector resultado
	for(size_t i = 0, i_r = 0; strv && strv[i]; i++, i_r++) {
		size_t len = strlen(strv[i]);
		for(size_t j = 0; j < len; j++) {
			resultado[i_r] = strv[i][j];
			i_r++;
		}
		if(i_r == contador - 1) break;
		resultado[i_r] = sep;
	}
	return resultado;	
}

void free_strv(char* strv[]) {
	for(size_t i = 0; strv[i]; i++) {
		free(strv[i]);
	}
	free(strv);
}