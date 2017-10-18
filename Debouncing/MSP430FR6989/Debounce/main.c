#include <msp430.h>

void button(void) {
  P1DIR |= ~BIT1;   // Sets Pin 1.1 as an input
  P1REN |=  BIT1;   // Pin 1.1 resistor enable
  P1OUT |=  BIT1;   // pullup resistor
  P1IE  |=  BIT1;   // pin 1.1 interrupt enable
  P1IES |= ~BIT1;   // interrupt on falling edge
  P1IFG &= ~BIT1;   // clear flag
}

void LED(void) {
  P1DIR  |= BIT0;
}

void main(void) {


    WDTCTL = WDTPW + WDTHOLD;  // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;  //disable high impedance mode

    button();
    LED();

    TA0CCR0 = 10000;                  // Sets CCR0 value to avoid bouncing
    TA0CTL = TASSEL_1 + MC_0 + TACLR; // SMCLK, continuous, Stopped
    TA0CCTL0 = CCIE;                  // capture/compare interrupt enable

  _BIS_SR(LPM0_bits + GIE);   // Enables Low Power Mode 0 and Global Interrupt
}


#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    P1OUT ^=  BIT0;      // toggle LED
    P1IE  &= ~BIT1;   // Disables interrupt
    P1IFG &= ~BIT1;   // clears flag
    TA0CTL |= BIT4;       // Sets timer to up-mode
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {


    P1IE  |= BIT1;  // Enables the interrupt
    TA0CTL &= ~BIT4;    // Stops
    TA0CTL |= BIT2;     // Resets the clock
    P1IFG &= ~BIT1; // Clears interrupt flag
}