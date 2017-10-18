#include <msp430.h>
int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
        P1DIR |= BIT2;  //Pin 1.2 is jumped to the LED header output
         P1SEL |= BIT2;  //pin set to TA0.1
         P2DIR &= ~BIT1;  //set P2.1 as input
         P2REN |= BIT1;  //pin 2.1 resistor enable
         P2OUT |= BIT1;  //pullup resistor
         P2IE |= BIT1;   //pin 2.1 enable interrupt
         P2IES |= BIT1;  //interrupt on falling edge
         P2IFG &= ~BIT1;  //clear flag

         P4DIR |= BIT7;    //pin 4.7 as an output
         P4SEL &= ~(BIT7);  //sets I/O
         P4OUT &= ~BIT7;    //pin 4.7 initially off

         TA0CCR0  = 500-1;      //sets max value timer will count to
         TA0CCTL1 = OUTMOD_7;    //timer in set/reset mode
         TA0CCR1 = 250; // 50% duty cycle
         TA0CTL = MC_1  + TASSEL_2 + ID_1; // up timer, SMCLK, div 2
         __bis_SR_register(GIE);       // global interrupt enable
         __no_operation(); // For debugger
}
#pragma vector=PORT2_VECTOR // button interrupt
__interrupt void Port_2(void){
    P2IE &= ~BIT1;  // disable interrupt
    _delay_cycles(50000);   //delay
    P2IE |= BIT1;               //reenable interrupt
    P2IES ^= BIT1;  //interrupt changes to rising edge
    if (~P2IES & BIT1){           // if button is pressed
        P4OUT ^= BIT7;              // turn on second LED
        TA0CCR1 += 50;              // increase duty by 10%
        if(TA0CCR1 == 550)  //if maximum value is exceeded
            TA0CCR1 = 0;            // reset CCR1
    }
    else if (P2IES & BIT1){      // if button is released
        P4OUT &= ~BIT7; // turn off second LED
    }
    P2IFG &= ~BIT1; //clear interrupt flag
}
