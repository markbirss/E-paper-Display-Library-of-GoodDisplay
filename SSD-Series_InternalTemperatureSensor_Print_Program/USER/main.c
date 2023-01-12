#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "Ap_29demo.h"
 

void lcd_chkstatus(void);
unsigned char  EPD_W21_ReadDATA(void);
void Epaper_READBUSY(void);
void read_temperture(void);
unsigned char HV_flag,version;
u16 tempvalue;
void driver_delay_us(unsigned int xus)
{
	for(;xus>1;xus--);
}
																		  
void driver_delay_xms(unsigned long xms)	
{	
    unsigned int i = 0 , j=0;
    for(j=0;j<xms;j++)
	{
        for(i=0; i<256; i++);
    }
}



 int main(void)
 {		
	unsigned int i;
	delay_init();	    	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	uart_init(115200);	 //Baud rate 115200

	GPIO_Configuration(); //SPI  init
  EPD_W21_Init();


	 while(1)
	 {
		 		EPD_W21_Init();
		

	read_temperture();	 
	printf("temperature£º%d\r\n",tempvalue); //Print the current temperature value
	delay_ms(500);	 
		 
		 


		 
		 
	 }	 
 }
void Epaper_READBUSY(void)
{ 
  while(1)
  {	 //=1 BUSY
     if(isEPD_W21_BUSY==0) break;;
  }  
}	 
void read_temperture(void)
 {

   unsigned char temp1,temp2;	
   EPD_W21_WriteCMD(0x18);
   EPD_W21_WriteDATA(0X80);
   EPD_W21_WriteCMD(0x22);
   EPD_W21_WriteDATA(0XB1);
   EPD_W21_WriteCMD(0x20);
   Epaper_READBUSY();
   
   EPD_W21_WriteCMD(0x1B);
	 GPIO_IO(0); 
    temp1=EPD_W21_ReadDATA();  //temp1£ºD11 D10 D9 D8 D7 D6 D5 D4
    temp2=EPD_W21_ReadDATA();  //temp2£ºD3  D2  D1 D0 0  0  0  0
   

   tempvalue=temp1<<8|temp2;  //Synthesize a 16 bit data
	//D11=0£¬DEC=+ tempvalue/16;  //D11=1,DEX=-tempvalue/16; 
	 tempvalue=tempvalue>>4;   //Move 4 bits to the right to become 12 bits of valid data
	 tempvalue=tempvalue/16; //Output temperature value                           
	 GPIO_IO(1); 

 }
