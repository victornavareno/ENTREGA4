//============================================================================
// ----------- PRACTICAS DE FUNDAMENTOS DE REDES Y COMUNICACIONES -----------
// ----------------------------- main.CPP ---------------------------------
//============================================================================

// AUTORES:
//  Víctor Andrés Navareño Moza
//  GRUPO 6 Prácticas - Miércoles 10:00 - 11:30

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "linkLayer.h"
#include "seleccionInterfaz.h"
#include "enviarRecibir.h"
#include "paroYEspera.h"
#include "stdio_ext.h"

using namespace std;

// ejecuta el programa entero, llamando a métodos definidos en seleccionInterfaz.h
int main()
{
    interface_t iface;
    unsigned char mac_destino[6]; // Almacenará la mac que recibirá nuestros paquetes
    unsigned char mac_src[6];
    unsigned char type[2] = {0x30, 0x00};

    // VARIABLES ENTREGA 3
    int numeroGrupo; // almacenamos el numero de nuestro grupo aquí, lo introduce el usuario
    int modoEstacion;
    int modoEnvio;

    imprimirSeleccionInterfaz();
    usuarioIntroduceInterfaz();

    iface = getInterfaz();
    imprimirMac(iface);

    numeroGrupo = SeleccionGrupo(type);

    // abrir puerto
    int puerto = OpenAdapter(&iface); // Puerto debe devolver 0 si no hay ningún problema al abrirse
    if (puerto != 0)
    {
        cout << "Error al abrir el puerto" << endl;
    }
    else
    {
        cout << "Puerto abierto correctamente" << endl;
    }

    modoEstacion = SeleccionModoEstacion();

    if (modoEstacion == 1)
    {
        cout << "Esperando a que se una la estación esclava..." << endl;
        EnviarTramaMaestro(numeroGrupo, &iface, mac_destino);
        cout << "Estación esclava encontrada. ";
        ImprimirMacDestino(mac_destino);
    }
    else if (modoEstacion == 2)
    {
        cout << "Esperando a que se una la estación maestra..." << endl;
        RecibirTramaEsclavo(numeroGrupo, &iface, mac_destino);
        cout << "Estación maestra encontrada. ";
        ImprimirMacDestino(mac_destino);
    }

    SeleccionModoEnvio(modoEstacion, numeroGrupo, &iface, mac_src, mac_destino, type);

    // cerramos el puerto
    if (CloseAdapter(&iface) == 0)
    {
        cout << "Puerto cerrado" << endl;
    }
    else
        cout << " ### ERROR AL CERRAR EL PUERTO ### " << endl;
    return 0;
}