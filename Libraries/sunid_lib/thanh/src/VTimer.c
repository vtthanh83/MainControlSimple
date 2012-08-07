/***********************************************************************//**
 * @file		VTimer.c
 * @brief		Controller used to create Virtual Timer		
 * @version		1.0
 * @date		25. Feb. 2011
 * @author		BKIT Hardware club - BKIT4U Team
 *
 * 				Service used to create Virtual Timer
 * @todo		Change period for VTimer to 1ms instead of 10ms
 **********************************************************************/
#include "VTimer.h"
/**
 *	@defgroup VTimerController	Virtual Timer Controller
 *	@ingroup  Controller
 *	@{
 */

/** 		TimerPeriod: Array contain the current value for each Virtual Timer 
 *	@par	if TimerPeriod[i] = 0 ==> Timer i is free and can be reused
 *  @par	if TimerPeriod[i] = 1 ==> Timer i is fired and is waiting to be call VTimer_IsFired to be reused
 */
__IO uint32_t TimerPeriod[NumberOfVirTimer];

///Init Virtual Timer Controller
void VTimer_InitController(void)
{
	uint8_t i = 0;
	for(i = 0; i< NumberOfVirTimer; i++)
	{
		TimerPeriod[i] = 0;
	}
}
/********************************************************************//**
 * @brief		Check if an Virtual Timer is fired or not
 * @param[in]	TimerId: Id of VTimer that you want to check
 * @return 		
 *				- 1 : Fired
 *				- 0 : Not Fired
 *********************************************************************/
uint8_t VTimer_IsFired(uint8_t TimerId)
{	
	if (TimerPeriod[TimerId] == 1)
		return 1;
	else return 0;
}
/********************************************************************//**
 * @brief		Virtual Timer Service to update VTimer status
 * @param[in]	None
 * @warning		This Service has to be called from SysTick_Handler()
 * @return 		None
 *********************************************************************/
void VTimer_Service(void)
{
	uint8_t i = 0;
	for(i = 0; i< NumberOfVirTimer; i++)
	{
		if (TimerPeriod[i]>1)
			TimerPeriod[i] --;
	}	
}
/********************************************************************//**
 * @brief		Setup a VTimer to fire after a period
 * @param[in]	TimerId: ID of VTimer
 * @param[in]	period: VTimer will fire after this period(ms)
 * @return 			- 0 : FAIL
 *					- 1 : SUCCESS
 *********************************************************************/
uint8_t VTimer_Set(uint8_t TimerId, uint32_t period)
{	
	
	if (TimerPeriod[TimerId] >0) return 0;
	TimerPeriod[TimerId] = period/SUNID_TIM_TICK+1;
	return 1;
}
/********************************************************************//**
 * @brief		Get ID of a Free Virtual Timer 
 * @param[in]	None
 * @warning		You have only a NumberOfVirTimer
 * @return 		ID of a Free Virtual Timer
 *********************************************************************/
uint8_t VTimer_GetID()
{
	uint8_t i = 1;
	for(i = 1; i< NumberOfVirTimer; i++)
	{
		if (TimerPeriod[i] == 0)
			return i;
	}	
	return 0;
}
/********************************************************************//**
 * @brief		Release a VTimer that you don't use anymore
 * @param[in]	TimerId: ID of VTimer that you want to release
 * @warning		You have only a NumberOfVirTimer, so release VTimer immediatelly if you 
 				don't you it anymore
 * @return 		None
 *********************************************************************/
void VTimer_Release(uint8_t TimerId)
{
	TimerPeriod[TimerId] = 0;
}
/********************************************************************//**
 * @brief		Delay milisecond 
 * @param[in]	period: time you want to delay (ms)
 * @warning		This function depends on System Tick to run, so don't call it 
 *				from SysTick_Handler()
 *			
 *				Period has to above 10ms because System Tick is 10ms
 * @return 		None
 *********************************************************************/
void delay_ms(uint32_t period)
{
	TimerPeriod[0] = 1+period/SUNID_TIM_TICK;
	while (TimerPeriod[0]!=1);
}
void Delay_ms(uint32_t period)
{
	TimerPeriod[0] = 1+period/SUNID_TIM_TICK;
	while (TimerPeriod[0]!=1);
}
/**
 * @}
 */
