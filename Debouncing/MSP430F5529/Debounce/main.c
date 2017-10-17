#include <msp430.h>

int state = 0;



void main(void)

{

    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer



    P1DIR |= BIT0;//set P1.0 as output

    P4DIR |= BIT7;//set P1.6

    P2DIR &= ~BIT1;//set P5.5 as input

    P2REN |= BIT1;//enables the pull up resistor at P5.5

    P2OUT |= BIT1;//set pull up resistor at P5.5

    P2IE |= BIT1;//enable interrupt

    P2IES |= BIT1;//toggle edge

    P2IFG &= ~BIT1;//clear flag





    TA0CCTL0 = CCIE;

    TA0CCR0 = 30000;



    __enable_interrupt();

    __bis_SR_register(LPM0 + GIE); // LPM0 with interrupts enabled

}



#pragma vector= PORT2_VECTOR

__interrupt void PORT_2(void)

{

    TA0CTL = TASSEL_2 + MC_1 + TACLR + ID_2;//start the timer

    if(state == 0)

    {

        P1OUT ^= BIT0;

        P2IFG &= BIT1;

        P4OUT &= ~BIT7;

    }

    else

    {

        P2IFG &= ~BIT1;

        P4OUT |= BIT7;

    }

    P2IE &= ~BIT1;//stop P1.3 from inputting

}



// Timer A0 interrupt service routine

#pragma vector = TIMER0_A0_VECTOR

__interrupt void Timer_A (void)

{



    if(state == 0)

    {

        P2IES &= ~BIT1;

        state = 1;

    }

    else if(state == 1)

    {

        P2IFG &= ~BIT1;

        P2IES |= BIT1;

        state = 0;

    }



    P2IE |= BIT1;

    TA0CTL &= ~TASSEL_2;

    TA0CTL |= TACLR;



}
