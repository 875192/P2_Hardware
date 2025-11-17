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
#include "timer1.h"
#include "timer2.h"
#include "timer3.h"
#include "cola.h"
#include "44blib.h"
#include "44b.h"
#include "sudoku_2025.h"

/*--- variables globales ---*/
#include "tableros.h"

/* Apuntar a una de las cuadrículas disponibles */
CELDA (*cuadricula)[NUM_COLUMNAS] = cuadricula_C_C;
int celdas_vacias = 0;


/*--- codigo de funciones ---*/
void Main(void)
{
	
	/* Variables para observar la cola durante la depuración */
	ColaDebug* p_cola;              // Puntero a la estructura completa de la cola
	
	/* Inicializa controladores */
	sys_init();         // Inicializacion de la placa, interrupciones y puertos
	timer2_init();      // Inicializacion del timer2 para medicion de tiempo
	timer1_init();      // Inicializacion del timer1 para latido (heartbeat) - Paso 6
	cola_init();        // Inicializacion de la cola de depuracion (Paso 4)
	Eint4567_init();	// inicializamos los pulsadores. Cada vez que se pulse se ver� reflejado en el 8led
	D8Led_init();       // inicializamos el 8led

	/* Valor inicial de los leds */
	leds_off();
	
	/* Apuntar a la cola para poder observarla en el depurador */
	p_cola = cola_global;
	
	/* Bucle principal */
	while (1)
	{
		/* El latido (LED2 parpadeando) se gestiona automáticamente por timer1 */
		/* El programa está vivo mientras el LED2 parpadee a 6 Hz */
	}
}