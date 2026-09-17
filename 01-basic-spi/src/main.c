/*
 * 01 - SPI master de baza
 *
 * PIC18F8722 transmite codurile pentru cifre prin SPI. Valoarea
 * receptionata in acelasi transfer este afisata pe PORTD.
 */
#include <xc.h>
#define SPI_CLK RC3
#define SPI_SDI RC4
#define SPI_SDO RC5
#define _XTAL_FREQ 4000000
unsigned char data1;

void SPI_write(unsigned char data){
SSPBUF = data;
while(!SSPSTATbits.BF);}

char SPI_read(unsigned char data){
SSPBUF = data;
while(!SSPSTATbits.BF);
return SSPBUF;}

void SPI_init(){
TRISCbits.SPI_CLK = 0;
TRISCbits.SPI_SDO = 0;
TRISCbits.SPI_SDI = 1;}

void SPI_open(){
//configurare ceas Fosc/64 - 65KHz
SSPM3 = 0;
SSPM2 = 0;
SSPM1 = 1;
SSPM0 = 0;
//biti pentru nivel
CKE = 1;
CKP = 0;
//bit pt esantionare
SMP = 0;
SSPEN = 1;}

void SPI_close(){
SSPEN = 0;}

void main(void){
TRISD  = 0x0;
unsigned ch[]={0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
SPI_init();
SPI_open();
et1:
for(int i=0;i<=9;i++){
SPI_write(ch[i]);
data1 = SPI_read(ch[i]);
for(int i=0;i<=99;i++){
__delay_ms(10);}
LATD  = data1;}

goto et1;
SPI_close();
   while (1){}
 }
