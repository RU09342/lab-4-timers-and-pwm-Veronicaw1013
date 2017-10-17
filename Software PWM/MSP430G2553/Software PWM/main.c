#include <msp430.h>

int main(void){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    P1SEL &= ~(BIT6);       // sets port mux
    P1SEL &= ~(BIT0);       // sets port mux
    P1DIR |= BIT6;    // set pin 1.6 as an output
    P1DIR |= BIT0;    // set pin 1.0 as an output
    P1OUT &= ~BIT6;

    P1DIR &= ~BIT3;   // set pin 1.3 as an input
    P1REN |= BIT3;    // Enables pull up or pull down resistor on pin 1.3
    P1OUT |= BIT3;    // Selects pullup resistor

    P1IE |= BIT3;   // interrupt enable on pin 1.3
    P1IES |= BIT3;  //set interrupt to falling edge
    P1IFG &= ~BIT3; // clear interrupt flag
    TA0CTL= MC_1  + TASSEL_2; //up timer, SMCLK
    TA0CCTL1 = (CCIE);  // capture/control interrupt enable
    TA0CCTL0 = (CCIE);
    TA0CCR0= 500-1;        // max value timer will count up to
    TA0CCR1= 250;          // 50% duty cycle at start
    _BIS_SR(LPM0_bits + GIE);        // Enter LPM0 with global interrupt enable
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR (void){
    if(TA0CCR1==500){
    }
    else{
        P1OUT &= ~BIT0; // LED off
    }
    TA0CCTL1&=~BIT0;    // clear flag
    }
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR (void){
    if(TA0CCR1 != 0){
    P1OUT |= BIT0;       // LED on
    }
    TA0CCTL0 &=~BIT0;    //clears flags
}
#pragma vector=PORT1_VECTOR      // if the button is pressed...
__interrupt void Port_1(void){
    P1IE &= ~BIT3;              //disable interrupt
    _delay_cycles(50000);       // wait...
    P1IE |= BIT3;               //re-enable interrupt
    P1IES ^= BIT3;              // flip interrupt edge
    if (~P1IES & BIT3){           //if interrupt entered on posedge
        P1OUT ^= BIT6;              // flip other led
        TA0CCR1 += 50;              // increase duty by 10%
        if(TA0CCR1 == 550)
            TA0CCR1 = 0;            // stay on if at 100%
    }
    else if (P1IES & BIT3){      // executes on the negedge
        P1OUT ^= BIT6;
    }
    P1IFG &= ~BIT3; //clear interrupt flag
}
