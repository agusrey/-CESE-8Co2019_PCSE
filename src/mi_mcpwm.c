/*=====[mi_mcpwm.c]=====================================================
 * Copyright Agustín Rey <agustinrey61@gmail.com>
 * All rights reserved.
 * Licencia: Texto de la licencia o al menos el nombre y un link
 (ejemplo: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 0.0.0
 * Fecha de creacion: 2016/06/10
 */

/*=====[Inclusion de su propia cabecera]=====================================*/

#include "../../tp_pcse_rtos1/inc/mi_mcpwm.h"

#include "mcpwm_18xx_43xx.h"

/*=====[Inclusiones de dependencias de funciones privadas]===================*/

#include "stdint.h"
#include "chip.h"
#include "mcpwm_18xx_43xx.h"
#include "chip_lpc43xx.h"
#include "sapi.h"


/*=====[Macros de definicion de constantes privadas]=========================*/


/*=====[Macros estilo funcion privadas]======================================*/



/*=====[Definiciones de tipos de datos privados]=============================*/



/*=====[Definiciones de Variables globales publicas externas]================*/



/*=====[Definiciones de Variables globales publicas]=========================*/



/*=====[Definiciones de Variables globales privadas]=========================*/



/*=====[Prototipos de funciones privadas]====================================*/



/*=====[Implementaciones de funciones publicas]==============================*/

/*	void mcpwm_pin_init(void)
 * Programa los pines del MCPWM
 *
 * MCOA0	P4_0	PIN 1	FUNC1
 * MCOA1	P5_5	PIN 58	FUNC1
 * MCOA2	P5_7	PIN 65	FUNC1
 * MCOB0	P5_4	PIN 57	FUNC1
 * MCOB1	P5_6	PIN 63	FUNC1
 * MCOB2	P5_0	PIN 37	FUNC1
 * MABORT	P6_10	PIN 100	FUNC1 (en este caso es entrada con pull-up)
 */

void MCPWM_Pin_Init(void)
{
	/* El prototipo es:
	 *void Chip_SCU_PinMux(uint8_t port, uint8_t pin, uint16_t mode, uint8_t func)
	 */

	Chip_SCU_PinMux(4, 0, SCU_MODE_8MA_DRIVESTR | SCU_MODE_INACT | SCU_MODE_ZIF_DIS, FUNC1);
	Chip_SCU_PinMux(5, 5, SCU_MODE_8MA_DRIVESTR | SCU_MODE_INACT | SCU_MODE_ZIF_DIS, FUNC1);
	Chip_SCU_PinMux(5, 7, SCU_MODE_8MA_DRIVESTR | SCU_MODE_INACT | SCU_MODE_ZIF_DIS, FUNC1);
	Chip_SCU_PinMux(5, 4, SCU_MODE_8MA_DRIVESTR | SCU_MODE_INACT | SCU_MODE_ZIF_DIS, FUNC1);
	Chip_SCU_PinMux(5, 6, SCU_MODE_8MA_DRIVESTR | SCU_MODE_INACT | SCU_MODE_ZIF_DIS, FUNC1);
	Chip_SCU_PinMux(5, 0, SCU_MODE_8MA_DRIVESTR | SCU_MODE_INACT | SCU_MODE_ZIF_DIS, FUNC1);
	Chip_SCU_PinMux(6, 10, SCU_MODE_PULLUP, FUNC1);
}

/*********************************************************************//**
 * @brief		Initializes the MCPWM peripheral
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @return		None
 **********************************************************************/
void MCPWM_Init(LPC_MCPWM_T *MCPWMx)
{
	/* Turn On MCPWM PCLK */
	//LPC_CGU->BASE_VPB1_CLK = (SRC_PL160M_0<<24) | (1<<11);
	//CGU_EntityConnect(CGU_CLKSRC_PLL1, CGU_BASE_APB1);
	MCPWMx->CAPCON_CLR = MCPWM_CAPCLR_CAP(0) | MCPWM_CAPCLR_CAP(1) | MCPWM_CAPCLR_CAP(2);

	MCPWMx->INTF_CLR = MCPWM_INT_ILIM(0) | MCPWM_INT_ILIM(1) | MCPWM_INT_ILIM(2) | MCPWM_INT_IMAT(0)
			| MCPWM_INT_IMAT(1) | MCPWM_INT_IMAT(2) | MCPWM_INT_ICAP(0) | MCPWM_INT_ICAP(1)
			| MCPWM_INT_ICAP(2);

	MCPWMx->INTEN_CLR = MCPWM_INT_ILIM(0) | MCPWM_INT_ILIM(1) | MCPWM_INT_ILIM(2)
			| MCPWM_INT_IMAT(0) | MCPWM_INT_IMAT(1) | MCPWM_INT_IMAT(2) | MCPWM_INT_ICAP(0)
			| MCPWM_INT_ICAP(1) | MCPWM_INT_ICAP(2);
}

/********************************************************************
 * @brief		Enabel/disable el 3-phase AC motor mode en el MCPWM
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * 				acMode should be ENABLE or DISABLE
 * @return		None
 **********************************************************************/

void MCPWM_ACMode(LPC_MCPWM_T *MCPWMx, uint32_t acMode)
{
	if (acMode)
	{
		MCPWMx->CON_SET = MCPWM_CON_ACMODE;
	}
	else
	{
		MCPWMx->CON_CLR = MCPWM_CON_ACMODE;
	}
}

/*********************************************************************//**
 * @brief		Write to MCPWM shadow registers - Update the value for period
 * 				and pulse width in MCPWM peripheral.
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	channelNum	Channel Number, should be: 0..2.
 * @param[in]	channelSetup Pointer to a MCPWM_CHANNEL_CFG_Type structure
 * 				that contains the configuration information for the specified
 * 				MCPWM channel.
 * @return		None
 **********************************************************************/
void MCPWM_WriteToShadow(LPC_MCPWM_T *MCPWMx, uint32_t channelNum,
		MCPWM_CHANNEL_CFG_Type *channelSetup)
{
	if (channelNum == MCPWM_CHANNEL_0)
	{
		MCPWMx->LIM[0] = channelSetup->channelPeriodValue;
		MCPWMx->MAT[0] = channelSetup->channelPulsewidthValue;
	}
	else if (channelNum == MCPWM_CHANNEL_1)
	{
		MCPWMx->LIM[1] = channelSetup->channelPeriodValue;
		MCPWMx->MAT[1] = channelSetup->channelPulsewidthValue;
	}
	else if (channelNum == MCPWM_CHANNEL_2)
	{
		MCPWMx->LIM[2] = channelSetup->channelPeriodValue;
		MCPWMx->MAT[2] = channelSetup->channelPulsewidthValue;
	}
}

/*********************************************************************//**
 * @brief		Configures each channel in MCPWM peripheral according to the
 * 				specified parameters in the MCPWM_CHANNEL_CFG_Type.
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	channelNum	Channel number, should be: 0..2.
 * @param[in]	channelSetup Pointer to a MCPWM_CHANNEL_CFG_Type structure
 * 				that contains the configuration information for the specified
 * 				MCPWM channel.
 * @return		None
 **********************************************************************/
void MCPWM_ConfigChannel(LPC_MCPWM_T *MCPWMx, uint32_t channelNum,
		MCPWM_CHANNEL_CFG_Type * channelSetup)
{
	if (channelNum <= 2)
	{
		if (channelNum == MCPWM_CHANNEL_0)
		{
			MCPWMx->TC[0] = channelSetup->channelTimercounterValue;
			MCPWMx->LIM[0] = channelSetup->channelPeriodValue;
			MCPWMx->MAT[0] = channelSetup->channelPulsewidthValue;
		}
		else if (channelNum == MCPWM_CHANNEL_1)
		{
			MCPWMx->TC[1] = channelSetup->channelTimercounterValue;
			MCPWMx->LIM[1] = channelSetup->channelPeriodValue;
			MCPWMx->MAT[1] = channelSetup->channelPulsewidthValue;
		}
		else if (channelNum == MCPWM_CHANNEL_2)
		{
			MCPWMx->TC[2] = channelSetup->channelTimercounterValue;
			MCPWMx->LIM[2] = channelSetup->channelPeriodValue;
			MCPWMx->MAT[2] = channelSetup->channelPulsewidthValue;
		}
		else
		{
			return;
		}

		if (channelSetup->channelType == MCPWM_CHANNEL_CENTER_MODE)
		{
			MCPWMx->CON_SET = MCPWM_CON_CENTER(channelNum);
		}
		else
		{
			MCPWMx->CON_CLR = MCPWM_CON_CENTER(channelNum);
		}

		if (channelSetup->channelPolarity == MCPWM_CHANNEL_PASSIVE_HI)
		{
			MCPWMx->CON_SET = MCPWM_CON_POLAR(channelNum);
		}
		else
		{
			MCPWMx->CON_CLR = MCPWM_CON_POLAR(channelNum);
		}

		if (channelSetup->channelDeadtimeEnable == ENABLE)
		{
			MCPWMx->CON_SET = MCPWM_CON_DTE(channelNum);

			MCPWMx->DT &= ~(MCPWM_DT(channelNum, 0x3FF));

			MCPWMx->DT |= MCPWM_DT(channelNum, channelSetup->channelDeadtimeValue);
		}
		else
		{
			MCPWMx->CON_CLR = MCPWM_CON_DTE(channelNum);
		}

		if (channelSetup->channelUpdateEnable == ENABLE)
		{
			MCPWMx->CON_CLR = MCPWM_CON_DISUP(channelNum);
		}
		else
		{
			MCPWMx->CON_SET = MCPWM_CON_DISUP(channelNum);
		}
	}
}

/*********************************************************************//**
 * @brief		Start MCPWM activity for each MCPWM channel
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	channel0 State of this command on channel 0:
 * 					- ENABLE: 'Start' command will effect on channel 0
 * 					- DISABLE: 'Start' command will not effect on channel 0
 * @param[in]	channel1 State of this command on channel 1:
 * 					- ENABLE: 'Start' command will effect on channel 1
 * 					- DISABLE: 'Start' command will not effect on channel 1
 * @param[in]	channel2 State of this command on channel 2:
 * 					- ENABLE: 'Start' command will effect on channel 2
 * 					- DISABLE: 'Start' command will not effect on channel 2
 * @return		None
 **********************************************************************/
void MCPWM_Start(LPC_MCPWM_T *MCPWMx, uint32_t channel0, uint32_t channel1, uint32_t channel2)
{
	uint32_t regVal = 0;

	regVal = (channel0 ? MCPWM_CON_RUN(0) : 0) | (channel1 ? MCPWM_CON_RUN(1) : 0)
			| (channel2 ? MCPWM_CON_RUN(2) : 0);

	MCPWMx->CON_SET = regVal;
}


/*********************************************************************//**
 * @brief		Stop MCPWM activity for each MCPWM channel
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	channel0 State of this command on channel 0:
 * 					- ENABLE: 'Stop' command will effect on channel 0
 * 					- DISABLE: 'Stop' command will not effect on channel 0
 * @param[in]	channel1 State of this command on channel 1:
 * 					- ENABLE: 'Stop' command will effect on channel 1
 * 					- DISABLE: 'Stop' command will not effect on channel 1
 * @param[in]	channel2 State of this command on channel 2:
 * 					- ENABLE: 'Stop' command will effect on channel 2
 * 					- DISABLE: 'Stop' command will not effect on channel 2
 * @return		None
 **********************************************************************/
void MCPWM_Stop(LPC_MCPWM_T *MCPWMx, uint32_t channel0,
		uint32_t channel1, uint32_t channel2)
{
	uint32_t regVal = 0;

	regVal = (channel0 ? MCPWM_CON_RUN(0) : 0) | (channel1 ? MCPWM_CON_RUN(1) : 0) \
				| (channel2 ? MCPWM_CON_RUN(2) : 0);

	MCPWMx->CON_CLR = regVal;
}



/*=====================================================================================
 * void MCPWM_InitChannels(LPC_MCPWM_T *MCPWMx, MCPWM_CHANNEL_CFG_Type *canal[])
 * Inicializa los canales a 10Khz, centrado y width 50%
 * ====================================================================================
 */

void MCPWM_InitChannels(LPC_MCPWM_T *MCPWMx, MCPWM_CHANNEL_CFG_Type canal[])
{

	canal[0].channelType = MCPWM_CHANNEL_CENTER_MODE;
	canal[0].channelPolarity = MCPWM_CHANNEL_PASSIVE_LO;
	canal[0].channelUpdateEnable = ENABLE;
	canal[0].channelDeadtimeEnable = DISABLE;
	canal[0].channelTimercounterValue = 0;
	canal[0].channelPeriodValue = 20400;
	canal[0].channelPulsewidthValue = 10200;
	MCPWM_ConfigChannel(LPC_MCPWM, 0, &canal[0]);

	canal[1].channelType = MCPWM_CHANNEL_CENTER_MODE;
	canal[1].channelPolarity = MCPWM_CHANNEL_PASSIVE_LO;
	canal[1].channelUpdateEnable = ENABLE;
	canal[1].channelDeadtimeEnable = DISABLE;
	canal[1].channelTimercounterValue = 0;
	canal[1].channelPeriodValue = 20400;
	canal[1].channelPulsewidthValue = 10200;
	MCPWM_ConfigChannel(LPC_MCPWM, 1, &canal[1]);

	canal[2].channelType = MCPWM_CHANNEL_CENTER_MODE;
	canal[2].channelPolarity = MCPWM_CHANNEL_PASSIVE_LO;
	canal[2].channelUpdateEnable = ENABLE;
	canal[2].channelDeadtimeEnable = DISABLE;
	canal[2].channelTimercounterValue = 0;
	canal[2].channelPeriodValue = 20400;
	canal[2].channelPulsewidthValue = 10200;

	MCPWM_ConfigChannel(LPC_MCPWM, 2, &canal[2]);
}

/*********************************************************************//**
 * @brief		Configures the specified interrupt in MCPWM peripheral
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	ulIntType	Interrupt type, should be:
 * 					- MCPWM_INTFLAG_LIM0	:Limit interrupt for channel (0)
 * 					- MCPWM_INTFLAG_MAT0	:Match interrupt for channel (0)
 * 					- MCPWM_INTFLAG_CAP0	:Capture interrupt for channel (0)
 * 					- MCPWM_INTFLAG_LIM1	:Limit interrupt for channel (1)
 * 					- MCPWM_INTFLAG_MAT1	:Match interrupt for channel (1)
 * 					- MCPWM_INTFLAG_CAP1	:Capture interrupt for channel (1)
 * 					- MCPWM_INTFLAG_LIM2	:Limit interrupt for channel (2)
 * 					- MCPWM_INTFLAG_MAT2	:Match interrupt for channel (2)
 * 					- MCPWM_INTFLAG_CAP2	:Capture interrupt for channel (2)
 * 					- MCPWM_INTFLAG_ABORT	:Fast abort interrupt
 * @param[in]	NewState	New State of this command, should be:
 * 					- ENABLE.
 * 					- DISABLE.
 * @return		None
 *
 * Note: all these ulIntType values above can be ORed together for using as input parameter.
 **********************************************************************/
void MCPWM_IntConfig(LPC_MCPWM_T *MCPWMx, uint32_t ulIntType, FunctionalState NewState)
{
	if (NewState)
	{
		MCPWMx->INTEN_SET = ulIntType;
	}
	else
	{
		MCPWMx->INTEN_CLR = ulIntType;
	}
}


/*********************************************************************//**
 * @brief		Sets/Forces the specified interrupt for MCPWM peripheral
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	ulIntType	Interrupt type, should be:
 * 					- MCPWM_INTFLAG_LIM0	:Limit interrupt for channel (0)
 * 					- MCPWM_INTFLAG_MAT0	:Match interrupt for channel (0)
 * 					- MCPWM_INTFLAG_CAP0	:Capture interrupt for channel (0)
 * 					- MCPWM_INTFLAG_LIM1	:Limit interrupt for channel (1)
 * 					- MCPWM_INTFLAG_MAT1	:Match interrupt for channel (1)
 * 					- MCPWM_INTFLAG_CAP1	:Capture interrupt for channel (1)
 * 					- MCPWM_INTFLAG_LIM2	:Limit interrupt for channel (2)
 * 					- MCPWM_INTFLAG_MAT2	:Match interrupt for channel (2)
 * 					- MCPWM_INTFLAG_CAP2	:Capture interrupt for channel (2)
 * 					- MCPWM_INTFLAG_ABORT	:Fast abort interrupt
 * @return		None
 * Note: all these ulIntType values above can be ORed together for using as input parameter.
 **********************************************************************/
void MCPWM_IntSet(LPC_MCPWM_T *MCPWMx, uint32_t ulIntType)
{
	MCPWMx->INTF_SET = ulIntType;
}

/*********************************************************************//**
 * @brief		Clear the specified interrupt pending for MCPWM peripheral
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	ulIntType	Interrupt type, should be:
 * 					- MCPWM_INTFLAG_LIM0	:Limit interrupt for channel (0)
 * 					- MCPWM_INTFLAG_MAT0	:Match interrupt for channel (0)
 * 					- MCPWM_INTFLAG_CAP0	:Capture interrupt for channel (0)
 * 					- MCPWM_INTFLAG_LIM1	:Limit interrupt for channel (1)
 * 					- MCPWM_INTFLAG_MAT1	:Match interrupt for channel (1)
 * 					- MCPWM_INTFLAG_CAP1	:Capture interrupt for channel (1)
 * 					- MCPWM_INTFLAG_LIM2	:Limit interrupt for channel (2)
 * 					- MCPWM_INTFLAG_MAT2	:Match interrupt for channel (2)
 * 					- MCPWM_INTFLAG_CAP2	:Capture interrupt for channel (2)
 * 					- MCPWM_INTFLAG_ABORT	:Fast abort interrupt
 * @return		None
 * Note: all these ulIntType values above can be ORed together for using as input parameter.
 **********************************************************************/
void MCPWM_IntClear(LPC_MCPWM_T *MCPWMx, uint32_t ulIntType)
{
	MCPWMx->INTF_CLR = ulIntType;
}

/*********************************************************************//**
 * @brief		Check whether if the specified interrupt in MCPWM is set or not
 * @param[in]	MCPWMx 	Motor Control PWM peripheral selected, should be: LPC_MCPWM
 * @param[in]	ulIntType	Interrupt type, should be:
 * 					- MCPWM_INTFLAG_LIM0	:Limit interrupt for channel (0)
 * 					- MCPWM_INTFLAG_MAT0	:Match interrupt for channel (0)
 * 					- MCPWM_INTFLAG_CAP0	:Capture interrupt for channel (0)
 * 					- MCPWM_INTFLAG_LIM1	:Limit interrupt for channel (1)
 * 					- MCPWM_INTFLAG_MAT1	:Match interrupt for channel (1)
 * 					- MCPWM_INTFLAG_CAP1	:Capture interrupt for channel (1)
 * 					- MCPWM_INTFLAG_LIM2	:Limit interrupt for channel (2)
 * 					- MCPWM_INTFLAG_MAT2	:Match interrupt for channel (2)
 * 					- MCPWM_INTFLAG_CAP2	:Capture interrupt for channel (2)
 * 					- MCPWM_INTFLAG_ABORT	:Fast abort interrupt
 * @return		None
 **********************************************************************/
FlagStatus MCPWM_GetIntStatus(LPC_MCPWM_T *MCPWMx, uint32_t ulIntType)
{
	return ((MCPWMx->INTF & ulIntType) ? SET : RESET);
}




/*=====[Implementaciones de funciones de interrupcion publicas]==============*/


/*=====[Implementaciones de funciones privadas]==============================*/

