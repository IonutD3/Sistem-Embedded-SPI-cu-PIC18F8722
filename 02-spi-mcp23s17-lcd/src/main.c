/*
 * 02 - SPI + MCP23S17 + LCD 16x2
 *
 * MCP23S17 este folosit ca expander GPIO pentru LCD.
 */
#include <xc.h>
#define IODIRA_address 0x00
#define IODIRB_address 0x01
#define GPIOA_address 0x12
#define GPIOB_address 0x13
#define CS PORTAbits.RA2
#define    SPI1_CLK    RC3     
#define    SPI1_SDO    RC5 
#define _XTAL_FREQ 4000000
#define linie1_inc 0x80
#define linie2_inc 0xC0

char SPI_read(unsigned char data){
SSPBUF = data;
while(!SSPSTATbits.BF);
return SSPBUF;}

void SPI_write(unsigned char data){
SSPBUF = data;
while(!SSPSTATbits.BF);}

void SPI_init(){
TRISCbits.SPI1_CLK = 0; 
TRISCbits.SPI1_SDO = 0;}

void SPI_open (){
//biti pt. selectarea modului de lucru al portului serial sincron
//Fosc/64
SSPM3 = 0;
SSPM2 = 0;
SSPM1 = 1;
SSPM0 = 0;
//biti pentru selectarea ceasului
CKE = 1;
CKP = 0;
//bit de esantionare
SMP = 0;
//activare port serial
SSPEN = 1;}

void SPI_close(){
SSPEN = 0;}

void MCP_init(){
TRISFbits.RF6 = 0;
RF6 = 1;
TRISAbits.RA2 = 0;
CS = 1;}

void GPIO_set (char address, char value){
CS=0;
SPI_write(0x40);
SPI_write(address);
SPI_write(value);
CS=1;}

void IODIR_set (char address,char dir){
CS=0;
SPI_write(0x40);
SPI_write(address);
SPI_write(dir);
CS=1;}

void LCD_cmd (char c){
GPIO_set (GPIOA_address,0x00);
__delay_ms(50);
GPIO_set (GPIOB_address, c);
__delay_ms(50);
GPIO_set (GPIOA_address, 0x40);
__delay_ms(50);
GPIO_set (GPIOA_address, 0x00);
__delay_ms(50);}

void LCD_init(){
IODIR_set (IODIRA_address, 0x00);
IODIR_set (IODIRB_address, 0x00);
LCD_cmd (0b00111111);
LCD_cmd (0b00001111);
LCD_cmd (0b00000001);
LCD_cmd (0b00000110);}

void LCD_out (char litera, int linie, int coloana){
if (linie==1){
LCD_cmd (linie1_inc+coloana);}
if (linie==2){
LCD_cmd (linie2_inc+coloana);}
GPIO_set (GPIOA_address,0x80);
__delay_ms(50);
GPIO_set (GPIOB_address, litera);
__delay_ms(50);
GPIO_set (GPIOA_address,0xc0);
__delay_ms(50);
GPIO_set (GPIOA_address,0x00);
__delay_ms(50);}

void main(void){
unsigned char ch[]={'0','1','2','3','4','5','6','7','8','9'};
MCP_init();
SPI_init();
SPI_open();
LCD_init();
et:
for (int i=0;i<=9;i++){
LCD_out(ch[i],1,1);
for(int i=0;i<=99;i++){
__delay_ms(5);}
}
goto et;
SPI_close();
while(1){} // Mesajul ramane afisat.
}
