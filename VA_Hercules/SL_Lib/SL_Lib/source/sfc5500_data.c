/*
 * sfc5500_data.c
 *
 *  Created on: Mar 4, 2022
 *      Author: aakash
 */

#include <stdio.h>
#include <stdlib.h>
#include "sci.h"
#include "reg_sci.h"
#include "sfc5500_data.h"

// Takes a valid data frame, the length of the data frame, and a boolean
// value that is 0 if it's a data frame is sent by the master (AKA no
// state byte) and 1 if it's a data frame received by the master.
// If the data frame passed in lacks the checksum byte, but all other
// necessary bytes (addr, command, len, and data) are present in the right
// spots, then the function will still operate as intended.

//=============================================================================

/* The following function calculates the checksum based on the frame content.

Inputs: dataArr = Data packet to be transmitted or already received.
	  isData = This byte is set to 1 if data is received i.e. state byte is present 
			in dataArr. This byte is kept 0 if data is transmitted i.e. state
			byte is absent.
Output: Calculated checksum byte.

*/

uint8_t getCheckSum(uint8_t* dataArr, uint8_t length)
{
    uint8_t i = 0;
    int sum = 0;

    for(i = 1; i < length; ++i)
    {
        sum += dataArr[i];
    }

    // Add addr, opc, and fourth byte (state/len) together.
//    int sum = (int)(dataArr[1] + dataArr[2] + dataArr[3]);
//    uint8_t len = dataArr[3]; // If it's not data (state byte), length is fourth byte
//
//    if (isData)
//	    {
//    	  // Need fifth byte, as that's length
//            sum += (uint8_t)(len = dataArr[4]);
//    	}
    /*
    * Data needs to be summed together now. If len is 0, the loop will not
    * execute at all. If there is data, than the first byte will be off by
    * one based on whether the master is sending or receiving data.
    * The isData byte will automatically account for this because it's 0
    * when it's transmitting the data and 1 when it is receiving data.
    */
//    for (i = 0; i < len; ++i)
//	    {
//            sum += dataArr[i + 4 + isData];
//    	}
    return ~(uint8_t)(sum & 0x00ff);
}

//=============================================================================

/* Following function varifies the data frame received from the flow controller.
 * Inputs:  Data Arr =  Data array received from the flow controller,
 *          length =  Total Length of data array received from the flow controller.
 * Outputs: i) datapacket.data = rx data received from the flow controller,
 *          ii) datapacket.numDataBytes = Length of rx data,
 *          iii) datapacket.state_error = internal flow controller errors,
 *          iv) datapacket.error_flag = communication status.
 * Action =   It should perform the checksum internally to the function & make sure the data packets received are correct.
*/
dataPacket dataParser(uint8_t* receiveBuffer, uint32_t length)
{
    uint8_t i = 0;
    uint8_t j = 0;
    dataPacket d;
    uint8_t receivedData[256] = {};
    uint8_t numExceptions = 0;
    uint8_t preStuffLength = length;
    d.error_flag = no_error;

    receivedData[0] = 0x7E; //Start Byte

    for (i = 1; i < length; ++i)
    {
        if (receiveBuffer[i + numExceptions] == 0x7d)
        {
            ++numExceptions;
            --preStuffLength;
        // Next byte is actual state byte, but stuffed
        // Inverting logic: invert back third msb
            receivedData[i] = receiveBuffer[i + numExceptions] < 0x50 ?
                    receiveBuffer[i + numExceptions] & 0b11011111 : receiveBuffer[i + numExceptions] | 0b00100000;
        }
        else
        {
            receivedData[i] = receiveBuffer[i + numExceptions];
        }
    }

    d.state_error = receivedData[3];
    d.numDataBytes = receivedData[4];
    receivedData[preStuffLength - 1] = 0x7E; //Stop Byte

    // Verify check sum
    // First error to check for is checkSum
    if(receivedData[preStuffLength - 2] != getCheckSum(receivedData, preStuffLength-2)) d.error_flag = checksum_error;

    for (j = 0; j < d.numDataBytes; ++j)
	{
        d.data[j] = receivedData[j + 5];
   	}
    return d;
}

//=============================================================================

/* Following function verifies the Checksum byte to confirm the integrity of data
bytes received.

Inputs: dataArr = Data packet already received from the children.
	  length =  length of dataArr that is received.
Output: Checksum error flag =  0 for error present, 1 for no error.
*/

uint8_t verifyCheckSum(uint8_t receivedData[], uint32_t length)
{
    return receivedData[length - 2] == getCheckSum(receivedData, length);
}

//=============================================================================

/* Following function takes various parameters as an inputs and
 * outputs a data frame into dataFrameBuffer.
 *
 * Inputs: addr =  Address of the flow controller
 *         command =  Operation command that needs to be sent
 *         len = length of data bytes that is being sent.Usually 7.
 *         inputData = Sub-command that is sent.
 *
 * Output: dataFrameBuffer = Output dataframe being transmitted to flow controller.
 *
 * NOTE: The length of dataFrameBuffer MUST be at least
 * len + 6 or greater. The function will ALWAYS fill the buffer
 * with exactly len + 6 bytes--no more, no less.
 */

void buildData(gasSource addr, ShdlcDriver command, uint8_t len,
    uint8_t* inputData, uint8_t* dataBuffer, uint8_t* dataBufferLength)
{
    uint8_t ipDataCtr = 0; // Counter for input Data frame.
    uint8_t dataCtr = 0;   // Size of data counter.
    uint8_t localBuffer[261] = {}; // Local data frame buffer.
    uint8_t localBufferLength = 4; // Length counter for local data frame buffer.
    uint8_t numExceptions = 0; // Number of exceptions encountered in the dataframe.

    localBuffer[0] = 0x7E; // Start Byte
    localBuffer[1] = addr; // Address Byte
    localBuffer[2] = command; // Command Byte
    localBuffer[3] = len; // Length Byte

    // Calculate the checksum.

    for (ipDataCtr = 0; ipDataCtr < len; ++ipDataCtr)
		{ // Data Bytes
        	localBuffer[ipDataCtr + 4] = inputData[ipDataCtr];
        	++localBufferLength;
    	}

    localBuffer[len + 4]
        = getCheckSum(localBuffer, localBufferLength); // Checksum ****Should it be len + 4.
    ++localBufferLength;

    localBuffer[len + 5] = 0x7E; // Stop Byte
    ++localBufferLength;

    dataBuffer[0] = 0x7E; // Start Byte
    for (dataCtr = 1; dataCtr < localBufferLength-1; ++dataCtr)
    {
        if (    localBuffer[dataCtr] == 0x7E
             || localBuffer[dataCtr] == 0x7D
             || localBuffer[dataCtr] == 0x11
             || localBuffer[dataCtr] == 0x13)
        {
            dataBuffer[dataCtr + numExceptions] = 0x7D;
            dataBuffer[dataCtr + ++numExceptions]
                = localBuffer[dataCtr] > 0x70 ? localBuffer[dataCtr] & 0b11011111: localBuffer[dataCtr] | 0b00100000;
        }
        else
        {
            dataBuffer[dataCtr + numExceptions] = localBuffer[dataCtr];
        }
    }
    dataBuffer[dataCtr + numExceptions] = 0x7E; // Stop Byte
    *dataBufferLength = dataCtr + numExceptions +1;
}

//=============================================================================

/* Following function converts the float input values into int8_t data packets.
 * Input:   val = Memory location where float values are stored.
 * Output:  data =  Memory location where output int8_t data values are stored.
 */

void floatToData(float *val, uint8_t* data)
{
    uint8_t *sends;
    uint8_t i = 0;
    uint8_t arrangedSends[sizeof(float)];
    sends = (uint8_t *) val;

    for (i=1; i<=sizeof(float); i++)
    {
        arrangedSends[i-1] = sends[sizeof(float)-i];
    }

    memcpy(data, arrangedSends, sizeof(float));
}

//================================================================================

error_type dataToFloat(float* fl_ptr, uint8_t data[])
{
    uint8_t i = 0;
    uint32_t int_fl = 0;
    // Save three most significant bytes
    for (i = 0; i < 3; ++i)
    {
        int_fl += data[i];
        int_fl <<= 8; // TODO: Is this valid syntax?
    }
    // Save least significant byte
    int_fl += data[3];
    *fl_ptr = *((float*)&int_fl);

//    printf("the hexadecimal 0x%08x becomes %.2f as a float\n", &int_fl, *fl_ptr);

    return no_error;
}

//=============================================================================

void sciTransmitData(sciBASE_t *sci, uint8 *text, uint32 length)
{
    while(length--)
    {
        while ((sciREG->FLR & 0x4) == 4); /* wait until busy */
        sciSendByte(sciREG,*text++);      /* send out text   */
        wait(1);
    };
}

//=============================================================================

void wait(uint32 time)
{
    time--;
}

