#include <msp430.h>

int main(void){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;     //disable high impedance mode
    P9SEL0 &= ~(BIT7);       // sets port mux
    P1SEL0 &= ~(BIT0);       // sets port mux
    P9DIR |= BIT7;    // pin 9.7 as an output
    P1DIR |= BIT0;    // pin 1.0 as an output
    P9OUT &= ~BIT7;

    P1DIR &= ~BIT1;   // pin 1.1 as an input
    P1REN |= BIT1;    // pin 1.1 resistor enable
    P1OUT |= BIT1;    // pullup resistor

    P1IE |= BIT1;   // pin 1.1 enable interrupt
    P1IES |= BIT1;  //interrupt on falling edge
    P1IFG &= ~BIT1; //clear flag
    TB0CTL= MC_1  + TBSSEL_2 + ID_1; // up timer, SMCLK, div 2
    TB0CCTL1 = (CCIE);  // capture/compare interrupt enable
    TB0CCTL0 = (CCIE);
    TB0CCR0= 500-1;        // max value timer will count up to
    TB0CCR1= 250;          // 50% duty cycle at start
    _BIS_SR(LPM0_bits + GIE);        // Enter LPM0 with global interrupt enable
}

#pragma vector=TIMER0_B1_VECTOR
__interrupt void Timer0_B1_ISR (void){
    if(TB0CCR1==500){
    }
    else{
        P1OUT &= ~BIT0; // LED off
    }
    TB0CCTL1&=~BIT0;    // clear flag
    }
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR (void){
    if(TB0CCR1 != 0){
    P1OUT |= BIT0;       // LED on
    }
    TB0CCTL0 &=~BIT0;    //clears flags
}
#pragma vector=PORT1_VECTOR      // if the button is pressed...
__interrupt void Port_1(void){
    P1IE &= ~BIT1;              //disable interrupt
    _delay_cycles(50000);       // wait...
    P1IE |= BIT1;               //re-enable interrupt
    P1IES ^= BIT1;              // flip interrupt edge
    if (~P1IES & BIT1){           //if interrupt entered on posedge
        P9OUT ^= BIT7;              // flip other led
        TB0CCR1 += 50;              // increase duty by 10%
        if(TB0CCR1 == 550)
            TB0CCR1 = 0;            // stay on if at 100%
    }
    else if (P1IES & BIT1){      // executes on the negedge
        P9OUT &= ~BIT7;
    }
    P1IFG &= ~BIT1; //clear interrupt flag
}
