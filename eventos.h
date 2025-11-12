/*********************************************************************************************
* Fichero:		eventos.h
* Autor:		
* Descrip:		Definición de eventos para la cola de depuración
* Version:		1.0
*********************************************************************************************/

#ifndef _EVENTOS_H_
#define _EVENTOS_H_

/*--- Definición de identificadores de eventos ---*/
/* 
 * Los eventos de los botones se identifican directamente por el valor de rEXTINTPND:
 * - Botón izquierdo (EINT4): 4
 * - Botón derecho (EINT5): 8
 */


/*--- Maquina de estados ---*/
typedef enum {
    ESPERANDO_PULSACION,
    REBOTE_PRESION,
    MONITORIZANDO,
    REBOTE_DEPRESION
} EstadoPulsador;

#endif /* _EVENTOS_H_ */
