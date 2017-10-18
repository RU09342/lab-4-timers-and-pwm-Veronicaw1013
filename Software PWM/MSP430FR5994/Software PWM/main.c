 #include <msp430.h>

int main(void){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;     //disable high impedance mode
    P1SEL0 &= ~(BIT0);       // sets port mux
    P1SEL0 &= ~(BIT1);       // sets port mux
    P1DIR |= BIT0;    // set pin 1.0 as an output
    P1DIR |= BIT1;    // set pin 1.1 as an output
    P1OUT &= ~BIT1;

    P5DIR &= ~BIT6;   // set pin 5.6 as an input
    P5REN |= BIT6;    // Enables pull up or pull down resistor on pin 5.6
    P5OUT |= BIT6;    // Selects pullup resistor

    P5IE |= BIT6;   // interrupt enable on pin 5.6
    P5IES |= BIT6;  //set interrupt to falling edge
    P5IFG &= ~BIT6; // clear interrupt flag
    TB0CTL= MC_1  + TBSSEL_2 + ID_1; //up timer, SMCLK, div 2
    TB0CCTL1 = (CCIE);  // capture/c interrupt enable
    TB0CCTL0 = (CCIE);
    TB0CCR0= 500-1;        // max value timer will count up to
    TB0CCR1= 250;          // 50% duty cycle at start
    _BIS_SR(LPM0_bits + GIE);        // Enter LPM0 with global interrupt enable
}

#pragma vector=TIMER0_B1_VECTOR
__interrupt void Timer0_B1_ISR (void){
    if(TB0CCR1==500){ // if CCR1 reaches max value
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
#pragma vector=PORT5_VECTOR      // if the button is pressed...
__interrupt void Port_1(void){
    P5IE &= ~BIT6;              //disable interrupt
    _delay_cycles(50000);       // wait...
    P5IE |= BIT6;               //re-enable interrupt
    P5IES ^= BIT6;              // flip interrupt edge
    if (~P5IES & BIT6){           //if interrupt entered on posedge
        P1OUT ^= BIT1;              // flip other led
        TB0CCR1 += 50;              // increase duty by 10%
        if(TB0CCR1 == 550)
            TB0CCR1 = 0;            
    }
    else if (P5IES & BIT6){      // executes on the negedge
        P1OUT &= ~BIT1;
    }
    P5IFG &= ~BIT6; //clear interrupt flag
}
