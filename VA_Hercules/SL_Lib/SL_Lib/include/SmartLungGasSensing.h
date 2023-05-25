/*
 * SmartLungGasSensing.h
 *
 *  Created on: Dec 12, 2022
 *      Author: NKS
 */

#ifndef INCLUDE_SMARTLUNGGASSENSING_H_
#define INCLUDE_SMARTLUNGGASSENSING_H_

#ifdef __cplusplus
extern "C" {
//#include <iostream>
#endif

#include <stdint.h>
#include "hal_stdtypes.h"

//uint8_t FDO2_buffer;

uint8_t dataReceiveFlag;
char dest[256];
extern uint8_t DeviceInfo[];
extern uint8_t UniqueID[];
extern uint8_t readOxy[];
extern uint8_t readOxyRAW[];
extern uint8_t disableBroadcast[];
extern uint8_t setBaudRate[];

struct Oxysensorinfo {
    char  id[1];
    char  oxyChannel[1];
    char  rev[3];
    char  model[10];
    char  uniqueID[10];
    float  partialOxy;
    float  temperature;
    uint32_t  status;
    float  dphi;
    float  signalIntensity;
    float  ambientLight;
    float  ambientPressure;
    float  relativeHumidity;
};

struct Oxysensorinfo inletO2[1];
struct Oxysensorinfo outletO2[1];

void co2SensorInit(void);

void inletO2Init(void);

void exhaustO2Init(void);

void bloodFlowSensorInit(void);

void FDO2DeviceInfo(void);

void FDO2Data(uint8_t* cmd, uint32 cmdLength, struct Oxysensorinfo* o2Sensor);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SMARTLUNGGASSENSING_H_ */
