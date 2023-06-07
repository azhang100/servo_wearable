/*
 * sfc5500_commands.h
 *
 *  Created on: Mar 5, 2022
 *      Author: aakash
 */

#ifndef INCLUDE_SFC5500_COMMANDS_H_
#define INCLUDE_SFC5500_COMMANDS_H_

#include <stdio.h>
#include <stdlib.h>
#include "sci.h"
#include "reg_sci.h"
#include "sfc5500_data.h"

extern uint8_t dataReceivedFlag;

//=============================================================================

error_type Shdlc_GetDeviceInfo (gasSource childAddr, char* info_string, uint8_t* num_characters,
                               deviceInfoType infoType, uint8_t* stateByte);
error_type SFC5500_ReadMeasuredValue (gasSource childAddr, scaling_type scaling,
                                      float* measuredValue, uint8_t* stateByte);
error_type SFC5500_SetSetpoint (gasSource childAddr, scaling_type scaling,
                                float setPoint, uint8_t* stateByte);
error_type SFC5500_GetSetPoint (gasSource childAddr, scaling_type scaling,
                                float* currentSetPoint, uint8_t* stateByte);
error_type dataReceive(uint8_t* rFrame, uint8_t* dataByteNum) ;

error_type dataTransmit(uint8_t* tFrame, uint8_t tFrameLength);

error_type setpointUpdate(float targetConcentration, float targetFlow);

error_type calculateSetPoint(float targetO2Concentration, float * targetSweepFlow);

error_type measureCurrentFlows(float* flow1, float* flow2, float* flow3);

#endif /* INCLUDE_SFC5500_COMMANDS_H_ */
