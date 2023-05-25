/*
 * servo_sensirion.c
 *
 *  Created on: Feb 14, 2021
 *      Author: Navid Shaikh
 */

#include "servo_sensirion.h"
#include "servo_ventilation.h"
#include "i2c.h"
#include "gio.h"
#include "rti.h"
#include "system.h"
#include "het.h"



//=============================================================================
etError SF05_WriteCommand(etCommands cmd, et_address address)
{
    // error code
    uint8 DATA[2] = {0x00, 0x00};
    float startTime = 0;
    float waitTimeLimit = 1;   // wait time of 0.5 second
    etError writeError = NO_ERROR;

    DATA[0] = cmd >> 8; // Upper Byte
    DATA[1] = cmd & 0x00FF;  // Lower Byte

    startTime = Run_Time_Since_Epoch ();

    // Configure address of Slave to talk to
    i2cSetSlaveAdd(i2cREG1, address);

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

    if (writeError == NO_ERROR)
    {
        // Set Stop after programmed Count
        i2cSetStop(i2cREG1);
    }

    // Wait until Bus Busy is cleared
    while(i2cIsBusBusy(i2cREG1) == true && ((Run_Time_Since_Epoch() - startTime) < waitTimeLimit));

    if ((Run_Time_Since_Epoch() - startTime) >= waitTimeLimit)
            {
                writeError = ACK_ERROR;
            }

    else
            {
                writeError = NO_ERROR;
                startTime = Run_Time_Since_Epoch();

                // Wait until Stop is detected
                while((i2cIsStopDetected(i2cREG1) == 0) && ((Run_Time_Since_Epoch() - startTime) < waitTimeLimit));

                if ((Run_Time_Since_Epoch() - startTime) >= waitTimeLimit)
                        {
                            writeError = ACK_ERROR;
                        }

                else
                        {
                            writeError = NO_ERROR;

                            // Clear the Stop condition
                            i2cClearSCD(i2cREG1);
                        }

            }

    return writeError;

}

//=============================================================================
etError SF05_ReadCommandResult(uint8 *result, et_address address)
{

    float startTime = 0;
    float waitTimeLimit = 1;   // wait time of 0.5 second
    etError readError = NO_ERROR;

    startTime = Run_Time_Since_Epoch ();

    // Configure address of Slave to talk to
    i2cSetSlaveAdd(i2cREG1, address);

    // Set direction to Receiver
    // Note: Optional - It is done in Init
    i2cSetDirection(i2cREG1, I2C_RECEIVER);

    // Configure Data count
    // Note: Optional - It is done in Init, unless user want to change
    i2cSetCount(i2cREG1, sizeof(result));

    // Set mode as Master
    i2cSetMode(i2cREG1, I2C_MASTER);

    // Transmit Start Condition
    i2cSetStart(i2cREG1);

    // Transmit DATA_COUNT number of data in Polling mode
    i2cReceive(i2cREG1, sizeof(result), result);

    // Set Stop after programmed Count
    i2cSetStop(i2cREG1);

    // Wait until Bus Busy is cleared
    while((i2cIsBusBusy(i2cREG1) == true) && ((Run_Time_Since_Epoch() - startTime) < waitTimeLimit));

    if ((Run_Time_Since_Epoch() - startTime) >= waitTimeLimit)
        {
             readError = ACK_ERROR;
        }
    else
        {
             readError = NO_ERROR;
             startTime = Run_Time_Since_Epoch();

             // Wait until Stop is detected
             while((i2cIsStopDetected(i2cREG1) == 0) && ((Run_Time_Since_Epoch() - startTime) < waitTimeLimit));

             if ((Run_Time_Since_Epoch() - startTime) >= waitTimeLimit)
             {
                 readError = ACK_ERROR;
             }

             else
             {
                 readError = NO_ERROR;

                 // Clear the Stop condition
                 i2cClearSCD(i2cREG1);
             }
        }

    return readError;

}

//=============================================================================
etError SF05_GetFlow(float *current_flow)
{
    uint16 raw_flow_value = 0x0000;
    uint8 flow_buffer[2] = {0x00, 0x00};
    etError error = NO_ERROR;

    error = SF05_WriteCommand(FLOW_MEASUREMENT, SF05_Address);
//    delay(100);   // Start-up time for the flow sensor is 100mS.

    if (error == NO_ERROR)
    {
        error = SF05_ReadCommandResult(flow_buffer, SF05_Address);

//        delay(100);
    }

    if (error == NO_ERROR)
    {
        // filter message to show just flow value
        raw_flow_value = (flow_buffer[0] << 8) | flow_buffer[1];
        *current_flow = ((float)raw_flow_value - OFFSET_FLOW) / SCALE_FLOW;
    }

    return error;
}

//=============================================================================
//float get_avg_flow_val(mux_address flow_sensor)
//{
//   uint8 i = 0;
//   float current_flow_level = 0;
//   static float avg_flow_level = 0;
////   uint8 random = 0;
//
//   etError flowError = NO_ERROR;
//   etError muxError = NO_ERROR;
//
//   muxError = tcaselect(TCA1, flow_sensor);
////   delay (10);
//
//   while (muxError == ACK_ERROR)
//   {
//       printf("i2c mux error has occurred. Flow sensor error\n");
//       reset_sensor (I2C_Multiplexer_rst);
// //      delay (10);
//       muxError = tcaselect(TCA1, flow_sensor);
//   }
//
//
////   delay(10);
//
//   if (muxError == NO_ERROR)
//   {
//       for (i=0; i<2; i++)
//       {
//           flowError = SF05_GetFlow( &current_flow_level);
//
//           if (flowError == ACK_ERROR)
//           {
//               // Hard reset the sensor;
//               reset_sensor (Flow_Sensor1_rst);
//
////               delay(1000);
//               // print the error message
//               printf("Communication error occurred. Flow sensor is reset. \n");
//               i2cInit();
////               i--;
////               delay(500);
//           }
//           else if(flowError == NO_ERROR)
//           {
//               avg_flow_level += current_flow_level;
////               delay(20);
//           }
//       }
//       avg_flow_level /= 5;
//
//       if (avg_flow_level < 0.01)
//       {
//           avg_flow_level = 0;
//       }
//   }
//
////   random = rand ();
////   avg_flow_level = random *0.1;
//
//   return avg_flow_level;
//}

//=============================================================================
// Gets the temperature [°C] and the humidity [%RH].

etError SHTC3_GetTempAndHumi(mux_address temp_sensor, float *temp, float *humidity)
{

    uint8 temp_buffer[2] = {0x00, 0x00};
    uint8 humi_buffer[2] = {0x00, 0x00};
    uint16 raw_temp = 0; // temperature raw value from sensor
    uint16 raw_humi = 0; // humidity raw value from sensor

    etError muxError = NO_ERROR;
    etError tempError = NO_ERROR;

    muxError = tcaselect(TCA1, temp_sensor);
//    delay(10);

    if (muxError == ACK_ERROR)
    {
        printf("i2c mux error has occurred. Temp sensor error\n");
        reset_sensor (I2C_Multiplexer_rst);
//        delay (10);
        muxError = tcaselect(TCA1, temp_sensor);
    }

//    delay(10);

    if (muxError == NO_ERROR)
    {
        SHTC3_Wakeup();
//        delay(10);

        SHTC3_SoftReset();
//        delay(20);

        tempError = SF05_WriteCommand(MEAS_T_RH_POLLING, SHTC3_Address);
//        delay(15);

        if (tempError == NO_ERROR)
        {
            tempError = SF05_ReadCommandResult(temp_buffer, SHTC3_Address);
//            delay(20);
            tempError = SF05_ReadCommandResult(humi_buffer, SHTC3_Address);
//            delay(20);
        }

        if(tempError == NO_ERROR)
        {
            // filter message to show just temperature value
            raw_temp = (temp_buffer[0] << 8) | temp_buffer[1];

            // filter message to show just humidity value
            raw_humi = (humi_buffer[0] << 8) | humi_buffer[1];

            *temp = SHTC3_CalcTemperature(raw_temp);
            *humidity = SHTC3_CalcHumidity(raw_humi);
        }
    }

    return tempError;
}

//===============================================================================

static float SHTC3_CalcTemperature(uint16_t rawValue)
{
  // calculate temperature [°C]
  // T = -45 + 175 * rawValue / 2^16
  return 175 * (float)rawValue / 65536.0f - 45.0f;
}

//===============================================================================

static float SHTC3_CalcHumidity(uint16_t rawValue)
{
  // calculate relative humidity [%RH]
  // RH = rawValue / 2^16 * 100
  return 100 * (float)rawValue / 65536.0f;
}

//===============================================================================

void SHTC3_Sleep(void)
{

    SF05_WriteCommand(SLEEP, SHTC3_Address);

    return;
}

//===============================================================================

void SHTC3_Wakeup(void)
{

    SF05_WriteCommand(WAKEUP, SHTC3_Address);

    return;
}

//===============================================================================

void SHTC3_SoftReset(void)
{

    SF05_WriteCommand(SOFT_RESET_TH, SHTC3_Address);

    return;
}

//===============================================================================

//etError ABP_getPressure(mux_address pressure_sensor, float *pressure)
//{
//    uint16 raw_pressure_value = 0x0000;
//    uint8 pressure_buffer[2] = {0x00, 0x00};
//    uint16 raw_data = 0x0000;
//    uint8 status = 0x01;
//    etError muxError = NO_ERROR;
//    etError pressError = NO_ERROR;
//
//    muxError = tcaselect(TCA2, pressure_sensor);
////    delay(10);
//
//    if (muxError == ACK_ERROR)
//    {
//        printf("i2c mux error has occurred. Pressure sensor error\n");
////        muxError = tcaselect(TCA2, pressure_sensor);
//    }
//
////    delay(10);
//
//    if (muxError == NO_ERROR)
//    {
//        while (status != 0x00)   // Check if new dataset is available for the measurement
//        {
//
//            pressError = SF05_ReadCommandResult(pressure_buffer, ABP_Address);
////            delay(100);
//
//            if (pressError == ACK_ERROR)
//            {
//                break;
//            }
//            status = pressure_buffer[0] >> 6;
//            if (status != 0x00)
//            {
//                reset_sensor (Pressure_Sensor_rst);
////                delay(10);
//            }
//        }
//
//        if(pressError == NO_ERROR)
//        {
//            // filter message to show just flow value
//            raw_data = (pressure_buffer[0] << 8) | pressure_buffer[1];
//            raw_pressure_value = raw_data & 0x3FFF;
//
//            *pressure = (((float)raw_pressure_value - 1638)* 51.7149) / 2621.4;   //in mmHg
//        }
//    }
//
//    return pressError;
//}
//===============================================================================

etError ABP2_getPressure(mux_address pressure_sensor, float *pressure)
{
//    uint16 raw_pressure_value = 0x0000;
//    uint8 pressure_buffer[2] = {0x00, 0x00};
//    uint16 raw_data = 0x0000;
    uint8 status = 0x01;

    uint8_t data[4]; // holds output data
    double press_counts = 0; // digital pressure reading [counts]
//    double temp_counts = 0; // digital temperature reading [counts]
//    double pressure = 0; // pressure reading [bar, psi, kPa, etc.]
//    double temperature = 0; // temperature reading in deg C
    double outputmax = 15099494; // output at maximum pressure [counts]
    double outputmin = 1677722; // output at minimum pressure [counts]
    double pmax = 750.06; // maximum value of pressure range [bar, psi, kPa, etc.]
    double pmin = 0; // minimum value of pressure range [bar, psi, kPa, etc.]
    double percentage = 0; // holds percentage of full scale data

    etError muxError = NO_ERROR;
    etError pressError = NO_ERROR;

    muxError = tcaselect(TCA2, pressure_sensor);

    if (muxError == ACK_ERROR)
    {
        printf("i2c mux error has occurred. Pressure sensor error\n");
//        muxError = tcaselect(TCA2, pressure_sensor);
    }
//    delay(10);



    if (muxError == NO_ERROR)
    {
//        while (status != 0x00)   // Check if new dataset is available for the measurement
//        {

            pressError = ABP2_WriteCommand(ABP2_Address);
//            delay(10);
//            pressError = SF05_ReadCommandResult(pressure_buffer, ABP2_Address);
            pressError = ABP2_ReadCommandResult(data, ABP2_Address);
//            delay(200);

//            if (pressError == ACK_ERROR)
//            {
//                break;
//            }
//            status = data[0] >> 6;
//            status = data[0];
//        }

        if(pressError == NO_ERROR)
        {

            press_counts = data[3] + data[2] * 256 + data[1] * 65536; // calculate digital pressure counts
//            temp_counts = data[6] + data[5] * 256 + data[4] * 65536; // calculate digital temperature counts
//            temperature = (temp_counts * 200 / 16777215) - 50; // calculate temperature in deg c
            percentage = (press_counts / 16777215) * 100; // calculate pressure as percentage of full scale
            //calculation of pressure value according to equation 2 of datasheet
            *pressure = ((press_counts - outputmin) * (pmax - pmin)) / (outputmax - outputmin) + pmin;
//            // filter message to show just flow value
//            raw_data = (pressure_buffer[0] << 8) | pressure_buffer[1];
//            raw_pressure_value = raw_data & 0x3FFF;
//
//            *pressure = (((float)press_counts - 1638)* 51.7149) / 2621.4;   //in mmHg

//            *pressure *= 51.7149;  //in mmHg
        }
    }

    return pressError;
}

//=============================================================================
etError ABP2_WriteCommand(et_address address)
{
    // error code
    uint8 DATA[3] = {0xAA, 0x00, 0x00};
    float startTime = 0;
    float waitTimeLimit = 1;   // wait time of 0.5 second
    etError writeError = NO_ERROR;

//    DATA[0] = cmd >> 8; // Upper Byte
//    DATA[1] = cmd & 0x00FF;  // Lower Byte

    startTime = Run_Time_Since_Epoch ();

    // Configure address of Slave to talk to
    i2cSetSlaveAdd(i2cREG1, address);

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

    if (writeError == NO_ERROR)
    {
        // Set Stop after programmed Count
        i2cSetStop(i2cREG1);
    }

    // Wait until Bus Busy is cleared
    while(i2cIsBusBusy(i2cREG1) == true && ((Run_Time_Since_Epoch() - startTime) < waitTimeLimit));

    if ((Run_Time_Since_Epoch() - startTime) >= waitTimeLimit)
            {
                writeError = ACK_ERROR;
            }

    else
            {
                writeError = NO_ERROR;
                startTime = Run_Time_Since_Epoch();

                // Wait until Stop is detected
                while((i2cIsStopDetected(i2cREG1) == 0) && ((Run_Time_Since_Epoch() - startTime) < waitTimeLimit));

                if ((Run_Time_Since_Epoch() - startTime) >= waitTimeLimit)
                        {
                            writeError = ACK_ERROR;
                        }

                else
                        {
                            writeError = NO_ERROR;

                            // Clear the Stop condition
                            i2cClearSCD(i2cREG1);
                        }

            }

    return writeError;

}

//=============================================================================

etError ABP2_ReadCommandResult(uint8 *result, et_address address)
{

    float startTime = 0;
    float waitTimeLimit = 2;   // wait time of 0.5 second
    etError readError = NO_ERROR;

    startTime = Run_Time_Since_Epoch ();

    // Configure address of Slave to talk to
    i2cSetSlaveAdd(i2cREG1, address);

    // Set direction to Receiver
    // Note: Optional - It is done in Init
    i2cSetDirection(i2cREG1, I2C_RECEIVER);

    // Configure Data count
    // Note: Optional - It is done in Init, unless user want to change
    i2cSetCount(i2cREG1, sizeof(result));

    // Set mode as Master
    i2cSetMode(i2cREG1, I2C_MASTER);

    // Transmit Start Condition
    i2cSetStart(i2cREG1);

    // Transmit DATA_COUNT number of data in Polling mode
    i2cReceive(i2cREG1, sizeof(result), result);

    // Set Stop after programmed Count
    i2cSetStop(i2cREG1);

    // Wait until Bus Busy is cleared
    while((i2cIsBusBusy(i2cREG1) == true) && ((Run_Time_Since_Epoch() - startTime) < waitTimeLimit));

    if ((Run_Time_Since_Epoch() - startTime) >= waitTimeLimit)
        {
             readError = ACK_ERROR;
        }
    else
        {
             readError = NO_ERROR;
             startTime = Run_Time_Since_Epoch();

             // Wait until Stop is detected
             while((i2cIsStopDetected(i2cREG1) == 0) && ((Run_Time_Since_Epoch() - startTime) < waitTimeLimit));

             if ((Run_Time_Since_Epoch() - startTime) >= waitTimeLimit)
             {
                 readError = ACK_ERROR;
             }

             else
             {
                 readError = NO_ERROR;

                 // Clear the Stop condition
                 i2cClearSCD(i2cREG1);
             }
        }

    return readError;

}
