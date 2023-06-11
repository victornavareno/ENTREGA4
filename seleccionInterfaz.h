// AUTORES:
//  Víctor Andrés Navareño Moza
//  Javier Crespo Gómez
//  GRUPO 6 Prácticas - Miércoles 10:00 - 11:30

#include <stdio.h>
#include <iostream>
#include "linkLayer.h"
#include "paroYEspera.h"

//      Conjunto de métodos para elegir, asignar y mostrar una interfaz de tu equipo y su dirección MAC

using namespace std;

/*
 * Muestra la lista de interfaces de tu equipo, indexadas para que el usuario elija una
 */
void imprimirSeleccionInterfaz();

/*
 * Permite al usuario introducir el índice de la interfaz que desea seleccionar
 * EL ÍNDICE INTRODUCIDO DEBE SER VÁLIDO
 */
void usuarioIntroduceInterfaz();

/*
 * Imprime la direccion MAC asociada a la interfaz pasada
 * recibe interface_t interfaz
 */
void imprimirMac(interface_t interfaz);

/*
 * devuelve la interfaz elegida
 */
interface_t getInterfaz();