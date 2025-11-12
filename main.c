/*********************************************************************************************
* Fichero:	main.c
* Autor:    
* Descrip:	punto de entrada de C
* Version:      <P4-ARM.timer-leds>
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "8led.h"
#include "button.h"
#include "led.h"
#include "timer.h"
#include "timer2.h"
#include "cola.h"
#include "44blib.h"
#include "44b.h"

/*--- variables globales ---*/


/*--- codigo de funciones ---*/
void Main(void)
{
	
	/* Variables para observar la cola durante la depuración */
	ColaDebug* p_cola;              // Puntero a la estructura completa de la cola
	
	/* Inicializa controladores */
	sys_init();         // Inicializacion de la placa, interrupciones y puertos
	timer2_init();      // Inicializacion del timer2 para medicion de tiempo
	cola_init();        // Inicializacion de la cola de depuracion (Paso 4)
	timer_init();	    // Inicializacion del temporizador
	Eint4567_init();	// inicializamos los pulsadores. Cada vez que se pulse se ver� reflejado en el 8led
	D8Led_init();       // inicializamos el 8led

	/* Valor inicial de los leds */
	leds_off();
	led1_on();
	
	/* Apuntar a la cola para poder observarla en el depurador */
	p_cola = cola_global;
	
	/* Bucle principal */
        while (1)
        {
                /* Actualiza la máquina de estados de los pulsadores */
                button_task();

                /* Cambia los leds con cada interrupcion del temporizador */
                if (switch_leds == 1)
                {
                        leds_switch();
                        switch_leds = 0;
                }
        }
}
