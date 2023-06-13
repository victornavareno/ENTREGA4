#include "sondeo.h"
#include "paroYEspera.h"

void mostrarMenuConfirmacion()
{
    cout << endl;
    cout << "¿Acepta el cierre de la comunicacion?" << endl;
    cout << "[1] Si" << endl;
    cout << "[2] No" << endl;
}

void MaestroSondeo(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2])
{
    unsigned char direccion;
    unsigned char control;
    unsigned char numeroTrama;

    // INCIALIZANDO VARIABLES:
    direccion = 'T';   // DIRECCION: 'T' PORQUE ES OPERACION DE SONDEO
    control = 5;       // CONTORL: 5 PORQUE ES TRAMA DE CONTROL ENQ
    numeroTrama = '0'; // OSCILA ENTRE 0 Y 1, PRIMERA SIEMPRE 0

    // UNA VEZ INICIADO EL SONDEO, ENVIAMOS LA TRAMA DE CONTROL ENQ - VALOR 5
    EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, control, numeroTrama); // ENVIAMOS Y MOSTRAMOS TRAMA DE CONTROL ENQ
    MostrarTrama('E', direccion, control, numeroTrama, ' ');

    // RECIBIMOS LA RESPUESTA HASTA VALOR 6 - TRAMA ACK Y PODEMOS EMPEZAR CON EL PROTOCOLO
    RecibirTramaControl(interfaz, direccion, control, numeroTrama); // RECIBIENDO TRAMAS...

    // SI EL VALOR DE CONTROL 6: TRAMA ACK, LA MOSTRAMOS
    if (control == 6)
    {
        MostrarTrama('R', direccion, control, numeroTrama, ' '); // como es de control, añadimos ' ' en lugar de BCE
        cout << endl;                                          
    }

    // ### ESCRITURA EN FICHERO ######
    ofstream flujoOutput;
    flujoOutput.open("RProtoc.txt"); // INTRODUCE EL NOMBRE DEL FICHERO AQUI
    char cadena[254];
    unsigned char longitudCadena; // LA RECIBIRE DEL ESCLAVO
    // ###############################

    bool fin = false;
    while (control != 4 && !fin)
    {
        RecibirTramaParoyEspera(interfaz, direccion, control, numeroTrama, cadena, longitudCadena);

        if(control != 4){ // AUN NO HEMOS RECIBIDO LA TRAMA 'EOT'(4)
            if (longitudCadena != 0 && flujoOutput.is_open())
            {
                flujoOutput.write(cadena, longitudCadena); // ESCRIBIMOS EN FICHERO
                control = 6;
            }
            else
            {
                control = 21; // PASARA A SER TRAMA NACK
            }
        }
        
        else if (control == 4) // SI CONTROL 4: HEMOS RECIBIDO 'EOT', AHORA EL MAESTRO DEBE CONFIRMAR SI SEGUIR CON LA COMUNICACION O NO
        {
            while (!fin)
            {
                cout << endl;
                MostrarTrama('R', direccion, control, numeroTrama, ' ');

                // MUESTRO EL MENU PARA SEGUIR ENVIANDO TRAMAS O NO
                mostrarMenuConfirmacion();
                unsigned char teclaPulsada; 
                teclaPulsada = getch(); // INPUT DEL USUARIO
                
                if (teclaPulsada == 2)  // PULSACION DE 2: CONTINUAMOS CON LA COMUNICACION ENVIANDO TRAMA NACK (21)
                {
                    EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, 21, numeroTrama);
                    MostrarTrama('E', direccion, control, numeroTrama, ' ');
                    cout << endl;

                    // SWAP numeroTrama
                    if (numeroTrama == '0')
                    {
                        numeroTrama = '1';
                    }
                    else
                        numeroTrama = '0';
                }

                // PULSACION DE 1 O TECLA ERRONEA - CERRAMOS LA COMUNICACION
                else
                {
                    cout << "Cerrando comunicacion..." << endl;
                    control = 6;
                    fin = true;
                    // MOSTRAREMOS LA TRAMA ENVIADA ANTES DE SALIR DEL METODO
                }
            }
        }

        // ENVIAMOS Y MOSTRAMOS LA TRAMA FINAL CONTROL DE RESPUESTA ACK
        EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, control, numeroTrama);
        MostrarTrama('E', direccion, control, numeroTrama, ' ');
    }

    flujoOutput.close();
    cout << endl
         << "Fin de sondeo por parte del Maestro" << endl;
}

void EsclavoSondeo(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2], unsigned char direccion, unsigned char control, unsigned char numeroTrama)
{
    // ENVIAMOS LA TRAMA DE CONTROL (ACK)
    EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, 6, numeroTrama);
    MostrarTrama('E', direccion, 6, numeroTrama, ' ');
    cout << endl;

    // COMENZAMOS EL PROTOCOLO DE ENVIO DE FICHERO POR PARO Y ESPERA
    EnviarFicheroParoyEspera(interfaz, mac_origen, mac_destino, tipo, direccion, control, numeroTrama);
    cout << endl;

    unsigned char numTramaEnviada = '0'; // LA PRIMERA TRAMA SERA 0 - SE UTILIZARA PARA ENVIAR TRAMAS
    while (control != 6)    // EJECUTAMOS MIENTRAS NO RECIBAMOS UN ACK - CONTROL 6 (CONFIRMACION)
    {
        // ENVIAMOS LA TRAMA EOT (4)
        EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, 4, numTramaEnviada);

        // RECIBIMOS LA TRAMA DE RESPUESTA - CONFIRMACION ACK (6)
        RecibirTramaControl(interfaz, direccion, control, numeroTrama); 

        if (control == 21 || control == 6) // SI RECIBIMOS -> 21: NACK || 6: ACK
        {
            MostrarTrama('R', direccion, control, numeroTrama, ' ');
            cout << endl;
        }

        // SWAP PARA RESPUESTA
        if (numTramaEnviada == '0')
        {
            numTramaEnviada = '1';
        }
        else 
        {
            numTramaEnviada = '0';
        }
    }

    cout << endl
         << "Fin de sondeo por parte del Esclavo" << endl;
}