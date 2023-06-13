#include <stdio.h>
#include <stdio_ext.h>
#include <fstream>
#include <iostream>
#include "linkLayer.h"


using namespace std;

// Ejecuta el protocolo sondeo en el maestro
void MaestroSondeo(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2]);


// Ejecuta el protocolo sondeo en el esclavo
void EsclavoSondeo(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2], unsigned char direccion, unsigned char control, unsigned char numeroTrama);