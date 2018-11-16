#include "analog.h"
#include "fechautil.h"
#include "hash.h"
#include "strutil.h"
#include <string.h>
#define CSV_SEP ','
enum { NUM, AER, ORI, DES, TAI, PRI, FEC, DEP, TIE, CAN };
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

// Devería recibir un puntero hash, y un abb. Acá solo se los crea.
hash_t *iniciar_vuelos() {
  hash_t *vuelos = hash_crear(free);
  // Maybe not working
  abb_t *abb_vuelos = abb_crear(vueloscmp, free);
  if (!vuelos || !abb_vuelos)
    return NULL;
  return vuelos;
}
void inicializar_vuelo(char **datos, vuelo_t *vuelo) {
  vuelo->numero = atoi(datos[NUM]);
  vuelo->aerolinea = datos[AER];
  vuelo->origen = datos[ORI];
  vuelo->destino = datos[DES];
  vuelo->tail_number = datos[TAI];
  vuelo->prioridad = atoi(datos[PRI]);
  vuelo->fecha = crearFecha(datos[FEC]);
  vuelo->departure_delay = atoi(datos[DEP]);
  vuelo->tiempo_vuelo = atoi(datos[TIE]);
  vuelo->cancelado = atoi(datos[CAN]);
}

bool agregar_archivo(hash_t *vuelos, abb_t *tablero_vuelos,
                     const char *nombre_archivo) {
  FILE *archivo_vuelos = fopen(nombre_archivo, "r");

  if (!archivo_vuelos)
    return false;

  char *linea = NULL;
  size_t cantidad = 0;
  ssize_t leidos = 0;
  size_t contador = 0;

  while ((leidos = getline(linea, &cantidad, archivo_vuelos)) > 0) {
    linea[leidos - 1] = '\0';
    char **datos_vuelo = split(linea, CSV_SEP);
    char *numero_vuelo = datos[NUM];
    vuelo_t *vuelo = NULL;
    if (hash_pertenece(hash, clave)) {
      vuelo = (vuelo_t *)hash_obtener(vuelos, numero_vuelo);
      inicializar_vuelo(datos_vuelo, vuelo);
    } else {
      vuelo_t *vuelo = malloc(sizeof(vuelo_t));
      inicializar_vuelo(datos_vuelo, vuelo);
      hash_guardar(vuelos, numero_vuelo, vuelo);
      abb_guardar(tablero_vuelos, numero_vuelo, NULL);
    }
    free(datos_vuelo);
  }
  return true;
}

bool ver_tablero(hash_t *vuelos, abb_t *abb_vuelos, int cant_vuelos,
                 const char *modo, fecha_t *desde, fecha_t *hasta) {
  abb_iter_t *iter_vuelos = abb_iter_in_crear(abb_vuelos);
  char *fecha_desde = fechaAStr(desde);
  // La clave debería ser otra, y esta primitiva hay que crearla
  abb_iter_in_avanzar_hasta(fecha_desde);
  while (abb_iter_int_avanzar(iter_vuelos)) {
    printf("%s \n", abb_iter_in_ver_actual(iter_vuelos));
  }
  return true;
}

bool info_vuelo(hash_t *vuelos, const char *cod_vuelo) {
  vuelo_t *vuelo = (vuelo_t *)hash_obtener(vuelos, cod_vuelo);
  if (!vuelo)
    return false;
  mostrar_vuelo(vuelo);
  return true;
}

bool prioridad_vuelos(hash_t *vuelos, int cant_vuelos) {}

bool borrar(hash_t *vuelos, fecha_t desde, decha_t hasta) {}

void mostrar_vuelo(vuelo_t *vuelo) {
  char *fecha_hora = fechaAStr(vuelo->fecha);
  printf("%d %s %s %s %d %s %d %d %d\n", vuelo->numero, vuelo->aerolinea,
         vuelo->origen, vuelo->destino, vuelo->numero_cola, vuelo->prioridad,
         fecha_hora, vuelo->retraso_salida, vuelo->tiempo_vuelo,
         vuelo->cancelado);
  free(fecha_hora);
}
