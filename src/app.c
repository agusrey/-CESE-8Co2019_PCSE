/*=====[Nombre del programa]===================================================
 * Copyright 2019 Agustín Rey <agustinrey61@gmail.com>
 * All rights reserved.
 * Licencia: Texto de la licencia o al menos el nombre y un link
 (ejemplo: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 0.0.0
 * Fecha de creacion: 2019/06/12
 */

/*=====[Inclusiones de dependencias de funciones]============================*/
#include "app.h"         // <= Su propia cabecera

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "mi_mcpwm.h"	//manejo del MCPWM
#include "task.h"
#include "semphr.h"
#include "sapi.h"        // <= Biblioteca sAPI

#include "Botones.h"

/*================[Prototipos]=============================================*/
void Led_parpadea(void* taskParmPtr);
void Update_MCPWM(void* taskParmPtr);
void envia(void* taskParmPtr);
/*=====[Macros de definición de constantes privadas]=========================*/

/*=====[Definiciones de variables globales externas]=========================*/

/*=====[Definiciones de variables globales publicas]=========================*/

/*=====[Definiciones de variables globales privadas]=========================*/
typedef struct {
	uint32_t pulse_width[3];
} MCPWM_CHANNEL_PULSEWIDTH;

static MCPWM_CHANNEL_CFG_Type Pwm_Channels[3];
static MCPWM_CHANNEL_PULSEWIDTH canal;
uint32_t cnt = 0;
uint32_t recibido;
uint8_t bp = 0xff;

SemaphoreHandle_t semaforo;

//========= FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main(void) {

	// ---------- CONFIGURACIONES ------------------------------

	// Inicializar y configurar la plataforma
	boardConfig();

	gpioInit(GPIO8, GPIO_OUTPUT); //prepara GPIO8 como salida. Esta salida se togglea con la interrupción del MCPWM

	MCPWM_Init(LPC_MCPWM);							//inicializa el MCPWM
	MCPWM_Pin_Init();					//configura pines de salida del MCPWM
	MCPWM_ACMode(LPC_MCPWM, ENABLE);			//configura el MCPWM en modo AC
	MCPWM_InitChannels(LPC_MCPWM, Pwm_Channels);//inicializa los 3 canales del MCPWM

	//======== Configura la Interrupción del MCPWM
	//MCPWM_IntConfig(LPC_MCPWM, MCPWM_INTFLAG_LIM0, ENABLE); //configura interrupción cuando se completa el período en el canal 0
	//NVIC_EnableIRQ(MCPWM_IRQn);				//habilita la inerrupción en el NVIC
	//NVIC_SetPriority(MCPWM_IRQn, 2); 						//prioridad 2

	//========= CREACIÓN DE LAS TAREAS =================================
	xTaskCreate(Led_parpadea,                  // Funcion de la tarea a ejecutar
			(const char *) "Led_parpadea", // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE * 2, // Cantidad de stack de la tarea
			0,                          // Parametros de tarea
			tskIDLE_PRIORITY + 3,         // Prioridad de la tarea(baja)
			0                         // Puntero a la tarea creada en el sistema
			);

	xTaskCreate(Update_MCPWM,                  // Funcion de la tarea a ejecutar
			(const char *) "Update_MCPWM", // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE * 2, // Cantidad de stack de la tarea
			0,                          // Parametros de tarea
			tskIDLE_PRIORITY + 1,         // Prioridad de la tarea(baja)
			0                         // Puntero a la tarea creada en el sistema
			);

	xTaskCreate(tecla,                  // Funcion de la tarea a ejecutar
			(const char *) "Teclado", // Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE * 2, // Cantidad de stack de la tarea
			0,                          // Parametros de tarea
			tskIDLE_PRIORITY + 3,         // Prioridad de la tarea(baja)
			0                         // Puntero a la tarea creada en el sistema
			);

	// Iniciar scheduler
	vTaskStartScheduler();

	// ---------- REPETIR POR SIEMPRE --------------------------
	while ( TRUE) {

	}

	// NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
	// directamenteno sobre un microcontroladore y no es llamado por ningun
	// Sistema Operativo, como en el caso de un programa para PC.
	return 0;
}

/*
 * Programas de prueba, tratando de que funcione un semáforo con la interrupción del MCPWM
 *
 * Busco que la interrupción del MCPWM interactúe con una tarea del freeRtos, para aplicar lo aprendido en la materia
 *
 * La idea es que cuando en el MCPWM se cumpla un período se genere una interrupción  (cada 200uSeg)
 * En el handler se cargarán los nuevos valores de Pulse Width en los shadow registers del MCPWM
 * Los shadow registers se cargan automáticamente en los match registers al completarse el período actual (dentro de 200uSeg)
 *
 * Una tarea calcula el valor del pulse width y hace un semaphore take a la espera de que el handler haga un give, indicando
 * que ya tomó el último cálculo y que es necesario que la tarea calcule el próximo valor (que deberá estar disponible antes de
 * los 200uSeg).
 *
 * Después de varios días de intentarlo estoy trabado. Cuando hago el take deja de uncionar el programa
 *
 */

//===== Tarea que parpadea un led, solo para mostrar que está con vida
void Led_parpadea(void* taskParmPtr) {
	while (TRUE) {
		gpioToggle(LED3);
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

//===== Tarea que calculará el nuevo valor del pulse width ============
void Update_MCPWM(void* taskParmPtr) {
	bool_t dir = TRUE;
	gpioMap_t boton;
	MCPWM_CHANNEL_CFG_Type ChannelConfig;

	while (true) {

		switch (bp) {
		case 0:
			MCPWM_Start(LPC_MCPWM, 1, 1, 1); //arranca el MCPWM los 3 canales a la vez
			break;
		case 1:
			MCPWM_Stop(LPC_MCPWM, 1, 1, 1); //arranca el MCPWM los 3 canales a la vez
			break;

		case 2:
			if (canal.pulse_width[0] < 20000)
			{
				canal.pulse_width[0] += 200;
				ChannelConfig.channelPulsewidthValue = canal.pulse_width[0];
				ChannelConfig.channelPeriodValue = 20400;
				MCPWM_WriteToShadow(LPC_MCPWM, 0, &ChannelConfig);
			}
			else
				dir = FALSE;
			break;
		case 3:
			if (canal.pulse_width[0] > 2000){
				canal.pulse_width[0] -= 200;
				ChannelConfig.channelPulsewidthValue = canal.pulse_width[0];
				ChannelConfig.channelPeriodValue = 20400;
				MCPWM_WriteToShadow(LPC_MCPWM, 0, &ChannelConfig);
			}
			else
				dir = TRUE;
			break;
		}
		bp = 0xff;
		vTaskDelay(10);
		/*
		 *acá puse un delay, porque el semáforo aún no me funciona
		 *Para que funcione con el semáforo hay que descomentar el take y también
		 *descomentar el give en Botones.c
		 */
		//xSemaphoreTake(semaforo, portMAX_DELAY);
	}
}



/*
 * Handler de interrupción del MCPWM
 *
 */


/*
void MCPWM_IRQHandler(void) {

	MCPWM_CHANNEL_CFG_Type ChannelConfig;

	bool pedido = TRUE;
	static uint8_t cnt = 100;

	xHigherPriorityTaskWoken = pdFALSE;

	if (MCPWM_GetIntStatus(LPC_MCPWM, MCPWM_INTFLAG_LIM0)) {
		MCPWM_IntClear(LPC_MCPWM, MCPWM_INTFLAG_LIM0);
		gpioToggle(GPIO8); //se togglea un pin que sirve de disparo del osciloscopio
		if ((cnt--) == 0) {
			cnt = 100;
			gpioToggle(LED1); //se togglea un led como testigo de que está interrumpiendo
		}

		ChannelConfig.channelPulsewidthValue = canal.pulse_width[0];
		ChannelConfig.channelPeriodValue = 20400;
		MCPWM_WriteToShadow(LPC_MCPWM, 0, &ChannelConfig);

		NVIC_ClearPendingIRQ(MCPWM_IRQn);
	}
}
*/
