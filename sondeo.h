#include <stdio.h>
#include <stdio_ext.h>
#include <fstream>
#include <iostream>
#include "linkLayer.h"


using namespace std;

// EJECUTA EL PROTOCOLO DE SONDEO PARA EL MAESTRO
void MaestroSondeo(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2]);


// EJECUTA EL PROTOCOLO DE SONDEO PARA EL ESCLAVO
void EsclavoSondeo(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2], unsigned char direccion, unsigned char control, unsigned char numeroTrama);