/** @file servo_functions.c
*   @brief servo regulator Implementation functions File
*
*   Created on: Oct 9, 2019
*   Author: Navid Shaikh
*
*/
#include <stdio.h>
#include <time.h>
#include "servo_functions.h"
#include "servo_sensirion.h"
#include "servo_ventilation.h"
#include "i2c.h"
#include "gio.h"
#include "sci.h"
#include "rti.h"
#include "system.h"
#include "het.h"
#include "SmartLungGasSensing.h"

// Calling eCAP & etPWM related libraries
#include "etpwm.h"
#include "ecap.h"

// Power Monitor related functions
//#include "ina233.h"

// PID_Autotuner related functions
//#include "Simulink_PID_Autotuner.h"
//#include "Simulink_PID_Autotuner.c"

//=============================================================================

 /* CO2 Sensor related Commands
 */
    // check out manual for information about the commands: http://www.co2meters.com/Documentation/Manuals/Manual-GSS-Sensors.pdf
    // note: second parameter of sciSend is subtracted by 1 so it won't send the null byte '\0' as part of the command

    uint8 mode[] = "K 2\r\n"; // streaming mode:1,  polling mode:2
    uint8 output[] = "M 4\r\n"; // show digitally filtered CO2 measurements
    uint8 read[] = "Z\r\n"; // report latest CO2 measurement (in ppm/10)
    uint8 read_filter[] = "a\r\n"; // "A 16\r\n" recommended setting for 0.5LPM flow rate
    uint8 set_filter[] = "A 16\r\n";
    uint8 calibration[] = "X 3333\r\n";
    uint8 multiplier[] = ".\r\n";
    uint8 autoCalibration[] = "@ \r\n";
    uint8 compensation[] = "s\r\n"; // read the pressure compensation value

//=============================================================================

    /* O2 Sensor related Commands
     */

    char deviceInfo[8] = "#VERS \r\n"; // Collect device information
    char uniqID[8] = "#IDNR \r\n"; // Read unique ID number
    char measure_oxygen[8] = "#MOXY \r\n"; // Measure Oxygen and return the results
    char Baud_rate[12] = "#BAUD9600 \r\n"; // Set baud rate

//=============================================================================

//======================== Global Variables==================================//

uint16 currentCommand = 0x0000;
uint8 RTI_TIMEOUT;
//static uint8 msec = 0;
//static uint8 sec = 0;
//static uint8 minute = 0;
static float time_elapsed = 0;

extern uint8_t dataReceiveFlag;


//=============================================================================
uint8 Data_Display_Disabled;
//=============================================================================
struct duty_flow_pair *ref_values;
//=============================================================================
float ref_duty_cycle[10] = {10, 20, 30, 40, 50, 60, 70, 80 };
float ref_flow[10] = {0.8, 1.9, 3.3, 4.75, 6.5, 8.25, 10.5, 13.25};
float flow_moisture_flush = 13.25;
//=============================================================================

//extern float current_co2_level1;

//void michiganTime(void)
//{
//    time_t rawTime;
//
//    time(&rawTime);
//    currentTime = localtime( &rawTime);
//    currentTime->tm_hour += 2;
//    printf("Current local time and date: %s", asctime(currentTime));
//}

//=============================================================================

//void delay(uint32_t ms)
//{
//    uint32_t time;
//
////    rtiDisableNotification(rtiREG1,rtiNOTIFICATION_COMPARE1);
//
//    for (time = 0; time < ms; ++time)
//    {
//        /* Enable RTI Compare 0 interrupt notification */
//        rtiEnableNotification(rtiREG1,rtiNOTIFICATION_COMPARE0);
//        rtiStartCounter(rtiREG1, rtiCOUNTER_BLOCK0);
//        while(RTI_TIMEOUT == 0);
//        RTI_TIMEOUT = 0;
//        rtiStopCounter(rtiREG1, rtiCOUNTER_BLOCK0);
//        rtiDisableNotification(rtiREG1,rtiNOTIFICATION_COMPARE0);
//    }
//
////    rtiEnableNotification(rtiREG1,rtiNOTIFICATION_COMPARE1);
//
//}

//=============================================================================

//void delay_100us(uint32 ms_fl)
//{
//    uint32 time;
////    uint32 ms;
//
////    rtiDisableNotification(rtiREG1,rtiNOTIFICATION_COMPARE1);
//
////    ms = (uint8_t ) ms_fl;
//
//    for (time = 0; time < ms_fl; ++time)
//    {
//        /* Enable RTI Compare 0 interrupt notification */
//        rtiEnableNotification(rtiREG1,rtiNOTIFICATION_COMPARE1);
//        rtiStartCounter(rtiREG1, rtiCOUNTER_BLOCK0);
//        while(RTI_TIMEOUT == 0);
//        RTI_TIMEOUT = 0;
//        rtiStopCounter(rtiREG1, rtiCOUNTER_BLOCK0);
//        rtiDisableNotification(rtiREG1,rtiNOTIFICATION_COMPARE1);
//    }
//
////    rtiEnableNotification(rtiREG1,rtiNOTIFICATION_COMPARE1);
//
//}
//=============================================================================
//void Enable_Clk(void)
//{
//    /* Enable RTI Compare 1 interrupt notification */
//    rtiEnableNotification(rtiREG1,rtiNOTIFICATION_COMPARE1);
//    rtiStartCounter(rtiREG1, rtiCOUNTER_BLOCK1);
//}

//=============================================================================
float Run_Time_Since_Epoch(void)
{
   return time_elapsed;
}

//=============================================================================
void SF05_PowerOnFlowSensor(void)
{
    // Set GIOA[5] as an output port while set the GIOA[2] as an input
    gioPORTA->DIR = 0xFFFFFF20;

    //Turn on the power for GIOA[5]
    gioSetBit(gioPORTA, 5U, 1U);
}

//=============================================================================
void CO2_sensor_init(void)
{

//   uint8 co2_buffer[10];

//    // set operating mode for sensor

//    co2ReceivedFlag = 1;

    dataReceiveFlag = 1;

    sciSend(sciREG, sizeof(mode), (uint8*) mode);
    sciReceive(sciREG, 10,  &co2_buffer[0]);

   while (dataReceiveFlag != 0);
//   delay(100);

//    while (co2ReceivedFlag != 0);

//    // set digital filter setting
//    sciSend(sciREG3, sizeof(set_filter) - 1, (uint8*) set_filter);
//    sciReceive(sciREG3, 9,  (uint8*) co2_buffer);
//   delay(100);
//
//    // set output mode for sensor
//    sciSend(sciREG3, sizeof(output) - 1, (uint8*) output);
//    sciReceive(sciREG3, 7, (uint8*) co2_buffer);
//    delay(100);

 //  co2ReceivedFlag = 1;

//       dataReceiveFlag = 1;
//
//    // Calibrate the CO2 sensor
//       sciReceive(sciREG, 10, (uint8*) co2_buffer);
//       sciSend(sciREG, sizeof(calibration), (uint8*) calibration);

//       while (dataReceiveFlag != 0);

//    while (co2ReceivedFlag != 0);

//    delay(100);

//    sciSend(sciREG3, sizeof(multiplier), multiplier);
////    //Keep this delay at minimum 40ms or the CO2 sensor will give wrong readings intermittently.
//    delay(50);
////    // read the co2 value
//    sciReceive(sciREG3, sizeof(mul_buffer), mul_buffer);
//    //Keep this delay at minimum 40ms or the CO2 sensor will give wrong readings intermittently.

//    sciSend(sciREG3, sizeof(autoCalibration), (uint8*) autoCalibration);
//    sciReceive(sciREG3, 6, (uint8*) &co2_buffer[0]);

//    co2ReceivedFlag = 1;
   dataReceiveFlag = 1;

   sciSend(sciREG, sizeof(multiplier), (uint8*)multiplier);
   // Keep this delay at minimum 40ms or the CO2 sensor will give wrong readings intermittently.

   // read the co2 value
   sciReceive(sciREG, sizeof(mul_buffer),&mul_buffer[0]);

   while (dataReceiveFlag != 0);
//   delay(50);

}

//=============================================================================

void O2_sensor_init(void)
{
    o2ReceivedFlag = 1;

    sciReceive(sciREG, sizeof(result_deviceInfo), &result_deviceInfo[0]);

    sciSend(sciREG, sizeof(deviceInfo), (uint8*)deviceInfo);

    while (o2ReceivedFlag != 0);

    o2ReceivedFlag = 1;

    // read the o2 value
    sciReceive(sciREG, sizeof(result_uniqID), &result_uniqID[0]);

    sciSend(sciREG, sizeof(uniqID), (uint8*)uniqID);

    while (o2ReceivedFlag != 0);

}

//=============================================================================
float get_avg_CO2_val(void)
{
   uint8 i;
   float current_co2_level = 0;
   float avg_co2_level = 0;

   for (i=0; i<4; i++)
   {
//       current_co2_level = get_CO2_val();
       get_CO2_val();
       avg_co2_level += current_co2_level;
//       delay(50);
   }
   avg_co2_level /= 4;

   return avg_co2_level;
}

//=============================================================================
//float get_CO2_val(void)
float get_CO2_val(void)
{
    uint8 co2_buffer1[10] = {0,0,0,0,0,0,0,0,0,0};
    uint8 mul_buffer[10] = {0,0,0,0,0,0,0,0};
    char co2_value[5];
    char mul_value[5];
    float multiple = 0;
    float current_co2_level = 0;
//    uint8 random = 0;


//    co2ReceivedFlag = 1;
    dataReceiveFlag = 1;

    // request co2 value
    sciSend(sciREG, sizeof(read), (uint8*)read);

    // read the co2 value
    sciReceive(sciREG, sizeof(co2_buffer1), &co2_buffer1[0]);
    //Keep this delay at minimum 40ms or the CO2 sensor will give wrong readings intermittently.

//    while (co2ReceivedFlag != 0);

    while (dataReceiveFlag != 0);

//    dataReceiveFlag = 1;
//
//    // request co2 value
//        sciSend(sciREG, sizeof(compensation), (uint8*)compensation);
//
//        // read the co2 value
//        sciReceive(sciREG, sizeof(co2_buffer1), &co2_buffer1[0]);
//
//        while (dataReceiveFlag != 0);
//
//        printf("compensation is %10s\n", co2_buffer1);
//    delay(100);

    dataReceiveFlag = 1;
    // read the co2 value
    sciSend(sciREG, sizeof(multiplier),(uint8*) multiplier);
    sciReceive(sciREG, sizeof(mul_buffer), &mul_buffer[0]);

    while (dataReceiveFlag != 0);
    // Keep this delay at minimum 40ms or the CO2 sensor will give wrong readings intermittently.

//
//    delay(100);
    // Keep this delay at minimum 40ms or the CO2 sensor will give wrong readings intermittently.

    // filter message to show just numerical co2 value
    sscanf((char *)co2_buffer1+3, "%5s", co2_value);
    current_co2_level = atof(co2_value);
    sscanf((char *)mul_buffer+3, "%5s", mul_value);
    multiple = atof(mul_value);
    current_co2_level *= 0.00076;
    current_co2_level *= multiple;

    printf("[egco2=%f]\n", current_co2_level);

//    rtU.current_co2_level = current_co2_level;

//    random = rand ();
//    current_co2_level = random *0.5;
    return current_co2_level;
}

//=============================================================================

//float get_CO2_val(void)
void get_O2_val(void)
{
    o2ReceivedFlag = 1;

    // read the co2 value
    sciReceive(sciREG, sizeof(result_o2), &result_o2[0]);

    // request co2 value
    sciSend(sciREG, sizeof(measure_oxygen), (uint8*)measure_oxygen);

    while (o2ReceivedFlag != 0);
}

//=============================================================================
float calculate_PWM(float expectedValue, float actualValue)
{
//    static float pre_error = 0;
//    static float integral = 0;
//    static float integralUpdated = 0;
//    static sint8 saturation = 0;
//    double error;
//    float derivative;
    float Duty_Cycle;
    //actualValue = current_co2_level;
    // Calculate the error value
//    if (currentFlags.co2IsUpdated == true)
//    {
//        if (MODE == CO2_CONTROL)
//        {
//            expectedValue = currentTarget.targetCo2Level;
//            currentFlags.co2IsUpdated = false;
//        }
//
//        else
//        {
//            currentFlags.co2IsUpdated = false;
//        }
//
//    }
//
//    else if (currentFlags.flowIsUpdated == true)
//    {
//        if (MODE == FLOW_CONTROL)
//        {
//            expectedValue = currentTarget.targetFlowLevel;
//            currentFlags.flowIsUpdated = false;
//        }
//
//        else
//        {
//            currentFlags.flowIsUpdated = false;
//        }
//
//    }

//    if (MODE == CO2_CONTROL)
    {
        pwm_error = actualValue- expectedValue;
    }

//    else
        if (MODE == FLOW_CONTROL)
    {
        pwm_error = expectedValue- actualValue;
    }


    if (((pwm_error > 0) && (pwm_saturation == 1))||((pwm_error < 0) && (pwm_saturation == -1)))
        {
        pwm_integralUpdated = 0;
        }
    else
    {
        if (fabs(pwm_error) > EPSILON)
         {
            pwm_integralUpdated = pwm_integral + pwm_error;
            pwm_integral = pwm_integralUpdated;
         }
    }

    pwm_derivative = pwm_error - pwm_pre_error;

    if (currentFlags.kpIsdUpdated == true)
    {
        if (MODE == CO2_CONTROL)
        {
            co2PidGains.kp = newco2PidGains.kp;
            Display_PID (newco2PidGains.kp);
        }
        else
        {
            flowPidGains.kp = newflowPidGains.kp;
            Display_PID (newflowPidGains.kp);
        }

       currentFlags.kpIsdUpdated = false;
    }

    else if (currentFlags.kiIsdUpdated == true)
    {
        if (MODE == CO2_CONTROL)
        {
            co2PidGains.ki = newco2PidGains.ki;
            Display_PID (newco2PidGains.ki);
        }
        else
        {
            flowPidGains.ki = newflowPidGains.ki;
            Display_PID (newflowPidGains.ki);
        }

        currentFlags.kiIsdUpdated = false;
    }

    else if (currentFlags.kdIsdUpdated == true)
    {
        if (MODE == CO2_CONTROL)
        {
            co2PidGains.kd = newco2PidGains.kd;
            Display_PID (newco2PidGains.kd);
        }
        else
        {
            flowPidGains.kd = newflowPidGains.kd;
            Display_PID (newflowPidGains.kd);
        }
        currentFlags.kdIsdUpdated = false;
    }

//    if (MODE == CO2_CONTROL)
//    {
        Duty_Cycle = co2PidGains.kp*pwm_error + co2PidGains.ki*pwm_integralUpdated + co2PidGains.kd*pwm_derivative;
        Duty_Cycle /= 10;
//    }

//    else
//    {
//        Duty_Cycle = flowPidGains.kp*pwm_error + flowPidGains.ki*pwm_integralUpdated + flowPidGains.kd*pwm_derivative;
//    }

    // Duty_Cycle limiter
    if(Duty_Cycle > MAX) {
        pwm_saturation = 1;
        Duty_Cycle = MAX;
    }

    else if (Duty_Cycle < MIN) {
        pwm_saturation = -1;
        Duty_Cycle = MIN;
    }

    else
    {
        pwm_saturation = 0;
    }
    pwm_pre_error = pwm_error;

//    rtU.Duty_Cycle = Duty_Cycle;

//    if (rtU.Enable == 0) {
//        rtU.Enable = 1;
//    }
    //return 1;
    return Duty_Cycle;
}

//=============================================================================

float calculate_tEGCO2(float expectedValue, float actualValue)
{
    static float tEGCO2_pre_error = 0;
    static float tEGCO2_integral = 0;
    static float tEGCO2_integralUpdated = 0;
    static sint8 tEGCO2_saturation = 0;
    double tEGCO2_error;
    float tEGCO2_derivative;
    float tEGCO2;

    if (MODE == MEASUREMENT)
    {
        tEGCO2_error = expectedValue- actualValue;
    }

    if (((tEGCO2_error > 0) && (tEGCO2_saturation == 1))||((tEGCO2_error < 0) && (tEGCO2_saturation == -1)))
        {
        tEGCO2_integralUpdated = 0;
        }
    else
    {
        if (fabs(tEGCO2_error) > EPSILON)
         {
            tEGCO2_integralUpdated = tEGCO2_integral + tEGCO2_error;
            tEGCO2_integral = tEGCO2_integralUpdated;
         }
    }

    tEGCO2_derivative = tEGCO2_error - tEGCO2_pre_error;

    tEGCO2 = tEGCO2Gains.kp*tEGCO2_error + tEGCO2Gains.ki*tEGCO2_integralUpdated + tEGCO2Gains.kd*tEGCO2_derivative;

    // tEGCO2 limiter
    if(tEGCO2 > tEGCO2_MAX) {
        tEGCO2_saturation = 1;
        tEGCO2 = tEGCO2_MAX;
    }

    else if (tEGCO2 < tEGCO2_MIN) {
        tEGCO2_saturation = -1;
        tEGCO2 = tEGCO2_MIN;
    }

    else
    {
        tEGCO2_saturation = 0;
    }
    tEGCO2_pre_error = tEGCO2_error;

    return tEGCO2;
}

//=============================================================================
float calculateConcentration (float expectedValue, float actualValue)
{
    static float pre_error = 0;
    static float integral = 0;
    static float integralUpdated = 0;
    static sint8 saturation = 0;
    double error;
    float derivative;
    float concentration;

    // Calculate the error value
//    error = actualValue- expectedValue;

    error = expectedValue- actualValue;


    if (((error > 0) && (saturation == 1))||((error < 0) && (saturation == -1)))
       {
            integralUpdated = 0;
       }
    else
    {
        if (fabs(error) > EPSILON)
         {
            integralUpdated = integral + error;
            integral = integralUpdated;
         }
    }

    derivative = error - pre_error;

    concentration = co2PidGains.kp*error + co2PidGains.ki*integralUpdated + co2PidGains.kd*derivative;

    // CO2 Concentration limiter
    if(concentration > CO2ConcMax) {
        saturation = 1;
        concentration = CO2ConcMax;
    }

    else if (concentration < CO2ConcMin) {
        saturation = -1;
        concentration = CO2ConcMin;
    }

    else
    {
        saturation = 0;
    }
    pre_error = error;

    return concentration;
}

//=============================================================================
float calculateSweep (float expectedValue, float actualValue)
{
    static float pre_error = 0;
    static float integral = 0;
    static float integralUpdated = 0;
    static sint8 saturation = 0;
    double error;
    float derivative;
    float sweepFlow;

    // Calculate the error value
    error = actualValue- expectedValue;

    if (((error > 0) && (saturation == 1))||((error < 0) && (saturation == -1)))
       {
            integralUpdated = 0;
       }
    else
    {
        if (fabs(error) > EPSILON)
         {
            integralUpdated = integral + error;
            integral = integralUpdated;
         }
    }

    derivative = error - pre_error;

    sweepFlow = co2PidGains.kp*error + co2PidGains.ki*integralUpdated + co2PidGains.kd*derivative;

    // Sweep Flow rate limiter
    if(sweepFlow > sweepMax) {
        saturation = 1;
        sweepFlow = sweepMax;
    }

    else if (sweepFlow < sweepMin) {
        saturation = -1;
        sweepFlow = sweepMin;
    }

    else
    {
        saturation = 0;
    }
    pre_error = error;

    return sweepFlow;
}
//=============================================================================
float calculate_Heater_PWM(float expected_val, float actual_val)
{
    static float pre_error;
    static float integral;
    double error;
    float derivative;
    float Duty_Cycle;

    error = expected_val - actual_val;

    if (error > EPSILON)
    {
        integral = integral + error;
    }
    derivative = error - pre_error;

    Duty_Cycle = heaterPidGains.kp*error + heaterPidGains.ki*integral + heaterPidGains.kd*derivative;

    // Duty_Cycle limiter
    if(Duty_Cycle > MAX_Heater) {
        Duty_Cycle = MAX_Heater;
    }

    else if (Duty_Cycle < MIN_Heater) {
        Duty_Cycle = MIN_Heater;
    }
    pre_error = error;

    return Duty_Cycle;
}

//=============================================================================
void humidity_control(float *temp, float *humidity, float *duty)
{
    float Temp = 0;
    float Humidity = 0;
    //float Target_Humidity = 45;
    float targetTemp = 39;
    float Heater_Duty = 30;

    etError error = NO_ERROR;

    error = SHTC3_GetTempAndHumi(Humidity_Sensor1, &Temp, &Humidity);
    if (error == ACK_ERROR)
    {
        printf("Humidity_Sensor comm error has occurred.\n");
//        i2cInit();
//        delay(100);
    }

    if (error == NO_ERROR)
    {
        //Heater_Duty = calculate_Heater_PWM(Target_Humidity, Humidity);
        Heater_Duty = calculate_Heater_PWM(targetTemp, Temp);
        pwmSetDuty(hetRAM1, pwm2, Heater_Duty);
    }

    *temp = Temp;
    *humidity = Humidity;
    *duty = Heater_Duty;
}

//=============================================================================
//void rtiNotification(rtiBASE_t *rtiREG, uint32 notification)
//{
//    if (notification == rtiNOTIFICATION_COMPARE0)
//    {
//        RTI_TIMEOUT = 1;
//    }
//
//    else if (notification == rtiNOTIFICATION_COMPARE1)
//    {
//        msec += 1;
//        time_elapsed += 0.01;
//
//        if (msec == 100)
//            {
//                sec += 1;
//                msec = 0;
//            }
//
//        if (sec == 60)
//            {
//                minute += 1;
//                sec = 0;
//            }
//
//        if (rtU.Enable == 1)
//        {
//            rt_OneStep();
//        }
//    }
////    switch (notification)
////    {
////        case rtiNOTIFICATION_COMPARE0:
////
////                RTI_TIMEOUT = 1;
////
////        break;
////
////        case rtiNOTIFICATION_COMPARE1:
////
////                msec += 1;
////                time_elapsed += 0.01;
////
////                if (msec == 100)
////                    {
////                        sec += 1;
////                        msec = 0;
////                    }
////
////                if (sec == 60)
////                    {
////                        minute += 1;
////                        sec = 0;
////                    }
////
////                if (rtU.Enable == 1)
////                {
////                    rt_OneStep();
////                }
////
////         break;
////    }
//}
//=============================================================================

//void enable_data_receipt()
//{
//    sciReceive(sciREG1, sizeof(touchscreen_buffer), &touchscreen_buffer[0]);
//    currentFlags.dataIsRequested = true;
//}

//=============================================================================
//uint16 Blower_Speed_Check(gioPORT_t *port, uint32 pwm_pin)
//{
//    uint8 counter = 0;
//    gioEnableNotification(port,pwm_pin);
//    rtiStartCounter(rtiREG1,rtiCOUNTER_BLOCK0);
//    while(RTI_TIMEOUT==0);
//    RTI_TIMEOUT=0;
//    gioDisableNotification(port,pwm_pin);
//    rtiStopCounter(rtiREG1,rtiCOUNTER_BLOCK0);
//    return (counter * 60);
//}

//=============================================================================
//void Data_Display(float time, float Duty_Cycle, float co2_level, float flow_level, float ambient_pressure, float pressure_drop)
void Data_Display(float co2_level, float flow_level, float ambient_pressure, float pressure_drop)
{
    if (!(Data_Display_Disabled))
        {
            Display_CO2_Level (co2_level);
//            delay(10);
            Display_Flow_Level1 (flow_level);
//            delay(10);
            Display_Ambient_Pressure(ambient_pressure);
//            delay(10);
//            Display_Pressure_Drop(pressure_drop);
//            delay(10);
        }
}

//=============================================================================
void Display_CO2_Level(float co2_level)
{
    char msg3[16] = {};
    char CO2[5] = {0,0,0,0,0};
    sprintf(msg3,"co2_level=");
//    sprintf(CO2, "%.2f", co2_level);
    if (co2_level < 10.00)
    {
        sprintf(CO2, "%.3f", co2_level);
    }
    else if (co2_level < 100.00)
    {
        sprintf(CO2, "%.2f", co2_level);
    }
    else
    {
        sprintf(CO2, "%.f", co2_level);
    }

    strcat(msg3, CO2);
    strcat(msg3,"\r");
    while (((sciREG->FLR & 0x4) == 4)); /* wait until busy */
    sciSend(sciREG, sizeof(msg3), (uint8*)msg3);
//    delay(20);

}
//=============================================================================
void Display_Flow_Level1(float flow_level)
{
    char msg1[17] = {};
    char flow_rate1[5] = {0,0,0,0,0};
    sprintf(msg1,"flow_level=");
//    sprintf(flow_rate1, "%.2f", flow_level);
    if (flow_level < 10.00)
    {
        sprintf(flow_rate1, "%.3f", flow_level);
    }
    else
    {
        sprintf(flow_rate1, "%.2f", flow_level);
    }

    strcat(msg1, flow_rate1);
    strcat(msg1,"\r");
    while (((sciREG->FLR & 0x4) == 4)); /* wait until busy */
    sciSend(sciREG, sizeof(msg1), (uint8*)msg1);
//    delay(20);
}

//============================================================================

void Display_Ambient_Pressure(float ambPressure)
{
    char msg2[16] = {};
    char pressure[5] = {0,0,0,0,0};
//    ambPressure = 730;
    sprintf(msg2,"amb_press=");
    if (ambPressure < 10.00)
    {
        sprintf(pressure, "%.3f", ambPressure);
    }
    else if (ambPressure < 100.00)
    {
        sprintf(pressure, "%.2f", ambPressure);
    }
    else
    {
        sprintf(pressure, "%.1f", ambPressure);
    }
    strcat(msg2, pressure);
    strcat(msg2,"\r");
    while (((sciREG->FLR & 0x4) == 4)); /* wait until busy */
    sciSend(sciREG, sizeof(msg2), (uint8*) msg2);
//    delay(20);
}
//=============================================================================

void Display_Pressure_Drop(float pressure_drop)
{
    char msg4[17] = {};
    char pressure[5] = {0,0,0,0,0};
//    pressure_drop = rand ();
//    pressure_drop *= 0.01;
    sprintf(msg4,"press_drop=");
    if (pressure_drop < 10.00)
    {
        sprintf(pressure, "%.3f", pressure_drop);
    }
    else
    {
        sprintf(pressure, "%.2f", pressure_drop);
    }
    strcat(msg4, pressure);
    strcat(msg4,"\r");
    while (((sciREG->FLR & 0x4) == 4)); /* wait until busy */
    sciSend(sciREG, sizeof(msg4), (uint8*) msg4);
//    delay(20);
}

//=============================================================================
void Display_PID (float value)
{
    char msg[10] = {};
    char gain_value[6] = {0,0,0,0,0,0};
    if ((value == newco2PidGains.kp)||(value == co2PidGains.kp))
    {
        sprintf(msg,"Kp=");
    }
    else if ((value == newco2PidGains.ki)||(value == co2PidGains.ki))
    {
        sprintf(msg,"Ki=");
    }
    else if ((value == newco2PidGains.kd)||(value == co2PidGains.kd))
    {
        sprintf(msg,"Kd=");
    }


    if (value >= 10)
    {
        sprintf(gain_value, "%.3f", value);
    }
    else
    {
        sprintf(gain_value, "%.4f", value);
    }

    strcat(msg, gain_value);
    strcat(msg,"\r");
    sciSend(sciREG, sizeof(msg), (uint8*) msg);
//    delay(20);
}

//=============================================================================
void modeIsUpdated(void)
{
//    char msg[8] = {};
    if (currentFlags.modeIsUpdated == true)
    {
//        sprintf(msg,"t=false\r");
//        sciSend(sciREG1, sizeof(msg), (uint8*) msg);
        if (MODE == CO2_CONTROL)
        {
            Display_PID (co2PidGains.kp);
            Display_PID (co2PidGains.ki);
            Display_PID (co2PidGains.kd);
        }
        else
        {
            Display_PID (flowPidGains.kp);
            Display_PID (flowPidGains.ki);
            Display_PID (flowPidGains.kd);
        }
//        delay(20);
    }

    currentFlags.modeIsUpdated = false;
}

//=============================================================================
//void store_data(struct publishData_val_pair *par, int data_count)
//void store_data(float time, float Duty, float co2_level, float flow_level, float voltage,
//                float current, float power, float energy, float Blower1_Temp, float Blower2_Temp,
//                float CO2_Temp, float CO2_Humi, float Heater_Duty, float Pressure)
//{
//    uint8 i;
//    static uint32 counter = 1;
//    uint8 data_count = 13;
//    struct publishData_val_pair *par;
//
//    // format message for sending to touch-screen (according to translate.txt)
//    par = (struct publishData_val_pair*) malloc (sizeof(struct publishData_val_pair)*14);
//
//    delay(10);
//
//    strcpy( par[0].data, "time");
//    par[0].value = time;
//
//    strcpy( par[1].data, "Duty");
//    par[1].value = Duty;
//
//    strcpy( par[2].data, "CO2_Level");
//    par[2].value = co2_level;
//
//    strcpy( par[3].data, "Flow_Level");
//    par[3].value = flow_level;
//
//    strcpy( par[4].data, "Voltage");
//    par[4].value = voltage;
//
//    strcpy( par[5].data, "Current");
//    par[5].value = current;
//
//    strcpy( par[6].data, "Power");
//    par[6].value = power;
//
//    strcpy( par[7].data, "Energy");
//    par[7].value = energy;
//
//    strcpy( par[8].data, "Blower1_Temp");
//    par[8].value = Blower1_Temp;
//
//    strcpy( par[9].data, "Blower2_Temp");
//    par[9].value = Blower2_Temp;
//
//    strcpy( par[10].data, "CO2_Sens_Temp");
//    par[10].value = CO2_Temp;
//
//    strcpy( par[11].data, "CO2_Sens_Humi");
//    par[11].value = CO2_Humi;
//
//    strcpy( par[12].data, "Heater_Duty");
//    par[12].value = Heater_Duty;
//
//    strcpy( par[13].data, "Pressure");
//    par[13].value = Pressure;
//
//    /*-------------------------------------------------------------------------
//     --------------------------------------------------------------------------*/
//
//    /* Create a file for test data storage */
//    FILE *test_data;
//
//    char counterString[7];
//    sprintf(counterString, "%d", counter);
//    char fileName[50];
//    strcpy(fileName, "/animalStudy1/test_");
//    strcat(fileName,counterString);
//    strcat(fileName,".txt");
//
//    test_data= fopen(fileName,"w");
//     for (i=0; i<= data_count; i++)
//     {
//         fprintf (test_data, "%s = %.3f, ", par[i].data, par[i].value);
//     }
//     fprintf(test_data, "\n");
//     fclose(test_data);
//     counter++;
//
//     free(par);
//}
//
////=============================================================================
//
//void merge_file(void)
//{
//    static uint32 counter;
//
//    /* Create a file for test data storage */
//    FILE *test_data;
//    FILE *merged_file;
//
//    merged_file= fopen("mergedFile.txt", "w");
//    char c;
//
//    char counterString[7];
//
//    for (counter = 1; counter <= 4197; counter++)
//    {
//        sprintf(counterString, "%d", counter);
//        char fileName[50];
//        strcpy(fileName, "/tmp2/test_");
//        strcat(fileName,counterString);
//        strcat(fileName,".txt");
//        test_data= fopen(fileName,"r");
//        // Copy contents of first file to file3.txt
//        while ((c = fgetc(test_data)) != EOF)
//            fputc(c, merged_file);
//
//        fclose(test_data);
//    }
//
//fclose(merged_file);
//}

//===============================================================================
//void store_data(float currentTime, float Duty, float co2_level, float flow_level, float voltage,
//                float current, float power, float energy, float target_co2_level, float Ambient_Pressure)
//{
//    uint8 i;
//    static uint32 counter = 1;
//    uint8 data_count = 10;
//    struct publishData_val_pair *par;
//    time_t rawTime;
//
//
//    // format message for sending to touch-screen (according to translate.txt)
//    par = (struct publishData_val_pair*) malloc (sizeof(struct publishData_val_pair)*10);
//
//    delay(10);
//
//    time (&rawTime);
//    Time = localtime( &rawTime);
//    Time->tm_hour += 2;
//
//    strcpy( par[0].data, asctime(Time));
//    par[0].value = currentTime;
//
//    strcpy( par[1].data, "Duty");
//    par[1].value = Duty;
//
//    strcpy( par[2].data, "CO2_Level");
//    par[2].value = co2_level;
//
//    strcpy( par[3].data, "Flow_Level");
//    par[3].value = flow_level;
//
//    strcpy( par[4].data, "Voltage");
//    par[4].value = voltage;
//
//    strcpy( par[5].data, "Current");
//    par[5].value = current;
//
//    strcpy( par[6].data, "Power");
//    par[6].value = power;
//
//    strcpy( par[7].data, "Energy");
//    par[7].value = energy;
//
//    strcpy( par[8].data, "Target_CO2");
//    par[8].value = target_co2_level;
//
//    strcpy( par[9].data, "Ambient_Pressure");
//    par[9].value = Ambient_Pressure;
//
//    /*-------------------------------------------------------------------------
//     --------------------------------------------------------------------------*/
//
//    /* Create a file for test data storage */
//    FILE *test_data;
//
//    char counterString[7];
//    sprintf(counterString, "%d", counter);
//    char fileName[50];
//    strcpy(fileName, "/animalStudy1/test_");
//    strcat(fileName,counterString);
//    strcat(fileName,".txt");
//
//
//
//    test_data= fopen(fileName,"w");
//    fprintf (test_data, "%s,", par[0].data);
//     for (i=1; i< data_count; i++)
//     {
//         fprintf (test_data, "%s = %.3f, ", par[i].data, par[i].value);
//     }
//     fprintf(test_data, "\n");
//     fclose(test_data);
//     counter++;
//
//     free(par);
//}

//=============================================================================
//void store_data(float currentTime, float tEGCO2, float pCO2, float EGCO2, float sweepFlow,
//                float measuredSweepFlow, float Ambient_Pressure)//
void store_data(float currentTime, float targetSweep, float tEGO2, float EGO2, float measuredSweepFlow,
                float measuredO2Flow, float Ambient_Pressure)
{
    uint8 i;
    static uint32 counter = 1;
    uint8 data_count = 7;
    struct publishData_val_pair *par;
    time_t rawTime;


    // format message for sending to touch-screen (according to translate.txt)
    par = (struct publishData_val_pair*) malloc (sizeof(struct publishData_val_pair)*7);

//    delay(10);

    time (&rawTime);
    Time = localtime( &rawTime);
    Time->tm_hour += 2;

    strcpy( par[0].data, asctime(Time));
    par[0].value = currentTime;

    strcpy( par[1].data, "SweepFlow");
    par[1].value = targetSweep;

    strcpy( par[2].data, "tEGO2");
    par[2].value = tEGO2;

    strcpy( par[3].data, "EGO2");
    par[3].value = EGO2;

    strcpy( par[4].data, "Actual_Sweep");
    par[4].value = measuredSweepFlow;

    strcpy( par[5].data, "Added_O2");
    par[5].value = measuredO2Flow;

    strcpy( par[6].data, "Ambient_Pressure");
    par[6].value = Ambient_Pressure;

    /*-------------------------------------------------------------------------
     --------------------------------------------------------------------------*/

    /* Create a file for test data storage */
    FILE *test_data;

    char counterString[7];
    sprintf(counterString, "%d", counter);
    char fileName[50];
    strcpy(fileName, "/Benchtop1/test_");
    strcat(fileName,counterString);
    strcat(fileName,".txt");



    test_data= fopen(fileName,"w");
    fprintf (test_data, "%s,", par[0].data);
     for (i=1; i< data_count; i++)
     {
         fprintf (test_data, "%s = %.3f, ", par[i].data, par[i].value);
     }
     fprintf(test_data, "\n");
     fclose(test_data);
     counter++;

     free(par);
}

//=============================================================================

void merge_file(void)
{
    static uint32 counter;

    /* Create a file for test data storage */
    FILE *test_data;
    FILE *merged_file;

    merged_file= fopen("mergedFile.txt", "w");
    char c;

    char counterString[7];

    for (counter = 1; counter <= 4197; counter++)
    {
        sprintf(counterString, "%d", counter);
        char fileName[50];
        strcpy(fileName, "/tmp2/test_");
        strcat(fileName,counterString);
        strcat(fileName,".txt");
        test_data= fopen(fileName,"r");
        // Copy contents of first file to file3.txt
        while ((c = fgetc(test_data)) != EOF)
            fputc(c, merged_file);

        fclose(test_data);
    }

fclose(merged_file);
}


////=============================================================================
//void Display_Duty_Cycle (float Duty_Cycle)
//{
//    char Duty[5]= {0,0,0,0,0};
//    sprintf(Duty, "%.2f", Duty_Cycle);
//    char msg[19] = "Duty_Cycle=";
//    strcat(msg, Duty);
//    strcat(msg,"\r\n");
//    sciSend(sciREG1, sizeof(msg), (uint8*) msg);
//    delay(10);
//}
//
////=============================================================================
//void Display_Voltage_Level(float voltage)
//{
//    char volt[6]= {0,0,0,0,0,0};
//    sprintf(volt, "%.3f", voltage);
//    char msg[20] = "voltage=";
//    strcat(msg, volt);
//    strcat(msg,"\r\n");
//    sciSend(sciREG1, sizeof(msg), (uint8*)msg);
//    delay(10);
//}
//
////=============================================================================
//void Display_Current(float current)
//{
//    char amps[6] = {0,0,0,0,0,0};
//    sprintf(amps, "%.3f", current);
//    char msg[20] = "current=";
//    strcat(msg, amps);
//    strcat(msg,"\r\n");
//    sciSend(sciREG1, sizeof(msg), (uint8*)msg);
//    delay(10);
//}
//
////=============================================================================
//void Display_Power(float power)
//{
//    char pwr[6] = {0,0,0,0,0,0};
//    sprintf(pwr, "%g", power);
//    char msg[20] = "power=";
//    strcat(msg, pwr);
//    strcat(msg,"\r\n");
//    sciSend(sciREG1, sizeof(msg), (uint8*)msg);
//    delay(10);
//}
//
////=============================================================================
//void Display_Energy(float energy)
//{
//    char enrgy[6] = {0,0,0,0,0,0};
//    sprintf(enrgy, "%g", energy);
//    char msg[20] = "energy=";
//    strcat(msg, enrgy);
//    strcat(msg,"\r\n");
//    sciSend(sciREG1, sizeof(msg), (uint8*)msg);
//    delay(10);
//}
//
////=============================================================================
//void Display_Time(float time)
//{
//    char tme[6] = {0,0,0,0,0,0};
//    sprintf(tme, "%.2f", time);
//    char msg[20] = "time=";
//    strcat(msg, tme);
//    strcat(msg,"\r\n");
//    sciSend(sciREG1, sizeof(msg), (uint8*)msg);
//    delay(10);
//}
//
////=============================================================================
//void Display_CO2_Sensor_Temp(float CO2_Sensor_Temp)
//{
//    char CO2_Temp[10];
//    sprintf(CO2_Temp, "%.2f", CO2_Sensor_Temp);
//    char msg[25] = "co2_temp=";
//    strcat(msg, CO2_Temp);
//    strcat(msg,"\r\n");
//    sciSend(sciREG1, sizeof(msg) - 1, (uint8*) msg);
//    delay(10);
//}
//
////=============================================================================
//void Display_CO2_Sensor_Humi(float CO2_Sensor_Humi)
//{
//    char CO2_Humi[10];
//    sprintf(CO2_Humi, "%.2f", CO2_Sensor_Humi);
//    char msg[25] = "co2_humi=";
//    strcat(msg, CO2_Humi);
//    strcat(msg,"\r\n");
//    sciSend(sciREG1, sizeof(msg) - 1, (uint8*) msg);
//    delay(10);
//}
//
////=============================================================================
//void Display_Inlet_Pressure(float Pressure)
//{
//    char Inlet_Pressure[10];
//    sprintf(Inlet_Pressure, "%.4f", Pressure);
//    char msg[25] = "Inlet_Pressure=";
//    strcat(msg, Inlet_Pressure);
//    strcat(msg,"\r\n");
//    sciSend(sciREG1, sizeof(msg) - 1, (uint8*) msg);
//    delay(10);
//}
//
////=============================================================================
//void Display_Heater_Duty_Cycle(float Heater_Duty_Cycle)
//{
//        char Heater_Duty[10];
//        sprintf(Heater_Duty, "%.2f", Heater_Duty_Cycle);
//        char msg[25] = "co2_duty=";
//        strcat(msg, Heater_Duty);
//        strcat(msg,"\r\n");
//        sciSend(sciREG1, sizeof(msg) - 1, (uint8*) msg);
//        delay(10);
//}

//=============================================================================

void config_etPWM_eCAP(void)
{
    /* Alternate code for configuring ETPWM and ECAP */
    /* Configure ETPWM1 */
    /* Set the TBCLK frequency =  VCLK3 frequency = 75MHz */
//    etpwmSetClkDiv(etpwmREG1, ClkDiv_by_1, HspClkDiv_by_1);
    /* Set the time period as 1000 ns (Divider value = (1000ns * 75MHz) - 1 = 74)*/
//    etpwmSetTimebasePeriod(etpwmREG1, 74);
    /* Configure Compare A value as half the time period */
//    etpwmSetCmpA(etpwmREG1, 37);

    /* Configure mthe module to set PWMA value as 1 when CTR=0 and as 0 when CTR=CmpA  */
//    etpwmActionQualConfig_t configPWMA;
//    configPWMA.CtrEqZero_Action = ActionQual_Set;
//    configPWMA.CtrEqCmpAUp_Action = ActionQual_Clear;
//    configPWMA.CtrEqPeriod_Action = ActionQual_Disabled;
//    configPWMA.CtrEqCmpADown_Action = ActionQual_Disabled;
//    configPWMA.CtrEqCmpBUp_Action = ActionQual_Disabled;
//    configPWMA.CtrEqCmpBDown_Action = ActionQual_Disabled;
//    etpwmSetActionQualPwmA(etpwmREG1, configPWMA);

    /* Start counter in CountUp mode */
//    etpwmSetCount(etpwmREG1, 0);
//    etpwmSetCounterMode(etpwmREG1, CounterMode_Up);
//    etpwmStartTBCLK();

    /* Alternate code for configuring ECAP */
        /* Configure ECAP5 */
        /* Configure Event 1 to Capture the rising edge */
        ecapSetCaptureEvent1(ecapREG2, RISING_EDGE, RESET_DISABLE);
        /* Configure Event 2 to Capture the falling edge */
        ecapSetCaptureEvent2(ecapREG2, FALLING_EDGE, RESET_DISABLE);
        /* Configure Event 3 to Capture the rising edge with reset counter enable */
        ecapSetCaptureEvent3(ecapREG2, RISING_EDGE, RESET_ENABLE);
        /* Set Capture mode as Continuous and Wrap event as CAP3  */
        ecapSetCaptureMode(ecapREG2, CONTINUOUS, CAPTURE_EVENT3);
        ecapSetEventPrescaler(ecapREG2, ecapPrescale_By_10);
        /* Start counter */
        ecapStartCounter(ecapREG2);
        /* Enable Loading on Capture */
        ecapEnableCapture(ecapREG2);

}

//=============================================================================

etError tcaselect(uint32 mux, mux_address sensor)
{
    // error code
    uint8 DATA[1];

    float startTime = 0;
    float waitTimeLimit = 1;   // wait time of 1 second
    etError error = NO_ERROR;

    startTime = Run_Time_Since_Epoch ();

//    delay(20);

    DATA[0] = 1 << sensor; // Configure the address of the port where data needs to be sent
    // Configure address of Slave to talk to
    i2cSetSlaveAdd(i2cREG1, mux);  // update the address of i2c multiplexer based on the selected model
    // Set direction to Transmitter
    // Note: Optional - It is done in Init
    i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
    // Configure Data count
    // Note: Optional - It is done in Init, unless user want to change
    i2cSetCount(i2cREG1, sizeof(DATA));
    // Set mode as Master
    i2cSetMode(i2cREG1, I2C_MASTER);
    // Transmit Start Condition
    i2cSetStart(i2cREG1);
    i2cSend(i2cREG1, sizeof(DATA), DATA);
    // Set Stop after programmed Count
    i2cSetStop(i2cREG1);
//    delay(10);
    // Wait until Bus Busy is cleared
    while(i2cIsBusBusy(i2cREG1) == true  && ((Run_Time_Since_Epoch () - startTime) < waitTimeLimit));

            if (Run_Time_Since_Epoch ()-startTime >= waitTimeLimit)
               {
                    error = ACK_ERROR;
               }
               else
               {
                    error = NO_ERROR;
                    startTime = Run_Time_Since_Epoch();

                    // Wait until Stop is detected
                    while((i2cIsStopDetected(i2cREG1) == 0) && ((Run_Time_Since_Epoch() - startTime) < waitTimeLimit));

                    if ((Run_Time_Since_Epoch() - startTime) >= waitTimeLimit)
                             {
                                 error = ACK_ERROR;
                             }

                    else
                             {
                                 error = NO_ERROR;

                                 // Clear the Stop condition
                                 i2cClearSCD(i2cREG1);
                             }
               }

               return error;
}

//=============================================================================

void reset_sensor (sensor_RST sensor)
{
    if ((sensor == Flow_Sensor1_rst ) || (sensor == I2C_Multiplexer_rst))
            {
                uint32 bit = sensor;

                gioSetBit(gioPORTA, bit, 0U); //setting UP bit turns on the Flow sensor 1=ON; 0=OFF
//                delay(1000);

                gioToggleBit(gioPORTA, bit); // Flow sensor 1 is ON
//                delay(100);
            }

    else if (sensor == I2C_Multiplexer_rst)
            {
                uint32 bit = sensor;

                gioSetBit(gioPORTA, bit, 0U); //setting UP bit turns on the Flow sensor 1=ON; 0=OFF
//                delay(100);

                gioToggleBit(gioPORTA, bit); // Flow sensor 1 is ON
//                delay(10);
            }

//    else if ((sensor == CO2_Sensor_rst) || (sensor == Temp_Sensor1_rst) || (sensor == Temp_Sensor2_rst)
//            || (sensor == Humidity_Sensor1_rst))

    else if ((sensor == CO2_Sensor_rst) || (sensor == Ambient_Pressure_Sensor_rst)
            || (sensor == Humidity_Sensor1_rst) || (sensor == Pressure_Sensor_rst))
            {
                uint32 bit = sensor;

                gioSetBit(gioPORTA, bit, 1U); //setting UP bit turns off the sensor 1=OFF; 0=ON
//                delay(1000);

                gioToggleBit(gioPORTA, bit);  //  sensor is ON
//                delay(100);
            }
}

//=============================================================================
//void BT_Send_Msg(uint8 BTcmd[], uint8 x)
//{
//    // Initiate the conversation with the Bluetooth device
//    sciSend(sciREG3, x, BTcmd);
//    delay(100);
//}
//
////=============================================================================
//void rt_OneStep(void)
//{
//  static boolean_T OverrunFlag = false;
//
//  /* Disable interrupts here */
//
//  /* Check for overrun */
//  if (OverrunFlag) {
//    rtmSetErrorStatus(rtM, "Overrun");
//    return;
//  }
//
//  OverrunFlag = true;
//
//  /* Save FPU context here (if necessary) */
//  /* Re-enable timer or interrupt here */
//  /* Set model inputs here */
//
//  /* Step the model for base rate */
//  PID_Autotuner_step();
//
//  /* Get model outputs here */
//
//  /* Indicate task complete */
//  OverrunFlag = false;
//
//  /* Disable interrupts here */
//  /* Restore FPU context here (if necessary) */
//  /* Enable interrupts here */
//}

//=============================================================================

//void moisture_removal_init(void)
//{
//    uint8 i;
//    uint8 j;
//    float initial_pressure[6] = {0,0,0,0,0,0};
//    float initial_flow[6] = {0,0,0,0,0,0};
//    float duty;
//    float flow = 0;
//    uint8 size = 1;
//    //------------------------------------------------------------------------
//    uint8 k;
//    char fileName[50];
//    //------------------------------------------------------------------------
//
//    // Power Monitoring variables
//    float current_time = 0;
//    float Voltage = 0;
//    float Current = 0;
//    float Power = 0;
//    float Energy = 0;
//    //------------------------------------------------------------------------
//
//    struct duty_flow_pair *dutyFlowValues;
//    //------------------------------------------------------------------------
//
//    // format message for sending to touch-screen (according to translate.txt)
//    dutyFlowValues = (struct duty_flow_pair*) malloc (sizeof(struct duty_flow_pair)*11);
//    delay(10);
//
//    // Run the blower at peak duty cycle i.e flush duty cycle for 2 seconds to clear any previous moisture.
//    blower_speed_update (duty_flush);
//
//    for (i = 0; i <= 5; i++)
//    {
//        initial_flow[i] = get_avg_flow_val(Inlet_Flow_Sensor);
//        ABP_getPressure(Inlet_Pressure_Sensor, &initial_pressure[i]);
//        delay(300);
////        Run_Power_Monitor(&Voltage, &Current, &Power, &Energy, &current_time);
//        Run_Power_Monitor(&Voltage, &Current, &Power, &Energy);
//    }
//
////    store_data(duty_flush, duty_flush, 0, initial_flow[4], Voltage,
////                    Current, Power, Energy, 0, 0,
////                    0, 0, 0, initial_pressure[4]);
//
//    ref_duty_cycle[0] = MIN;
//    blower_speed_update (ref_duty_cycle[0]);
//    delay(3000);
////    Run_Power_Monitor(&Voltage, &Current, &Power, &Energy, &current_time);
//    Run_Power_Monitor(&Voltage, &Current, &Power, &Energy);
//
//    for (j=0; j<10; j++)
//    {
//        flow += get_avg_flow_val(Inlet_Flow_Sensor);
//        delay(50);
//    }
//    ref_flow[0] = flow/j;
//    delay(100);
//    dutyFlowValues[0].duty_cycle = ref_duty_cycle[0];
//    dutyFlowValues[0].flow = ref_flow[0];
////    Run_Power_Monitor(&Voltage, &Current, &Power, &Energy, &current_time);
//    Run_Power_Monitor(&Voltage, &Current, &Power, &Energy);
////    store_data (current_time, ref_duty_cycle[0], 0, ref_flow[0],
////                Voltage, Current, Power, Energy,
////                0,0,0,0,0,0);
//
//    for (duty=5; duty<=MAX; duty = duty+10)
//    {
//        flow = 0;
//        if (duty<= MIN)
//        {
//            duty = MIN;
//        }
//        ref_duty_cycle[size] = duty;
//        blower_speed_update (ref_duty_cycle[size]);
//        delay(5000);
//        for (j=0; j<10; j++)
//        {
//            flow += get_avg_flow_val(Inlet_Flow_Sensor);
//            delay(50);
//        }
//        ref_flow[size] = flow/j;
////        Run_Power_Monitor(&Voltage, &Current, &Power, &Energy, &current_time);
//        Run_Power_Monitor(&Voltage, &Current, &Power, &Energy);
//        dutyFlowValues[size].duty_cycle = ref_duty_cycle[size];
//        dutyFlowValues[size].flow = ref_flow[size];
//        size++;
////        store_data(current_time, duty, 0, ref_flow[size], Voltage,
////                        Current, Power, Energy, 0, 0,
////                        0, 0, 0, 0);
//    }
////    flow = 0;
////    ref_duty_cycle[size] = duty;
////    blower_speed_update (ref_duty_cycle[size]);
////    delay(3000);
////    for (j=0; j<10; j++)
////    {
////        flow += get_avg_flow_val(Inlet_Flow_Sensor);
////        delay(50);
////    }
////    ref_flow[size] = flow/j;
////    Run_Power_Monitor(&Voltage, &Current, &Power, &Energy, &current_time);
////    dutyFlowValues[size].duty_cycle = ref_duty_cycle[size];
////    dutyFlowValues[size].flow = ref_flow[size];
////    store_data(current_time, ref_duty_cycle[size], 0, ref_flow[size], Voltage,
////               Current, Power, Energy, 0, 0, 0, 0, 0, 0);
//
////    blower_speed_update (duty_flush);
////    delay(3000);
////    flow_moisture_flush = get_avg_flow_val(Inlet_Flow_Sensor);
////    delay(100);
////    Run_Power_Monitor(&Voltage, &Current, &Power, &Energy, &current_time);
////    store_data(current_time, duty_flush, 0, flow_moisture_flush, Voltage,
////               Current, Power, Energy, 0, 0,
////               0, 0, 0, 0);
//
//    //-------------------------------------------------------------------------
//    /* Create a file for test data storage */
//    FILE *test_data;
//
//    strcpy(fileName, "/tmp2/ref_data");
//    strcat(fileName,".txt");
//
//    test_data = fopen(fileName,"w");
//
//     for (k=0; k<= size-1; k++)
//     {
//         fprintf (test_data, "%.3f    %.3f \n", dutyFlowValues[k].duty_cycle, dutyFlowValues[k].flow);
//     }
//
//     fprintf(test_data, "\n");
//     fclose(test_data);
//     free(dutyFlowValues);
//     //------------------------------------------------------------------------
//
//     blower_speed_update (MIN);
//     duty_flow_calculation_init = 0;
//
//}

//=============================================================================
//void monitor_moisture_level(float duty_level, float flow_level)
//{
//    float duty_low;
//    float duty_high;
//    float flow_low;
//    float flow_high;
//    float flow_compare;
//    float flow_recent = 0;
//    uint8 min;
//    uint8 max;
//    float flow_flush;
//    //------------------------------------------------------------------------
//
//    // Power Monitoring variables
//    float current_time = 0;
//    float Voltage = 0;
//    float Current = 0;
//    float Power = 0;
//    float Energy = 0;
//    //------------------------------------------------------------------------
//
//    min = floor(duty_level/5) - 1;
//    max = min+1;
//
//    if (duty_level >= ref_duty_cycle[0])
//    {
//
//        duty_low = ref_duty_cycle[min];
//        duty_high = ref_duty_cycle[max];
//
//        flow_low = ref_flow[min];
//        flow_high = ref_flow[max];
//        flow_flush = flow_moisture_flush;
//
//        flow_compare = flow_low + ((flow_high - flow_low)*((duty_level - duty_low)/(duty_high - duty_low)));
//
//        if (flow_level <= 0.55*flow_compare)
//        {
//            blower_speed_update (duty_flush);
//            delay(3000);
//            flow_recent = get_avg_flow_val(Inlet_Flow_Sensor);
////            Run_Power_Monitor(&Voltage, &Current, &Power, &Energy, &current_time);
//            Run_Power_Monitor(&Voltage, &Current, &Power, &Energy);
//
//            while (flow_recent <= 0.85*flow_flush)
//            {
//                blower_speed_update (duty_flush);
//                delay(1000);
//                flow_recent = get_avg_flow_val(Inlet_Flow_Sensor);
////                Run_Power_Monitor(&Voltage, &Current, &Power, &Energy, &current_time);
//                Run_Power_Monitor(&Voltage, &Current, &Power, &Energy);
////                store_data(current_time, duty_flush, 0, flow_recent, Voltage,
////                           Current, Power, Energy, 0, 0,
////                           0, 0, 0, 0);
//            }
//        }
//
//    }
//
//    blower_speed_update (duty_level);
//}
//=============================================================================
/*BRIAN'S CODE*/
float elapsedTime = 0;
float lastEGCO2 = 0;
float integral = 0;
float * egco2PID(float EGCO2){
    elapsedTime += 0.01;
    float tEGCO2 = 20;
    float Kp = 1;
    float Ki = 0;
    float Kd = 0;
    float tSweep = 0;
    float error = EGCO2 - tEGCO2;
    integral += (error*elapsedTime);
    tSweep = (error*Kp) + (integral*Ki);
    if(tSweep <  EGCO2 * 0.015){
            tSweep =  EGCO2 * 0.015;
            if(tSweep < 0.1){
              tSweep = 0.1;
            }
          }
    printf("[tSweep=%f]\n", tSweep);
    return & tSweep;
}

//void sweepPID(float tSweep, float sweep){
//    float currSweep = 0;
//    SFC5500_SetSetpoint (NITROGEN, NORMALIZED,
//                                tSweep, &status.state_error);
//
//    SFC5500_GetSetPoint (NITROGEN, NORMALIZED,
//                                &currSweep, &status.state_error);
//    printf("[sweep=%f%]", currSweep);
//}
