/*
 * Manual.c
 *
 *  Created on: Jul 1, 2024
 *      Author: 0163022
 */

#include "main.h"
#include "fault_handling.h"
#include "QS_Switch.h"

extern SystemState currentState;
#define INACTIVE_TIME_MS  8000 // 8 seconds to exit manual mode

uint8_t manualMode = 0;
uint8_t manualModeActive = 0;
uint32_t manualStartTime = 0;

void Enter_Manual_Mode(void)
    {
    uint32_t manualStartTime = HAL_GetTick();
    uint8_t manualMode = 1;

    while (manualMode)
	{
	if ((HAL_GetTick() - manualStartTime) > INACTIVE_TIME_MS) //><EXIT manual mode if more that 8 seconds without any action
	    {
	    manualMode = 0;
	    manualModeActive = 0;
	    }
	if (HAL_GPIO_ReadPin(GPIOB, GPIO_Manual_Pin) == GPIO_PIN_SET)
	    {
	    HAL_Delay(300); // Debounce delay
	    if (HAL_GPIO_ReadPin(GPIOB, GPIO_Manual_Pin) == GPIO_PIN_SET)
		{
		manualStartTime = HAL_GetTick();
		if (currentState == STATE_DC_MODE)
		    {
		    ControlMotor(1);  //><Go to AC Mode
		    currentState = STATE_AC_MODE; // AC Mode
		    }
		else
		    {
		    ControlMotor(-1);  //><Go to DC Mode
		    currentState = STATE_DC_MODE; // AC Mode
		    }
		}
	    }
	if (currentState == STATE_DC_MODE)
	    {
	    HAL_GPIO_TogglePin(GPIOB, GPIO_LED_DCM_Pin);
	    }
	else
	    {
	    HAL_GPIO_TogglePin(GPIOB, GPIO_LED_ACM_Pin);
	    }
	HAL_Delay(500);
	//HAL_IWDG_Refresh(&hiwdg); // Refresh watchdog timer
	}
    }

