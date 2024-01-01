/*
 * sfc5500_commands.c
 *
 *  Created on: Mar 5, 2022
 *      Author: aakash
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "sci.h"
#include "reg_sci.h"
#include "gio.h"
#include "sfc5500_commands.h"
#include "servo_functions.h"

uint8_t dataReceivedFlag;

//=============================================================================

/* The following function collects the device related information from the flow controllers.
 * Inputs: slaveAdr =  Address of the flow controller
 *         responseTimeoutMs =  delay time between data transmission & data reception.
 *         input = Type of information that is to be requested from the flow controller.
 * Outputs:info_string = Output data string where the data will be stored
 *         num_characters = No. of data bytes that are received from the flow controllers.
 *         stateByte = Flow controller internal error if present.
 */

// Sends the valid slave address and input enum. The
// info_string, num_characters, and stateByte will be
// filled accordingly. The error is returned.


error_type Shdlc_GetDeviceInfo (gasSource childAddr, char* info_string, uint8_t* num_characters,
                               deviceInfoType infoType, uint8_t* stateByte)
{
    uint8_t dataByte[1] = {infoType};
    uint8_t frame[256] = {}; //
    uint8_t frameLength = 0;
    uint8_t receiveBuffer[256];
    uint8_t dataByteNum = 0;

    memset(receiveBuffer, 0, sizeof(receiveBuffer));

    buildData(childAddr, GetDeviceInfo, sizeof(dataByte), &dataByte[0], &frame[0], &frameLength);

    gioSetBit(gioPORTA, rs485_tx, 1U);
    dataTransmit(&frame[0], frameLength);
//    sciTransmitData(sciREG3, &frame[0], frameLength);
//    delay(2);          // for Baudrate of 9600
    gioSetBit(gioPORTA, rs485_tx, 0U);
    dataReceive(&receiveBuffer[0], &dataByteNum);

//    receiveBuffer[4] = 0x7D;
//    receiveBuffer[5] = 0x5E;

    dataPacket received = dataParser(receiveBuffer, dataByteNum);

    if (received.error_flag != no_error) return received.error_flag;

    *stateByte = received.state_error;
    *num_characters = received.numDataBytes;
    memcpy(info_string, received.data,received.numDataBytes);

    return no_error;
}

//=============================================================================

error_type SFC5500_ReadMeasuredValue (gasSource childAddr,scaling_type scaling,
                                      float* measuredValue, uint8_t* stateByte)
{
    uint8_t dataByte[1] = {scaling};
    uint8_t frame[256] = {}; // Frame is bigger to account for received value
    uint8_t frameLength = 0;
    uint8_t receiveBuffer[256];
    uint8_t dataByteNum = 0;
    float fl_ptr;

    memset(receiveBuffer, 0, sizeof(receiveBuffer));

    buildData(childAddr, ReadMeasuredFlow, sizeof(dataByte), &dataByte[0], &frame[0], &frameLength);

    gioSetBit(gioPORTA, rs485_tx, 1U);
    dataTransmit(&frame[0], frameLength);
//    sciTransmitData(sciREG3, &frame[0], frameLength);
//    delay(2);          // for Baudrate of 9600
    gioSetBit(gioPORTA, rs485_tx, 0U);
    dataReceive(&receiveBuffer[0], &dataByteNum);

    dataPacket received = dataParser(receiveBuffer, dataByteNum);

    if (received.error_flag != no_error) return received.error_flag;
    *stateByte = received.state_error;

    dataToFloat(&fl_ptr, received.data);

    *measuredValue = fl_ptr;
//    printf("Current Flow is %.2f \n", *measuredValue);

    return no_error;
}

//=============================================================================

// Takes in a slave address and type of scaling for the float. If
// isGetting is 0, then the master is setting the setpoint and the
// float value will be used as an input. Else, isGetting is non-zero
// and the float value will be saved to the destination pointed to
// by setPoint. The scaling factor will apply the same way regard-
// less of the value isGetting.

error_type SFC5500_SetSetpoint (gasSource childAddr, scaling_type scaling,
                                float targetSetPoint, uint8_t* stateByte)
{
    uint8_t dataBytes[5] = {};
    dataBytes[0] = scaling;
    uint8_t frame[256] = {};
    uint8_t frameLength = 0;
    uint8_t receiveBuffer[256];
    uint8_t dataByteNum = 0;

    memset(receiveBuffer, 0, sizeof(receiveBuffer));
    // Setting case
    floatToData(&targetSetPoint, &dataBytes[1]);

    buildData(childAddr, SetPoint, sizeof(dataBytes), &dataBytes[0], &frame[0], &frameLength);

    gioSetBit(gioPORTA, rs485_tx, 1U);
    dataTransmit(&frame[0], frameLength);
//    sciTransmitData(sciREG3, &frame[0], frameLength);
//    delay(2);
    gioSetBit(gioPORTA, rs485_tx, 0U);
    dataReceive(&receiveBuffer[0], &dataByteNum);

    dataPacket received = dataParser(receiveBuffer, dataByteNum);

    if (received.error_flag != no_error) return received.error_flag;
    *stateByte = received.state_error;

    // Getting case
    return no_error;
}

//=============================================================================

error_type SFC5500_GetSetPoint (gasSource childAddr, scaling_type scaling,
                                float* currentSetPoint, uint8_t* stateByte)
{
    uint8_t dataByte[1] = {scaling};
    uint8_t frame[256] = {};
    uint8_t frameLength = 0;
    uint8_t receiveBuffer[256];
    uint8_t dataByteNum = 0;
    float fl_ptr;

    memset(receiveBuffer, 0, sizeof(receiveBuffer));

    buildData(childAddr, SetPoint, sizeof(dataByte), &dataByte[0], &frame[0], &frameLength);

    gioSetBit(gioPORTA, rs485_tx, 1U);
    dataTransmit(&frame[0], frameLength);
//    sciTransmitData(sciREG3, &frame[0], frameLength);
//    delay(2);
    gioSetBit(gioPORTA, rs485_tx, 0U);
    dataReceive(&receiveBuffer[0], &dataByteNum);

    dataPacket received = dataParser(receiveBuffer, dataByteNum);

    if (received.error_flag != no_error) return received.error_flag;
    *stateByte = received.state_error;

    dataToFloat(&fl_ptr, received.data);

    *currentSetPoint = fl_ptr;

    // Getting case
    return no_error;
}

//=============================================================================

error_type dataReceive(uint8_t* rFrame, uint8_t* dataByteNum)
{
//    while (dataReceivedFlag != NONE); // Wait till reception is idle
    // Do-while loop: It will receive first and then check the while
    // loop condition. The first receive should switch the flag
    // away from NONE so that this loop is not pointless.
    uint16_t counter = 0;
    uint8_t numStartStop = 0;

    do {
        dataReceivedFlag = 1;
        sciReceive(scilinREG, 1, &rFrame[counter]);
        while (dataReceivedFlag != 0);
        if (rFrame[counter++] == 0x7E) {
            ++numStartStop;
        }
    } while (numStartStop < 2);
    *dataByteNum = counter;
    return no_error;
}

//=============================================================================

error_type dataTransmit(uint8_t* tFrame, uint8_t tFrameLength)
{
    uint16_t counter = 0;
    uint8_t rFrame[256];

    while(tFrameLength--)
    {
        dataReceivedFlag = 1;
        while ((scilinREG->FLR & 0x4) == 4); /* wait until busy */
        sciSendByte(scilinREG,*tFrame++);      /* send out text   */
        sciReceive(scilinREG, 1, &rFrame[counter]);
        while (dataReceivedFlag != 0);
        counter++;
    };

    return no_error;
}

//=============================================================================

error_type setpointUpdate(float targetConcentration, float targetFlow)
{

    float targetCO2SetPoint = 0.00f;           // normalized
    float currentCO2SetPoint = 0.00f;          // normalized
    float targetO2SetPoint = 0.00f;            // normalized
    float currentO2SetPoint = 0.00f;           // normalized
    dataPacket status;

    targetCO2SetPoint = (targetConcentration*0.2*targetFlow)/100;
    targetO2SetPoint = (1-(targetConcentration/100))*0.1*targetFlow;
    // read current setpoint (normalized)

//   while (targetCO2SetPoint != currentCO2SetPoint)
//   {
       // read current setpoint (normalized)
       SFC5500_SetSetpoint (CO2, NORMALIZED,
                            targetCO2SetPoint, &status.state_error);

       SFC5500_GetSetPoint (CO2, NORMALIZED,
                            &currentCO2SetPoint, &status.state_error);

       printf("current CO2 setpoint is %.2f ml/min\n", currentCO2SetPoint* 5000.0f);

//   }

       SFC5500_SetSetpoint (NITROGEN, NORMALIZED,
                         targetO2SetPoint, &status.state_error);

       SFC5500_GetSetPoint (NITROGEN, NORMALIZED,
                                 &currentO2SetPoint, &status.state_error);

       printf("current Oxygen setpoint is %.2f ml/min\n", currentO2SetPoint* 10000.0f);

return no_error;
}

//=============================================================================

error_type measureCurrentFlows(float* flow1, float* flow2, float* flow3)
{
    dataPacket status;

    error_type error = no_error;

    gioSetBit(gioPORTA, rs485_rx, 0U);

      // read current O2 flow (normalized)
         error = SFC5500_ReadMeasuredValue (NITROGEN, NORMALIZED,
                                             flow1, &status.state_error);
              if (error == 0)
                   {

                  *flow1 *= 10000.0f;
//                      printf("Measured Air flow: %6.2f%\t\t ml/min\n", *flow1* 10000.0f);
                   }

      // read current O2 flow (normalized)
         error = SFC5500_ReadMeasuredValue (OXYGEN, NORMALIZED,
                                                     flow2, &status.state_error);
               if (error == 0)
                    {
                   *flow2 *= 10000.0f;
//                       printf("Measured CO2 flow: %6.2f%\t\t ml/min\n", *flow2* 200.0f);
                    }

      // read current CO2 flow (normalized)
                  error = SFC5500_ReadMeasuredValue (CO2, NORMALIZED,
                                                              flow3, &status.state_error);
                        if (error == 0)
                             {
                            *flow3 *= 200.0f;
                                printf("Measured CO2 flow: %6.2f%\t\t ml/min\n", *flow2* 200.0f);
                             }

               gioSetBit(gioPORTA, rs485_rx, 1U);

}

//=============================================================================

error_type calculateSetPoint(float targetO2Concentration, float* targetSweep)
{

    //printf("[tSWEEP@CHECK=%f]\n", *targetSweep);
    float targetSweepFlow = *targetSweep;
    float targetAirSetPoint = 0.0f;           // normalized
    float currentAirSetPoint = 0.0f;          // normalized
    float targetO2SetPoint = 0.0f;            // normalized
    float currentO2SetPoint = 0.0f;           // normalized
    float targetCO2SetPoint = 0.0f;           // normalized
    float currentCO2SetPoint = 0.0f;          // normalized
    float targetAirFlow = 0.0f;               // in ml/min
    dataPacket status;

//    targetO2SetPoint = ((targetO2Concentration - 21)*targetSweepFlow)/10000; // normalized
//
//    targetO2SetPoint = (1/0.79)*((targetO2Concentration/100)-0.21)* (targetSweepFlow/100);  //normalized
//
//    targetAirSetPoint = (targetSweepFlow - (targetO2SetPoint*100))/100;
//    targetAirSetPoint =(*targetSweep + 0.0109)/5.95;
      targetAirSetPoint = (*targetSweep*0.1204)+0.0313;
// / 6.21
//    targetAirSetPoint = 0.01;
//
//    targetO2SetPoint = 0.0;
//
//    targetCO2SetPoint = 0.0;

    // read current setpoint (normalized)

    gioSetBit(gioPORTA, rs485_rx, 0U);


//   while (targetCO2SetPoint != currentCO2SetPoint)
//   {
       // read current setpoint (normalized)
       SFC5500_SetSetpoint (NITROGEN, NORMALIZED,
                            targetAirSetPoint, &status.state_error);

       SFC5500_GetSetPoint (NITROGEN, NORMALIZED,
                            &currentAirSetPoint, &status.state_error);


       // read current setpoint (normalized)
       SFC5500_SetSetpoint (OXYGEN, NORMALIZED,
                            targetAirSetPoint, &status.state_error);


       SFC5500_GetSetPoint (OXYGEN, NORMALIZED,
                            &currentAirSetPoint, &status.state_error);

//       printf("current Air setpoint is %.2f ml/min\n", currentSweepSetPoint* 10000.0f);

//   }

//       SFC5500_SetSetpoint (CO2, NORMALIZED,
//                            currentCO2SetPoint, &status.state_error);
//
//       SFC5500_GetSetPoint (CO2, NORMALIZED,
//                                 &currentCO2SetPoint, &status.state_error);



//       printf("current Oxygen setpoint is %.2f ml/min\n", currentO2SetPoint* 10000.0f);

       gioSetBit(gioPORTA, rs485_rx, 1U);

return no_error;
}

error_type calculateSetPointCO2(float targetO2Concentration, float * targetSweep)
{
    //printf("[tSWEEP@CHECK=%f]\n", *targetSweep);
    //float targetSweepFlow = *targetSweep;
    float targetAirSetPoint = 0.0f;           // normalized
    float currentAirSetPoint = 0.0f;          // normalized
    float targetO2SetPoint = 0.0f;            // normalized
    float currentO2SetPoint = 0.0f;           // normalized
    float targetCO2SetPoint = 0.0f;           // normalized
    float currentCO2SetPoint = 0.0f;          // normalized
    float targetAirFlow = 0.0f;               // in ml/min
    dataPacket status;

//    targetO2SetPoint = ((targetO2Concentration - 21)*targetSweepFlow)/10000; // normalized
//
//    targetO2SetPoint = (1/0.79)*((targetO2Concentration/100)-0.21)* (targetSweepFlow/100);  //normalized
//
//    targetAirSetPoint = (targetSweepFlow - (targetO2SetPoint*100))/100;

    *targetSweep = 18*(*targetSweep)+0.1;
    targetCO2SetPoint = *targetSweep;
    //printf("CHECK CO@,%f\n", targetCO2SetPoint);
    //targetCO2SetPoint = .1; x1 = .02, y1 = .5, x2 = 0.03, y2 = 0.7


//    targetAirSetPoint = 0.01;
//
//    targetO2SetPoint = 0.0;
//
//    targetCO2SetPoint = 0.0;

    // read current setpoint (normalized)

    gioSetBit(gioPORTA, rs485_rx, 0U);

//   while (targetCO2SetPoint != currentCO2SetPoint)
//   {
       // read current setpoint (normalized)
//       printf("current Air setpoint is %.2f ml/min\n", currentSweepSetPoint* 10000.0f);

//   }

       SFC5500_SetSetpoint (CO2, NORMALIZED,
                            targetCO2SetPoint, &status.state_error);

       SFC5500_GetSetPoint (CO2, NORMALIZED,
                                 &currentCO2SetPoint, &status.state_error);



//       printf("current Oxygen setpoint is %.2f ml/min\n", currentO2SetPoint* 10000.0f);

       gioSetBit(gioPORTA, rs485_rx, 1U);

return no_error;
}
