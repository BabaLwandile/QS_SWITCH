/*
 * fault_handling.h
 *
 *  Created on: Jun 24, 2024
 *      Author: 0163022
 */

#ifndef INC_FAULT_HANDLING_H_
#define INC_FAULT_HANDLING_H_

#include "stm32f1xx_hal.h"
#include "main.h"

typedef enum
    {
    STATE_STALL,
    STATE_TIMEOUT,
    STATE_MISMATCH,
    STATE_INBETWEEN,
    STATE_ES_NOT_ACTIVE
    } FaultState;

FaultState FAULT(void);   ///>< It must return the fault state
void ControlMotor(int);

#endif /* INC_FAULT_HANDLING_H_ */
