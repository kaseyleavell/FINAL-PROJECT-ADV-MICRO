#include <msp430.h>


/**
 * Final Project main.c
 */
#define LED 0x04;
#define SLEEP; __delay_cycles(700);

int sample = 0;
int outVal = 0;
unsigned int adc;
int fired = 0;
int i = 0;
int startInc = 0;

void readADC();
void startComm();
void setLine();
void stopComm();

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE; // ADC10ON, interrupt enabled
    ADC10CTL1 = INCH_0 + CONSEQ2;						// input A1
    ADC10AE0 |= 0x01;                         // PA.1 ADC option select
    P1OUT &=~ 0xFF;
    P1DIR &=~ 0x03;
    //I2C initialization
    P2DIR |= 0xFF;

    //beginning comm timer

    CCR0 = 4;

    //pwm timer
    P2DIR |= LED;
    P2SEL |= LED;
    P2SEL2 &=~ LED;
    TA1CCR0 = 26;        //Pwm period which equates to 38 kHz
    TA1CCR1 = 13;
    TA1CTL = TASSEL_2 + MC_1;
    TA1CCTL1 = OUTMOD_7;    //start the PWM capabilities on LED
    TACTL = TASSEL_2 + MC_1;
    __enable_interrupt();
    ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
    while(1)
    {
    	if(sample == 1)
    	{
    		startComm();
    		sample = 0;
    	}
    }
}
// ADC10 interrupt service routine
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
	readADC();
}
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    if(i<10)
    {
    	 setLine();		//set the line to
    }else
    {
    	i = 0;
    	outVal = 0;
    	stopComm();
    }
}
void startComm()
{
    //activate comm timer after pulling line high
	int garbageBit[10] = {1,0,1,0,1,0,1,0,1,0};
    CCTL0 = CCIE;       //start the timer interrupt for TA0
    //clock off starting 10-bits
}
void setLine()
{
    //the value of pushVal corresponds to the PWM to send
    if(outVal & 0x0200)
    {
    	//turn on P2DIR
    	P2DIR |= LED;
    }else
    {
    	P2DIR &=~ LED;
    }
    i++;
    outVal = outVal << 1;
}
void readADC()
{
    outVal = ADC10MEM;
    sample = 1;
}
void stopComm()
{
	CCTL0 &=~ CCIE;
	P2DIR &=~ LED;
	startInc = 0;
}
