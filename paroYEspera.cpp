#include "paroYEspera.h"

using namespace std;

bool MaestroParoYEspera(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2])
{
    // MENU SELECCION PAROYESPERA  -  MAESTRO
    cout << "Seleccione el tipo de operacion: " << endl;
    cout << "[1] Operacion seleccion. " << endl;
    cout << "[2] Operacion sondeo. " << endl;
    cout << "[3] Salir. " << endl;

    unsigned char teclaPulsada = getch();
    if (teclaPulsada == '1') // MODO SELECCION MAESTRO
    {
        cout << "PULSADO : 1" << endl;
        MaestroSeleccion(interfaz, mac_origen, mac_destino, tipo);
    }
    else if (teclaPulsada == '2') // MODO SONDEO MAESTRO
    {
        MaestroSondeo(interfaz, mac_origen, mac_destino, tipo);
    }
    else if (teclaPulsada == '3') // SALIR
    {
        cout << "Saliendo de Paro y Espera..." << endl;
        return true;
    }
    else
        cout << "TECLA INVALIDA" << endl; // BREAK O ALGO ASI
        return false;
}

void MaestroSeleccion(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2])
{
    unsigned char direccion = 'R'; // El valor de Dirección será ‘R’ operación de Selección - 'T' operación de Sondeo

    unsigned char control = 5; // Depende del tipo de trama de control que se use:
                               // • Trama de control ENQ: Valor 05.
                               // • Trama de control EOT: Valor 04.
                               // • Trama de control ACK: Valor 06.
                               // • Trama de control NACK: Valor 21.
                               // • Trama de datos - STX: Valor 02.

    unsigned char numeroTrama = '0'; // oscilará entre los valores ‘0’ y ‘1’ (como carácter, no int ni nada raro)

    // Enviamos la trama de control
    EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, control, numeroTrama);

    // Mostramos la trama enviada por pantalla (POR ESO LA 'E')
    MostrarTrama('E', direccion, control, numeroTrama, ' ');

    // Seguimos recibiendo tramas de control
    RecibirTramaControl(interfaz, direccion, control, numeroTrama);

    // Cuando recibamos una trama de control con valor control 6, la mostramos con una 'R', de recibida:
    if (control == 6)
    {
        MostrarTrama('R', direccion, control, numeroTrama, ' ');
    }

    // Envio de fichero en Paro y espera (SOLO MAESTRO)
    EnviarFicheroParoyEspera(interfaz, mac_origen, mac_destino, tipo, direccion, control, numeroTrama);
    cout << endl;

    // // Envio de la trama 'EOT' para indicar fin de envio
    EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, 4, '0');

    // Seguimos recibiendo tramas de control
    RecibirTramaControl(interfaz, direccion, control, numeroTrama);

    // Cuando recibamos una trama de control con valor control 6, la mostramos con una 'R', de recibida:
    if (control == 6)
    {
        MostrarTrama('R', direccion, control, numeroTrama, ' ');
    }

    cout << "Fin de Seleccion por parte del Maestro" << endl;
}

void EsclavoSeleccion(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2], unsigned char direccion, unsigned char control, unsigned char numeroTrama)
{
    ofstream outputStream;
    outputStream.open("RProtoc.txt");
    char cadena[254];
    unsigned char longitud;
    bool fin = false;

    // ENVIAMOS LA TRAMA DE CONTROL (ACK)
    EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, 6, numeroTrama);

    while (control != 4 && !fin)
    {
        RecibirTramaParoyEspera(interfaz, direccion, control, numeroTrama, cadena, longitud);

        if (control == 4)
        {
            printf("\n");
            MostrarTrama('R', direccion, control, numeroTrama, ' ');
            control = 6;
            fin = true;
        }
        else
        {
            if (longitud != 0 && outputStream.is_open())
            {
                outputStream.write(cadena, longitud);
                control = 6;
            }
            else
            {
                control = 21;
            }
        }
        EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, control, numeroTrama);
    }

    outputStream.close();
    cout << "Fin de Seleccion por parte del Esclavo." << endl;
}

// Recibe las tramas de control que envia el maestro
void EsclavoParoYEspera(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2]) {
    printf("Estas en modo esclavo\n\n");
    unsigned char direccion, control, NTrama;

    RecibirTramaControl(interfaz, direccion, control, NTrama);
    if(direccion == 'R') {
        MostrarTrama('R', direccion, control, NTrama, ' ');
        EsclavoSeleccion(interfaz, mac_origen, mac_destino, tipo ,direccion, control, NTrama);
    }
    if(direccion == 'T') {
        MostrarTrama('R', direccion, control, NTrama, ' ');
        EsclavoSondeo(interfaz, mac_origen, mac_destino, tipo ,direccion, control, NTrama);
    }       
}

void EnviarTramaControl(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2], unsigned char direccion, unsigned char control, unsigned char numeroTrama)
{
    // Las tramas de control contendrán 3 valores diferenciados: direccion, control y numeroTrama.
    // Los enviamos juntos en un Array:
    unsigned char datosTramaControl[3] = {
        direccion,
        control,
        numeroTrama};

    // Construimos la trama
    unsigned char *tramaEnviada = BuildFrame(mac_origen, mac_destino, tipo, datosTramaControl);

    // Enviamos la trama
    SendFrame(interfaz, tramaEnviada, 3);

    // liberamos memoria de la trama que hemos enviado
    free(tramaEnviada);
}

// BCE SOLO NECESARIO PARA TRAMAS DE DATOS
void MostrarTrama(unsigned char tipo, unsigned char direccion, unsigned char control, unsigned char numeroTrama, char BCE)
{
    if (control == 2) // TRAMAS DE DATOS - STX
    {
        cout << tipo << "  " << direccion << "  "  << "STX  " <<  numeroTrama << "  " << BCE << endl;
    }

    else // TRAMAS DE CONTROL
    {
        cout << tipo << "  " << direccion << "  "; // Si la trama es enviada, tipo = 'E', si es Recibida, tipo = 'R'
        switch (control)
        {
        case 4:
            cout << "EOT  " << numeroTrama << endl;
            break;
        case 5:
            cout << "ENQ  " << numeroTrama << endl;
            break;
        case 6:
            cout << "ACK  " << numeroTrama << endl;
            break;
        default:
            cout << "EOT  " << numeroTrama << endl;
            break;
        }
    }
}

void RecibirTramaControl(interface_t *interfaz, unsigned char &direccion, unsigned char &control, unsigned char &numeroTrama)
{
    bool recibida = false;
    while (!recibida)
    {
        // Esperamos a recibir una trama de control
        apacket_t trama = ReceiveFrame(interfaz);

        if (trama.packet != NULL)
        {
            recibida = true;
            // if (trama.packet[12] == 0x88 && trama.packet[13] == 0x08){ // NO HACE FALTA COMPROBAR

            // Extraemos la direccion, control y numero de trama de la trama recibida, y actualizamos los valores input-output
            direccion = trama.packet[14];
            control = trama.packet[15];
            numeroTrama = trama.packet[16];
        }
    }
}


// Envia las tramas de datos
void EnviarTramaDatos(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2], unsigned char direccion, unsigned char control, unsigned char numeroTrama, unsigned char longitud, unsigned char cadena[], unsigned char BCE)
{
    int i = 0;
    unsigned char datos[259];
    datos[0] = direccion;
    datos[1] = control;
    datos[2] = numeroTrama;
    datos[3] = longitud;

    while (i < longitud)
    {
        datos[4 + i] = cadena[i];
        i++;
    }

    datos[4 + i] = BCE;
    unsigned char *trama = BuildFrame(mac_origen, mac_destino, tipo, datos);
    SendFrame(interfaz, trama, longitud + 5);
    MostrarTrama('E', direccion, control, numeroTrama, BCE);
    free(trama);
}

// REEESCRIBIR PARA ANTICOPIA!! TODO TODO TODO TODOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO##~#@~@#~|@#~|@#~|@#~|@#~|@#~|@#4
void EnviarFicheroParoyEspera(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2], unsigned char direccion, unsigned char control, unsigned char numeroTrama)
{
    ifstream flujoLectura("EProtoc.txt"); // introduce aquí el archivo  y la ruta del fichero que deseas enviar

    char cadena[254]; // cadena a leer
    unsigned char numeroTramaRecibida = ' ';
    int longitud;             // longitud de la trama a enviar (sirve para calcular BCE)
    unsigned char correccion; // variable auxiliar para corregir la trama enviada

    int numErrores = 0; // cuenta el numero de errores introducidos
    //__fpurge(stdin);

    if (flujoLectura.is_open())
    {
        while (!flujoLectura.eof())
        {
            // Leemos del fichero e inicializamos los parametros
            flujoLectura.read(cadena, 254);
            longitud = flujoLectura.gcount();
            unsigned char BCE = CalculoBCE(longitud, cadena);
            control = 2; // NUMERO PARA STX

            // Introducimos el error si se ha pulsado F4
            if (numErrores != 0)
            {
                printf("INTRODUCIENDO ERROR\n");
                correccion = cadena[0];
                cadena[0] = 184;
                numErrores--;
            }

            // Enviamos la trama de datos
            EnviarTramaDatos(interfaz, mac_origen, mac_destino, tipo, direccion, control, numeroTrama, longitud, (unsigned char *)cadena, BCE);
            printf("\n");

            // Esperamos a recibir la trama de respuesta del esclavo:
            while (control != 6 || numeroTrama != numeroTramaRecibida)
            {
                RecibirTramaControl(interfaz, direccion, control, numeroTramaRecibida);

                if (control == 21) // 21: Trama NACK - ERROR
                {                  // Si envian una trama NACK, corregimos los datos y los enviamos de nuevo
                    cadena[0] = correccion;
                    control = 2;
                    MostrarTrama('R', direccion, control, numeroTrama, ' ');
                    // ENVIAMOS LA RESPUESTA
                    EnviarTramaDatos(interfaz, mac_origen, mac_destino, tipo, direccion, control, numeroTrama, longitud, (unsigned char *)cadena, BCE);
                    printf("\n");
                }
            }
            // Trama de respuesta:
            MostrarTrama('R', direccion, control, numeroTrama, ' ');

            // INVERTIMOS VALORES DE Numero de trama
            if (numeroTrama == '1')
                numeroTrama = '0';

            if (numeroTrama == '0')
                numeroTrama = '1';
        }
    }
    else
        cout << "   ####  ERROR AL ABRIR EL FICHERO  ####   " << endl;
    // flujoLectura.close();
}


// Para calcular el valor del BCE de una trama se debe hacer la operación lógica XOR
// dos a dos entre todos los caracteres de los datos correspondiente al contenido del fichero.
char CalculoBCE(int longitudCadena, char cadena[])
{
    if (longitudCadena > 0) // Siempre debe ser mayor que 0
    {
        char BCE = cadena[0];

        for (int i = 1; i < longitudCadena; i++)
        {
            BCE = BCE xor cadena[i];
        }

        return BCE; // DEVUELVO UNA CADENA DE 0 Y 1 QUE CORRESPONDE CON EL BCE (IDENTIFICADOR)
    }
    else return 0; // Error
}

void RecibirTramaParoyEspera(interface_t *interfaz, unsigned char &direccion, unsigned char &control, unsigned char &numeroTrama, char cadena[], unsigned char &longitud)
{
    bool recibida = false;

    while (!recibida)
    {
        apacket_t trama = ReceiveFrame(interfaz);
        if (trama.packet != NULL)
        {
            recibida = true;
            direccion = trama.packet[14];
            control = trama.packet[15];
            numeroTrama = trama.packet[16];

            // SI EL CONTROL ES STX, ES DECIR 2(TRAMA DE DATOS), LOS DATOS SE GUARDAN EN EL FICHERO DIRECTAMENTE
            // E IMPRIMIMOS POR PANTALLA LA TRAMA RECIBIDA
            if (control == 2)
            {
                longitud = trama.packet[17];
                unsigned char BCE = trama.packet[18 + longitud];

                for (int i = 0; i < longitud; i++)
                    cadena[i] = trama.packet[18 + i];

                unsigned char BCECalc = CalculoBCE(longitud, cadena);

                if (BCECalc != BCE)
                    longitud = 0;

                MostrarTrama('R', direccion, control, numeroTrama, BCE);
                cout << BCECalc << endl;
            }
        }
    }
}
