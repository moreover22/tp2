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

void inicializar_vuelo(char**datos,vuelo_t*vuelo){
	vuelo->numero=atoi(datos[0]);
	vuelo->aerolinea=datos[1]);
	vuelo->origen=datos[2];
	vuelo->destino=datos[3];
	vuelo->tail_number=datos[4];
	vuelo->prioridad=atoi(datos[5]);
	vuelo->fecha=malloc(sizeof(fecha_t));
	vuelo->departure_delay=atoi(datos[7]);
	vuelo->tiempo_vuelo=atoi(datos[8]);
	vuelo->cancelado=atoi(datos[9]);
}

bool agregar_archivo(hash_t* vuelos,abb_t*  tablero_vuelos,const char *nombre_archivo) {
	
	FILE* archivo_vuelos=fopen(nombre_archivo,"r");
	
	if(!archivo_vuelos)return false;
	
	char* linea=NULL;
	size_t cantidad=0;
	
	ssize_t leidos=0;
	size_t contador=0;
	
	while((leidos=getline(linea,&cantidad,archivo_vuelos)>0){
		linea[leidos-1]='\0';
		char** datos_vuelo=split(linea,",");
		char *clave_vuelo=datos[0];
		vuelo_t*vuelo=NULL;
		if(hash_pertenece(hash,clave)){
			vuelo=(vuelo_t*)hash_obtener(hash,clave);
			inicializar_vuelo(datos,vuelo);
		}
		else{
			vuelo_t*vuelo=malloc(sizeof(vuelo_t));
			inicializar_vuelo(datos,vuelo);
			hash_guardar(hash,clave_vuelo,vuelo);
			abb_guardar(tablero_vuelos,clave,vuelo->numero);
		}
		free(datos);
	}
	
	return true;
}

bool ver_tablero(hash_t *vuelos, int cant_vuelos, const char *modo,
                 fecha_t desde, fecha_t hasta) {}
bool info_vuelo(hash_t *vuelos, const char *cod_vuelo) {}

bool prioridad_vuelos(hash_t *vuelos, int cant_vuelos) {}

bool borrar(hash_t *vuelos, fecha_t desde, decha_t hasta) {}
