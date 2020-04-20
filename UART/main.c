#include <msp430.h>
#include "libclock/clock.h"
#include <stdint.h>

void UART_Send(uint8_t byte);
int8_t UART_Get(uint8_t data[]);
void UART_config(uint16_t br, uint8_t isMSB, uint8_t par, uint8_t nStops);

uint8_t byteRX[1];


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5;

	UART_config(9600, 0, 2, 1);

    P1DIR |= (BIT1|BIT0); // LEDs
    P1OUT &= ~BIT1;
    P1OUT &= ~BIT0;


	while (1){

	    UART_Get(byteRX);

	    if (byteRX[0] == 'A') {
	        P1OUT |= BIT1;
	        P1OUT &= ~BIT0;
        }
	    else  {
	        P1OUT &= ~BIT1;
	        P1OUT |= BIT0;
        }
	}
}

void UART_config(uint16_t br, uint8_t isMSB, uint8_t par, uint8_t nStops){
    uint32_t brdiv;

    UCA3CTLW0 = UCSWRST;           // Setar para come�ar a configura��o

    UCA3CTLW0 |= (isMSB? UCMSB:0)        |  // MSB ou LSB
                 (par? UCPAR:0)          |  // Tem paridade?
                 (par==2? UCPAR__EVEN:0) |  // Paridade par==2/impar==1
                 (nStops==2? UCSPB_1:0)  |  // Numero de paradas 2stops==2
                 UCSSEL__SMCLK;             // Seleciona o clock -> SMCLK

    // MODO DE BAIXA FREQU�NCIA
    brdiv = (1000000<<3)/br;
    UCA3BRW = brdiv>>3;
    UCA3MCTLW = (brdiv & 0x07)<<1;


    // Configura os pinos TX(P6.0) e RX(P6.1)
    P6SEL1 &= ~(BIT0 | BIT1);
    P6SEL0 |= BIT0 | BIT1;

    UCA3CTLW0 &= ~UCSWRST;              // Resetar para terminar a configura��o
}

void UART_Send(uint8_t byte){

    while(!(UCA3IFG & UCTXIFG));
    UCA3TXBUF = byte;
}

int8_t UART_Receive(uint8_t data[]){

    TA0CTL  = TASSEL__ACLK | MC__UP | ID__4 | TACLR;
    TA0CCR0 = 0xFFFF; // 8seg

    while(!(UCA3IFG & UCRXIFG) && !(TA0CTL & TAIFG));
    if(TA0CTL & TAIFG) return -1;
    data[0] = UCA3RXBUF;
    return 0;

}

