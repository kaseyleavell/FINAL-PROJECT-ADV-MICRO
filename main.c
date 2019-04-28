#include <msp430.h>


/**
 * Final Project main.c
 */
#define LED 0x04;
#define SLEEP; __delay_cycles(4000);

int sample = 0;
unsigned volatile int outVal = 0;
unsigned int adc;
int fired = 0;
int i = 0;
int temp = 0;
int temp1 = 0;
int temp2 = 0;
int startInc = 0;

void readADC();
void startComm();
void setLine();
void stopComm();

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

	ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE; // ADC10ON, interrupt enabled
	ADC10CTL1 = INCH_0;						// input A1
	ADC10AE0 |= 0x01;                         // PA.1 ADC option select
	P1OUT &=~ 0xFF;
	P1DIR &=~ 0x03;
	//I2C initialization
	P2DIR |= 0xFF;

	//beginning comm timer

	CCR0 = 200;
	//pwm timer
	//outVal = 726;
	P2DIR |= LED;
	P2SEL |= LED;
	P2SEL2 &=~ LED;
	TA1CCR0 = 26;        //Pwm period which equates to 38 kHz
	TA1CCR1 = 0;
	TA1CCTL1 = OUTMOD_7;    //start the PWM capabilities on LED
	TA1CTL = TASSEL_2 + MC_1;
	TACTL = TASSEL_2 + MC_1;
	__enable_interrupt();
	ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
	//CCTL0 = CCIE;
	while(1)
	{
		if(sample == 1)
		{
			sample = 0;
			startComm();

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
	if(i<11)
	{
		if(outVal & 0x0400)
		{
			//turn on P2DIR
			TA1CCR1 = 13;
		}else
		{
			TA1CCR1 = 0;
		}
		i++;
		outVal = outVal << 1;
	}
	if(i==11)
	{
		i = 0;
		//outVal = 0;
		//testing multiple byte sending
		stopComm();
	}
}
void startComm()
{
	//activate comm timer after pulling line high
	outVal |= 0x0400;
	//clock on starting 10-bits
	CCTL0 = CCIE;
}
void stopComm()
{
	CCTL0 &=~ CCIE;
	SLEEP;
	TA1CCR1 = 0;
	outVal = 0;
	ADC10CTL0 |= ADC10IE;
	ADC10CTL0 |= ENC + ADC10SC;
//	if(startInc == 3)
//	{
//		SLEEP;
//	}
}
void readADC()
{
	ADC10CTL0 &=~ ADC10IFG;
	ADC10CTL0 &=~ ADC10IE;
	outVal = ADC10MEM;
//	if(startInc == 0){
//		temp = outVal;
//	}if(startInc == 1)
//	{
//		temp1 = outVal;
//	}if(startInc == 2)
//	{
//		temp2 = outVal;
//	}
	sample = 1;
	startInc++;
}
