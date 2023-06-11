#include <stdio.h>
#include <stdio_ext.h>
#include <fstream>
#include <iostream>
#include "linkLayer.h"


using namespace std;

// Metodo encargado de la operacion de sondeo del maestro
void MaestroSondeo(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2]);


// Metodo encargado de la operacion de sondeo del esclavo
void EsclavoSondeo(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2], unsigned char direccion, unsigned char control, unsigned char numeroTrama);