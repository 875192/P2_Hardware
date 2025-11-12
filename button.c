/*********************************************************************************************
* Fichero:      button.c
* Autor:
* Descrip:      Funciones de manejo de los pulsadores (EINT6-7)
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "button.h"
#include "8led.h"
#include "cola.h"
#include "eventos.h"
#include "timer2.h"
#include "timer3.h"
#include "44blib.h"
#include "44b.h"
#include "def.h"

/*--- variables globales del módulo ---*/
/* int_count la utilizamos para sacar un número por el 8led.
  Cuando se pulsa un botón sumamos y con el otro restamos. ¡A veces hay rebotes! */
static unsigned int int_count = 0;

/* Callback para recibir la confirmación de pulsaciones filtradas por el timer3 */
static void boton_confirmado(uint8_t boton_id)
{
        switch (boton_id)
        {
                case EVENTO_BOTON_IZQUIERDO:
                        int_count++;
                        cola_depuracion(timer2_count(), EVENTO_BOTON_CONFIRMADO_IZQ, int_count);
                        break;
                case EVENTO_BOTON_DERECHO:
                        int_count--;
                        cola_depuracion(timer2_count(), EVENTO_BOTON_CONFIRMADO_DER, int_count);
                        break;
                default:
                        cola_depuracion(timer2_count(), EVENTO_PULSADOR_DESCARTADO, boton_id);
                        break;
        }

        D8Led_symbol(int_count & 0x000f);
}

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void Eint4567_ISR(void) __attribute__((interrupt("IRQ")));

/*--- código de funciones ---*/
void Eint4567_ISR(void)
{
        /* Identificar la interrupción (hay dos pulsadores) */
        unsigned int pending = rEXTINTPND & 0xF;
        uint8_t boton_id = 0;

        if (pending & 0x4)
        {
                boton_id = EVENTO_BOTON_IZQUIERDO;
        }
        else if (pending & 0x8)
        {
                boton_id = EVENTO_BOTON_DERECHO;
        }

        if (boton_id != 0U)
        {
                /* Registrar la pulsación detectada */
                cola_depuracion(timer2_count(), boton_id, int_count);

                /* Iniciar la máquina de antirrebotes. Si está ocupada, registrar el evento */
                if (!timer3_start_antirrebote(boton_id))
                {
                        cola_depuracion(timer2_count(), EVENTO_PULSADOR_DESCARTADO, boton_id);
                }
        }

        /* Finalizar ISR */
        rEXTINTPND = 0xf;                               // borra los bits en EXTINTPND
        rI_ISPC   |= BIT_EINT4567;              // borra el bit pendiente en INTPND
}

void Eint4567_init(void)
{
        /* Inicializar el temporizador dedicado a la eliminación de rebotes */
        timer3_init(boton_confirmado);

        /* Configuracion del controlador de interrupciones. Estos registros están definidos en 44b.h */
        rI_ISPC    = 0x3ffffff;         // Borra INTPND escribiendo 1s en I_ISPC
        rEXTINTPND = 0xf;               // Borra EXTINTPND escribiendo 1s en el propio registro
        rINTMOD    = 0x0;               // Configura las lineas como de tipo IRQ
        rINTCON    = 0x1;               // Habilita int. vectorizadas y la linea IRQ (FIQ no)
        rINTMSK    &= ~(BIT_EINT4567);  // habilitamos interrupcion linea eint4567 en vector de mascaras

        /* Establece la rutina de servicio para Eint4567 */
        pISR_EINT4567 = (int) Eint4567_ISR;

        /* Configuracion del puerto G */
        rPCONG  = 0xffff;                       // Establece la funcion de los pines (EINT0-7)
        rPUPG   = 0x0;                  // Habilita el "pull up" del puerto
        rEXTINT = rEXTINT | 0x22222222;   // Configura las lineas de int. como de flanco de bajada

        /* Por precaucion, se vuelven a borrar los bits de INTPND y EXTINTPND */
        rEXTINTPND = 0xf;                               // borra los bits en EXTINTPND
        rI_ISPC   |= BIT_EINT4567;              // borra el bit pendiente en INTPND
}
