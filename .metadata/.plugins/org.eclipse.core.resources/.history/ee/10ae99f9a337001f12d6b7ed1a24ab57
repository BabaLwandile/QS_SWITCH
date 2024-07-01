/*
 * QS_Switch.h
 *
 *  Created on: Jun 25, 2024
 *      Author: 0163022
 */

#ifndef INC_QS_SWITCH_H_
#define INC_QS_SWITCH_H_

// State definitions
typedef enum
    {
    STATE_SAFE_MODE,
    STATE_DC_MODE,
    STATE_AC_MODE,
    STATE_FAULT_MODE,
    STATE_MANUAL_MODE,
    STATE_TO_DC,
    STATE_TO_AC
    } SystemState;

uint32_t read_current(void);
void HandleFault(void);
void Enter_Manual_Mode(void);

#endif /* INC_QS_SWITCH_H_ */

