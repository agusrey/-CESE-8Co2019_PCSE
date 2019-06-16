/*=====[mi_mcpwm.h]=====================================================
 * Copyright Agustín Rey <agustinrey61@gmail.com>
 * All rights reserved.
 * Licencia: Texto de la licencia o al menos el nombre y un link
 (ejemplo: BSD-3-Clause <https://opensource.org/licenses/BSD-3-Clause>)
 *
 * Version: 0.0.0
 * Fecha de creacion: 2016/06/10
 */

/*=====[Evitar inclusion multiple comienzo]==================================*/

#ifndef TP_PCSE_APP_INC_MI_MCPWM_H_
#define TP_PCSE_APP_INC_MI_MCPWM_H_

/*=====[Inclusiones de dependencias de funciones publicas]===================*/
#include "stdint.h"
#include "chip.h"
#include "mcpwm_18xx_43xx.h"
#include "chip_lpc43xx.h"

/*=====[C++ comienzo]========================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

/*=====[Macros Privados]====================================================*/
/** Edge aligned mode for channel in MCPWM */
#define MCPWM_CHANNEL_EDGE_MODE			((uint32_t)(0))
/** Center aligned mode for channel in MCPWM */
#define MCPWM_CHANNEL_CENTER_MODE		((uint32_t)(1))

/** Polarity of the MCOA and MCOB pins: Passive state is LOW, active state is HIGH */
#define MCPWM_CHANNEL_PASSIVE_LO		((uint32_t)(0))
/** Polarity of the MCOA and MCOB pins: Passive state is HIGH, active state is LOW */
#define MCPWM_CHANNEL_PASSIVE_HI		((uint32_t)(1))

/* Output Patent in 3-phase DC mode, the internal MCOA0 signal is routed to any or all of
 * the six output pins under the control of the bits in this register */
#define MCPWM_PATENT_A0		((uint32_t)(1<<0))	/**< MCOA0 tracks internal MCOA0 */
#define MCPWM_PATENT_B0		((uint32_t)(1<<1))	/**< MCOB0 tracks internal MCOA0 */
#define MCPWM_PATENT_A1		((uint32_t)(1<<2))	/**< MCOA1 tracks internal MCOA0 */
#define MCPWM_PATENT_B1		((uint32_t)(1<<3))	/**< MCOB1 tracks internal MCOA0 */
#define MCPWM_PATENT_A2		((uint32_t)(1<<4))	/**< MCOA2 tracks internal MCOA0 */
#define MCPWM_PATENT_B2		((uint32_t)(1<<5))	/**< MCOB2 tracks internal MCOA0 */

/* Interrupt type in MCPWM */
/** Limit interrupt for channel (0) */
#define MCPWM_INTFLAG_LIM0	MCPWM_INT_ILIM(0)
/** Match interrupt for channel (0) */
#define MCPWM_INTFLAG_MAT0	MCPWM_INT_IMAT(0)
/** Capture interrupt for channel (0) */
#define MCPWM_INTFLAG_CAP0	MCPWM_INT_ICAP(0)

/** Limit interrupt for channel (1) */
#define MCPWM_INTFLAG_LIM1	MCPWM_INT_ILIM(1)
/** Match interrupt for channel (1) */
#define MCPWM_INTFLAG_MAT1	MCPWM_INT_IMAT(1)
/** Capture interrupt for channel (1) */
#define MCPWM_INTFLAG_CAP1	MCPWM_INT_ICAP(1)

/** Limit interrupt for channel (2) */
#define MCPWM_INTFLAG_LIM2	MCPWM_INT_ILIM(2)
/** Match interrupt for channel (2) */
#define MCPWM_INTFLAG_MAT2	MCPWM_INT_IMAT(2)
/** Capture interrupt for channel (2) */
#define MCPWM_INTFLAG_CAP2	MCPWM_INT_ICAP(2)

/** Fast abort interrupt */
#define MCPWM_INTFLAG_ABORT	MCPWM_INT_ABORT

/*********************************************************************//**
 * Macro defines for MCPWM Interrupt register
 **********************************************************************/
/* Interrupt registers, these macro definitions below can be applied for these
 * register type:
 * - MCPWM Interrupt Enable read address
 * - MCPWM Interrupt Enable set address
 * - MCPWM Interrupt Enable clear address
 * - MCPWM Interrupt Flags read address
 * - MCPWM Interrupt Flags set address
 * - MCPWM Interrupt Flags clear address
 */
/** Limit interrupt for channel (n) */
#define MCPWM_INT_ILIM(n)	(((n>=0)&&(n<=2)) ? ((uint32_t)(1<<((n*4)+0))) : (0))
/** Match interrupt for channel (n) */
#define MCPWM_INT_IMAT(n)	(((n>=0)&&(n<=2)) ? ((uint32_t)(1<<((n*4)+1))) : (0))
/** Capture interrupt for channel (n) */
#define MCPWM_INT_ICAP(n)	(((n>=0)&&(n<=2)) ? ((uint32_t)(1<<((n*4)+2))) : (0))
/** Fast abort interrupt */
#define MCPWM_INT_ABORT		((uint32_t)(1<<15))

/*********************************************************************
 * Macro defines for MCPWM Control register
 **********************************************************************/
/* MCPWM Control register, these macro definitions below can be applied for these
 * register type:
 * - MCPWM Control read address
 * - MCPWM Control set address
 * - MCPWM Control clear address
 */
/**< Stops/starts timer channel n */
#define MCPWM_CON_RUN(n)		(((n>=0)&&(n<=2)) ? ((uint32_t)(1<<((n*8)+0))) : (0))
/**< Edge/center aligned operation for channel n */
#define MCPWM_CON_CENTER(n)		(((n<=2)) ? ((uint32_t)(1<<((n*8)+1))) : (0))
/**< Select polarity of the MCOAn and MCOBn pin */
#define MCPWM_CON_POLAR(n)		(((n<=2)) ? ((uint32_t)(1<<((n*8)+2))) : (0))
/**< Control the dead-time feature for channel n */
#define MCPWM_CON_DTE(n)		(((n<=2)) ? ((uint32_t)(1<<((n*8)+3))) : (0))
/**< Enable/Disable update of functional register for channel n */
#define MCPWM_CON_DISUP(n)		(((n<=2)) ? ((uint32_t)(1<<((n*8)+4))) : (0))
/**< Control the polarity for all 3 channels */
#define MCPWM_CON_INVBDC		((uint32_t)(1<<29))
/**< 3-phase AC mode select */
#define MCPWM_CON_ACMODE		((uint32_t)(1<<30))
/**< 3-phase DC mode select */
#define MCPWM_CON_DCMODE		(((uint32_t)1<<31))

/*********************************************************************//**
 * Macro defines for MCPWM Dead-time register
 **********************************************************************/
/** Dead time value x for channel n */
#define MCPWM_DT(n,x)		(((n<=2)) ? ((uint32_t)((x&0x3FF)<<(n*10))) : (0))

/*=====[Macros Públicos]=========================*/
/*********************************************************************//**
 * Macro defines for MCPWM Capture clear address register
 **********************************************************************/
/** Clear the MCCAP (n) register */
#define MCPWM_CAPCLR_CAP(n)		(((n<=2)) ? ((uint32_t)(1<<n)) : (0))


/*=====[Macros estilo funcion publicas]======================================*/

//#define printInt(printer,number) printIntFormat((printer),(number),(DEC_FORMAT))
//#define printlnString(printer,string);   {printString((printer),(string));\
                                          printEnter((printer));}
/*=====[Definiciones de tipos de datos publicos y Enumeraciones]=============================*/
/*********************************************************************//**
 * @brief MCPWM enumeration
 **********************************************************************/
/**
 * @brief	MCPWM channel identifier definition
 */
typedef enum
{
	MCPWM_CHANNEL_0 = 0, /**< MCPWM channel 0 */
	MCPWM_CHANNEL_1, /**< MCPWM channel 1 */
	MCPWM_CHANNEL_2 /**< MCPWM channel 2 */
} en_MCPWM_Channel_Id;

/*********************************************************************//**
 * @brief MCPWM structure definitions
 **********************************************************************/
/**
 * @brief Motor Control PWM Channel Configuration structure type definition
 */
typedef struct
{
	uint32_t channelType; /**< Edge/center aligned mode for this channel,
	 should be:
	 - MCPWM_CHANNEL_EDGE_MODE: Channel is in Edge mode
	 - MCPWM_CHANNEL_CENTER_MODE: Channel is in Center mode
	 */
	uint32_t channelPolarity; /**< Polarity of the MCOA and MCOB pins, should be:
	 - MCPWM_CHANNEL_PASSIVE_LO: Passive state is LOW, active state is HIGH
	 - MCPWM_CHANNEL_PASSIVE_HI: Passive state is HIGH, active state is LOW
	 */
	uint32_t channelDeadtimeEnable; /**< Enable/Disable DeadTime function for channel, should be:
	 - ENABLE.
	 - DISABLE.
	 */
	uint32_t channelDeadtimeValue; /**< DeadTime value, should be less than 0x3FF */
	uint32_t channelUpdateEnable; /**< Enable/Disable updates of functional registers,
	 should be:
	 - ENABLE.
	 - DISABLE.
	 */
	uint32_t channelTimercounterValue; /**< MCPWM Timer Counter value */
	uint32_t channelPeriodValue; /**< MCPWM Period value */
	uint32_t channelPulsewidthValue; /**< MCPWM Pulse Width value */
} MCPWM_CHANNEL_CFG_Type;

/**
 * @brief MCPWM Capture Configuration type definition
 */
typedef struct
{
	uint32_t captureChannel; /**< Capture Channel Number, should be in range from 0 to 2 */
	uint32_t captureRising; /**< Enable/Disable Capture on Rising Edge event, should be:
	 - ENABLE.
	 - DISABLE.
	 */
	uint32_t captureFalling; /**< Enable/Disable Capture on Falling Edge event, should be:
	 - ENABLE.
	 - DISABLE.
	 */
	uint32_t timerReset; /**< Enable/Disable Timer reset function an capture, should be:
	 - ENABLE.
	 - DISABLE.
	 */
	uint32_t hnfEnable; /**< Enable/Disable Hardware noise filter function, should be:
	 - ENABLE.
	 - DISABLE.
	 */
} MCPWM_CAPTURE_CFG_Type;

/*=====[Prototipos de funciones publicas]====================================*/

void MCPWM_Init(LPC_MCPWM_T *MCPWMx);
void MCPWM_Pin_Init(void);
void MCPWM_ACMode(LPC_MCPWM_T *MCPWMx, uint32_t acMode);
void MCPWM_ConfigChannel(LPC_MCPWM_T *MCPWMx, uint32_t channelNum,
		MCPWM_CHANNEL_CFG_Type * channelSetup);
void MCPWM_Start(LPC_MCPWM_T *MCPWMx, uint32_t channel0, uint32_t channel1, uint32_t channel2);
void MCPWM_Stop(LPC_MCPWM_T *MCPWMx, uint32_t channel0,	uint32_t channel1, uint32_t channel2);
void MCPWM_WriteToShadow(LPC_MCPWM_T *MCPWMx, uint32_t channelNum,
		MCPWM_CHANNEL_CFG_Type *channelSetup);
void MCPWM_InitChannels(LPC_MCPWM_T *MCPWMx, MCPWM_CHANNEL_CFG_Type canal[]);
void MCPWM_IntConfig(LPC_MCPWM_T *MCPWMx, uint32_t ulIntType, FunctionalState NewState);
void MCPWM_IntSet(LPC_MCPWM_T *MCPWMx, uint32_t ulIntType);
void MCPWM_IntClear(LPC_MCPWM_T *MCPWMx, uint32_t ulIntType);
FlagStatus MCPWM_GetIntStatus(LPC_MCPWM_T *MCPWMx, uint32_t ulIntType);
int svpwm(void);
/*=====[Prototipos de funciones publicas de interrupcion]====================*/

/*=====[C++ fin]=============================================================*/

#ifdef __cplusplus
}
#endif

/*=====[Evitar inclusion multiple fin]=======================================*/

#endif /* TP_PCSE_APP_INC_MI_MCPWM_H_ */
