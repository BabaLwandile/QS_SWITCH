/*
 * QS_Switch.c
 *
 *  Created on: Jun 25, 2024
 *      Author: 0163022
 */
#include "fault_handling.h"
#include "main.h"
#include "QS_Switch.h"

extern ADC_HandleTypeDef hadc1;

SystemState currentState = STATE_SAFE_MODE;
#define MANUAL_MODE_DELAY 3000 // 3 seconds to activate manual mode

uint32_t read_current(void)
    {
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
	{
	return HAL_ADC_GetValue(&hadc1);
	}
    return 0;
    }

void RunQSLoop(void)
    {
    ///><Ckecking the Current mode of the QS Switch
    if ((HAL_GPIO_ReadPin(GPIOA, GPIO_DCFB_Pin) == GPIO_PIN_SET)
	    && (HAL_GPIO_ReadPin(GPIOA, GPIO_ACFB_Pin) == GPIO_PIN_RESET))
	{
	currentState = STATE_DC_MODE;
	}
    else if ((HAL_GPIO_ReadPin(GPIOA, GPIO_ACFB_Pin) == GPIO_PIN_SET)
	    && (HAL_GPIO_ReadPin(GPIOA, GPIO_DCFB_Pin) == GPIO_PIN_RESET))
	{
	currentState = STATE_AC_MODE;
	}

    ///><Ckecking for manual mode activation
    if ((HAL_GPIO_ReadPin(GPIOA, GPIO_Manual_Pin)) == GPIO_PIN_SET)
	{
	HAL_Delay(MANUAL_MODE_DELAY);
	if ((HAL_GPIO_ReadPin(GPIOA, GPIO_Manual_Pin)) == GPIO_PIN_SET)
	    {
	    currentState = STATE_MANUAL_MODE;
	    }
	}
    //><Monitoring the command signals
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_ACS_Pin) == GPIO_PIN_SET
	    && HAL_GPIO_ReadPin(GPIOA, GPIO_Enable_Signal_Pin) == GPIO_PIN_SET)
	{
	currentState = STATE_TO_AC;
	}
    else if (HAL_GPIO_ReadPin(GPIOA, GPIO_DCS_Pin) == GPIO_PIN_SET
	    && HAL_GPIO_ReadPin(GPIOA, GPIO_Enable_Signal_Pin) == GPIO_PIN_SET)
	{
	currentState = STATE_TO_DC;
	}

///>< Execute the current mode of the controller
    switch (currentState)
	{
    case STATE_DC_MODE:
	HAL_GPIO_WritePin(GPIOB, GPIO_LED_DCM_Pin, SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_LED_ACM_Pin, RESET);

	break;
    case STATE_AC_MODE:
	HAL_GPIO_WritePin(GPIOB, GPIO_LED_ACM_Pin, SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_LED_DCM_Pin, RESET);
	break;
    case STATE_SAFE_MODE:
	HandleFault(); //><This will handle the fault if the QS Switch didn't receive either DC of AC Feedback
	break;
    case STATE_MANUAL_MODE:
	Enter_Manual_Mode();
	break;
    case STATE_FAULT_MODE:
	HandleFault();
	break;
    case STATE_TO_AC:
	break;
	ControlMotor(1);  //><Clockwise rotation
	HAL_GPIO_WritePin(GPIOB, GPIO_LED_ACM_Pin, SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_LED_DCM_Pin, RESET);
    case STATE_TO_DC:
	break;
	ControlMotor(-1);  //><Anti-Clockwise rotation
	HAL_GPIO_WritePin(GPIOB, GPIO_LED_DCM_Pin, SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_LED_ACM_Pin, RESET);
	}
    }
