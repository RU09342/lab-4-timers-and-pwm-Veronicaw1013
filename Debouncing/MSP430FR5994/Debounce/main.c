#include <msp430.h>

void button(void) {
  P5DIR |= ~BIT6;   // Sets Pin 1.1 as an input
  P5REN |=  BIT6;   // Pin 1.1 resistor enable
  P5OUT |=  BIT6;   // pullup resistor
  P5IE  |=  BIT6;   // pin 1.1 interrupt enable
  P5IES |= ~BIT6;   // interrupt on falling edge
  P5IFG &= ~BIT6;   // clear flag
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
    TA0CTL = TASSEL_1 + MC_0 + TACLR; // ACLK, Stopped, clear timer 
    TA0CCTL0 = CCIE;                  // capture/compare interrupt enable

  _BIS_SR(LPM0_bits + GIE);   // Enables Low Power Mode 0 and Global Interrupt
}


#pragma vector=PORT5_VECTOR
__interrupt void Port_5(void) {
    P1OUT ^=  BIT0;      // toggle LED
    P5IE  &= ~BIT6;   // Disables interrupt
    P5IFG &= ~BIT6;   // clears flag
    TA0CTL |= BIT4;       // Sets timer to up-mode
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {


    P5IE  |= BIT6;  // Enables the interrupt
    TA0CTL &= ~BIT4;    // Stops
    TA0CTL |= BIT2;     // Resets the clock
    P5IFG &= ~BIT6; // Clears interrupt flag
}
