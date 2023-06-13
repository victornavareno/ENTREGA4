#include <stdio.h>
#include <iostream>
#include <fstream>
#include "linkLayer.h"
#include "paroYEspera.h"
#include "sondeo.h"
#include "stdio_ext.h"

using namespace std;


void RecibirEsclavo(interface_t *interfaz, int numGrupo);

void SeleccionModoEnvio(int modo, int numGrupo, interface_t *interfaz, unsigned char mac_src[6], unsigned char mac_destino[6], unsigned char tipo[2]);

void ImprimirMacDestino(unsigned char mac_destino[6]);

int SeleccionGrupo(unsigned char tipo[2]);

int SeleccionModoEstacion();

void EnviarTramaMaestro(int numGrupo, interface_t *interfaz, unsigned char mac_destino[6]);

void RecibirTramaEsclavo(int numGrupo, interface_t *interfaz, unsigned char mac_destino[6]);

void EnviarCaracter(interface_t *iface, char _dato, unsigned char mac_src[6], unsigned char mac_dst[6], unsigned char type[2]);

bool EnviarFichero(interface_t *interfaz, unsigned char mac_src[6], unsigned char mac_destino[6], unsigned char tipo[2]);

char RecibirCaracter(interface_t *iface);

void EnviarRecibirCaracter(interface_t *interface, unsigned char mac_src[6], unsigned char mac_destino[6], unsigned char tipo[2]);