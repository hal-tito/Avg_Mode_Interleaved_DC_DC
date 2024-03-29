
/**
  TMR1 Generated Driver API Source File 

  @Company
    Microchip Technology Inc.

  @File Name
    tmr1.c

  @Summary
    This is the generated source file for the TMR1 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for driver for TMR1. 
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.170.0
        Device            :  PIC24FJ64GA002
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.61
        MPLAB             :  MPLAB X v5.45
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

// General Thumb sucked tuning rule
//ki largely affects settling time.
//ki largely affects system stability.
//Increase ki, and increase kp to be at least 3.5*ki
//If you reach unstable point, kp down
//kp = 2*ki
//Speed wise, system is sensitive to ki
//Stability wise, system is more sensitive to kp
//kp = 0.0003; ki = 0.000075
//As loop execution time decreases, coefficients have to increase
//As loop execution time increases, coefficients have to decrease


#define     kp_v        0.045256            //Voltage PID proportional component
#define     ki_v        0.025228            //Voltage PID integral component
#define     kd_v        0.0                 //Voltage PID derivative component
#define     kp1_i       0.002256             //Inductor 1 Current PID proportional component
#define     ki1_i       0.000828             //Inductor 1 Current PID integral component
#define     kd1_i       0.0                 //Inductor 1 Current PID derivative component
#define     kp2_i       0.002256             //Inductor 2 Current PID proportional component
#define     ki2_i       0.000828             //Inductor 2 Current PID integral component
#define     kd2_i       0.0                 //Inductor 2 Current PID derivative component
#define     MAX_DUTY    159.0               //Controller max duty cycle value
#define     MAX_CURRENT 18.0                //Maximum Inductor current
#define     cntrl_max   0.8                 //Maximum duty cycle 0.8 out of 1.0
#define     cntrl_min   0.0                 //Minimum duty cycle is 0.0

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "mcc.h"
#include "tmr1.h"

/**
 Section: File specific functions
*/
void (*TMR1_InterruptHandler)(void) = NULL;
void TMR1_CallBack(void);

/**
  Section: Data Type Definitions
*/

/** TMR Driver Hardware Instance Object

  @Summary
    Defines the object required for the maintenance of the hardware instance.

  @Description
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None.
*/

typedef struct _TMR_OBJ_STRUCT
{
    /* Timer Elapsed */
    volatile bool           timerElapsed;
    /*Software Counter value*/
    volatile uint8_t        count;

} TMR_OBJ;

static TMR_OBJ tmr1_obj;

/**
  Section: Driver Interface
*/

void TMR1_Initialize (void)
{
    //TMR1 0; 
    TMR1 = 0x00;
    //Period = 0.00013 s; Frequency = 16000000 Hz; PR1 2079; 
    PR1 = 0x81F;
    //TCKPS 1:1; TON enabled; TSIDL disabled; TCS FOSC/2; TSYNC disabled; TGATE disabled; 
    T1CON = 0x8000;

    if(TMR1_InterruptHandler == NULL)
    {
        TMR1_SetInterruptHandler(&TMR1_CallBack);
    }

    IFS0bits.T1IF = false;
    IEC0bits.T1IE = true;
	
    tmr1_obj.timerElapsed = false;

}


void __attribute__ ( ( interrupt, no_auto_psv ) ) _T1Interrupt (  )
{

    extern float Vref;
    extern float Vactual;
    extern float Iref1;
    extern float Iactual1;
    extern float Iref2;
    extern float Iactual2;
    
    extern float Iref_tot;
    extern int vloop_cnt;
    
    extern float e_v[5];
    extern float m_v[5];

    extern float e1_i[5];
    extern float m1_i[5];
    
    extern float e2_i[5];
    extern float m2_i[5];

    /* Check if the Timer Interrupt/Status is set */

    //***User Area Begin
    //Sensor stuff
//    Vactual = ((float)ADC1BUF0/1023.0)*5.0;
//    Vactual = Vactual*((1200+20)/20);
    Vactual = (float)ADC1BUF0 * 0.2981427175;
//    Iactual = ((float)ADC1BUF1/1023.0)*5.0;
//    Iactual = (2.5 - Iactual)/0.1;
    Iactual1 = 25 - 0.04887585532*(float)ADC1BUF1;
    Iactual2 = 25 - 0.04887585532*(float)ADC1BUF2;
    //Sensor stuff ends
    
    Iref1 = Iref_tot/2;
    Iref2 = Iref_tot/2;   
    
    m1_i[1] = m1_i[0];
    e1_i[2] = e1_i[1];
    e1_i[1] = e1_i[0];
    
    e1_i[0] = Iref1 - Iactual1;    //Inductor 1 Error Calculation
    
    m1_i[0] = (kp1_i + ki1_i + kd1_i)*e1_i[0] - (kp1_i + 2*kd1_i)*e1_i[1] + kd1_i*e1_i[2] + m1_i[1];
    
    
    //Limiting of the duty cycle for inductor 1
    if(m1_i[0] >= cntrl_max){
        m1_i[0] = cntrl_max;
    }
    else if(m1_i[0] <= cntrl_min){
        m1_i[0] = cntrl_min;
    }
    else{
        m1_i[0] = m1_i[0];
    }
    
    OC1RS = (int)(MAX_DUTY*m1_i[0]);        //Updating the duty cycle 1
   
    
    m2_i[1] = m2_i[0];
    e2_i[2] = e2_i[1];
    e2_i[1] = e2_i[0];
    
    e2_i[0] = Iref2 - Iactual2;    //Inductor 2 Error Calculation
    
    m2_i[0] = (kp2_i + ki2_i + kd2_i)*e2_i[0] - (kp2_i + 2*kd2_i)*e2_i[1] + kd2_i*e2_i[2] + m2_i[1];
    
    
    //Limiting of the duty cycle for inductor 2
    if(m2_i[0] >= cntrl_max){
        m2_i[0] = cntrl_max;
    }
    else if(m2_i[0] <= cntrl_min){
        m2_i[0] = cntrl_min;
    }
    else{
        m2_i[0] = m2_i[0];
    }
    
    OC2RS = (int)(MAX_DUTY*m2_i[0]);        //Updating the duty cycle 2
    
    
    vloop_cnt++;
//    vloop_cnt = 0;
    if(vloop_cnt == 20){
        vloop_cnt = 0;

        m_v[1] = m_v[0];
        e_v[2] = e_v[1];
        e_v[1] = e_v[0];
    
        e_v[0] = Vref - Vactual;    //Voltage Error Calculation
    
        m_v[0] = (kp_v + ki_v + kd_v)*e_v[0] - (kp_v + 2*kd_v)*e_v[1] + kd_v*e_v[2] + m_v[1];
        
        if(m_v[0] >= 1.0){m_v[0] = 1.0;}
        else if(m_v[0] <= 0.0){m_v[0] = 0.0;}
        else{m_v[0] = m_v[0];}
        
        Iref_tot = MAX_CURRENT*m_v[0];  //Limiting the reference current value
        
    }
    
    // ticker function call;
    // ticker is 1 -> Callback function gets called everytime this ISR executes
    if(TMR1_InterruptHandler) 
    { 
           TMR1_InterruptHandler(); 
    }

    //***User Area End

    tmr1_obj.count++;
    tmr1_obj.timerElapsed = true;
    IFS0bits.T1IF = false;
}

void TMR1_Period16BitSet( uint16_t value )
{
    /* Update the counter values */
    PR1 = value;
    /* Reset the status information */
    tmr1_obj.timerElapsed = false;
}

uint16_t TMR1_Period16BitGet( void )
{
    return( PR1 );
}

void TMR1_Counter16BitSet ( uint16_t value )
{
    /* Update the counter values */
    TMR1 = value;
    /* Reset the status information */
    tmr1_obj.timerElapsed = false;
}

uint16_t TMR1_Counter16BitGet( void )
{
    return( TMR1 );
}


void __attribute__ ((weak)) TMR1_CallBack(void)
{
    // Add your custom callback code here
}

void  TMR1_SetInterruptHandler(void (* InterruptHandler)(void))
{ 
    IEC0bits.T1IE = false;
    TMR1_InterruptHandler = InterruptHandler; 
    IEC0bits.T1IE = true;
}

void TMR1_Start( void )
{
    /* Reset the status information */
    tmr1_obj.timerElapsed = false;

    /*Enable the interrupt*/
    IEC0bits.T1IE = true;

    /* Start the Timer */
    T1CONbits.TON = 1;
}

void TMR1_Stop( void )
{
    /* Stop the Timer */
    T1CONbits.TON = false;

    /*Disable the interrupt*/
    IEC0bits.T1IE = false;
}

bool TMR1_GetElapsedThenClear(void)
{
    bool status;
    
    status = tmr1_obj.timerElapsed;

    if(status == true)
    {
        tmr1_obj.timerElapsed = false;
    }
    return status;
}

int TMR1_SoftwareCounterGet(void)
{
    return tmr1_obj.count;
}

void TMR1_SoftwareCounterClear(void)
{
    tmr1_obj.count = 0; 
}

/**
 End of File
*/
