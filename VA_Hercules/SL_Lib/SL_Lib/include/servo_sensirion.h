/*
 * servo_sensirion.h
 *
 *  Created on: Feb 14, 2021
 *      Author: Navid Shaikh
 */

#ifndef INCLUDE_SERVO_SENSIRION_H_
#define INCLUDE_SERVO_SENSIRION_H_

#include <stdio.h>
#include <string.h>
#include "sys_common.h"
#include "reg_gio.h"
#include "servo_functions.h"


/*-- Defines ------------------------------------------------------------------
 * CRC
 */

#define POLYNOMIAL 0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

//-----------------------------------------------------------------------------
/* Offset and scale factors from data-sheet (SFM3400). */

#define OFFSET_FLOW 32768.0f// offset flow
#define SCALE_FLOW 800.0f // scale factor flow

float Ambient_Pressure;

//-----------------------------------------------------------------------------
typedef enum {

    SF05_Address  = 0x40,        // I2C Address of Flow Sensor SFM3400
    SHTC3_Address = 0x70,        // I2C Address of Temperature/Humidity Sensor SHTC3
    ABP_Address = 0x28,           // I2C Address of Pressure Sensor ABPDRRT005PG2D3
    ABP2_Address = 0x28

}et_address;


//-- Enumerations -------------------------------------------------------------
/*  I2C Commands for flow sensor & the Humidity/Temperature Sensor */

typedef enum {

     /* Flow Sensor Commands */
     FLOW_MEASUREMENT        = 0x1000, // command: flow measurement
     READ_SERIAL_NUMBER_HIGH = 0x31AE, // command: read serial number (bit 31:16)
     READ_SERIAL_NUMBER_LOW  = 0x31AF, // command: read serial number (bit 15:0)
     SOFT_RESET              = 0X2000, // command: soft reset
     READ_SCALE_FACTOR       = 0x30DE, // command: read scale factor
     READ_OFFSET             = 0x30DF, // command: read offset value

     /* Temperature/Humidity Sensor Commands */
     READ_ID                 = 0xEFC8, // command: read ID register
     SOFT_RESET_TH           = 0x805D, // soft reset
     SLEEP                   = 0xB098, // sleep
     WAKEUP                  = 0x3517, // wakeup
     MEAS_T_RH_POLLING       = 0x7866, // meas. read T first, clock stretching disabled
     MEAS_T_RH_CLOCKSTR      = 0x7CA2, // meas. read T first, clock stretching enabled
     MEAS_RH_T_POLLING       = 0x58E0, // meas. read RH first, clock stretching disabled
     MEAS_RH_T_CLOCKSTR      = 0x5C24  // meas. read RH first, clock stretching enabled

} etCommands;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/* Writes command to the sensor. */
//==============================================================================
 etError SF05_WriteCommand(etCommands cmd, et_address address );
//==============================================================================

/* Reads command results from sensor. */
//==============================================================================
 etError SF05_ReadCommandResult(uint8 *result, et_address address);
//==============================================================================


//==============================================================================
 etError SF05_GetFlow(float *current_flow);
//==============================================================================


//==============================================================================
float get_avg_flow_val(mux_address flow_sensor);
//==============================================================================

/* Gets the temperature [°C] and the humidity [%RH]. */
//==============================================================================
etError SHTC3_GetTempAndHumi(mux_address temp_sensor, float *temp, float *humidity);
//==============================================================================


//==============================================================================
static float SHTC3_CalcTemperature(uint16_t rawValue);
//==============================================================================


//==============================================================================
static float SHTC3_CalcHumidity(uint16_t rawValue);
//==============================================================================


//==============================================================================
void SHTC3_Sleep(void);
//==============================================================================


//==============================================================================
void SHTC3_Wakeup(void);
//==============================================================================


//==============================================================================
void SHTC3_SoftReset(void);
//==============================================================================


//==============================================================================
etError ABP_getPressure(mux_address pressure_sensor, float *pressure);
//==============================================================================


//==============================================================================
etError ABP2_getPressure(mux_address pressure_sensor, float *pressure);
//==============================================================================


//==============================================================================
etError ABP2_WriteCommand(et_address address);
//==============================================================================


//==============================================================================
etError ABP2_ReadCommandResult(uint8 *result, et_address address);
//==============================================================================
#endif /* INCLUDE_SERVO_SENSIRION_H_ */
