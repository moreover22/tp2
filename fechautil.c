#include "fechautil.h"
#include "strutil.h"
#include <stdlib.h>

#define L_FECHA 10 // Longitud Fecha: 4 + 2 + 2 + 2(separadores)
#define L_HORA 8   // Longitud Hora: 2 + 2 + 2 + 2(separadores)
#define L_FECHA_HORA (L_FECHA + L_HORA + 1)
#define FECHA_HORA_SEP 'T'
#define FECHA_SEP '-'
#define HORA_SEP ':'
enum { FECHA, HORA };    // Orden Fecha_Hora
enum { ANIO, MES, DIA }; // Orden de Fecha
enum { HH, MM, SS };     // Orden de Hora

struct fecha {
  size_t d; // Día
  size_t m; // Mes
  size_t a; // Año
  size_t H; // Hora
  size_t M; // Min
  size_t S; // Seg
};

fecha_t *crearFecha(const char *str) {
  fecha_t *fecha_r = malloc(sizeof(fecha_t));
  if (!fecha)
    return NULL;

  char **fecha_hora = split(str, FECHA_HORA_SEP);
  char **fecha = split(fecha_hora[FECHA], FECHA_SEP);
  char **hora = split(fecha_hora[HORA], HORA_SEP);
  fecha->d = atoi(fecha[DIA]);
  fecha->m = atoi(fecha[MES]);
  fecha->a = atoi(fecha[ANIO]);

  fecha->H = atoi(hora[HH]);
  fecha->M = atoi(hora[MM]);
  fecha->S = atoi(hora[SS]);

  free_strv(hora);
  free_strv(fecha);
  free_strv(fecha_hora);

  return fecha_r;
}
// analogo strcmp. DOCUMENTAR BIEN
int fechacmp(const fecha_t a, const fecha_t b) {
  // Me fijo primero la fecha
  int dif_anio = a.a - b.a;
  if (dif_anio != 0)
    return dif_anio;
  // Mismo año
  int dif_mes = a.m - b.m;
  if (dif_mes != 0)
    return dif_mes;
  // Mismo año y mismo mes
  int dif_dia = a.d - b.d;
  if (dif_mes != 0)
    return dif_dia;
  // Mismo año, mes, dia
  // Me fijo la hora.
  int dif_hora = a.H - b.H;
  if (dif_hora != 0)
    return dif_hora;
  // Misma hora
  int dif_min = a.M - b.M;
  if (dif_min != 0)
    return dif_min;
  // Misma hora y minutos
  int dif_seg = a.S - b.S;
  return dif_seg;
}
// Creo que hay que devolverlo en Mem dinamica
char *fechaAStr(fecha_t *fecha) {
  char fecha[L_FECHA];
  sprintf(fecha, "%d%c%d%c%d", fecha.a, FECHA_SEP, fecha.m, FECHA_SEP, fecha.d);
  char hora[L_HORA];
  sprintf(hora, "%d%c%d%c%d", fecha.H, HORA_SEP, fecha.M, HORA_SEP, fecha.S,
          HORA_SEP);
  char *fecha_hora = malloc(sizeof(char) * L_FECHA_HORA);
  sprintf(fecha_hora, "%s%c%s", fecha, FECHA_HORA_SEP, hora);
  return fecha_hora;
}
