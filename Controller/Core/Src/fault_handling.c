/*
 * fault_handling.c
 *
 *  Created on: Jun 24, 2024
 *      Author: 0163022
 */

#include "fault_handling.h"
#include "motor_control.h"
#include "main.h"
#include "QS_Switch.h"

#define STALL_CURRENT 3.0 // 3A
#define TIMEOUT 5000 // 5 seconds
#define COOLDOWN_DURATION_MS 120000 //2 minutes cool down time
uint32_t adcValue;
float current;

FaultState FAULT(void)
    {
    adcValue = read_current();
    current = (adcValue / 4095.0) * 3.3; // Convert ADC value to current

    uint32_t startTime = HAL_GetTick();
    ///>< Checking the feedback from the micro switches on the QS switch
    if ((GPIO_ACFB_Pin == GPIO_PIN_RESET) && (GPIO_DCFB_Pin == GPIO_PIN_RESET))
	{
	return STATE_INBETWEEN;
	}
    ///>< Checking for stalling current
    else if (current >= STALL_CURRENT)
	{
	return STATE_STALL;
	}
    ///><Cheking for time out
    else if ((HAL_GetTick() - startTime) > TIMEOUT)
	{
	return STATE_TIMEOUT;
	}
    ///><Checking for mismatch
    else if ((GPIO_DCS_Pin == GPIO_PIN_SET && GPIO_ACFB_Pin == GPIO_PIN_SET)
	    || (GPIO_ACS_Pin == GPIO_PIN_SET && GPIO_DCFB_Pin == GPIO_PIN_SET))
	{
	return STATE_MISMATCH;

	}
    ///>< If it receive the signal/comand but the enable signal is not active, should return the fault
    else if ((GPIO_DCS_Pin == GPIO_PIN_SET
	    && GPIO_Enable_Signal_Pin == GPIO_PIN_RESET)
	    || (GPIO_ACS_Pin == GPIO_PIN_SET
		    && GPIO_Enable_Signal_Pin == GPIO_PIN_RESET))
	{
	return STATE_ES_NOT_ACTIVE;
	}
    return 0;
    }

void CoolDown(void)
    {
    uint32_t Time = HAL_GetTick();
    while ((HAL_GetTick() - Time) < COOLDOWN_DURATION_MS)
	{
	}
    }

void HandleFault(void)
    {

    uint8_t faultType = FAULT();

    switch (faultType)
	{
    case STATE_INBETWEEN:
	HAL_GPIO_WritePin(GPIOB, GPIO_LED_FAULT_Pin, GPIO_PIN_SET); ///>< Light the RED LED to indicate the fault
	///>< Move to safe mode
	ControlMotor(1);
	HAL_GPIO_WritePin(GPIOB, GPIO_LED_ACM_Pin, GPIO_PIN_SET); //Light the feedback lED
	HAL_GPIO_WritePin(GPIOB, GPIO_LED_FAULT_Pin, GPIO_PIN_RESET); ///>< Turn OFF the fault
	break;
    case STATE_TIMEOUT:
	HAL_GPIO_TogglePin(GPIOA, GPIO_LED_FAULT_Pin);
	HAL_Delay(500);
	// Stop the motor
	HAL_GPIO_WritePin(GPIOB, GPIO_Forward_Pin | GPIO_Reverse_Pin,
		GPIO_PIN_RESET);
	CoolDown();
	break;
    case STATE_STALL:
	HAL_GPIO_TogglePin(GPIOA, GPIO_LED_FAULT_Pin);
	HAL_Delay(500);
	// Stop the motor
	HAL_GPIO_WritePin(GPIOB, GPIO_Forward_Pin | GPIO_Reverse_Pin,
		GPIO_PIN_RESET);
	CoolDown();
	break;
    case STATE_MISMATCH:
	HAL_GPIO_WritePin(GPIOA, GPIO_LED_FAULT_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_LED_ACM_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_LED_DCM_Pin, GPIO_PIN_SET);
	CoolDown();
	HAL_GPIO_WritePin(GPIOA, GPIO_LED_FAULT_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_LED_ACM_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_LED_DCM_Pin, GPIO_PIN_RESET);
	break;
    case STATE_ES_NOT_ACTIVE:
	HAL_GPIO_WritePin(GPIOA, GPIO_LED_FAULT_Pin, GPIO_PIN_SET);
	HAL_GPIO_TogglePin(GPIOA, GPIO_LED_EN_Pin);
	HAL_Delay(500);
	// Stop the motor
	HAL_GPIO_WritePin(GPIOB, GPIO_Forward_Pin | GPIO_Reverse_Pin,
		GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_LED_FAULT_Pin, GPIO_PIN_RESET);
	break;
	}
    // After fault, switch to safe mode (AC Mode). 1 -> Clockwise
    ControlMotor(1);

    }

