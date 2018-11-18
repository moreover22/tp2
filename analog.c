#define _POSIX_C_SOURCE 200809L

#include "abb.h"
#include "analog.h"
#include "cola.h"
#include "fechautil.h"
#include "hash.h"
#include "strutil.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CSV_SEP ','
#define CLAVE_SEP '-'
#define L_CLAVE 26
enum { NUM, AER, ORI, DES, TAI, PRI, FEC, DEP, TIE, CAN };
// Constantes para agregar_archivo
#define ARGS_AGREGAR_ARCHIVO 1
#define AA_FILE_NAME 0
// Constantes para ver_tablero
#define ARGS_VER_TABLERO 4
enum { VT_CANT_VUELOS, VT_MODO, VT_DESDE, VT_HASTA };
// Constantes para info_vuelo
#define ARGS_INFO_VUELO 1
#define IV_NUM_VUELO 0
// Constantes para prioridad_vuelos
#define ARGS_PRIORIDAD_VUELO 1
#define PV_PRIORIDAD 0
// Constantes para borrar
#define ARGS_BORRAR 2
enum { B_DESDE, B_HASTA };
// Constante para abb_vueloscmp
#define AVC_FECHA 0
#define AVC_COD_VUELO 2

#define ASC "asc"
#define DESC "desc"
struct vuelos {
  hash_t *hash_vuelos;
  abb_t *abb_vuelos;
};

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

// ESTA  SIRVE PARA EL HEAP DE PRIORIDAD
int vueloscmp(const void *a, const void *b) {
  vuelo_t v_a = *(vuelo_t *)a;
  vuelo_t v_b = *(vuelo_t *)b;
  int fecha_comp = fechacmp(v_a.fecha, v_b.fecha);
  if (fecha_comp != 0)
    return fecha_comp;
  return v_a.numero - v_b.numero;
}
// ESTA  SIRVE PARA EL ABB
int abb_vueloscmp(const char *a, const char *b) {
  int resultado;
  // Clave = "fecha - codigo_vuelo"
  // split(Clave, ' ') -> ["fecha", "-", "codigo_vuelo", NULL]

  char **v_a = split(a, ' ');
  char **v_b = split(b, ' ');

  fecha_t *f_a = fecha_crear(v_a[AVC_FECHA]);
  fecha_t *f_b = fecha_crear(v_b[AVC_FECHA]);
  int fecha_comp = fechacmp(f_a, f_b);
  free(f_a);
  free(f_b);
  int cod_vuelo_a = atoi(v_a[AVC_COD_VUELO]);
  int cod_vuelo_b = atoi(v_b[AVC_COD_VUELO]);
  resultado = fecha_comp;
  if (resultado == 0)
    resultado = cod_vuelo_a - cod_vuelo_b;
  free_strv(v_a);
  free_strv(v_b);
  return resultado;
}

vuelos_t *iniciar_vuelos() {
  vuelos_t *vuelos = malloc(sizeof(vuelos_t));
  if (!vuelos)
    return NULL;
  hash_t *hash_vuelos = hash_crear(free);
  if (!hash_vuelos) {
    free(vuelos);
    return NULL;
  }
  abb_t *abb_vuelos = abb_crear(abb_vueloscmp, free);
  if (!abb_vuelos) {
    free(vuelos);
    hash_destruir(hash_vuelos);
    return NULL;
  }

  vuelos->hash_vuelos = hash_vuelos;
  vuelos->abb_vuelos = abb_vuelos;

  return vuelos;
}
void finalizar_vuelos(vuelos_t *vuelos) {
  hash_destruir(vuelos->hash_vuelos);
  abb_destruir(vuelos->abb_vuelos);
  free(vuelos);
}

void inicializar_vuelo(char **datos, vuelo_t *vuelo) {
  vuelo->numero = atoi(datos[NUM]);
  vuelo->aerolinea = datos[AER];
  vuelo->origen = datos[ORI];
  vuelo->destino = datos[DES];
  vuelo->numero_cola = datos[TAI];
  vuelo->prioridad = atoi(datos[PRI]);
  vuelo->fecha = fecha_crear(datos[FEC]);
  vuelo->retraso_salida = atoi(datos[DEP]);
  vuelo->tiempo_vuelo = atoi(datos[TIE]);
  vuelo->cancelado = atoi(datos[CAN]);
}

char *generar_clave(fecha_t *fecha, int numero_vuelo) {
  char *clave = malloc(sizeof(char) * L_CLAVE);
  char *fecha_hora = fecha_a_str(fecha);
  sprintf(clave, "%s %c %d", fecha_hora, CLAVE_SEP, numero_vuelo);
  free(fecha_hora);
  return clave;
}

bool _agregar_archivo(vuelos_t *vuelos, const char *nombre_archivo) {
  FILE *archivo_vuelos = fopen(nombre_archivo, "r");
  if (!archivo_vuelos)
    return false;
  hash_t *hash_vuelos = vuelos->hash_vuelos;
  abb_t *abb_vuelos = vuelos->abb_vuelos;

  char *linea = NULL;
  size_t cantidad = 0;
  ssize_t leidos = 0;

  while ((leidos = getline(&linea, &cantidad, archivo_vuelos)) > 0) {
    linea[leidos - 1] = '\0';
    char **datos_vuelo = split(linea, CSV_SEP);
    char *numero_vuelo = datos_vuelo[NUM];

    vuelo_t *vuelo = NULL;
    if (hash_pertenece(hash_vuelos, numero_vuelo)) {
      vuelo = (vuelo_t *)hash_obtener(hash_vuelos, numero_vuelo);
      inicializar_vuelo(datos_vuelo, vuelo);
    } else {
      vuelo = malloc(sizeof(vuelo_t));
      inicializar_vuelo(datos_vuelo, vuelo);
      hash_guardar(hash_vuelos, numero_vuelo, vuelo);
      char *clave = generar_clave(vuelo->fecha, vuelo->numero);
      abb_guardar(abb_vuelos, clave, NULL);
      free(clave);
    }
    free(datos_vuelo);
  }
  fclose(archivo_vuelos);
  return true;
}

bool _ver_tablero(vuelos_t *vuelos, size_t cant_vuelos, const char *modo,
                  fecha_t *desde, fecha_t *hasta) {
  char **resultado = malloc(sizeof(char *) * cant_vuelos);
  if (!resultado)
    return false;
  // Fecha - 0 (cualquier vuelo)
  char *clave = generar_clave(desde, 0);
  // Incremento 1 segundo la fecha límite
  fecha_sumar_segundos(hasta, 1);
  char *clave_limite = generar_clave(hasta, 0);

  abb_iter_t *iter_vuelos = abb_iter_in_crear_desde(vuelos->abb_vuelos, clave);
  free(clave);
  int i = 0;
  while (!abb_iter_in_al_final(iter_vuelos) && i < cant_vuelos) {
    const char *clave_actual = abb_iter_in_ver_actual(iter_vuelos);
    if (abb_vueloscmp(clave_actual, clave_limite) >= 0)
      break;
    resultado[i] = strdup(clave_actual);
    abb_iter_in_avanzar(iter_vuelos);
    i++;
  }
  free(clave_limite);
  abb_iter_in_destruir(iter_vuelos);

  bool descendente = strcmp(modo, DESC) == 0;
  for (int j = 0; j < i; j++) {
    int index;
    index = j;
    if (descendente)
      index = i - j - 1;
    printf("%s\n", resultado[index]);
    free(resultado[index]);
  }
  free(resultado);

  return true;
}

void mostrar_vuelo(vuelo_t *vuelo) {
  char *fecha_hora = fecha_a_str(vuelo->fecha);
  printf("%d %s %s %s %s %d %s %d %d %d\n", vuelo->numero, vuelo->aerolinea,
         vuelo->origen, vuelo->destino, vuelo->numero_cola, vuelo->prioridad,
         fecha_hora, vuelo->retraso_salida, vuelo->tiempo_vuelo,
         vuelo->cancelado);
  free(fecha_hora);
}

bool _info_vuelo(vuelos_t *vuelos, const char *cod_vuelo) {
  vuelo_t *vuelo = (vuelo_t *)hash_obtener(vuelos->hash_vuelos, cod_vuelo);
  if (!vuelo)
    return false;
  mostrar_vuelo(vuelo);
  return true;
}

bool _prioridad_vuelos(vuelos_t *vuelos, int cant_vuelos) { return true; }

bool _borrar(vuelos_t *vuelos, fecha_t *desde, fecha_t *hasta) {

  cola_t *resultado = cola_crear();
  if (!resultado)
    return false;

  // Fecha - 0 (cualquier vuelo)
  char *clave_inicial = generar_clave(desde, 0);
  // Incremento 1 segundo la fecha límite
  fecha_sumar_segundos(hasta, 1);

  char *clave_limite = generar_clave(hasta, 0);
  abb_iter_t *iter_vuelos =
      abb_iter_in_crear_desde(vuelos->abb_vuelos, clave_inicial);
  free(clave_inicial);
  while (!abb_iter_in_al_final(iter_vuelos)) {
    const char *clave_actual = abb_iter_in_ver_actual(iter_vuelos);
    if (abb_vueloscmp(clave_actual, clave_limite) >= 0)
      break;
    cola_encolar(resultado, strdup(clave_actual));
    abb_iter_in_avanzar(iter_vuelos);
  }

  while (!cola_esta_vacia(resultado)) {
    // Clave = "fecha - codigo_vuelo"
    // split(Clave, ' ') -> ["fecha", "-", "codigo_vuelo", NULL]
    char *actual = cola_desencolar(resultado);
    char **actual_v = split(actual, ' ');

    abb_borrar(vuelos->abb_vuelos, actual);
    hash_borrar(vuelos->hash_vuelos, actual_v[AVC_COD_VUELO]);
    printf("%s\n", actual);
    free(actual);
  }

  cola_destruir(resultado, NULL);
  free(clave_limite);
  abb_iter_in_destruir(iter_vuelos);

  return true;
}

bool agregar_archivo(vuelos_t *vuelos, char **args, size_t argc) {
  if (argc != ARGS_AGREGAR_ARCHIVO)
    return false;
  return _agregar_archivo(vuelos, args[AA_FILE_NAME]);
}

bool ver_tablero(vuelos_t *vuelos, char **args, size_t argc) {
  if (argc != ARGS_VER_TABLERO)
    return false;
  size_t cant_vuelos = atoi(args[VT_CANT_VUELOS]);
  char *modo = args[VT_MODO];
  if (strcmp(modo, DESC) != 0 && strcmp(modo, ASC) != 0)
    return false;
  fecha_t *desde = fecha_crear(args[VT_DESDE]);
  fecha_t *hasta = fecha_crear(args[VT_HASTA]);

  // Si pudo allocar memoria para las fechas y además son validas
  bool resultado =
      (desde && hasta) && _ver_tablero(vuelos, cant_vuelos, modo, desde, hasta);
  free(desde);
  free(hasta);
  return resultado;
}

bool info_vuelo(vuelos_t *vuelos, char **args, size_t argc) {
  if (argc != ARGS_INFO_VUELO)
    return false;
  return _info_vuelo(vuelos, args[IV_NUM_VUELO]);
}

bool prioridad_vuelos(vuelos_t *vuelos, char **args, size_t argc) {
  if (argc != ARGS_PRIORIDAD_VUELO)
    return false;
  int prioridad = atoi(args[PV_PRIORIDAD]);
  return _prioridad_vuelos(vuelos, prioridad);
}

bool borrar(vuelos_t *vuelos, char **args, size_t argc) {
  if (argc != ARGS_BORRAR)
    return false;
  bool ok = true;
  fecha_t *desde = fecha_crear(args[B_DESDE]);
  fecha_t *hasta = fecha_crear(args[B_HASTA]);
  if (fechacmp(desde, hasta) > 0)
    ok = false;
  // Si pudo allocar memoria para las fechas y además son validas
  ok &= (desde && hasta) && _borrar(vuelos, desde, hasta);
  free(desde);
  free(hasta);
  return ok;
}
