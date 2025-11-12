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
        EVENTO_BOTON_IRQ = 0x10,                // Se recibe una IRQ de los pulsadores
        EVENTO_BOTON_CONFIRMADO = 0x11, // La pulsación se mantiene tras el filtro inicial
        EVENTO_BOTON_DESCARTADO = 0x12, // La pulsación se descarta por considerarse rebote
        EVENTO_BOTON_SOLTADO = 0x13,    // Se detecta que el usuario ha soltado el botón
        EVENTO_BOTON_VALIDO = 0x14      // Pulsación válida tras completar el filtrado
} EventoDepuracion;

/*--- Máquina de estados ---*/
typedef enum {
        ESPERANDO_PULSACION,
        REBOTE_PRESION,
        MONITORIZANDO,
        REBOTE_DEPRESION
} EstadoPulsador;

#endif /* _EVENTOS_H_ */
