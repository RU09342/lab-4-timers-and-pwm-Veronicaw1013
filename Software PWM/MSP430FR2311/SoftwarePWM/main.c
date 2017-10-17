#include <msp430.h>

int main(void){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;     //disable high impedance mode
    P2SEL0 &= ~(BIT0);       // sets port mux
    P1SEL0 &= ~(BIT0);       // sets port mux
    P2DIR |= BIT0;    // set pin 2.0 as an output
    P1DIR |= BIT0;    // set pin 1.0 as an output
    P2OUT &= ~BIT0;

    P1DIR &= ~BIT1;   // set pin 1.1 as an input
    P1REN |= BIT1;    // Enables pull up or pull down resistor on pin 1.1
    P1OUT |= BIT1;    // Selects pullup resistor

    P1IE |= BIT1;   // interrupt enable on pin 1.1
    P1IES |= BIT1;  //set interrupt to falling edge
    P1IFG &= ~BIT1; // clear interrupt flag
    TB0CTL= MC_1  + TBSSEL_2 + ID_1; //up timer, SMCLK, div 2
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
        P2OUT ^= BIT0;              // flip other led
        TB0CCR1 += 50;              // increase duty by 10%
        if(TB0CCR1 == 550)
            TB0CCR1 = 0;            // stay on if at 100%
    }
    else if (P1IES & BIT1){      // executes on the negedge
        P2OUT &= ~BIT0;
    }
    P1IFG &= ~BIT1; //clear interrupt flag
}
