#ifndef __VTIMER_H
#define __VTIMER_H


#include "stm32sunid.h"
 /**
 * @addtogroup	VTimerControllerDefine Virtual Timer Define Operation
 * @ingroup		VTimerController
 * @{
 */
	#define NumberOfVirTimer 16
/**
 * @}
 */

 /**
 * @addtogroup	VTimerControllerPublic Virtual Timer Controller Public Function
 * @ingroup		VTimerController
 * @{
 */
	void VTimer_InitController(void);
	uint8_t VTimer_IsFired(uint8_t TimerId);
	void VTimer_Service(void);
	uint8_t VTimer_Set(uint8_t TimerId, uint32_t period);
	uint8_t VTimer_GetID(void);
	void VTimer_Release(uint8_t Timerid);
	void delay_ms(uint32_t period);
	void Delay_ms(uint32_t period);
/**
 * @}
 */

#endif

