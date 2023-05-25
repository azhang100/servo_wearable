/*
*   Created on: Feb 7, 2022
*   Author: Navid Shaikh
*/

#ifndef _INCLUDE_SFC5500DEVICE_H_
#define _INCLUDE_SFC5500DEVICE_H_

#include "sys_common.h"


#define ShdlcStartStopByte 0x7E

////-----------------------------------------------------------------------------
//typedef enum {
//	GetDeviceInfo = 0xD0,
//	GetDeviceErrorState = 0xD2,
//    DeviceReset = 0xD3,
//    FactoryReset = 0x92,
//    GetOrSetDeviceAddress = 0x90,
//    GetOrSetBaudrate = 0x91
//} ShdlcDriver;
//
////-----------------------------------------------------------------------------
//typedef enum {
//    Nitrogen = 0x07,
//    Oxygen = 0x01,
//    CO2 = 0x02
//}gasSourceAddress;

//-----------------------------------------------------------------------------
//typedef enum {
//	SFC5xxx_TranslateErrorCode;
//	SFC5xxx_ReadMeasuredFlow = 0x08,
//	SFC5xxx_ReadMeasuredFlowBuffered = 0x09,
//	SFC5xxx_GetCurrentCalibInfo;
//	SFC5xxx_LoadAndSelectCalibration;
//	SFC5xxx_SetSetpoint = 0x00,
//	SFC5xxx_GetSetpoint = 0x00,
//	SFC5xxx_SetSetpointAndReadMeasuredFlow = 0x03,
//	SFC5xxx_SetSetpointAndReadMeasuredFlow2Sensors = 0x04,
//	SFC5xxx_ValveSetInputSource = 0x20,
//	SFC5xxx_ValveGetInputSource = 0x20,
//	SFC5xxx_ValveSetUserDefinedValue;
//	SFC5xxx_ValveGetUserDefinedValue;
//	SFC5xxx_SetUserDefinedMediumUnit = 0x21,
//	SFC5xxx_GetUserDefinedMediumUnit = 0x21,
//	SFC5xxx_GetUserDefinedMediumFullscale;
//	SFC5xxx_GetSensorTemperatureInCelsius;
//	SFC5xxx_GetUserControllerGain;
//	SFC5xxx_SetUserControllerGain;
//	SFC5xxx_GetUsePressureDependentGain;
//	SFC5xxx_SetUsePressureDependentGain;
//	SFC5xxx_GetInletPressure;
//	SFC5xxx_SetInletPressure;
//	SFC5xxx_GetUseGasTemperatureCompensation;
//	SFC5xxx_SetUseGasTemperatureCompensation;
//	SFC5xxx_GetInletGasTemperature;
//	SFC5xxx_SetInletGasTemperature;
//
//}SFC5xxxDriver;

void enableUART(void);

//uint8_t getCheckSum(uint8_t addr, uint8_t opc, uint8_t len, uint8_t data);

//void Shdlc_GetDeviceInfo(gasSourceAddress SlaveAdr, uint32_t ResponseTimeoutMs, uint8_t InfoType, char* InfoString, uint32_t StringMaxSize);
	
//void Shdlc_GetDeviceInfo(gasSourceAddress SlaveAdr, uint32_t ResponseTimeoutMs, uint8_t InfoType, uint8_t* InfoString);

//void SFC5500_GetSetpoint(gasSourceAddress SlaveAdr, &currentSetpoint, 0);


#endif
