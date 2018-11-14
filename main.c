#include "analog.h"
#include "testing.h"
#include <stdio.h>
/* ******************************************************************
 *                        PROGRAMA PRINCIPAL
 * *****************************************************************/

int main(void) {
  /* Ejecuta todas las pruebas unitarias. */
  printf("~~~ PRUEBAS DEL ALUMNO ~~~\n");
  pruebas_tp2_alumno();
  return failure_count() > 0;
}
