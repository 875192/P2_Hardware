/*********************************************************************************************
* Fichero:	button.h
* Autor:
* Descrip:	Funciones de manejo de los pulsadores (EINT6-7)
* Version:
*********************************************************************************************/

#ifndef _BUTTON_H_
#define _BUTTON_H_

/*--- Declaración de funciones visibles del módulo button.c/button.h ---*/

void Eint4567_init(void);
void reiniciar_cuenta(void);
unsigned int obtener_cuenta(void);
void establecer_cuenta(unsigned int valor);

#endif /* _BUTTON_H_ */
