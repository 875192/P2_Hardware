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
#include "sudoku_2025.h"
#include "celda.h"

/*--- variables globales del módulo ---*/
/* int_count la utilizamos para sacar un número por el 8led.
  Cuando se pulsa un botón sumamos y con el otro restamos. ¡A veces hay rebotes! */
/* La variable int_count ahora es global y está definida en timer3.c */
extern volatile int int_count;
extern CELDA (*cuadricula)[NUM_COLUMNAS];
extern volatile EstadoSudoku estado_sudoku;
extern volatile int fila;
extern volatile int columna;
extern volatile int valor;

static const int DIGITOS[] = {cero, uno, dos, tres, cuatro, cinco, seis, siete, ocho, nueve};

static void mostrar_digito(uint8_t numero)
{
        if (numero < 10U)
        {
                D8Led_symbol(DIGITOS[numero]);
        }
}

static void reiniciar_seleccion_fila(void)
{
        fila = 0;
        columna = 0;
        valor = 0;
        estado_sudoku = INTRODUCIR_FILA;
        D8Led_symbol(F);
}

static void procesar_valor_celda(void)
{
        uint8_t fila_idx = (uint8_t)((fila > 0) ? (fila - 1) : 0);
        uint8_t col_idx = (uint8_t)((columna > 0) ? (columna - 1) : 0);
        CELDA *celda_obj = &cuadricula[fila_idx][col_idx];
        uint8_t valor_previo = celda_leer_valor(*celda_obj);

        celda_limpiar_error(celda_obj);

        if (valor == 0)
        {
                celda_poner_valor(celda_obj, 0);
                candidatos_actualizar_c(cuadricula);
                reiniciar_seleccion_fila();
                return;
        }

        if (!celda_es_candidato(*celda_obj, (uint8_t)valor))
        {
                celda_marcar_error(celda_obj);
                D8Led_symbol(E);
                estado_sudoku = INTRODUCIR_FILA;
                fila = 0;
                return;
        }

        celda_poner_valor(celda_obj, (uint8_t)valor);

        if (valor_previo == 0)
        {
                candidatos_propagar_c(cuadricula, fila_idx, col_idx);
        }
        else if (valor_previo != valor)
        {
                candidatos_actualizar_c(cuadricula);
        }

        reiniciar_seleccion_fila();
}

/* Callback para recibir la confirmación de pulsaciones filtradas por el timer3 */
static void boton_confirmado(uint8_t boton_id)
{
        cola_depuracion(timer2_count(), boton_id, estado_sudoku);

        switch (estado_sudoku)
        {
                case ESPERANDO_INICIO:
                        candidatos_actualizar_c(cuadricula);
                        reiniciar_seleccion_fila();
                        break;

                case INTRODUCIR_FILA:
                        if (boton_id == EVENTO_BOTON_DERECHO)
                        {
                                fila = (fila % 9) + 1;
                                mostrar_digito((uint8_t)fila);
                        }
                        else if (boton_id == EVENTO_BOTON_IZQUIERDO)
                        {
                                if (fila == 0)
                                {
                                        fila = 1;
                                }
                                estado_sudoku = INTRODUCIR_COLUMNA;
                                columna = 0;
                                D8Led_symbol(C);
                        }
                        break;

                case INTRODUCIR_COLUMNA:
                        if (boton_id == EVENTO_BOTON_DERECHO)
                        {
                                columna = (columna % 9) + 1;
                                mostrar_digito((uint8_t)columna);
                        }
                        else if (boton_id == EVENTO_BOTON_IZQUIERDO)
                        {
                                if (columna == 0)
                                {
                                        columna = 1;
                                }

                                if (celda_es_pista(cuadricula[fila - 1][columna - 1]))
                                {
                                        reiniciar_seleccion_fila();
                                }
                                else
                                {
                                        estado_sudoku = INTRODUCIR_VALOR;
                                        valor = 0;
                                        mostrar_digito((uint8_t)valor);
                                }
                        }
                        break;

                case INTRODUCIR_VALOR:
                        if (boton_id == EVENTO_BOTON_DERECHO)
                        {
                                valor = (valor + 1) % 10;
                                mostrar_digito((uint8_t)valor);
                        }
                        else if (boton_id == EVENTO_BOTON_IZQUIERDO)
                        {
                                estado_sudoku = (valor == 0) ? BORRAR_VALOR : VERIFICAR_VALOR;
                                procesar_valor_celda();
                        }
                        break;

                case VERIFICAR_VALOR:
                case BORRAR_VALOR:
                        procesar_valor_celda();
                        break;

                default:
                        break;
        }
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
                /* No registrar la pulsación inicial, solo las confirmadas */

                /* Iniciar la máquina de antirrebotes. Si está ocupada, no hacer nada */
                timer3_start_antirrebote(boton_id);
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

void reiniciar_cuenta(void)
{
        int_count = 0;
}

unsigned int obtener_cuenta(void)
{
        return (unsigned int)int_count;
}

void establecer_cuenta(unsigned int valor)
{
        int_count = (int)valor;
}
