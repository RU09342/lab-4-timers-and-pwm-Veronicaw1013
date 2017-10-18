#include <msp430.h>

void button(void) {
  P1DIR |= ~BIT3;   // Sets Pin 1.1 as an input
  P1REN |=  BIT3;   // Pin 1.1 resistor enable
  P1OUT |=  BIT3;   // pullup resistor
  P1IE  |=  BIT3;   // pin 1.1 interrupt enable
  P1IES |= ~BIT3;   // interrupt on falling edge
  P1IFG &= ~BIT3;   // clear flag
}

void LED(void) {
  P1DIR  |= BIT0;
}

void main(void) {


    WDTCTL = WDTPW + WDTHOLD;  // stop watchdog timer

    button();
    LED();

    TA0CCR0 = 1000;                  // Sets CCR0 value to avoid bouncing
    TA0CTL = TASSEL_1 + MC_0 + TACLR; // SMCLK, continuous, Stopped
    TA0CCTL0 = CCIE;                  // capture/compare interrupt enable

  _BIS_SR(LPM0_bits + GIE);   // Enables Low Power Mode 0 and Global Interrupt
}


#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    P1OUT ^=  BIT0;      // toggle LED
    P1IE  &= ~BIT3;   // Disables interrupt
    P1IFG &= ~BIT3;   // clears flag
    TA0CTL |= BIT4;       // Sets timer to up-mode
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {


    P1IE  |= BIT3;  // Enables the interrupt
    TA0CTL &= ~BIT4;    // Stops
    TA0CTL |= BIT2;     // Resets the clock
    P1IFG &= ~BIT3; // Clears interrupt flag
}
