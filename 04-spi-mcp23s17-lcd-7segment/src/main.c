/*
 * 04 - SPI + MCP23S17 + LCD + 7-segment
 *
 * Este exemplul cel mai complex din familia SPI: MCP23S17 controleaza
 * LCD-ul, iar SPI este folosit si pentru transmiterea codurilor de 7-segment.
 */
#include <xc.h>
#include <string.h>
#define IODIRA_address 0x00
#define IODIRB_address 0x01
#define GPIOA_address 0x12
#define GPIOB_address 0x13
#define CS PORTAbits.RA2
#define    SPI_CLK    RC3     
#define    SPI_SDI    RC4
#define    SPI_SDO    RC5 
#define _XTAL_FREQ 4000000
#define linie1_inc 0x80
#define linie2_inc 0xC0


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

void write_mesaj (char c[], int linie, int coloana){
for (int i=0;i<=strlen(c);i++){
LCD_out(c[i], linie, coloana++);}
}
void write_mes(char *c, int linie, int coloana){
while(*c)
LCD_out(*c++, linie, coloana++);}

void main(void){
TRISD  = 0x0;
int linie=1;
int coloana=0;
unsigned char ch[]={"Mesaj SPI SPI SPI SPI SPI "};
unsigned char ch2[]={'0','1','2','3','4','5','6','7','8','9'};
unsigned ch1[]={0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
int sir=strlen(ch);
MCP_init();
SPI_init();
SPI_open();
LCD_init();
//write_mes(ch,1,0);
for (int i=0;i<=sir;i++){
if(i>16)
LCD_cmd(0b00011000); //shiftare la coloana 
LCD_out(ch[i], linie, coloana++);}
for(int i=0;i<=9;i++){
SPI_write(ch1[i]);
data1 = SPI_read(ch1[i]);
for(int i=0;i<=99;i++){
__delay_ms(10);}
LATD  = data1;}
for (int i=0;i<=9;i++){
LCD_out(ch2[i],1,5);
for(int i=0;i<=99;i++){
__delay_ms(5);}
}
SPI_close();
while(1){}
}
