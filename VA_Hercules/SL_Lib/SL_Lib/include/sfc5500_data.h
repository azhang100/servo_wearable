/*
 * sfc5500_data.h
 *
 *  Created on: Mar 5, 2022
 *      Author: aakash
 */

#ifndef INCLUDE_SFC5500_DATA_H_
#define INCLUDE_SFC5500_DATA_H_
#define MAX_FRAME_SIZE 262

#include <stdio.h>
#include <stdlib.h>
#include "sci.h"
#include "reg_sci.h"


uint8_t SFC5500_ReceiveBuffer[256];

typedef enum {
    SetPoint = 0x00,
    ReadMeasuredFlow = 0x08,
    GetDeviceInfo = 0xD0,
    GetDeviceErrorState = 0xD2,
    DeviceReset = 0xD3,
    FactoryReset = 0x92,
    GetOrSetDeviceAddress = 0x90,
    GetOrSetBaudrate = 0x91
} ShdlcDriver;

//=============================================================================

typedef enum {
    PRODUCT_NAME = 0x01,
    ARTICLE_CODE = 0x02,
    SERIAL_NUMBER = 0x03
}deviceInfoType;

//=============================================================================

typedef enum {
    no_error = 0x00,
    checksum_error = 0x01,
    invalid_input = 0x02,
}error_type;

//=============================================================================

typedef struct {
    uint8_t data[255];
    uint8_t numDataBytes;
    uint8_t state_error;
    error_type error_flag;
}dataPacket;

//=============================================================================

typedef enum {
    NITROGEN = 0x00,
    OXYGEN = 0x01,
    CO2 = 0x02
}gasSource;

//=============================================================================

typedef enum {
    NORMALIZED = 0x00,
    PHYSICAL_VALUE = 0x01,
    USER_UNIT_VALUE = 0x02
}scaling_type;

//=============================================================================

// GLOBALS
static uint8_t validCheckSumFlag = 1;
// I'm not a fan of #define statements; I'll
// try to swap it to a static const later

uint8_t getCheckSum (uint8_t* dataArr, uint8_t length);

dataPacket dataParser (uint8_t* receiveBuffer, uint32_t length);

uint8_t verifyCheckSum (uint8_t receivedData[], uint32_t length);

void buildData (gasSource addr, ShdlcDriver command, uint8_t len, uint8_t* inputData, uint8_t* dataBuffer, uint8_t* dataBufferLength);

dataPacket dataHandler (gasSource addr, ShdlcDriver command, uint8_t len, uint8_t inputData[]);

error_type dataToFloat (float* fl_ptr, uint8_t data[]);

void floatToData(float *val, uint8_t* data);

void sciTransmitData(sciBASE_t *sci, uint8 *text, uint32 length);

void wait(uint32 time);

#endif /* INCLUDE_SFC5500_DATA_H_ */
