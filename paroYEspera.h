#include <stdio.h>
#include <stdio_ext.h>
#include <fstream>
#include <iostream>
#include "linkLayer.h"
#include "sondeo.h"

using namespace std;

//EJECUTA EL MENU DE SELECCION DE PARO Y ESPERA DEL MAESTRO
bool MaestroParoYEspera(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2]);

// EJECUTA EL MODO SELECCION DENTRO DE PARO Y ESPERA DEL MAESTRO
void MaestroSeleccion(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2]);

// EJECUTA EL MODO SELECCION DENTRO DE PARO Y ESPERA DEL ESCLAVO
void EsclavoSeleccion(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2], unsigned char direccion, unsigned char control, unsigned char numeroTrama);

// METODO QUE ENVIA TRAMAS DE CONTROL
void EnviarTramaControl(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2], unsigned char direccion, unsigned char control, unsigned char numeroTrama); 

// MUESTRA LA TRAMA DE CONTROL O DE DATOS DADOS SUS PARAMETROS
void MostrarTrama(unsigned char tipo, unsigned char direccion, unsigned char control, unsigned char numeroTrama, char BCE);

// METODO PARA RECIBIR LAS TRAMAS DE CONTROL Y MODIFICAR LOS PARAMETROS DIRECCION, CONTROL Y TRAMA
void RecibirTramaControl(interface_t *interfaz, unsigned char &direccion, unsigned char &control, unsigned char &numeroTrama);

// ENVIA UN FICHERO DIVIDIDO EN TRAMAS, PERO POR PARO Y ESPERA
void EnviarFicheroParoyEspera(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2],unsigned char direccion, unsigned char control, unsigned char numeroTrama);

// CALCULA EL BCE 
char CalculoBCE(int longitud, char cadena[]);

void EsclavoParoYEspera(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2]);

// RECIBIR TRAMAS EN PROTOCOLO PARO Y ESPERA
void RecibirTramaParoyEspera(interface_t *interfaz, unsigned char &direccion, unsigned char &control, unsigned char &numeroTrama, char cadena[], unsigned char &longitud);