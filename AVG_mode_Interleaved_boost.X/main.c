/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.170.0
        Device            :  PIC24FJ64GA002
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.61
        MPLAB 	          :  MPLAB X v5.45
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/
#include "mcc_generated_files/system.h"

/*
                         Main application
 */

float Vref = 48.8;          //Voltage Setpoint
float Iref1 = 2.2;          //Starting current Setpoint for inductor 1
float Iref2 = 2.2;          //Starting current Setpoint for inductor 2

float Iref_tot = 4.4;       //Reference current from outer voltage loop

float Iactual1 = 0.0;       //Measured Current for Inductor 1
float Iactual2 = 0.0;       //Measured Current for Inductor 1
float Vactual = 0.0;        //Measured Voltage

float e_v[5] = {0.0};       //Voltage error array
float m_v[5] = {0.0};       //Voltage PID output array

float e1_i[5] = {0.0};      //Inductor 1 Current error array
float m1_i[5] = {0.0};      //Inductor 1 Current PID output array
float e2_i[5] = {0.0};      //Inductor 2 Current error array
float m2_i[5] = {0.0};      //Inductor 2 Current PID output array

int vloop_cnt = 0;          //count variable for the outer voltage loop

int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    phase_shift(60);
    
    while (1)
    {
        // Add your application code
    }

    return 1;
}
/**
 End of File
*/

