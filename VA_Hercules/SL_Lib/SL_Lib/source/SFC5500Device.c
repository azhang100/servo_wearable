/*
 * SFC5500Device.c
 *
 *  Created on: Feb 7, 2022
 *      Author: Navid Shaikh
 */


#include "sys_common.h"
#include "SFC5500Device.h"
#include "sci.h"
#include "gio.h"
#include "servo_functions.h"

//sciREG1 is assigned to SFC5500 Flow controller communication
//Defined baudrate is 9600

void enableUART(void)
{
    // Enable data transmission
    gioSetBit(gioPORTA, rs485_rx, 0U);  //~RE- Receive enable enable = Low; Disable = high
    gioSetBit(gioPORTA, rs485_tx, 1U);  //TE- Transmit enable enable = high; Disable = low
}

//uint8_t getCheckSum(uint8_t addr, uint8_t opc, uint8_t len, uint8_t data)
//{
//    uint8_t sum = addr + opc + len + data;
//    sum &= 0x00ff;
//
//    return ~(uint8_t)(sum);
//}

//uint8_t Shdlc_CreateDataString(ShdlcDriver cmd, uint8_t Tx_data)
//{
//
//}

//void Shdlc_GetDeviceInfo(gasSourceAddress SlaveAdr, uint32_t ResponseTimeoutMs, uint8_t InfoType, char* InfoString, uint32_t StringMaxSize)
//void Shdlc_GetDeviceInfo(gasSourceAddress SlaveAdr, uint32_t ResponseTimeoutMs, uint8_t InfoType, uint8_t* InfoString)
//{
//        uint8_t addr = SlaveAdr;
//        uint8 data_buffer[7] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//
//    // 1. Create string that needs to be sent. Use an external function to create the string.
//        uint8_t getDeviceInfo[] = {0x7E, addr, 0xD0, 0x01, InfoType, getCheckSum(addr,0xD0,0x01,InfoType),0x7E};
//
//        sciReceive(sciREG1, sizeof(data_buffer), &data_buffer[0]);
//    // 3. Serially transmit the created string.
//        sciSend(sciREG1, sizeof(getDeviceInfo),(uint8*) getDeviceInfo);
//
//        delay(3);
//
//        sciReceive(sciREG3, sizeof(InfoString), &InfoString[0]);
//    // 4. delay (ResponseTimeoutMs)
//        delay(ResponseTimeoutMs);
//    // 5. Serially receive the string data. Check for an error (External function).
//    // 6. If error flag received, check the error & display the error on the GUI (External function).
//    // 7. If no error received, parse the data & transfer the received string to the InfoString.
//    // 8. Return  error flag
//}


