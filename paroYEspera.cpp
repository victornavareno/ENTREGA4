#include "paroYEspera.h"

using namespace std;

bool MaestroParoYEspera(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2])
{
    // MENU SELECCION PAROYESPERA  -  MAESTRO
    cout << "Seleccione el tipo de operacion: " << endl;
    cout << "[1] Operacion seleccion. " << endl;
    cout << "[2] Operacion sondeo. " << endl;
    cout << "[3] Salir. " << endl
         << endl;

    unsigned char teclaPulsada = getch();
    if (teclaPulsada == '1') // MODO SELECCION MAESTRO
    {
        cout << "PULSADO : 1 - Iniciando protocolo de seleccion..." << endl;
        MaestroSeleccion(interfaz, mac_origen, mac_destino, tipo);
    }
    else if (teclaPulsada == '2') // MODO SONDEO MAESTRO
    {
        cout << "PULSADO : 2 - Iniciando protocolo de sondeo..." << endl;
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

    unsigned char control = 5; // Al principio enviamos una trama ENQ para saber si el esclavo esta listo
                               // • Trama de control ENQ:  Valor 05.
                               // • Trama de control EOT:  Valor 04.
                               // • Trama de control ACK:  Valor 06.
                               // • Trama de control NACK: Valor 21.
                               // • Trama de datos - STX:  Valor 02.

    unsigned char numeroTrama = '0'; // oscilará entre los valores ‘0’ y ‘1’ (como carácter, no int ni nada raro)

    // Enviamos la trama de control /* DIRECCION // CONTROL // NUMERO DE TRAMA */
    EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, control, numeroTrama);

    // Mostramos la trama enviada por pantalla (POR ESO LA 'E') LA RECIBIDA MOSTRARA 'R'
    MostrarTrama('E', direccion, control, numeroTrama, ' ');

    // Seguimos recibiendo tramas de control
    RecibirTramaControl(interfaz, direccion, control, numeroTrama);

    // Cuando recibamos una trama de control con valor control 6 (ACK), la mostramos con una 'R', de recibida:
    // RECORDAMOS QUE ACK ES LA TRAMA DE CONFIRMACION, LA TRAMA ENVIADA HA SIDO RECIBIDA
    if (control == 6)
    {
        MostrarTrama('R', direccion, control, numeroTrama, ' ');
        cout << endl;
    }

    // Envio de fichero en Paro y espera (SOLO MAESTRO)
    EnviarFicheroParoyEspera(interfaz, mac_origen, mac_destino, tipo, direccion, control, numeroTrama);

    // Envio de la trama 'EOT' para indicar fin de envio
    EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, 4, '0');
    MostrarTrama('E', direccion, 4, numeroTrama, ' ');

    // Seguimos recibiendo tramas de control
    RecibirTramaControl(interfaz, direccion, control, numeroTrama);

    // Cuando recibamos una trama de control con valor control 6, la mostramos con una 'R', de recibida:
    if (control == 6)
    {
        MostrarTrama('R', direccion, control, numeroTrama, ' ');
    }

    cout << "Fin de Seleccion por parte del Maestro" << endl
         << endl;
}

void EsclavoSeleccion(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2], unsigned char direccion, unsigned char control, unsigned char numeroTrama)
{
    // EN ESTE FICHERO ESCRIBIREMOS LAS TRAMAS RECIBIDAS
    ofstream outputStream;
    outputStream.open("RProtoc.txt");

    // BUFFERED READER:
    char cadena[254];
    unsigned char longitud;

    // ENVIO LA TRAMA DE RESPUESTA CORRECTA(ACK) PARA ESTABLECER EL PROTOCOLO DE SELECCION
    EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, 6, numeroTrama);

    bool fin = false;
    while (!fin && control != 4)
    {
        RecibirTramaParoyEspera(interfaz, direccion, control, numeroTrama, cadena, longitud);
        if (control == 4)
        {
            cout << endl;
            MostrarTrama('R', direccion, control, numeroTrama, ' ');
            control = 6;
            fin = true;
        }

        else
        {
            if (longitud != 0 && outputStream.is_open())
            {
                outputStream.write(cadena, longitud); // ESCRIBIMOS EN EL FICHERO LA TRAMA RECIBIDA
                control = 6;
            }
            else
            {
                control = 21;
            }
        }
        EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, control, numeroTrama);
        MostrarTrama('E', direccion, control, numeroTrama, ' ');
    }

    outputStream.close();
    cout << "Fin de Seleccion por parte del Esclavo." << endl;
    cout << "Presione ESC para finalizar..." << endl;
}

// Recibe las tramas de control que envia el maestro
bool EsclavoParoYEspera(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2])
{
    cout << "Estas en modo esclavo" << endl;
    unsigned char direccion, control, NTrama;

    RecibirTramaControl(interfaz, direccion, control, NTrama);

    if (direccion == 'R')
    { // SI RECIBE UNA R ES PROTOCOLO SELECCION
        MostrarTrama('R', direccion, control, NTrama, ' ');
        EsclavoSeleccion(interfaz, mac_origen, mac_destino, tipo, direccion, control, NTrama);
    }

    if (direccion == 'T')
    { // SI RECIBE UNA 'T' ES PROTOCOLO SONDEO
        MostrarTrama('R', direccion, control, NTrama, ' ');
        EsclavoSondeo(interfaz, mac_origen, mac_destino, tipo, direccion, control, NTrama);
    }
    return true;
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

    // Enviamos la trama, que tendra un tamaño de 3 bytes (DIRECCION, CONTROL Y NUMEROTRAMA)
    SendFrame(interfaz, tramaEnviada, 3);

    // liberamos memoria de la trama que hemos enviado
    free(tramaEnviada);
}

// BCE SOLO NECESARIO PARA TRAMAS DE DATOS, SI NO ES DE DATOS BCE = ' '
void MostrarTrama(unsigned char tipo, unsigned char direccion, unsigned char control, unsigned char numeroTrama, char BCE)
{
    if (control == 2) // TRAMAS DE DATOS - STX
    {
        cout << tipo << "  " << direccion << "  "
             << "STX  " << numeroTrama;
        // imprimir el BCE
        printf("  %d", BCE);
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
        case 21:
            cout << "NACK  " << numeroTrama << endl;
            break;
        default:
            // cout << "EOT  " << numeroTrama << endl;
            cout << " ### ERROR LA TRAMA NO POSEE UN VALOR DE CONTROL CORRECTO ###" << endl;
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
    cout << endl;
    free(trama);
}

// Comprueba si se ha pulsado la tecla F4 = '27 + O + S'
// Si se ha pulsado F4, cont I/O se incrementa. Usaremos este metodo para introducir errores pulsando F4 durante el protocolo
void ComprobarPulsacionF4(int &cont)
{
    unsigned char teclaPulsada;
    if (kbhit())
    {
        teclaPulsada = getch();
        if (teclaPulsada == 27)
        {
            teclaPulsada = getch();
            if (teclaPulsada == 'O')
            {
                teclaPulsada = getch();
                if (teclaPulsada == 'S')
                {
                    __fpurge(stdin);
                    cont++;
                }
            }
        }
    }
}

// ENVIA EL FICHERO "EProtoc.txt" POR PARO Y ESPERA
void EnviarFicheroParoyEspera(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2], unsigned char direccion, unsigned char control, unsigned char numeroTrama)
{
    __fpurge(stdin);                      // limpiamos el input de teclado
    ifstream flujoLectura("EProtoc.txt"); // introduce aquí el archivo  y la ruta del fichero que deseas enviar

    char cadena[254]; // cadena a leer
    unsigned char numeroTramaRecibida;
    numeroTramaRecibida = ' ';
    int longitudCadena; // longitud de la trama a enviar (sirve para calcular BCE)

    // INTRODUCCION DE ERRORES AL PULSAR F4 - VERSION 5
    unsigned char correccion; // variable auxiliar para corregir la trama enviada
    int numErrores = 0;       // cuenta el numero de errores introducidos

    if (flujoLectura.is_open())
    {
        while (!flujoLectura.eof())
        {
            // LEEMOS DE FICHERO
            flujoLectura.read(cadena, 254);
            longitudCadena = flujoLectura.gcount();
            unsigned char BCE = CalculoBCE(longitudCadena, cadena);
            control = 2; // NUMERO PARA STX ESTAMOS ENVIANDO TRAMAS DE DATOS

            // SI SE HA PULSADO F4, INCREMENTO numErrores
            ComprobarPulsacionF4(numErrores);

            // Introducimos el error si se ha pulsado F4 durante el protocolo
            if (numErrores != 0)
            {
                cout << "INTRODUCIENDO ERROR..." << endl;
                correccion = cadena[0]; // Guardamos aqui el dato correcto para luego
                cadena[0] = 184;        // Introducimos el caracter especial (Error)
                numErrores--;
            }

            // Enviamos la trama de datos (QUE AHORA PUEDE CONTENER ERRORES)
            EnviarTramaDatos(interfaz, mac_origen, mac_destino, tipo, direccion, control, numeroTrama, longitudCadena, (unsigned char *)cadena, BCE);

            // Esperamos a recibir la trama de respuesta del esclavo:
            // ACK -> CONTROL == 6
            while (control != 6 || numeroTrama != numeroTramaRecibida)
            {
                RecibirTramaControl(interfaz, direccion, control, numeroTramaRecibida);

                // TODO VERSION 5 INTRODUCCION DE ERROES
                if (control == 21) // 21: Trama NACK - ERROR
                {                  // Si envian una trama NACK, corregimos los datos y los enviamos de nuevo TODO VERSION 5
                    control = 2;   // LA TRAMA CORREGIDA VUELVE A SER DE DATOS
                    cadena[0] = correccion;
                    MostrarTrama('R', direccion, control, numeroTrama, ' ');
                    cout << endl;
                    // ENVIAMOS LA RESPUESTA, AHORA CORREGIDA:
                    EnviarTramaDatos(interfaz, mac_origen, mac_destino, tipo, direccion, control, numeroTrama, longitudCadena, (unsigned char *)cadena, BCE);
                    MostrarTrama('E', direccion, control, numeroTrama, ' ');
                    cout << endl;
                }
            }

            // Mostramos la trama de respuesta del esclavo:
            MostrarTrama('R', direccion, control, numeroTrama, ' ');

            // INVERTIMOS VALORES DE numeroTrama PARA EL SIGUIENTE ENVIO
            if (numeroTrama == '1')
                numeroTrama = '0';

            else if (numeroTrama == '0')
                numeroTrama = '1';
        }
    }
    else
    {
        cout << "   ####  ERROR AL ABRIR EL FICHERO  ####   " << endl;
    }
    cout << endl;
    flujoLectura.close();
}

// Para calcular el valor del BCE de una trama se debe hacer la operación lógica XOR
// dos a dos entre todos los caracteres de los datos correspondiente al contenido del fichero.
char CalculoBCE(int longitudCadena, char cadena[])
{
    if (longitudCadena > 0) // Siempre debe ser mayor que 0
    {
        char BCE;
        BCE = cadena[0]; // inicializamos al primer valor (0)

        for (int i = 1; i < longitudCadena; i++)
        {
            BCE = BCE xor cadena[i];
        }
        return BCE;
    }
    else
        return 0; // Error
}

void RecibirTramaParoyEspera(interface_t *interfaz, unsigned char &direccion, unsigned char &control, unsigned char &numeroTrama, char cadena[], unsigned char &longitud)
{
    bool recibida = false;
    while (!recibida)
    {
        apacket_t trama = ReceiveFrame(interfaz);
        if (trama.packet != NULL)
        {
            direccion = trama.packet[14];
            control = trama.packet[15];
            numeroTrama = trama.packet[16];
            recibida = true;

            if (control == 2)
            {
                longitud = trama.packet[17];
                for (int i = 0; i < longitud; i++)
                {
                    cadena[i] = trama.packet[18 + i];
                }
                unsigned char BCERecibido = trama.packet[18 + longitud];
                unsigned char BCECalculado = CalculoBCE(longitud, cadena); // SERA EL BCE QUE CALCULE EL ESCLAVO Y RESPONDERA AFIRMATIVO SI ES IGUAL
                if (BCECalculado != BCERecibido)
                {
                    longitud = 0; // reseteamos la longitud de la cadena si no coinciden (ERROR)
                }
                MostrarTrama('R', direccion, control, numeroTrama, BCERecibido);
                printf(" %d\n", BCECalculado); // A CONTINUACION MOSTRAMOS EL BCE calculado por el esclavo para comparar
            }
        }
    }
}