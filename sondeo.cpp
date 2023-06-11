#include "sondeo.h"
#include "paroYEspera.h"

void mostrarMenuConfirmacion()
{
    cout << endl;
    cout << "Acepta el cierre de la comunicacion:" << endl;
    cout << "[1] Si" << endl;
    cout << "[2] No" << endl;
}
void MaestroSondeo(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2])
{
    unsigned char direccion;
    unsigned char control;
    unsigned char numeroTrama;

    direccion = 'T'; // DIRECCION: 'T' PORQUE ES OPERACION DE SONDEO

    control = 5; // CONTORL: 5 PORQUE ES TRAMA DE CONTROL ENQ

    numeroTrama = '0'; // OSCILA ENTRE 0 Y 1, PRIMERA SIEMPRE 0

    EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, control, numeroTrama); // ENVIAR TRAMA DE CONTROL ENQ

    RecibirTramaControl(interfaz, direccion, control, numeroTrama); // RECIBIENDO TRAMAS...

    // SI EL VALOR DE CONTROL 6: TRAMA ACK, LA MOSTRAMOS
    if (control == 6)
    {
        MostrarTrama('R', direccion, control, numeroTrama, ' '); // como es de control, añadimos ' '
        cout << endl;                                            // salto de linea
    }

    ofstream flujoOutput;
    flujoOutput.open("RProtoc.txt");
    char cadena[254];
    unsigned char longitud;
    bool fin = false;

    while (control != 4 && !fin)
    {
        RecibirTramaParoyEspera(interfaz, direccion, control, numeroTrama, cadena, longitud);

        if (control == 4)
        {
            while (!fin)
            {
                cout << endl;
                MostrarTrama('R', direccion, control, numeroTrama, ' ');
                mostrarMenuConfirmacion();

                unsigned char teclaPulsada;
                teclaPulsada = getch();

                switch (teclaPulsada)
                {
                case '1':
                    control = 6;
                    fin = true;
                    break;

                case '2':
                    EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, 21, numeroTrama);
                    cout << endl;

                    // Invertimos numeroTrama para pasar a la siguiente:
                    if (numeroTrama == '0')
                        numeroTrama = '1';
                    else
                        numeroTrama = '0';

                default:
                    cout << "Tecla inválida, cerrando la comunicación..." << endl;
                    control = 6;
                    fin = true;
                    break;
                }
            }
        }
        else
        {
            if (longitud != 0 && flujoOutput.is_open())
            {
                flujoOutput.write(cadena, longitud);
                control = 6;
            }
            else
            {
                control = 21;
            }
        }

        EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, control, numeroTrama);
    }

    flujoOutput.close();

    cout << endl <<  "Fin de sondeo por parte del Maestro" << endl;
}

void EsclavoSondeo(interface_t *interfaz, unsigned char mac_origen[6], unsigned char mac_destino[6], unsigned char tipo[2], unsigned char direccion, unsigned char control, unsigned char numeroTrama)
{
  unsigned char nTrama = '0';
    // ENVIAMOS LA TRAMA DE CONTROL (ACK)
    EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, 6, numeroTrama);
    cout << endl;
        printf("\n");

    // ENVIAMOS EL FICHERO
    EnviarFicheroParoyEspera(interfaz, mac_origen, mac_destino, tipo, direccion, control, numeroTrama);
    cout << endl;

    while(control != 6) {
        // ENVIAMOS LA TRAMA 'EOT'
        EnviarTramaControl(interfaz, mac_origen, mac_destino, tipo, direccion, 4, nTrama);

        // RECIBIMOS TRAMAS HASTA QUE NOS DEVUELVAN UNA CON EL VALOR DE CONTROL 6 Y LA IMPRIMIMOS
        RecibirTramaControl(interfaz, direccion, control, numeroTrama);

        if(control == 6 || control == 21) {
            MostrarTrama('R', direccion, control, numeroTrama, ' ');
            printf("\n");
        }

        if(nTrama == '0')
            nTrama = '1';
        else  nTrama = '0';
    }
    
    cout << endl <<  "Fin de sondeo por parte del Esclavo" << endl;
}
