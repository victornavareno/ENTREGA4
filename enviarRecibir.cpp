#include "enviarRecibir.h"

void RecibirEsclavo(interface_t *interfaz, int numGrupo)
{
    __fpurge(stdin);       // limpiamos el buffer de teclado
    bool cabecera = false; // nos servira para imprimir por pantalla que un fichero ha sido enviado

    while (!kbhit())
    {
        apacket_t tramaRecibida = ReceiveFrame(interfaz);

        if (tramaRecibida.packet != NULL && (tramaRecibida.packet[12] == (48 + numGrupo) && tramaRecibida.packet[13] == 0))
        {
            // cout << "LA LONGITUD DE LA TRAMA ES " << tramaRecibida.header.len << " BYTES" << endl;
            if (tramaRecibida.header.len == 15)
                cout << "Recibido: ";
            else if (cabecera == false)
            {
                cout << "     Fichero Recibido:     " << endl;
                cabecera = true;
            }

            char vectorCaracteres[tramaRecibida.header.len - 14];
            int j = 0;
            for (int i = 14; i < tramaRecibida.header.len; i++)
            {
                vectorCaracteres[j] = tramaRecibida.packet[i];
                cout << vectorCaracteres[j];
                j++;
            }
            cout << endl;
        }
    }
}

void SeleccionModoEnvio(int modo, int numGrupo, interface_t *interfaz, unsigned char mac_src[6], unsigned char mac_destino[6], unsigned char tipo[2])
{
    int modoEnvio;
    char teclaPulsada;

    if (modo == 1)
    { // MENU DE ELECCION MAESTRO
        printf("Selección de modo:\n");
        printf("    [F1]  - Envío de caracteres interactivo\n");
        printf("    [F2]  - Envío de un fichero\n");
        printf("    [F3]  - Protocolo paro y espera\n");
        // printf("    [F4] - Introduccion de errores durante el protocolo\n");
        printf("    [ESC] - Salir\n");
        cout << endl;
    }

    else if (modo == 2)
    { // MENU DE ELECCION ESCLAVO
        printf("Selección de modo:\n");
        printf("    [F1]  - Envío de caracteres interactivo\n");
        printf("    [F3]  - Protocolo paro y espera\n");
        // printf("    [F4] - Introduccion de errores durante el protocolo\n");
        printf("    [ESC] - Salir\n");
        cout << endl;
    }

    teclaPulsada = getch();
    while (teclaPulsada != 27)
    {
        if (modo == 1) // MODO MAESTRO
        {
            if (!kbhit())
            {
                teclaPulsada = getch();
                if (teclaPulsada != 27)
                {
                    cout << "Tecla inválida" << endl;
                }
                else
                {
                    if (kbhit())
                    {
                        teclaPulsada = getch();
                        if (teclaPulsada == 'O')
                        {
                            teclaPulsada = getch();
                            if (teclaPulsada == 'P') // F1 ENVIO DE CARACTERES INTERACTIVO
                            {
                                cout << "MODO ELEGIDO = F1 'Envío de caracteres interactivo' " << endl; // Envio de caracteres interactivo
                                EnviarRecibirCaracter(interfaz, mac_src, mac_destino, tipo);
                            }

                            else if (teclaPulsada == 'Q') // F2 ENVIO DE FICHERO
                            {
                                cout << "MODO ELEGIDO = F2 'Enviando un fichero...' " << endl; // Envio de un fichero
                                EnviarFichero(interfaz, mac_src, mac_destino, tipo);
                            }

                            else if (teclaPulsada == 'R')
                            {
                                printf("MODO ELEGIDO = F3 'Protocolo de paro y espera'\n"); // Protocolo de paro y espera para el maestro                                                                   // F3 PROTOCOLO DE PARO Y ESPERA
                                while(!MaestroParoYEspera(interfaz, mac_src, mac_destino, tipo));
                            }

                            else
                            {
                                cout << "Saliendo..." << endl;
                            }
                            __fpurge(stdin);
                        }
                    }
                }
            }
        }

        if (modo == 2) // MODO ESCLAVO
        {
            if (!kbhit())
            {
                RecibirEsclavo(interfaz, numGrupo);
                teclaPulsada = getch();
                if (teclaPulsada != 27)
                {
                    cout << "Tecla inválida" << endl;
                }
                else
                {
                    if (kbhit())
                    {
                        teclaPulsada = getch();
                        if (teclaPulsada == 'O')
                        {
                            teclaPulsada = getch();
                            if (teclaPulsada == 'P')
                            {
                                cout << "MODO ELEGIDO = F1 'Envío de caracteres interactivo' " << endl; // Envio de caracteres interactivo
                                EnviarRecibirCaracter(interfaz, mac_src, mac_destino, tipo);
                            }

                            else if (teclaPulsada == 'R')
                            {                                                                      // F3 PROTOCOLO DE PARO Y ESPERA
                                cout << "MODO ELEGIDO = F3 'Protocolo de paro y espera' " << endl; // Protocolo de paro y espera
                                EsclavoParoYEspera(interfaz, mac_src, mac_destino, tipo);
                            }
                        }
                    }
                }
            }
        }
    }
    cout << "Saliendo..." << endl;
    __fpurge(stdin);
}

void ImprimirMacDestino(unsigned char mac_destino[6])
{
    // asumimos que el una mac tiene 6 dígitos hexadecimales
    cout << "La MAC destino es: ";
    for (size_t i = 0; i < 6; i++)
    {
        // condicion para imprimir dos puntos entre cada par de digitos hexadecimales de la MAC
        if (!(i == 0 || i == 6))
        {
            cout << ":";
        }
        printf("%02X", mac_destino[i]); // el -%02 indica que queremos mínimo 2 cifras, la X indica que el formato de salida será hexadecimal con letras mayúsculas
    }
    cout << endl;
}

int SeleccionGrupo(unsigned char tipo[2])
{
    int numeroGrupo;
    cout << endl;
    printf("Introduzca el número de grupo:\n");
    cin >> numeroGrupo;
    cout << endl;

    tipo[0] = tipo[0] + numeroGrupo;
    return numeroGrupo;
}

int SeleccionModoEstacion()
{
    int modoEstacion;
    printf("Selección el modo de la estación:\n");
    printf("    [1] Modo Maestra\n");
    printf("    [2] Modo Esclava\n");
    cin >> modoEstacion;
    return modoEstacion;
}

void EnviarTramaMaestro(int numGrupo, interface_t *interfaz, unsigned char mac_destino[6]) // lo pongo entre paréntesis porque lo pasamos por parámetro
{
    unsigned char mac_broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // MAC DE BROADCAST
    // 33 = 48 + 3 (numero de grupo)
    // 01 = modo de funcionamiento Peticición de descubrimiento
    unsigned char type[2] = {static_cast<unsigned char>(0x30 + numGrupo), 0x01};
    GetMACAdapter(interfaz);

    apacket_t tramaRecibida;
    unsigned char *trama = BuildHeader(interfaz->MACaddr, mac_broadcast, type);

    if (trama == nullptr)
    {
        cout << "ERROR: La trama es nula" << endl;
    }

    if (interfaz == nullptr)
    {
        cout << "ERROR: La interfaz es nula" << endl;
    }

    SendFrame(interfaz, trama, 0);
    delete trama;

    bool TramaRecibidaCorrecta = false;
    while (!TramaRecibidaCorrecta)
    {
        tramaRecibida = ReceiveFrame(interfaz);
        if (tramaRecibida.packet != nullptr && tramaRecibida.packet[12] == type[0] && tramaRecibida.packet[13] == 0x02)
        {
            // coger la mac origen, que será la mac del esclavo
            int j = 0;
            for (int i = 0; i < 6; i++)
            {
                mac_destino[i] = tramaRecibida.packet[i + 6]; // cogemos la mac origen de la trama recibida
                j++;
            }
            TramaRecibidaCorrecta = true; // rompo bucle
        }
    }
}

void RecibirTramaEsclavo(int numGrupo, interface_t *interfaz, unsigned char mac_destino[6])
{
    unsigned char type[2] = {static_cast<unsigned char>(0x30 + numGrupo), 0x01};          // tipo que debe recibir
    unsigned char typeRespuesta[2] = {static_cast<unsigned char>(0x30 + numGrupo), 0x02}; // tipo que debe enviar
    bool TramaRecibidaEsBroadCast = false;
    GetMACAdapter(interfaz);

    while (!TramaRecibidaEsBroadCast)
    {
        apacket_t tramaRecibida = ReceiveFrame(interfaz);
        if (tramaRecibida.packet != nullptr && tramaRecibida.packet[12] == type[0] && tramaRecibida.packet[13] == type[1])
        {
            // coger la mac origen, que será la mac del maestro
            int j = 0;
            for (int i = 0; i < 6; i++)
            {
                mac_destino[j] = tramaRecibida.packet[i + 6]; // cogemos la mac origen de la trama recibida
                j++;
            }
            TramaRecibidaEsBroadCast = true; // rompo el bucle
        }
    }
    // crear y enviar trama de respuesta para el maestro
    unsigned char *tramaRespuesta;
    tramaRespuesta = BuildHeader(interfaz->MACaddr, mac_destino, typeRespuesta);
    SendFrame(interfaz, tramaRespuesta, 0);
    delete tramaRespuesta;
}

void EnviarCaracter(interface_t *iface, char _dato, unsigned char mac_src[6], unsigned char mac_dst[6], unsigned char type[2])
{
    // ReservarMemoria: Crea el puntero char *datoPtr, reservando direccion de memoria propia para almacenar el valor de la tecla pulsada por el usuario
    unsigned char *datoPtr = new unsigned char[1];
    // AlmacenarDatos, hace que el puntero apunte al dato que introduce el usuario
    *datoPtr = _dato;

    // construimos la trama llamando al método BuildFrame()
    unsigned char *trama = BuildFrame(mac_src, mac_dst, type, datoPtr);

    // Enviamos la trama
    SendFrame(iface, trama, 1);

    // Liberamos la memoria del puntero a dato y la trama (como la trama utiliza malloc, la liberamos con free)
    delete datoPtr;
    free(trama);
}

void EnviarFichero(interface_t *interfaz, unsigned char mac_src[6], unsigned char mac_destino[6], unsigned char tipo[2])
{
    GetMACAdapter(interfaz);
    ifstream flujoLectura("envio.txt"); // introduce aquí el archivo  y la ruta del fichero que deseas enviar
    unsigned char *tramaEnviada;
    char cadena[255];

    if (flujoLectura.is_open())
    {
        while (!flujoLectura.eof())
        {
            flujoLectura.read(cadena, 254);
            cadena[flujoLectura.gcount()] = '\0'; // añadimos este caracter especial en la posicion para indicar fin de trama
            tramaEnviada = BuildFrame(interfaz->MACaddr, mac_destino, tipo, (unsigned char *)cadena);
            SendFrame(interfaz, tramaEnviada, flujoLectura.gcount());
        }
        flujoLectura.close();
    }

    else
        cout << "   ####  ERROR AL ABRIR EL FICHERO  ####   " << endl;
}

char RecibirCaracter(interface_t *iface)
{
    // recibimos la trama que envía el usuario por la interfaz
    apacket_t trama = ReceiveFrame(iface);

    char caracter = 0; // inicializamos a 0, por si el usuario no envía nada

    // COMPROBAMOS QUE LA TRAMA RECIBIDA NO ESTÁ VACÍA
    if (trama.packet != NULL)
    {
        // filtro la trama recibida para asegurarme que es de mi compañero
        //  la trama debe tener tipo[1] == 0x00, modo normal
        //  ademas, nos quedamos con todos los datos de la trama, no solo el caracter
        unsigned char *caracterRecibido = (unsigned char *)malloc(1 * sizeof(const unsigned char));
        *caracterRecibido = trama.packet[14]; // los datos enviados se encuentran en la posicion 14 de la trama recibida
        caracter = *caracterRecibido;
        free(caracterRecibido);
    }
    return caracter;
}

void EnviarRecibirCaracter(interface_t *interface, unsigned char mac_src[6], unsigned char mac_destino[6], unsigned char tipo[2])
{
    char caracterRecibido;
    char teclaPulsada;
    cout << "  Pulse los caracteres a enviar:" << endl;

    // 27 = tecla ESCAPE
    while (teclaPulsada != 27)
    {
        caracterRecibido = RecibirCaracter(interface);

        if (caracterRecibido != 0)
        {
            printf("Recibido:%c\n", caracterRecibido);
        }
        if (kbhit()) // si hay algo en el buffer
        {
            teclaPulsada = getch(); // obtengo el caracter del usuario por teclado
            if (teclaPulsada != 27) // si el usuario no pulsa escape
            {
                // cout << "Enviando..." << endl;
                EnviarCaracter(interface, teclaPulsada, mac_src, mac_destino, tipo);
            }
            else if (teclaPulsada == 27)
            {
                cout << endl;
                cout << "Saliendo del modo F1 'Envío de caracteres interactivo'..." << endl;
                cout << endl;
            }
        }
    }
}
