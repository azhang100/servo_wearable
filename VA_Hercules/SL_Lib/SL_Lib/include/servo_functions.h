/*
 * servo_functions.h
 *
 *  Created on: Oct 9, 2019
 *      Author: Navid Shaikh-
 */

#ifndef _INCLUDE_SERVO_FUNCTIONS_H_
#define _INCLUDE_SERVO_FUNCTIONS_H_

//#ifdef __cplusplus
//extern "C" {
//#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "sys_common.h"
#include "reg_sci.h"
#include "reg_rti.h"
#include "reg_gio.h"

// Calling eCAP & etPWM related libraries
#include "etpwm.h"
#include "ecap.h"

//-----------------------------------------------------------------------------
/* Define the PID loop related parameters here */
#define EPSILON 0.001
#define MAX 0.9
#define MIN 0.04          //7.1
#define tEGCO2_MAX 50
#define tEGCO2_MIN 10
#define CO2ConcMax 100   // in %
#define CO2ConcMin 0     // in %
#define sweepMax 100     // in %
#define sweepMin 5       // in %
#define MAX_Heater 90
#define MIN_Heater 10
#define duty_flush 80

//-----------------------------------------------------------------------------
/* Define the multiplexers I2C ADDRESSES */
#define TCA1 0x71       /* I2C Multiplexer1 is for flow sensors, Temperature/Humidity Sensors & RTC board */
#define TCA2 0x72       /* I2C Multiplexer2 is for Pressure sensors & Power monitor */

//-----------------------------------------------------------------------------
/* Operating modes for PID controller */
typedef enum {
    CO2_CONTROL = 1, // Servo-regulator operating in CO2 level control mode
    FLOW_CONTROL = 2, // Servo-regulator operating in flow control mode
    O2_CONTROL = 3, // Servo-regulator operating in O2 control mode
    MEASUREMENT = 0
    }servo_mode;

servo_mode MODE;  // Controller mode

//-----------------------------------------------------------------------------
/* Operating status for PID controller */
typedef enum {
    Servo_On = 1, // Servo-regulator operating in CO2 level control mode
    Servo_Off = 0 // Servo-regulator operating in flow control mode
    }servo_status;

servo_status STATUS;  // Controller mode
//-----------------------------------------------------------------------------
/* Sensor Reset sequence */
typedef enum {
    Pressure_Sensor_rst = 5U,    // Inlet Pressure sensor is connected on GIO5 pin
    Flow_Sensor1_rst = 6U,    // Flow sensor 1 is connected on GIO6 pin
    CO2_Sensor_rst = 7U,      // CO2 sensor is connected on GIO7 pin
//    Temp_Sensor1_rst = 0U,    // Temperature Sensor 1 is connected on GIO0 pin
//    Temp_Sensor2_rst = 1U,    // Temperature Sensor 2 is connected on GIO1 pin
    Humidity_Sensor1_rst = 2U, // Humidity Sensor 1 is connected on GIO2 pin
    Ambient_Pressure_Sensor_rst = 0U,  // Barometric Pressure sensor is connected on GIOB0 pin
    I2C_Multiplexer_rst = 1U, // I2C Multiplexers are connected on GIOB1 pin
} sensor_RST;

//----------------------------------------------------------------------------

/* GPIO Sequence */
typedef enum {
    rs485_rx = 7U,
    rs485_tx = 6U,
    sfc5500_fc1 = 0U,
    sfc5500_fc2 = 1U,
    sfc5500_fc3 = 2U,
    vac_pump = 5U,
    alarm_en1 = 0U,
    alarm_en2 = 1U,
    uart_mux_s1 = 2U,
    uart_mux_s2 = 3U,
} gpio_config;

//-----------------------------------------------------------------------------
/* I2C Multiplexer Pin Addresses of Sensors */
typedef enum {
    /* Following Sensors belong to Multiplexer1 */
    Inlet_Flow_Sensor = 0,          //  System Inlet Flow Sensor
//    Outlet_Flow_Sensor = 1,         //  System Outlet Flow Sensor
    Temp_Sensor1 = 2,               //  Blower1 Temperature Sensor
    Temp_Sensor2 = 3,               //  Blower2 Temperature Sensor
    Humidity_Sensor1 = 5,           //  CO2 Sensor housing Humidity Sensor
    Humidity_Sensor2 = 4,//5,           //  Ambient Humidity Sensor
    RTC_Clock = 6,                  //  RTC Clock

    /* Following Sensors belong to Multiplexer2 */
    Inlet_Pressure_Sensor = 0,      //  Lung Inlet Pressure Sensor
    Ambient_Pressure_Sensor = 1,    //  Ambient Pressure Sensor
    Power_Monitor = 4               //  Power Monitor
} mux_address;

//-----------------------------------------------------------------------------
struct publishData_val_pair
{
    char data[20];
    float value;
};

//-----------------------------------------------------------------------------
struct duty_flow_pair
{
    float duty_cycle;
    float flow;
};

//-----------------------------------------------------------------------------
/*    Error codes */
typedef enum {
     NO_ERROR = 0x00, // no error
     ACK_ERROR = 0x01, // no acknowledgment error
     CHECKSUM_ERROR = 0x02 // checksum mismatch error
} etError;

//===========================================================================//
//================Touchscreen communcation related variables */==============//
//===========================================================================//
// Servo-regulator target CO2 values //
struct targetValues{
    float targetCo2Level;
    float targetFlowLevel;
};

struct targetValues currentTarget;

//===========================================================================//
// Serial Communication related data buffers //
uint8 touchscreen_buffer[7];
//===========================================================================//
//Parameter update flags //
struct touchscreenFlags{
    bool co2IsUpdated;
    bool flowIsUpdated;
    bool kpIsdUpdated;
    bool kiIsdUpdated;
    bool kdIsdUpdated;
    bool modeIsUpdated;
    bool dataIsRequested;
};

struct touchscreenFlags currentFlags;

//===========================================================================//
// Moisture removal benchmarking enable signal
uint8 duty_flow_calculation_init;

//===========================================================================//
//Servo-regulator PID gains
struct pidGains{
    float kp;
    float ki;
    float kd;
};

struct pidGains co2PidGains;
struct pidGains newco2PidGains;
struct pidGains flowPidGains;
struct pidGains newflowPidGains;
struct pidGains heaterPidGains;
struct pidGains tEGCO2Gains;
//===========================================================================//
struct tm *Time;
//===========================================================================//

uint8 co2_buffer[10];
uint8 co2_buffer1[10];
uint8 result_deviceInfo[17];
uint8 result_uniqID[26];
uint8 result_o2[21];
uint8 mul_buffer[10];
float multiple;
float EGCO2;             // in mmHg
float EGO2;              // in mmHg
float pre_EGCO2;         // in mmHg
float pCO2;              // in mmHg
uint8_t co2ReceivedFlag;
uint8_t o2ReceivedFlag;

//PID Controller flags
float pwm_pre_error;
float pwm_integral;
float pwm_integralUpdated;
sint8 pwm_saturation;
double pwm_error;
float pwm_derivative;
//===========================================================================//

//typedef struct {
//   unsigned char  "AT";
//   unsigned char  "AT+ADDR";
//}BTCommands;

//==============================================================================
/* This function is used to provide simple delay of no. of Microseconds specified
 *  by the user
 */
//==============================================================================
void michiganTime(void);
//==============================================================================

//==============================================================================
void delay(uint32_t ms);
//==============================================================================

//==============================================================================
void delay_100us(uint32_t ms_fl);
//==============================================================================

//==============================================================================
void Enable_Clk(void);
//==============================================================================

//==============================================================================
float Run_Time_Since_Epoch (void);
//==============================================================================

/* Power Supply control of the Flow Sensor */
//==============================================================================
void SF05_PowerOnFlowSensor(void);
//==============================================================================

/* Power Supply control of the CO2 Sensor */
//==============================================================================
void PowerOnCO2Sensor(void);
//==============================================================================

/* Initialize the CO2 sensor */
//==============================================================================
void CO2_sensor_init(void);
//==============================================================================

/* Initialize the O2 sensor */
//==============================================================================
void O2_sensor_init(void);
//==============================================================================

/* Read the CO2 value */
//==============================================================================
//float get_CO2_val(void);
float get_CO2_val(void);
//==============================================================================

/* Read the O2 value */
//==============================================================================
void get_O2_val(void);
//==============================================================================

/* Calculate the Error value for the flow & CO2 level control */
//==============================================================================
float calculate_PWM(float expectedValue, float actualValue);
//==============================================================================

/* Calculate the tEGCO2 for CO2 control mode */
//==============================================================================
float calculate_tEGCO2(float expectedValue, float actualValue);
//==============================================================================

//==============================================================================
float calculateConcentration (float expectedValue, float actualValue);
//==============================================================================

/* Calculate the Error value for the heat level control */
//==============================================================================
float calculate_Heater_PWM(float expected_val, float actual_val);
//==============================================================================

/* Calculate the Heat level in the CO2 sensor chamber */
//==============================================================================
void humidity_control(float *temp, float *humidity, float *duty);
//==============================================================================

//==============================================================================
void enable_data_receipt(void);
//==============================================================================

//==============================================================================
//uint16 Blower_Speed_Check(gioPORT_t *port, uint32 pwm_pin);
//==============================================================================

//==============================================================================
void Display_CO2_Level (float co2_level);
//==============================================================================

//==============================================================================
void Display_Flow_Level1 (float flow_level);
//==============================================================================

//==============================================================================
void modeIsUpdated(void);
//==============================================================================

//==============================================================================
void Display_Duty_Cycle (float Duty_Cycle);
//==============================================================================

//==============================================================================
void Display_Voltage_Level(float voltage);
//==============================================================================

//==============================================================================
void Display_Current(float current);
//==============================================================================

//==============================================================================
void Display_Power(float power);
//==============================================================================

//==============================================================================
void Display_Energy(float energy);
//==============================================================================

//==============================================================================
void Display_Time(float time);
//==============================================================================

//==============================================================================
void Display_CO2_Sensor_Temp(float CO2_Sensor_Temp);
//==============================================================================

//==============================================================================
void Display_CO2_Sensor_Humi(float CO2_Sensor_Humi);
//==============================================================================

//==============================================================================
void Display_Inlet_Pressure(float Pressure);
//==============================================================================

//==============================================================================
void Display_Ambient_Pressure(float ambPressure);
//==============================================================================

//==============================================================================
void Display_Pressure_Drop(float pressure_drop);
//==============================================================================

//==============================================================================
void Display_Heater_Duty_Cycle(float Heater_Duty_Cycle);
//==============================================================================

//==============================================================================
void Display_PID (float value);
//==============================================================================

//==============================================================================
float get_avg_CO2_val(void);
//==============================================================================

//==============================================================================
//void Data_Display(float time, float Duty_Cycle, float co2_level, float flow_level);
void Data_Display(float co2_level, float flow_level, float ambient_pressure, float pressure_drop);
//==============================================================================

//==============================================================================
//void store_data(struct publishData_val_pair *par, int data_count);
//void store_data(float time, float Duty, float co2_level, float flow_level, float voltage,
//                float current, float power, float energy, float Blower1_Temp, float Blower2_Temp,
//                float CO2_Temp, float CO2_Humi, float Heater_Duty, float Pressure);

//void store_data(float currentTime, float Duty, float co2_level, float flow_level, float voltage,
//                float current, float power, float energy, float target_co2_level, float Ambient_Pressure);

//void store_data(float currentTime, float tEGCO2, float EGCO2, float targetO2Concentration, float sweepFlow,
//                float measuredSweepFlow, float measuredAirFlow, float measuredO2Flow, float target_co2_level, float Ambient_Pressure);

//void store_data(float currentTime, float tEGCO2, float pCO2, float EGCO2, float sweepFlow,
//                float measuredSweepFlow, float Ambient_Pressure);

void store_data(float currentTime, float targetSweep, float tEGO2, float EGO2, float measuredSweepFlow,
                float measuredO2Flow, float Ambient_Pressure);
//==============================================================================

//==============================================================================
void BT_Send_Msg(uint8 BTcmd[], uint8 x);
//==============================================================================

//==============================================================================
void HC06_Bluetooth_init(void);
//==============================================================================

/* Measure Blower Speed */
//==============================================================================
void Measure_Speed(void);
//==============================================================================

/* Configure ETPWM and ECAP */
//==============================================================================
void config_etPWM_eCAP(void);
//==============================================================================

//==============================================================================
etError tcaselect(uint32 mux, mux_address sensor);
//==============================================================================

//==============================================================================
void reset_sensor (sensor_RST sensor);
//==============================================================================

//==============================================================================
void rt_OneStep(void);
//==============================================================================

//==============================================================================
void moisture_removal_init(void);
//==============================================================================

//==============================================================================
void monitor_moisture_level(float duty_level, float flow_level);
//==============================================================================

//==============================================================================
float calculateSweep (float expectedValue, float actualValue);
//==============================================================================

//}
#endif /* _INCLUDE_SERVO_FUNCTIONS_H_ */
