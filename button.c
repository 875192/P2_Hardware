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
#include "44blib.h"
#include "44b.h"
#include "def.h"

/*--- variables globales del módulo ---*/
/* int_count la utilizamos para sacar un número por el 8led.
   Cuando se pulsa un botón sumamos y con el otro restamos. */
static int int_count = 0;

/* Máquina de estados para eliminar rebotes */
static volatile EstadoPulsador estado_pulsador = ESPERANDO_PULSACION;
static volatile unsigned int boton_activo = 0;              // Botón en proceso (4 ó 8)
static volatile unsigned int instante_objetivo = 0; // Momento en el que vence el temporizador

/* Constantes de temporización en microsegundos */
#define TRP_US 20000u                     // Retardo inicial tras detectar la pulsación (20 ms)
#define TRD_US 20000u                     // Retardo tras detectar la depresión (20 ms)
#define MONITORIZACION_US 100000u // Periodo entre comprobaciones mientras el botón está pulsado (100 ms)

/* Máscaras de los pines de los botones en el puerto G */
#define MASCARA_BOTON_IZQ (1u << 4)
#define MASCARA_BOTON_DER (1u << 5)

/* Identificador interno para indicar que no hay botón en curso */
#define BOTON_NINGUNO 0

/*--- funciones auxiliares ---*/
static inline unsigned int leer_tiempo(void)
{
        return timer2_count();
}

static inline int tiempo_cumplido(unsigned int ahora, unsigned int objetivo)
{
        return ((int)(ahora - objetivo) >= 0);
}

static inline unsigned int mascara_boton(unsigned int boton)
{
        return (boton == 4) ? MASCARA_BOTON_IZQ : MASCARA_BOTON_DER;
}

static inline int boton_presionado(unsigned int boton)
{
        unsigned int mascara = mascara_boton(boton);
        return (rPDATG & mascara) == 0; // Pull-up activado: 0 cuando se pulsa
}

static void procesar_pulso_valido(unsigned int boton)
{
        if (boton == 4)
        {
                int_count++;
        }
        else if (boton == 8)
        {
                int_count--;
        }

        D8Led_symbol(int_count & 0x000f);
        cola_depuracion(leer_tiempo(), EVENTO_BOTON_VALIDO, boton);
}

static void reiniciar_maquina(void)
{
        boton_activo = BOTON_NINGUNO;
        estado_pulsador = ESPERANDO_PULSACION;
        instante_objetivo = 0;
        rINTMSK &= ~(BIT_EINT4567); // Rehabilitar interrupciones de los botones
}

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void Eint4567_ISR(void) __attribute__((interrupt("IRQ")));

/*--- código de funciones ---*/
void Eint4567_ISR(void)
{
        unsigned int which_int;

        /* Evitar atender nuevas pulsaciones mientras se procesa la actual */
        if (estado_pulsador != ESPERANDO_PULSACION)
        {
                /* Finalizar ISR */
                rEXTINTPND = 0xf;
                rI_ISPC |= BIT_EINT4567;
                return;
        }

        /* Identificar la interrupción (hay dos pulsadores) */
        which_int = rEXTINTPND & 0x0f;

        if ((which_int != 4) && (which_int != 8))
        {
                /* Finalizar ISR */
                rEXTINTPND = 0xf;
                rI_ISPC |= BIT_EINT4567;
                return;
        }

        cola_depuracion(leer_tiempo(), EVENTO_BOTON_IRQ, which_int);

        boton_activo = which_int;
        estado_pulsador = REBOTE_PRESION;
        instante_objetivo = leer_tiempo() + TRP_US;

        /* Deshabilitar temporalmente las interrupciones de los botones */
        rINTMSK |= BIT_EINT4567;

        /* Finalizar ISR */
        rEXTINTPND = 0xf;                               // borra los bits en EXTINTPND
        rI_ISPC   |= BIT_EINT4567;              // borra el bit pendiente en INTPND
}

void Eint4567_init(void)
{
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

void button_task(void)
{
        unsigned int ahora = leer_tiempo();

        switch (estado_pulsador)
        {
                case ESPERANDO_PULSACION:
                        /* No hay nada que hacer hasta la siguiente IRQ */
                        break;

                case REBOTE_PRESION:
                        if (tiempo_cumplido(ahora, instante_objetivo))
                        {
                                if ((boton_activo != BOTON_NINGUNO) && boton_presionado(boton_activo))
                                {
                                        cola_depuracion(ahora, EVENTO_BOTON_CONFIRMADO, boton_activo);
                                        estado_pulsador = MONITORIZANDO;
                                        instante_objetivo = ahora + MONITORIZACION_US;
                                }
                                else
                                {
                                        cola_depuracion(ahora, EVENTO_BOTON_DESCARTADO, boton_activo);
                                        reiniciar_maquina();
                                }
                        }
                        break;

                case MONITORIZANDO:
                        if (tiempo_cumplido(ahora, instante_objetivo))
                        {
                                if ((boton_activo != BOTON_NINGUNO) && !boton_presionado(boton_activo))
                                {
                                        cola_depuracion(ahora, EVENTO_BOTON_SOLTADO, boton_activo);
                                        estado_pulsador = REBOTE_DEPRESION;
                                        instante_objetivo = ahora + TRD_US;
                                }
                                else
                                {
                                        instante_objetivo = ahora + MONITORIZACION_US;
                                }
                        }
                        break;

                case REBOTE_DEPRESION:
                        if (tiempo_cumplido(ahora, instante_objetivo))
                        {
                                if ((boton_activo != BOTON_NINGUNO) && !boton_presionado(boton_activo))
                                {
                                        procesar_pulso_valido(boton_activo);
                                        reiniciar_maquina();
                                }
                                else
                                {
                                        /* Sigue pulsado: volvemos a monitorizar */
                                        estado_pulsador = MONITORIZANDO;
                                        instante_objetivo = ahora + MONITORIZACION_US;
                                }
                        }
                        break;

                default:
                        reiniciar_maquina();
                        break;
        }
}
