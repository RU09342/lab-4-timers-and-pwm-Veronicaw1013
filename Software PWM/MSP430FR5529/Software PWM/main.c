#include <msp430.h> 
unsigned int j = 0;
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings
    P1DIR |= BIT0;                          // Set P1.0 to output direction
    P1OUT &= ~BIT0;                         // Switch LED off
   // P1DIR |= BIT1;                          // Set P1.0 to output direction
    P1OUT &= ~BIT1;
    P1DIR  &= ~BIT1;                        // Set P1.1 as input
    P1OUT |= BIT1;                          // Configure P1.1 for Pull-Up
    P1REN |= BIT1;                          // Enable Pull Up of P1.1
    TA0CCTL1 = OUTMOD_7;                    // Reset/Set Mode
    TA0CTL = TASSEL_2 + MC_1 +TACLR ;       // SMCLK / Upmode
    TA0CCR0 = 100-1;                        // PWM Frequency 10 kHz
    TA0CCR1 = 50;                           // 50% Duty Cycle
 //   P1SEL0 |= BIT0;                         // PWM output to LED P1.0
    P1SEL &= BIT0;

while(1)
{
    if(!(P1IN & BIT1))
    {
//    P1OUT|= BIT1;
__delay_cycles(500);
if(TA0CCR1 <= 90)
{
TA0CCR0 = 0;
TA0CCR1 += 10;
TA0CCR0 = 100;
}
if(TA0CCR1 >= 100)
{
TA0CCR0 = 0;
TA0CCR1 = 10;
TA0CCR0 = 100;
}
}
for(j=100;j>0;j--);
{
__delay_cycles(1000);
}

}
}
