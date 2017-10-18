# Hardware PWM
## Veronica Williams, October 16, 2017, Code Updated: October 16, 2017 

A 1kHz PWM signal on an LED is produced with a 50% duty cycle on startup. With each button press the duty cycle increases by 10% until it hits 100% and it goes back down to 0%. A second LED is used to show if the button has triggered multiple interrupts. 

## Libraries for the MSP430
Msp430.h is a general header file that includes all the header files for boards in the MSP430 family. When creating a new project there is a pull down menu that will allow you to choose which board you are actually using. 

## General Format

The watchdog timer must be turned off for the five boards, and all of outputs were declared with their proper PxDIR ("0" sets an input and "1" sets an output), PxSEL ("0" is I/O function and "1" is peripheral module function), and PxOUT (when I/O is configured to output mode: 0b = output is low, 1b = output is high). PxOUT can also set whether the resistor is a pullup (1) or pulldown (0) resistor. PxREN (pullup/pulldown resistor enabled), PxIE (interrupt enable), PxIES ("0" is rising edge, "1" is falling edge), and PxIFG ("0" clears interrupt flag, "1" sets the interrupt) also had to be used as the button will be used for the interrupts. The inputs and outputs for each board can be seen later. For the MSP430G2553, MSP430FR2311, and MSPF5529 the LED that is receiving the PWM had to be jumped to another pin on the board that interracts with a timer as the regular LED pin is not connected to a CCRx register. 

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


For the MSP430FR2311, MSP430FR5994, and MSP430FR6989, the GPIO power-on default high-impedance mode must also be disabled within the main function.

PM5CTL0 &= ~LOCKLPM5;

The timer also had to be declared and set so that it will function properly in the interrupt. Tx0CTL determines which clock source will be used as well as what mode will be used (up, up down, or continuous). In this case SMCLK, which has a frequency of 1 MHz was used along with a divider of two, making the frequency 500 kHz. Up mode was also used for each board. Tx0CCTL0 enables capture/compareinterrupts timer x CCR0, and Tx0CCTL1 enables interrupts for timer x CCRn, in this case the timer is set to set/reset mode. The output will set when it reaches the value of CCR1 and reset when it reaches the value of CCR0. The value for the CCR0 register was set to 500-1, which is the clock source (500 kHz) divided by 1kHz. TA0CCR1 was set to 250 as that will produce a 50% duty cycle. Timer A was used for all of the boards except for the MSP430FR2311, which uses Timer B (TB0CTL, TB0CCTL0, TB0CCR0).

TA0CCR0  = 500-1;      //sets max value timer will count to

TA0CCTL1 = OUTMOD_7;    //timer in set/reset mode

TA0CCR1 = 250; // 50% duty cycle

TA0CTL = MC_1  + TASSEL_2 + ID_1; // up timer, SMCLK, div 2
         
In order to enable general interrupts the next line must be added at the end of the main function.The low power mode can be changed depending on the developer's needs.
 
_BIS_SR(GIE);

An interrupt occurs when the button is pressed. 

#pragma vector=PORT1_VECTOR      // if the button is pressed...

__interrupt void Port_1(void)
  
While the button is being pressed down, the second LED is turned on to show that the interrupt has been flagged. The interrupt is disabled to make sure that the button doesn't bounce. The value of CCR1 is incremented by 50 in order to increase the duty cycle by 10%. If the value of CCR1 reaches beyond the maximum value, or CCR0 = 500, then the value of CCR1 is set to zero which resets the duty cycle back to 0%.  Inside of the interrupt the interrupt is disabled for 50000 cycles so the button does not bounce. The interrupt is then renabled and the interrupt edge is flipped the the rising edge. This makes it so the interrupt will be flagged again when the button is released. When the button is released the second LED is turned off to show that the interrupt is no longer flagged. The interrupt flag is then cleared to be ready for the next button press.

{
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
