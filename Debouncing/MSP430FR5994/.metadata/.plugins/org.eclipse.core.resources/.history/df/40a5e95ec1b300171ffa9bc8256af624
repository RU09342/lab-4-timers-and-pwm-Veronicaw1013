#include <msp430.h>

int state = 0;



void main(void)

{

    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer



    P1DIR |= BIT0;//set P1.0 as output

    P1DIR |= BIT1;//set P1.6

    P5DIR &= ~BIT6;//set P5.5 as input

    P5REN |= BIT6;//enables the pull up resistor at P5.5

    P5OUT |= BIT6;//set pull up resistor at P5.5

    P5IE |= BIT6;//enable interrupt

    P5IES |= BIT6;//toggle edge

    P5IFG &= ~BIT6;//clear flag





    TA0CCTL0 = CCIE;

    TA0CCR0 = 30000;



    __enable_interrupt();

    __bis_SR_register(LPM0 + GIE); // LPM0 with interrupts enabled

}



#pragma vector= PORT5_VECTOR

__interrupt void PORT_5(void)

{

    TA0CTL = TASSEL_2 + MC_1 + TACLR + ID_2;//start the timer

    if(state == 0)

    {

        P1OUT ^= BIT0;

        P5IFG &= BIT6;

        P1OUT &= ~BIT1;

    }

    else

    {

        P5IFG &= ~BIT6;

        P1OUT |= BIT1;

    }

    P5IE &= ~BIT6;//stop P1.3 from inputting

}



// Timer A0 interrupt service routine

#pragma vector = TIMER0_A0_VECTOR

__interrupt void Timer_A (void)

{



    if(state == 0)

    {

        P5IES &= ~BIT6;

        state = 1;

    }

    else if(state == 1)

    {

        P5IFG &= ~BIT6;

        P5IES |= BIT6;

        state = 0;

    }



    P5IE |= BIT6;

    TA0CTL &= ~TASSEL_2;

    TA0CTL |= TACLR;



}