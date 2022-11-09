#include <SPI.h>
#include"Ap_29demo.h"
//IO settings
int BUSY_Pin = D0; 
int RES_Pin = D1; 
int DC_Pin = D2; 
int CS_Pin = D4; 
//HSCLK---D5
//HMOSI--D7
#define EPD_W21_CS_0 digitalWrite(CS_Pin,LOW)
#define EPD_W21_CS_1 digitalWrite(CS_Pin,HIGH)
#define EPD_W21_DC_0  digitalWrite(DC_Pin,LOW)
#define EPD_W21_DC_1  digitalWrite(DC_Pin,HIGH)
#define EPD_W21_RST_0 digitalWrite(RES_Pin,LOW)
#define EPD_W21_RST_1 digitalWrite(RES_Pin,HIGH)
#define isEPD_W21_BUSY digitalRead(BUSY_Pin)
////////FUNCTION//////
void driver_delay_us(unsigned int xus);
void driver_delay_xms(unsigned long xms);
void DELAY_S(unsigned int delaytime);     
void SPI_Delay(unsigned char xrate);
void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char command);
void EPD_W21_WriteCMD(unsigned char command);
//EPD
void EPD_W21_Init(void);
void EPD_init(void);
void PIC_display(const unsigned char* picData_old,const unsigned char* picData_new);
void EPD_sleep(void);
void EPD_refresh(void);
void lcd_chkstatus(void);
void PIC_display_Clean(void);
unsigned char HRES,VRES_byte1,VRES_byte2;
void Sys_run(void)
{
   ESP.wdtFeed(); //Feed dog to prevent system reset
  }

void LED_run(void)
{
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);   // Turn the LED on (Note that LOW is the voltage level
  delay(500);
  }


void setup() {
   pinMode(BUSY_Pin, INPUT); 
   pinMode(RES_Pin, OUTPUT);  
   pinMode(DC_Pin, OUTPUT);    
   pinMode(CS_Pin, OUTPUT);    
   //SPI
   SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0)); 
   SPI.begin ();  
}
//Tips//
/*When the electronic paper is refreshed in full screen, the picture flicker is a normal phenomenon, and the main function is to clear the display afterimage in the previous picture.
  When the local refresh is performed, the screen does not flash.*/
/*When you need to transplant the driver, you only need to change the corresponding IO. The BUSY pin is the input mode and the others are the output mode. */

void loop() {
  while(1)
  {
    //PICTURE1
    EPD_init(); //EPD init
    PIC_display(gImage_black1,gImage_red1);//EPD_picture1
    EPD_refresh();//EPD_refresh   
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    delay(6000);
    
    
    //PICTURE Clean
    EPD_init(); //EPD init
    PIC_display_Clean();
    EPD_refresh();//EPD_refresh   
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    while(1) 
    {
     Sys_run();//System run
     LED_run();//Breathing lamp
    }
  }


}
/////////////////////SPI/////////////////////////////////////
void SPI_Write(unsigned char value)                                    
{                                                           
    SPI.transfer(value);

}

void EPD_W21_WriteCMD(unsigned char command)
{
  EPD_W21_CS_0;                   
  EPD_W21_DC_0;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}
void EPD_W21_WriteDATA(unsigned char command)
{
  EPD_W21_CS_0;                   
  EPD_W21_DC_1;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}



/////////////////EPD settings Functions/////////////////////
void EPD_W21_Init(void)
{
  EPD_W21_RST_0;    // Module reset
  delay(10); //At least 10ms
  EPD_W21_RST_1;
  delay(10);  
}
void EPD_init(void)
{
    HRES=0x80;            //128
    VRES_byte1=0x01;      //296
    VRES_byte2=0x28;
  
    EPD_W21_Init(); //Electronic paper IC reset
  
    EPD_W21_WriteCMD(0x04);  
    lcd_chkstatus();//waiting for the electronic paper IC to release the idle signal

    EPD_W21_WriteCMD(0x00);     //panel setting
    EPD_W21_WriteDATA(0x0f);    //LUT from OTP--128x296
    EPD_W21_WriteDATA(0x89);    //Temperature sensor, boost and other related timing settings

    EPD_W21_WriteCMD(0x61);     //resolution setting
    EPD_W21_WriteDATA (HRES);          
    EPD_W21_WriteDATA (VRES_byte1);   
    EPD_W21_WriteDATA (VRES_byte2);

    EPD_W21_WriteCMD(0X50);     //VCOM AND DATA INTERVAL SETTING      
    EPD_W21_WriteDATA(0x77);    //WBmode:VBDF 17|D7 VBDW 97 VBDB 57   WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
}
void EPD_refresh(void)
{
    EPD_W21_WriteCMD(0x12);     //DISPLAY REFRESH   
    delay(1);          //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();
} 
void EPD_sleep(void)
{
    EPD_W21_WriteCMD(0X50);  //VCOM AND DATA INTERVAL SETTING  
    EPD_W21_WriteDATA(0xf7);
       
    EPD_W21_WriteCMD(0X02);   //power off
    lcd_chkstatus();
    EPD_W21_WriteCMD(0X07);   //deep sleep
    EPD_W21_WriteDATA(0xA5);
}


void PIC_display(const unsigned char* picData_old,const unsigned char* picData_new)
{
    unsigned int i;
    EPD_W21_WriteCMD(0x10);        //Transfer old data
    for(i=0;i<4736;i++)      
  {
    EPD_W21_WriteDATA(pgm_read_byte(&picData_old[i]));
  }
  
    EPD_W21_WriteCMD(0x13);        //Transfer new data
    for(i=0;i<4736;i++)      
  {
    EPD_W21_WriteDATA(pgm_read_byte(&picData_new[i]));
  }
}
void PIC_display_Clean(void)
{
    unsigned int i;
    EPD_W21_WriteCMD(0x10);        //Transfer old data
    for(i=0;i<4736;i++)      
  {
    EPD_W21_WriteDATA(0xff);
  }
  
    EPD_W21_WriteCMD(0x13);        //Transfer new data
    for(i=0;i<4736;i++)      
  {
    EPD_W21_WriteDATA(0xff);
  }
}
void lcd_chkstatus(void)
{
  while(1)
  {   //=0 BUSY
     if(isEPD_W21_BUSY==1) break;
     ESP.wdtFeed(); //Feed dog to prevent system reset
  }                  
}
//////////////////////////////////END/////////////////////////////////////////
