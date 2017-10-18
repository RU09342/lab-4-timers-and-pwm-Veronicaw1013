# Software Debouncing
## Veronica Williams, October 17, 2017, Code Updated: October 17, 2017 

Debounce buttons on MSP430 using timer modules. 

## Libraries for the MSP430
Msp430.h is a general header file that includes all the header files for boards in the MSP430 family. When creating a new project there is a pull down menu that will allow you to choose which board you are actually using. 

## General Format

The watchdog timer must be turned off for the five boards, and all of outputs were declared with their proper PxDIR ("0" sets an input and "1" sets an output), PxSEL ("0" is I/O function and "1" is peripheral module function), and PxOUT (when I/O is configured to output mode: 0b = output is low, 1b = output is high). PxOUT can also set whether the resistor is a pullup (1) or pulldown (0) resistor. PxREN (pullup/pulldown resistor enabled), PxIE (interrupt enable), PxIES ("0" is rising edge, "1" is falling edge), and PxIFG ("0" clears interrupt flag, "1" sets the interrupt) also had to be used as the button will be used for the interrupts. The inputs and outputs for each board can be seen later. For the MSP430G2553, MSP430FR2311, and MSPF5529 the LED that is receiving the PWM had to be jumped to another pin on the board that interracts with a timer as the regular LED pin is not connected to a CCRx register. 

 WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
 
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

For the MSP430FR2311, MSP430FR5994, and MSP430FR6989, the GPIO power-on default high-impedance mode must also be disabled within the main function.

PM5CTL0 &= ~LOCKLPM5;

The timer also had to be declared and set so that it will function properly in the interrupt. Tx0CTL determines which clock source will be used as well as what mode will be used (up, up down, or continuous). In this case ACLK, which has a frequency of 32 kHz was used in continuous mode for each board. Tx0CCTL0 enables capture/compareinterrupts timer x CCR0, and Tx0CCTL1 enables interrupts for timer x CCRn, in this case the timer is set to set/reset mode. The output will set when it reaches the value of CCR1 and reset when it reaches the value of CCR0. The value for the CCR0 register was set to 1000, which allowed for the button to not bounce and there wasn't a huge delay between button presses. Timer A was used for all of the boards except for the MSP430FR2311, which uses Timer B (TB0CTL, TB0CCTL0, TB0CCR0).

TA0CCR0 = 1000;                  // Sets CCR0 value to avoid bouncing

TA0CTL = TASSEL_1 + MC_0 + TACLR; // ACLK, Stopped, clear timer

TA0CCTL0 = CCIE;                  // capture/compare interrupt enable
         
In order to enable general interrupts the next line must be added at the end of the main function.The low power mode can be changed depending on the developer's needs.
 
_BIS_SR(GIE);

An interrupt occurs when the button is pressed. 

#pragma vector=PORT1_VECTOR      // if the button is pressed...

__interrupt void Port_1(void)
  
While the button is being pressed down,the LED is toggled, the interrupt is disabled for that pin, the interrupt flag is cleared , and the timer is set to up mode. All of these changes put together allows for some time in between interrupts and to avoid debouncing. 

{
    P1OUT ^=  BIT0;      // toggle LED
    
    P1IE  &= ~BIT1;   // Disables interrupt
    
    P1IFG &= ~BIT1;   // clears flag
    
    TB0CTL |= BIT4;       // Sets timer to up-mode
}

Once the value of CCR0 is reached, another interrupt is triggered. This interrupt renables interrupts for this pin, stops and resets the timer, and clears the interrupt flag. This "resets" the system to be ready for another button interrupt. 

#pragma vector=TIMER0_B0_VECTOR

__interrupt void Timer_B (void) {


    P1IE  |= BIT1;  // Enables the interrupt
    
    TB0CTL &= ~BIT4;    // Stops
    
    TB0CTL |= BIT2;     // Resets the clock
    
    P1IFG &= ~BIT1; // Clears interrupt flag
    
}



## Specific Code for Each Board
### MSP430G2553 where pins 1.6 and 1.0 are LEDs, pin 1.3 is a button-

Configure LEDs as outputs: P1DIR |= BIT2; P1DIR |= BIT0; 

Start with LEDs off: P1OUT &= !(BIT2 and BIT6);

Configure button as input: P1DIR &= ~BIT3; P1REN |= BIT3; P1OUT |= BIT3;

Configure button to interrupt: P1IE |= BIT3; P1IFG &= ~BIT3; P1IES ^= BIT3; (To flip interrupt to rising or falling edge)

Toggle LEDs: P1OUT ^= BIT6; P1OUT^=BIT2;

### MSP430FR6989 where pins 9.7 and 1.0 are LEDs, pin 1.1 is a button-

Configure LEDs as outputs: P9DIR |= BIT7; P1DIR |= BIT0;  

Start with LEDs off: P1OUT &= !(BIT0); P9OUT &= !(BIT0);

Configure button as input: P1DIR &= ~BIT1; P1REN |= BIT1; P1OUT |= BIT1;

Configure button to interrupt: P1IE |= BIT1; P1IFG &= ~BIT1; P1IES ^= BIT1; (To flip interrupt to rising or falling edge)

Toggle LEDs: P9OUT ^= BIT7; P1OUT^=BIT0;

### MSP430FR5994 where pin 1.1 and 1.0 are LEDs, pin 5.5 is a button-

Configure LEDs as outputs:  P1DIR |= BIT1; P1DIR |= BIT0;  

Start with LEDs off: P1OUT &= !(BIT1 and BIT0);

Configure button as input: P5DIR &= ~BIT5; P5REN |= BIT5; P5OUT |= BIT5;

Configure button to interrupt: P5IE |= BIT6; P5IFG &= ~BIT6; P5IES ^= BIT6; (To flip interrupt to rising or falling edge)

Toggle LEDs: P1OUT ^= BIT1; P1OUT^=BIT0;

### MSP430FR2311 where pin 1.0 and 2.0 are LEDs, pin 1.1 is a button-

Configure LEDs as outputs:  P2DIR |= BIT0; P2DIR |= BIT0; 

Start with LEDs off: P2OUT &= !(BIT0); P2OUT &= !(BIT0);

Configure button as input: P1DIR &= ~BIT1; P1REN |= BIT1; P1OUT |= BIT1;

Configure button to interrupt: P1IE |= BIT1; P1IFG &= ~BIT1; P1IES ^= BIT1; (To flip interrupt to rising or falling edge)

Toggle LEDs: P2OUT ^= 0x01; P2OUT^=BIT0;

### MSP430FR5529 where pin 1.2 and pin 4.7 are LEDs, pin 2.1 is a button-
Configure LEDs as outputs:  P1DIR |= BIT2;   P4DIR |= BIT7;  

Start with LEDs off: P1OUT &= !(BIT2); P4OUT &= !(BIT7);

Configure button as input: P2DIR &= ~BIT1; P2REN |= BIT1; P2OUT |= BIT1;

Configure button to interrupt: P2IE |= BIT1; P2IFG &= ~BIT1; P2IES ^= BIT1; (To flip interrupt to rising or falling edge)

Toggle LEDs: P1OUT ^= BIT2; P4OUT^=BIT7;

## Extra Work

Two buttons were debounced on the MSP430F5529. Admittedly, this was one of the easier boards to set up for debouncing two buttons as the buttons are on two separate ports. All that had to be done was replicate the code for the first button and change the pin from the first button to the second button. 

## References 

https://github.com/RU09342/lab-4-timers-and-pwm-nickgorab/blob/master/Debouncing/ALL_EXCEPT_2311/buttonDebouncing.c
