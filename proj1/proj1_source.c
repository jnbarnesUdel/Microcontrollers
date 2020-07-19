/*===================================CPEG222=================================
* Program: Project 1
* Authors: Jonathan Barnes, Joshua Chevalier
* Team #:36
* Date: 9/28/2017
* Description: This is a program that lights up led's sequentially with variable delays,
 * delay controlled by pot, direction controlled by butons
=============================================================================*/
#include <xc.h>
#include <plib.h>

#include "btn.h"
#include "utils.h"
#include "lcd.h"
#include <stdio.h>
#include "aic.h"
void delay_ms(int ms){
    int i,counter;
    for (counter=0; counter< ms; counter ++){
        for(i =0;i<70;i++){}   //software delay 1
       }
}


int LD_position = 0x01;
#define FOSC 80E6 
    #define CORE_TICK_PERIOD (FOSC/1000)
int d = 0;
int delay;


int main(void) {
    DDPCONbits.JTAGEN = 0;
    PORTSetPinsDigitalOut(IOPORT_A, BIT_7 | BIT_6 | BIT_5 | BIT_4 | BIT_3 | BIT_2 | BIT_1 | BIT_0);
    #define led0 LATAbits.LATA0
    #define led1 LATAbits.LATA1
    #define led2 LATAbits.LATA2
    #define led3 LATAbits.LATA3
    #define led4 LATAbits.LATA4
    #define led5 LATAbits.LATA5
    #define led6 LATAbits.LATA6
    #define led7 LATAbits.LATA7
    led0 = led1 = led2 = led3 = led4 = led5 = led6 = led7 = 0;
    
    #define btnl PORTBbits.RB0
    #define btnr PORTBbits.RB8
    #define btnc PORTFbits.RF0
    #define btnu PORTBbits.RB1
    #define btnd PORTAbits.RA15 
    #define pot PORTBbits.RB2 
    PORTSetPinsDigitalIn(IOPORT_B, BIT_0|BIT_8|BIT_2|BIT_1);
    PORTSetPinsDigitalIn(IOPORT_F, BIT_0);
    PORTSetPinsDigitalIn(IOPORT_A, BIT_15);
    btnl = btnr = btnc = btnu = btnd = 0;
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();
     OpenCoreTimer(CORE_TICK_PERIOD); 
     
      mConfigIntCoreTimer(CT_INT_ON | CT_INT_PRIOR_2); 
       INTEnableSystemMultiVectoredInt();
    
        LCD_Init(); 
       
        while(1)
        {
       delay = (pot * 990)/1023 + 10;
        char strMsg[80];   
        LCD_WriteStringAtPos("Team:36", 0, 0);
        sprintf(strMsg, "Delay: %04d ms", delay);
        LCD_WriteStringAtPos(strMsg,1,0);
        }
    
}


void __ISR(_CORE_TIMER_VECTOR, IPL2) CoreTimerHandler(void) {     // clear the interrupt flag     
    mCTClearIntFlag(); 
    
    // .. things to do 
    unsigned short buttonLock= 0; 
    // Debouncing variable  lo
    if(btnl && !buttonLock) //Check if BTN1 is pressed and buttonLock=0
    {
        d = 1;
        led0 = led1 = led2 = led3 = led4 = led5 = led6 = led7 = 0;
        LD_position = 0x01;
        buttonLock= 1; //Set debouncing variable high by ?locking? it
    }
    else if(btnr && !buttonLock) //Check if BTN1 is pressed and buttonLock=0
    {
        d = 2;
        led0 = led1 = led2 = led3 = led4 = led5 = led6 = led7 = 0;
        LD_position = 0x80;
        buttonLock= 1; //Set debouncing variable high by ?locking? it
    }
    else if(btnc && !buttonLock) //Check if BTN1 is pressed and buttonLock=0
    {
        d = 0;
        
        buttonLock= 1; //Set debouncing variable high by ?locking? it
    }
    else if(btnu && !buttonLock) //Check if BTN1 is pressed and buttonLock=0
    {
        d = 3;
        led0 = led1 = led2 = led3 = led4 = led5 = led6 = led7 = 0;
        LD_position = 0x01;
        buttonLock= 1; //Set debouncing variable high by ?locking? it
    }
    else if(btnd && !buttonLock) //Check if BTN1 is pressed and buttonLock=0
    {
        d = 4;
        
        led0 = led1 = led2 = led3 = led4 = led5 = led6 = led7 = 1;
        LD_position = 0x80;
        buttonLock= 1; //Set debouncing variable high by ?locking? it
    }
    else if(!btnl &&!btnr && !btnc && !btnu && !btnd && buttonLock)//BTN1 is off and the button is ?locked?
    {
        delay_ms(100); //Wait about 100 for switch to settle 
        buttonLock= 0;// ?unlock? the button
    }
    
    if(d == 1) //left
    {
        LD_position = LD_position <<1;
        if(LD_position > 128)
        {
            LD_position = 0x01;
        }
        LATA = LD_position;
    }
    if(d == 2) //right
    {
        LD_position = LD_position >>1;
        if(LD_position == 0)
        {
            LD_position = 0x80;
        }
        LATA = LD_position;
    }
    if(d == 3) //up
    {
        LD_position = (LD_position << 1) +1;
        if(LD_position > 255 || LD_position == 0)
            LD_position = 0x01;
        LATA = LD_position;
    }
    if(d == 4) //down
    {
        LD_position = (LD_position >> 1) +128;
        if(LD_position == 255)
        {
           led0 = led1 = led2 = led3 = led4 = led5 = led6 = led7 = 1;
           LD_position = 0x80;
        }
        LATA = LD_position;
    }
    // update the period     from pot
    
     UpdateCoreTimer(CORE_TICK_PERIOD);
 // .. Toggle the LED    
    mPORTDToggleBits(BIT_1); 
}
