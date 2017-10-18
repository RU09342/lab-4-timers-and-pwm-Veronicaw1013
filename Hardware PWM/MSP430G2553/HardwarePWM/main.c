#include <msp430.h>
int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
        P1DIR |= BIT2;  //Pin 1.2 is jumped to the LED header output
         P1SEL |= BIT2;  //pin set to TA0.1
         P1DIR &= ~BIT3;  //set P1.3 as input
         P1REN |= BIT3;  //pin 1.3 resistor enable
         P1OUT |= BIT3;  //pullup resistor
         P1IE |= BIT3;   //pin 1.3 enable interrupt
         P1IES |= BIT3;  //interrupt on falling edge
         P1IFG &= ~BIT3;  //clear flag

         P1DIR |= BIT6;    //pin 1.6 as an output
         P1SEL &= ~(BIT6);  //sets I/O
         P1OUT &= ~BIT6;    //pin 1.6 initially off

         TA0CCR0  = 500-1;      //sets max value timer will count to
         TA0CCTL1 = OUTMOD_7;    //timer in set/reset mode
         TA0CCR1 = 250; // 50% duty cycle
         TA0CTL = MC_1  + TASSEL_2 + ID_1; // up timer, SMCLK, div 2
         __bis_SR_register(GIE);       // global interrupt enable
         __no_operation(); // For debugger
}
#pragma vector=PORT1_VECTOR // button interrupt
__interrupt void Port_1(void){
    P1IE &= ~BIT3;  // disable interrupt
    _delay_cycles(50000);   //delay
    P1IE |= BIT3;               //reenable interrupt
    P1IES ^= BIT3;  //interrupt changes to rising edge
    if (~P1IES & BIT3){           // if button is pressed
        P1OUT ^= BIT6;              // turn on second LED
        TA0CCR1 += 50;              // increase duty by 10%
        if(TA0CCR1 == 550)  //if maximum value is exceeded
            TA0CCR1 = 0;            // reset CCR1
    }
    else if (P1IES & BIT3){      // if button is released
        P1OUT &= ~BIT6; // turn off second LED
    }
    P1IFG &= ~BIT3; //clear interrupt flag
}
