/*********************************************************************************************
* Fichero:		eventos.h
* Autor:		
* Descrip:		Definición de eventos para la cola de depuración
* Version:		1.0
*********************************************************************************************/

#ifndef _EVENTOS_H_
#define _EVENTOS_H_

/*--- Definición de identificadores de eventos ---*/
typedef enum {
        EVENTO_BOTON_IZQUIERDO = 4,
        EVENTO_BOTON_DERECHO   = 8,

        EVENTO_TIMER3_INICIO       = 20,
        EVENTO_TIMER3_MONITORIZA   = 21,
        EVENTO_TIMER3_DEPRESION    = 22,
        EVENTO_TIMER3_FIN          = 23,

        EVENTO_BOTON_CONFIRMADO_IZQ = 30,
        EVENTO_BOTON_CONFIRMADO_DER = 31,
        EVENTO_PULSADOR_DESCARTADO  = 40
} ID_Evento;


/*--- Maquina de estados ---*/
typedef enum {
    ESPERANDO_PULSACION,
    REBOTE_PRESION,
    MONITORIZANDO,
    REBOTE_DEPRESION
} EstadoPulsador;

#endif /* _EVENTOS_H_ */
