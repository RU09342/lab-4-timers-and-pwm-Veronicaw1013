#include <msp430.h>
int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P1SEL |= BIT0;         // sets port mux for timer output
    P1SEL &= ~BIT6;             // sets port mux
    P1SEL &= ~BIT3;             // sets port mux
    P1DIR |= BIT0;
    P1DIR |= BIT6;    // Sets pin 1.0 and pin 9.7 as an output
    P1OUT &= ~BIT6;    // Sets pull up to VCC
    P1DIR &= ~BIT3;   // sets pin 1.1 as an input
    P1REN |= BIT3;    // Enables pull up resistor on P1.1
    P1OUT |= BIT3;    // Sets pull up to VCC
    /* INTERRUPT INIT */
    P1IE |= BIT3;
    P1IES |= BIT3;
    P1IFG &= ~BIT3;
    TA0CTL= (MC_1  + TASSEL_1);            //up timer, Aclk
    TA0CCTL1 = (OUTMOD_7);
    TA0CCR0= 500;                           // sets maximum timer value
    TA0CCR1= 250;
    _BIS_SR(CPUOFF + GIE);                  // Enter LPM0 w/ interrupt
}
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    P1IE &= ~BIT3;
    _delay_cycles(50000);
    P1IE |= BIT3;               //re-enable interrupt
    P1IES ^= BIT3;
    if (~P1IES & BIT3){           //if interrupt entered on posedge
        P1OUT ^= BIT6;              // flip led
        TA0CCR1 += 50;              // increase duty by 10%
        if(TA0CCR1 == 550)
            TA0CCR1 = 0;            // stay on if at 100%
    }
    else if (P1IES & BIT3){      // executes on the negedge
        P1OUT ^= BIT6;
    }
    P1IFG &= ~BIT3;
}