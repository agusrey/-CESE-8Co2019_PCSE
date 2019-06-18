/*=====[Botones.c]===================================================
 * Copyright 2019 Agustín Rey <agustinrey61@gmail.com>
 * All rights reserved.
 * Licencia: Texto de la licencia o al menos el nombre y un link
 (ejemplo: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 0.0.0
 * Fecha de creacion: 2019/06/12
 */

/*=====[Inclusion de su propia cabecera]=====================================*/

#include "Botones.h"

/*=====[Inclusiones de dependencias de funciones privadas]===================*/
// sAPI header
#include "sapi.h"
#include "board.h"
#include "sapi_peripheral_map.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "semphr.h"

/*=====[Macros de definicion de constantes privadas]=========================*/

#define CANT_TECLAS 4

/*=====[Macros estilo funcion privadas]======================================*/

/*=====[Definiciones de tipos de datos privados]=============================*/
typedef struct {
	uint8_t numbot;
	bool press;
} BtnSt_t;

typedef struct {
	gpioMap_t boton;
	BotStatus_t status;
	delay_t demSt;
} arrebSt_t;

// Tipo de datos que renombra un tipo basico
//typedef uint32_t gpioRegister_t;

// Tipo de datos de puntero a funcion
//typedef void (*FuncPtrPrivado_t)(void *);

// Tipo de datos enumerado

// Tipo de datos estructua, union o campo de bits

#define TIEMPO_ANTIRREBOTE 40
/*=====[Definiciones de Variables globales publicas externas]================*/

extern SemaphoreHandle_t semaforo;
/*=====[Definiciones de Variables globales publicas]=========================*/

//int32_t varGlobalPublica = 0;
/*=====[Definiciones de Variables globales privadas]=========================*/

const gpioMap_t teclas[4] = { TEC1, TEC2, TEC3, TEC4 };
arrebSt_t BotSt[CANT_TECLAS];


/*=====[Prototipos de funciones privadas]====================================*/
static void fsmArrebInicia(arrebSt_t* Boton, gpioMap_t tecla);
static bool fsmArreb(arrebSt_t* Boton); //antirrebote botones, retorna TRUE si el botón es pulsado
extern uint8_t bp;

/*=====[Implementaciones de funciones publicas]==============================*/

void tecla(void* taskParmPtr) {
	uint32_t tecIndice;
	volatile static uint32_t numpul=0;

	for (tecIndice = 0; tecIndice <= CANT_TECLAS; tecIndice++)
		fsmArrebInicia(&BotSt[tecIndice], teclas[tecIndice]);

	while (1) {
		for (tecIndice = 0; tecIndice < CANT_TECLAS; tecIndice++) {
			if (fsmArreb(&BotSt[tecIndice])) {
				bp=tecIndice;
				//
				//Estesemáforo aún no me funciona
				//Para que funcione con el semáforo hay que descomentar el give y también
				//descomentar el take en app.c
				//
				xSemaphoreGive(semaforo);
			}

		}
	}
}

/*=====[Implementaciones de funciones de interrupcion publicas]==============*/
/*
 void UART0_IRQHandler(void)
 {
 // ...
 }
 */
/*=====[Implementaciones de funciones privadas]==============================*/

static void fsmArrebInicia(arrebSt_t* Boton, gpioMap_t tecla) {
	Boton->boton = tecla;
	Boton->status = UP;
}

static bool fsmArreb(arrebSt_t* Boton) {
	bool retorno = FALSE;
	if (Boton == NULL)
		return (retorno);

	switch (Boton->status) {
	default:
	case UP:
		if (!gpioRead(Boton->boton)) //detecta que se pulsó, va a falling
				{
			delayConfig(&Boton->demSt, TIEMPO_ANTIRREBOTE);
			delayRead(&Boton->demSt);	//al leerlo se le da arranque
			Boton->status = FALLING;
		}
		break;
	case FALLING:
		if (delayRead(&Boton->demSt)) //pasó el tiempo, verificar que la tecla sigue apretada
				{
			if (!gpioRead(Boton->boton)) {
				retorno = TRUE;
				Boton->status = DOWN;
			} else
				Boton->status = UP;
		}
		break;
	case DOWN:
		if (gpioRead(Boton->boton)) //detecta que se soltó, va a estado SOLTANDO
				{
			delayConfig(&Boton->demSt, TIEMPO_ANTIRREBOTE);
			delayRead(&Boton->demSt);	//al leerlo se le da arranque
			Boton->status = RISING;
		}
		break;
	case RISING:
		if (delayRead(&Boton->demSt)) //pasó el tiempo, verificar que la tecla sigue soltada
				{
			if (gpioRead(Boton->boton)) {
				Boton->status = UP;
			} else
				Boton->status = DOWN;
		}
		break;
	}
	return (retorno);
}

