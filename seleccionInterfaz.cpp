// AUTORES:
//  Víctor Andrés Navareño Moza
//  Javier Crespo Gómez
//  GRUPO 6 Prácticas - Miércoles 10:00 - 11:30

#include "seleccionInterfaz.h"

using namespace std;

// Variables globales:
interface_t iface;              // nuestra interfaz
pcap_if_t *avail_ifaces = NULL; // lista de interfaces del equipo

void imprimirSeleccionInterfaz()
{
    pcap_if_t *avail_ifaces_aux = NULL; // variable auxiliar para recorrer el vector de interfaces disponibles

    int indice = 0;

    avail_ifaces = GetAvailAdapters(); // asigno la lista de interfaces disponibles
    avail_ifaces_aux = avail_ifaces;

    printf("Interfaces disponibles: \n");
    while (avail_ifaces_aux->next != nullptr)
    {
        cout << "[" << indice << "] " << avail_ifaces_aux->name << endl;
        indice++;
        avail_ifaces_aux = avail_ifaces_aux->next;
    }
    delete avail_ifaces_aux;
}

// el usuario debe introducir un índice correcto
void usuarioIntroduceInterfaz()
{
    pcap_if_t *avail_ifaces_aux = NULL; // lo usaremos para recorrer el vector

    int indice = 0;
    int indiceElegido;
    bool encontrado = false;

    avail_ifaces_aux = avail_ifaces; // reseteo el puntero auxiliar
    cout << endl;
    printf("Seleccione interfaz: ");
    cin >> indiceElegido; // el usuario introduce la interfaz que quiere elegir

    printf("Interfaz elegida: ");
    while (!encontrado && avail_ifaces_aux->next)
    {
        if (indiceElegido == indice)
        {
            setDeviceName(&iface, avail_ifaces_aux->name);
            cout << avail_ifaces_aux->name << endl;
            encontrado = true;
        }
        indice++;
        avail_ifaces_aux = avail_ifaces_aux->next;
    }
    delete avail_ifaces_aux;
}

void imprimirMac(interface_t interfaz)
{
    interface_t *ifacePtrAux;
    ifacePtrAux = new interface_t; // reservo memoria para el puntero auxiliar
    GetMACAdapter(&interfaz);      // obtenemos la direccion MAC de la interfaz elegida por el usuario
    printf("La MAC es: ");

    // asumimos que el una mac tiene 6 dígitos hexadecimales
    for (size_t i = 0; i < 6; i++)
    {
        // condicion para imprimir dos puntos entre cada par de digitos hexadecimales de la MAC
        if (!(i == 0 || i == 6))
        {
            cout << ":";
        }
        printf("%02X", interfaz.MACaddr[i]); // el -%02 indica que queremos mínimo 2 cifras, la X indica que el formato de salida será hexadecimal con letras mayúsculas
    }
    printf("\n"); // salto de linea
    delete ifacePtrAux;
}

interface_t getInterfaz()
{
    return iface;
}