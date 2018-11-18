#define _POSIX_C_SOURCE 200809L

#include "abb.h"
#include "analog.h"
#include "cola.h"
#include "fechautil.h"
#include "hash.h"
#include "heap.h"
#include "strutil.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CSV_SEP ','
#define CLAVE_SEP '-'
#define L_CLAVE 29
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
// Constante para heap_comparar
#define VPC_PRIORDAD 0
#define VPC_NUM_VUELO 2

#define ASC "asc"
#define DESC "desc"
struct vuelos {
  hash_t *hash_vuelos;
  abb_t *abb_vuelos;
};

typedef struct vuelo {
  char *numero;
  char *aerolinea;
  char *origen;
  char *destino;
  char *numero_cola;
  char *prioridad;
  fecha_t *fecha;
  int retraso_salida;
  int tiempo_vuelo;
  int cancelado;
} vuelo_t;

// ESTA  SIRVE PARA EL HEAP DE PRIORIDAD
int vueloscmp(const void *a, const void *b) {
  vuelo_t *v_a = (vuelo_t *)a;
  vuelo_t *v_b = (vuelo_t *)b;

  int fecha_comp = fechacmp(v_a->fecha, v_b->fecha);
  if (fecha_comp != 0)
    return fecha_comp;
  return atoi(v_a->numero) - atoi(v_b->numero);
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

void destruir_vuelo(void *dato) {
  vuelo_t *vuelo = dato;
  free(vuelo->fecha);
  free(vuelo->numero);
  free(vuelo->aerolinea);
  free(vuelo->origen);
  free(vuelo->destino);
  free(vuelo->numero_cola);
  free(vuelo->prioridad);
  free(vuelo);
}

vuelos_t *iniciar_vuelos() {
  vuelos_t *vuelos = malloc(sizeof(vuelos_t));
  if (!vuelos)
    return NULL;
  hash_t *hash_vuelos = hash_crear(destruir_vuelo);
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
  vuelo->numero = strdup(datos[NUM]);
  vuelo->aerolinea = strdup(datos[AER]);
  vuelo->origen = strdup(datos[ORI]);
  vuelo->destino = strdup(datos[DES]);
  vuelo->numero_cola = strdup(datos[TAI]);
  vuelo->prioridad = strdup(datos[PRI]);
  vuelo->fecha = fecha_crear(datos[FEC]);
  vuelo->retraso_salida = atoi(datos[DEP]);
  vuelo->tiempo_vuelo = atoi(datos[TIE]);
  vuelo->cancelado = atoi(datos[CAN]);
}

char *generar_clave(fecha_t *fecha, char *numero_vuelo) {
  char *clave = malloc(sizeof(char) * (L_CLAVE));
  char *fecha_hora = fecha_a_str(fecha);
  sprintf(clave, "%s %c %s", fecha_hora, CLAVE_SEP, numero_vuelo);
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
    free_strv(datos_vuelo);
  }
  free(linea);
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
  printf("%s %s %s %s %s %s %s %d %d %d\n", vuelo->numero, vuelo->aerolinea,
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
int vuelos_prioridad_cmp(const void *a, const void *b) {
  char **datos_vuelo1 = split((char *)a, ' ');
  char **datos_vuelo2 = split((char *)b, ' ');
  int prioridad1 = atoi(datos_vuelo1[VPC_PRIORDAD]);
  int prioridad2 = atoi(datos_vuelo2[VPC_PRIORDAD]);
  char *num_vuelo1 = datos_vuelo1[VPC_NUM_VUELO];
  char *num_vuelo2 = datos_vuelo2[VPC_NUM_VUELO];
  int resultado = prioridad2 - prioridad1;
  if (resultado == 0)
    resultado = strcmp(num_vuelo2, num_vuelo1);
  free_strv(datos_vuelo1);
  free_strv(datos_vuelo2);
  return resultado;
}

char *generar_elemento_heap(char *prioridad, char *numero_vuelo) {
  size_t lng_clave = strlen(prioridad) + strlen(numero_vuelo) + 10;
  char *clave = malloc(lng_clave * sizeof(char));
  sprintf(clave, "%s %c %s", prioridad, CLAVE_SEP, numero_vuelo);
  return clave;
}

bool _prioridad_vuelos(vuelos_t *vuelos, int cant_vuelos) {
  heap_t *vuelos_mayor_prioridad = heap_crear(vuelos_prioridad_cmp);
  if (!vuelos_mayor_prioridad)
    return false;
  hash_iter_t *iter = hash_iter_crear(vuelos->hash_vuelos);

  if (!iter)
    return false;

  vuelo_t *vuelo = NULL;
  char *clave = NULL;

  for (int i = 0; i < cant_vuelos; i++) {
    if (hash_iter_al_final(iter))
      break;
    clave = (char *)hash_iter_ver_actual(iter);
    vuelo = (vuelo_t *)hash_obtener(vuelos->hash_vuelos, clave);
    char *elemento_heap =
        generar_elemento_heap(vuelo->prioridad, vuelo->numero);

    heap_encolar(vuelos_mayor_prioridad, elemento_heap);
    hash_iter_avanzar(iter);
  }

  char *vuelo_menor_prioridad = (char *)heap_ver_max(vuelos_mayor_prioridad);
  while (!hash_iter_al_final(iter)) {
    clave = (char *)hash_iter_ver_actual(iter);
    vuelo = (vuelo_t *)hash_obtener(vuelos->hash_vuelos, clave);
    char *vuelo_nuevo =
        (char *)generar_elemento_heap(vuelo->prioridad, vuelo->numero);

    if (vuelos_prioridad_cmp(vuelo_nuevo, vuelo_menor_prioridad) < 0) {
      free(heap_desencolar(vuelos_mayor_prioridad));
      heap_encolar(vuelos_mayor_prioridad, vuelo_nuevo);
      vuelo_menor_prioridad = (char *)heap_ver_max(vuelos_mayor_prioridad);
    }
    // free(vuelo_nuevo);
    hash_iter_avanzar(iter);
  }

  hash_iter_destruir(iter);

  size_t cantidad_vuelos = heap_cantidad(vuelos_mayor_prioridad);
  char **vuelos_prioritarios = malloc(cantidad_vuelos * sizeof(char *));

  for (int i = (int)cantidad_vuelos - 1; i >= 0; i--) {
    vuelos_prioritarios[i] = (char *)heap_desencolar(vuelos_mayor_prioridad);
  }

  for (int i = 0; i < cantidad_vuelos; i++) {
    printf("%s\n", vuelos_prioritarios[i]);
    free(vuelos_prioritarios[i]);
  }

  heap_destruir(vuelos_mayor_prioridad, free);
  free(vuelos_prioritarios);
  return true;
}

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
    vuelo_t *vuelo_borrado =
        hash_borrar(vuelos->hash_vuelos, actual_v[AVC_COD_VUELO]);
    destruir_vuelo(vuelo_borrado);
    printf("%s\n", actual);
    free(actual);
    free_strv(actual_v);
  }

  cola_destruir(resultado, free);
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
