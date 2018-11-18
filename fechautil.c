#include "fechautil.h"
#include "strutil.h"
#include <stdio.h>
#include <stdlib.h>
#define L_FECHA 11 // Longitud Fecha: 4 + 2 + 2 + 2(separadores) + 1
#define L_HORA 9   // Longitud Hora: 2 + 2 + 2 + 2(separadores) + 1
#define L_FECHA_HORA (L_FECHA + L_HORA + 1)
#define FECHA_HORA_SEP 'T'
#define FECHA_SEP '-'
#define HORA_SEP ':'
enum { FECHA, HORA };    // Orden Fecha_Hora
enum { ANIO, MES, DIA }; // Orden de Fecha
enum { HH, MM, SS };     // Orden de Hora
#define SEGUNDOS 60      // Segundos en 1 minuto
#define MINUTOS 60       // Minutos en 1 hora
struct fecha {
  int d; // Día
  int m; // Mes
  int a; // Año
  int H; // Hora
  int M; // Min
  int S; // Seg
};

fecha_t *fecha_crear(const char *str) {
  if (!fecha_valida(str))
    return NULL;
  fecha_t *fecha_r = malloc(sizeof(fecha_t));
  if (!fecha_r)
    return NULL;

  char **fecha_hora = split(str, FECHA_HORA_SEP);
  char **fecha = split(fecha_hora[FECHA], FECHA_SEP);
  char **hora = split(fecha_hora[HORA], HORA_SEP);

  fecha_r->d = atoi(fecha[DIA]);
  fecha_r->m = atoi(fecha[MES]);
  fecha_r->a = atoi(fecha[ANIO]);

  fecha_r->H = atoi(hora[HH]);
  fecha_r->M = atoi(hora[MM]);
  fecha_r->S = atoi(hora[SS]);

  free_strv(hora);
  free_strv(fecha);
  free_strv(fecha_hora);

  return fecha_r;
}
// analogo strcmp. DOCUMENTAR BIEN
int fechacmp(const fecha_t *a, const fecha_t *b) {
  // Me fijo primero la fecha
  int dif_anio = a->a - b->a;
  if (dif_anio != 0)
    return dif_anio;
  // Mismo año
  int dif_mes = a->m - b->m;
  if (dif_mes != 0)
    return dif_mes;
  // Mismo año y mismo mes
  int dif_dia = a->d - b->d;
  if (dif_dia != 0)
    return dif_dia;
  // Mismo año, mes, dia
  // Me fijo la hora->
  int dif_hora = a->H - b->H;
  if (dif_hora != 0)
    return dif_hora;
  // Misma hora
  int dif_min = a->M - b->M;
  if (dif_min != 0)
    return dif_min;
  // Misma hora y minutos
  int dif_seg = a->S - b->S;
  return dif_seg;
}

// Creo que hay que devolverlo en Mem dinamica
char *fecha_a_str(fecha_t *fecha) {
  char fecha_s[L_FECHA];
  sprintf(fecha_s, "%04d%c%02d%c%02d", fecha->a, FECHA_SEP, fecha->m, FECHA_SEP,
          fecha->d);
  char hora[L_HORA];
  sprintf(hora, "%02d%c%02d%c%02d", fecha->H, HORA_SEP, fecha->M, HORA_SEP,
          fecha->S);
  char *fecha_hora = malloc(sizeof(char) * (L_FECHA_HORA));
  sprintf(fecha_hora, "%s%c%s", fecha_s, FECHA_HORA_SEP, hora);
  return fecha_hora;
}

// Hacerlo bien jajaja.
bool fecha_valida(const char *str) { return true; }

void fecha_sumar_segundos(fecha_t *fecha, int segundos) {
  fecha->S = fecha->S + segundos;
}
