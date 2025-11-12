/*********************************************************************************************
* Fichero:	button.c
* Autor:
* Descrip:	Funciones de manejo de los pulsadores (EINT6-7)
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "button.h"
#include "8led.h"
#include "cola.h"
#include "eventos.h"
#include "timer2.h"
#include "44blib.h"
#include "44b.h"
#include "def.h"

/*--- variables globales del m�dulo ---*/
/* int_count la utilizamos para sacar un n�mero por el 8led.
  Cuando se pulsa un bot�n sumamos y con el otro restamos. �A veces hay rebotes! */
static unsigned int int_count = 0;

/* declaraci�n de funci�n que es rutina de servicio de interrupci�n
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void Eint4567_ISR(void) __attribute__((interrupt("IRQ")));

/*--- c�digo de funciones ---*/
void Eint4567_ISR(void)
{
	/* Identificar la interrupción (hay dos pulsadores)*/
	int which_int = rEXTINTPND;
	
	/* Registrar evento en la cola de depuración (guardamos which_int directamente) */
	switch (which_int)
	{
		case 4:
			int_count++; // incrementamos el contador
			/* Registrar pulsación del botón izquierdo (EINT4 = 4) */
			cola_depuracion(timer2_count(), which_int, int_count);
			break;
		case 8:
			int_count--; // decrementamos el contador
			/* Registrar pulsación del botón derecho (EINT5 = 8) */
			cola_depuracion(timer2_count(), which_int, int_count);
			break;
		default:
			break;
	}
	D8Led_symbol(int_count & 0x000f); // sacamos el valor por pantalla (módulo 16)

	/* Finalizar ISR */
	rEXTINTPND = 0xf;				// borra los bits en EXTINTPND
	rI_ISPC   |= BIT_EINT4567;		// borra el bit pendiente en INTPND
}

void Eint4567_init(void)
{
	/* Configuracion del controlador de interrupciones. Estos registros est�n definidos en 44b.h */
	rI_ISPC    = 0x3ffffff;		// Borra INTPND escribiendo 1s en I_ISPC
	rEXTINTPND = 0xf;       	// Borra EXTINTPND escribiendo 1s en el propio registro
	rINTMOD    = 0x0;		// Configura las lineas como de tipo IRQ
	rINTCON    = 0x1;		// Habilita int. vectorizadas y la linea IRQ (FIQ no)
	rINTMSK    &= ~(BIT_EINT4567);  // habilitamos interrupcion linea eint4567 en vector de mascaras

	/* Establece la rutina de servicio para Eint4567 */
	pISR_EINT4567 = (int) Eint4567_ISR;

	/* Configuracion del puerto G */
	rPCONG  = 0xffff;        		// Establece la funcion de los pines (EINT0-7)
	rPUPG   = 0x0;                  // Habilita el "pull up" del puerto
	rEXTINT = rEXTINT | 0x22222222;   // Configura las lineas de int. como de flanco de bajada

	/* Por precaucion, se vuelven a borrar los bits de INTPND y EXTINTPND */
	rEXTINTPND = 0xf;				// borra los bits en EXTINTPND
	rI_ISPC   |= BIT_EINT4567;		// borra el bit pendiente en INTPND
}
