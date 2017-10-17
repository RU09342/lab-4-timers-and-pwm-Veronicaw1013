#include <msp430.h>
int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;  // adjust power settings
    P1SEL0 |= BIT0;         // sets port mux for timer output
    P2SEL0 &= ~BIT0;             // sets port mux
    P1SEL0 &= ~BIT1;             // sets port mux
    P1DIR |= BIT0;
    P2DIR |= BIT0;    // Sets pin 1.0 and pin 9.7 as an output
    P2OUT &= ~BIT0;    // Sets pull up to VCC
    P1DIR &= ~BIT1;   // sets pin 1.1 as an input
    P1REN |= BIT1;    // Enables pull up resistor on P1.1
    P1OUT |= BIT1;    // Sets pull up to VCC
    /* INTERRUPT INIT */
    P1IE |= BIT1;
    P1IES |= BIT1;
    P1IFG &= ~BIT1;
    TB0CTL= (MC_1  + TBSSEL_1);            //up timer, Aclk
    TB0CCTL1 = (OUTMOD_7);
    TB0CCR0= 500;                           // sets maximum timer value
    TB0CCR1= 250;
    _BIS_SR(CPUOFF + GIE);                  // Enter LPM0 w/ interrupt
}
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    P1IE &= ~BIT1;
    _delay_cycles(50000);
    P1IE |= BIT1;               //re-enable interrupt
    P1IES ^= BIT1;
    if (~P1IES & BIT1){           //if interrupt entered on posedge
        P2OUT ^= BIT0;              // flip led
        TB0CCR1 += 50;              // increase duty by 10%
        if(TB0CCR1 == 550)
            TB0CCR1 = 0;            // stay on if at 100%
    }
    else if (P1IES & BIT1){      // executes on the negedge
        P2OUT ^= BIT0;
    }
    P1IFG &= ~BIT1;
}
