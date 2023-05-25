/*
 * servo_ventilation.h
 *
 *  Created on: Feb 14, 2021
 *      Author: Navid Shaikh
 */

#ifndef INCLUDE_SERVO_VENTILATION_H_
#define INCLUDE_SERVO_VENTILATION_H_



/******************************************************************************/
/*                            INCLUDE FILES                                     */
/******************************************************************************/
// Calling Blower driver PWM related libraries
#include "het.h"

// Calling eCAP & etPWM related libraries
#include "etpwm.h"
#include "ecap.h"

/******************************************************************************/
/*                               #DEFINE                                        */
/******************************************************************************/


/******************************************************************************/
/*                           ENUM DECLARATION                                   */
/******************************************************************************/

typedef enum{
    Blower1_PWM = pwm0,
    Blower2_PWM = pwm1
} blower_pwm;

/*-----------------------------------------------------------------------------*/

typedef enum {
    Blower1_disable = 2U,    // Blower 1 is connected on GIOB2 pin
    Blower2_disable = 3U,    // Blower 2 is connected on GIOB3 pin
} blower_disable;

blower_pwm active_blower;


float blower_on_time;
/******************************************************************************/
/*                         FUNCTION DECLARATION                                 */
/******************************************************************************/

/*Define the Ventilation Initialization Sequence*/
void ventilation_init(void);
//==============================================================================

/*Calculate the Blower Speed */
float Blower_Speed_Check(ecapBASE_t *ecap);
//==============================================================================

/*Disable the Blower */
void disable_blower(blower_disable bit);
//==============================================================================

/*Update the Blower speed */
void blower_speed_update (float new_duty);
//==============================================================================

/* Update the blower speed based on the calculated duty cycle */

//void rearm_capture(ecapBASE_t *ecap);




#endif
