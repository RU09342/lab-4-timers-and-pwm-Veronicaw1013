#include <msp430.h>
int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;  //disable high impedance mode
    P1SEL0 |= BIT0;         // sets port mux for timer output
    P9SEL0 &= ~BIT7;             // sets port mux
    P1SEL0 &= ~BIT1;             // sets port mux
    P1DIR |= BIT0;		// Sets pin 1.0 and pin 9.7 as an output
    P9DIR |= BIT7;    
    P9OUT &= ~BIT7;    // pin 9.7 initially off
    P1DIR &= ~BIT1;   // sets pin 1.1 as an input
    P1REN |= BIT1;    //pin 1.1 resistor enable
    P1OUT |= BIT1;    //pullup resistor
   
    P1IE |= BIT1; //pin 1.1 enable interrupt
    P1IES |= BIT1;//interrupt on falling edge
    P1IFG &= ~BIT1; //clear flag
    TA0CTL= MC_1  + TASSEL_2 + ID_1; //up timer, SMCLK, div 2
    TA0CCTL1 = (OUTMOD_7); //timer in set/reset mode
    TA0CCR0= 500;   // sets maximum timer value
    TA0CCR1= 250; // 50% duty cycle
    _BIS_SR(CPUOFF + GIE); // Enter LPM0 w/ interrupt
}
#pragma vector=PORT1_VECTOR   // button interrupt
__interrupt void Port_1(void){
    P1IE &= ~BIT1;     // disable interrupt
    _delay_cycles(50000);	//delay
    P1IE |= BIT1;               //re-enable interrupt
    P1IES ^= BIT1;				//interrupt changes to rising edge
    if (~P1IES & BIT1){           // if button is pressed
        P9OUT ^= BIT7;              // turn on second LED
        TA0CCR1 += 50;              // increase duty by 10%
        if(TA0CCR1 == 550)			//if maximum value is exceeded
            TA0CCR1 = 0;            // reset CCR1
    }
    else if (P1IES & BIT1){      // if button is released
        P9OUT ^= BIT7;			// turn off second LED
    }
    P1IFG &= ~BIT1;				//clear interrupt flag
}
