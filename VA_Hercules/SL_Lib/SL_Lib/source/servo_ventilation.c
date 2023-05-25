/*
 * servo_ventilation.c
 *
 *  Created on: Feb 17, 2021
 *      Author: Navid Shaikh
 */


/******************************************************************************/
/*                                INCLUDE FILES                                */
/******************************************************************************/

#include <stdio.h>
#include "servo_ventilation.h"
#include "servo_sensirion.h"
#include "servo_functions.h"
#include "system.h"

// Calling eCAP & etPWM related libraries
#include "etpwm.h"
#include "ecap.h"
#include "gio.h"

/******************************************************************************/
/*                            FUNCTION BODY                                    */
/******************************************************************************/

void ventilation_init(void)
{

    float Blower1_Temp = 0;
    float Blower1_Humidity = 0;
    uint32 init_duty = 15;                    // Initialize the duty cycle with 10%
//    active_blower = Blower1_PWM;             // Always initialize with blower1 as a default blower (DEFAULT)

    etError error = NO_ERROR;

    disable_blower(Blower2_disable);                     // Disable Blower2
    pwmSetDuty(hetRAM1, active_blower, init_duty);

    Blower_Speed_Check(ecapREG2);

//    /* check the temperature of the blower1 outlet temperature */
//    error = SHTC3_GetTempAndHumi(Temp_Sensor1, &Blower1_Temp, &Blower1_Humidity);
//
//    if (error == ACK_ERROR)
//    {
//        printf("Temp_Sensor1 comm error has occurred.\n");
////        reset_sensor (Temp_Sensor1_rst);
////        delay(200);
////        error = SHTC3_GetTempAndHumi(Temp_Sensor1, &Blower1_Temp, &Blower1_Humidity);
//    }

    /* if the temperature of blower2 is above 40*C, then switch immediately to blower2 */
//    if (Blower1_Temp >= 37) {
//        active_blower = Blower2_PWM;
//        disable_blower(Blower1_disable);                // Disable Blower1
//        pwmSetDuty(hetRAM1, active_blower, init_duty);
//    }

}

//=============================================================================

//void rearm_capture(ecapBASE_t *ecap)
//{
//    ecap->ECCTL2 |= 0x0008U;
//}

//=============================================================================

void disable_blower(blower_disable bit)
{
    uint32 enable_bit;
    uint32 disable_bit;

    if (bit == Blower1_disable )
            {
                disable_bit = bit;
                gioSetBit(gioPORTB, disable_bit, 0U); //turns off the blower 1
                delay(500);
                enable_bit = Blower2_disable;
                gioSetBit(gioPORTB, enable_bit, 1U); // Turn on blower 2
            }

    else if (bit == Blower2_disable)
            {
                disable_bit = bit;
                gioSetBit(gioPORTB, disable_bit, 0U); //turns off the blower 2
                delay(500);
                enable_bit = Blower1_disable;
                gioSetBit(gioPORTB, enable_bit, 1U); // Turn on blower 1
            }
}

//=============================================================================

void blower_speed_update (float new_duty)
{
//    float active_blower_temp = 0;
//    float active_blower_humi = 0;
    static float Time_elapsed1 = 0;
    float Time_elapsed2 = 0;
    float time_added;

    Time_elapsed2 = Run_Time_Since_Epoch ();
    time_added = Time_elapsed2 -Time_elapsed1;

    if (duty_flow_calculation_init == 0)
    {
        blower_on_time += time_added;

        if (blower_on_time >= 600)
        {
            if (active_blower == Blower1_PWM)
            {
                active_blower = Blower2_PWM;
                disable_blower(Blower1_disable);
            }

            else if (active_blower == Blower2_PWM)
            {
                active_blower = Blower1_PWM;
                disable_blower(Blower2_disable);
            }

            blower_on_time = 0;
        }

        else
        {
            if (active_blower == Blower1_PWM)
            {
                active_blower = Blower1_PWM;
                disable_blower(Blower2_disable);
            }

            else if (active_blower == Blower2_PWM)
            {
                active_blower = Blower2_PWM;
                disable_blower(Blower1_disable);
            }
        }
    }

    else
    {
        if (active_blower == Blower1_PWM)
        {
            active_blower = Blower1_PWM;
            disable_blower(Blower2_disable);
        }

        else if (active_blower == Blower2_PWM)
        {
            active_blower = Blower2_PWM;
            disable_blower(Blower1_disable);
        }
    }




//    if (active_blower == Blower1_PWM)
//    {
//        SHTC3_GetTempAndHumi(Temp_Sensor1, &active_blower_temp, &active_blower_humi);
//        if (active_blower_temp >= 35) {
//            active_blower = Blower2_PWM;
//            disable_blower(Blower1_disable);                // Enable Blower2
//        }
//    }
//
//    else if (active_blower == Blower2_PWM)
//    {
//        SHTC3_GetTempAndHumi(Temp_Sensor2, &active_blower_temp, &active_blower_humi);
//        if (active_blower_temp >= 35) {
//            active_blower = Blower1_PWM;
//            disable_blower(Blower2_disable);                // Enable Blower2
//        }
//    }

    pwmSetDuty(hetRAM1, active_blower, new_duty);

    Time_elapsed1 = Time_elapsed2;
}

//=============================================================================

float Blower_Speed_Check(ecapBASE_t *ecap)  // ecapREG2- for blower1; ecapREG3- for blower2
{
    float duty = 0;
    float period = 0;
    float frequency = 0;
    float speed = 0;

//    rearm_capture(ecapREG3);
    ecap->ECCTL2 |= 0x0008U;

    delay(40);   // temporary make-shift mechanism, have to use interrupt based robust solution.

    uint32 cap_1 = ecapGetCAP1(ecap);
    uint32 cap_2 = ecapGetCAP2(ecap);
    uint32 cap_3 = ecapGetCAP3(ecap);

    duty = (cap_2 - cap_1)/(VCLK3_FREQ*1000);
    period = (cap_3 - cap_1)/(VCLK3_FREQ*1000);
    frequency = 1000/period;
    speed = frequency * 60;

//    printf("Duty = %fms\n", duty);
//    printf("Period = %fms\n", period);
//    printf("Frequency = %fHz\n", frequency);
//    printf("Speed = %f\n\n", speed);

    return speed;
}
