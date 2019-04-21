#include <msp430.h>


/**
 * Final Project main.c
 */
#define LED 0x04;
#define SLEEP; __delay_cycles(700);

int factor = 1;
int rowOut;
unsigned int adc;
int fired = 0;
int outArray[10]= {0};
char dispArray[4] = {0};

int temp,temp2,i,avg_adc;

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

    CCR0 = 1000;


    //pwm timer
    P2DIR |= LED;
    P2SEL |= LED;
    P2SEL2 &=~ LED;
    TA1CCR0 = 26;        //Pwm period which equates to 38 kHz
    TA1CCR1 = 1;
    TA1CTL = TASSEL_2 + MC_1;

    __enable_interrupt();
    while(1)
    {
        readADC();
    }
}
// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{

}
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    __disable_interrupt();
    setLine();
    __enable_interrupt();
}
void startComm()
{
    //activate comm timer after pulling line high
    CCTL0 = CCIE;       //start the timer interrupt for TA0
    TACTL = TASSEL_2 + MC_1;
    TA1CCTL1 = OUTMOD_7;    //start the PWM capabilities on LED
    TA1CCR1 = 1;
}
void setLine()
{
    //the value of pushVal corresponds to the PWM to send
    TA1CCR1 = adc*factor;       //the direct ADC value times a factor for PWM
}
void stopComm()
{
    //pull line low???
}
void readADC()
{
    ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
    __enable_interrupt();
    adc = ADC10MEM;
    startComm();
}
