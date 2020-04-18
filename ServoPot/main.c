/*SERVO MOTOR
     //P3.5 -> Pino do PWM
 */



#include <msp430.h>
#include "libclock/clock.h"
#include <stdint.h>

uint16_t a0;

void main() {

    WDTCTL = WDTPW | WDTHOLD;               // Trava o watchdog
    PM5CTL0 &= ~LOCKLPM5;                   // Destrava os pinos digitais

    clockInit();                            // Inicializa o sistema de clocks
                                            // ACLK @32768Hz, SMCLK @1MHz,
                                            // MCLK @16MHz


    // PINO DO PWM
    P3SEL1 &= ~BIT5;
    P3SEL0 |= BIT5;
    P3DIR |= BIT5;


    /* Configuração servo motor*/
    TB0CTL = TBSSEL__SMCLK | MC__UP | TBCLR;
    TB0CCTL4 = OUTMOD_7;
    TB0CCR0 = 20000-1;
    TB0CCR4 = 510;   //0°: 500 e 180°: 2548; dt: 8 por passo; sao 256 passos (8 bits)


    /*Configuração da luminosidade do LED*/
    TA1CTL = TASSEL__ACLK | MC__UP | TACLR;
    TA1CCR0 = 326;
    TA1CCR1 = 163;
    TA1CCTL1 = OUTMOD_7;



    ADC12CTL0 &= ~ADC12ENC;              // Zerar o enable para configurar a interface

    ADC12CTL0 = ADC12SHT0_1    |        //  8 ADC12CLK cycles
                ADC12ON;                 // Liga o ADC

    ADC12CTL1 = ADC12SSEL_1    |        // Seleciona o clock ACLK
                ADC12SHP       |        // Ativa o Sample-Timer
                ADC12SHS_4     |        // TA1.1
                ADC12CONSEQ_2;           // Seleciona o modo de operacao -> Repeated - sequences

    ADC12CTL2 = ADC12RES__8BIT;         // Numero de batida para converter o sinal

    ADC12CTL3 = ADC12CSTARTADD_0;        // Começar na MEM0

    P3SEL0 |= (BIT0|BIT1);             // P3.0 -> A12

    // Seleciona canais e ativa EOS no último
    ADC12MCTL0  = ADC12INCH_12|ADC12EOS;

    ADC12CTL0 |= ADC12ENC;

    /* Loop infinito */
    while(1){
        while(!(ADC12IFGR0 & ADC12IFG0));

        a0 = ADC12MEM0;
        TB0CCR4  = (a0*8)+510;
    }
}


