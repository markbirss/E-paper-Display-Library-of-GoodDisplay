#include <SPI.h>
#include"Ap_29demo.h"
//IO settings
int BUSY_Pin = A14; 
int RES_Pin = A15; 
int DC_Pin = A16; 
int CS_Pin = A17; 
//SCLK--GPIO23
//MOSI---GPIO18

#define EPD_W21_CS_0 digitalWrite(CS_Pin,LOW)
#define EPD_W21_CS_1 digitalWrite(CS_Pin,HIGH)
#define EPD_W21_DC_0  digitalWrite(DC_Pin,LOW)
#define EPD_W21_DC_1  digitalWrite(DC_Pin,HIGH)
#define EPD_W21_RST_0 digitalWrite(RES_Pin,LOW)
#define EPD_W21_RST_1 digitalWrite(RES_Pin,HIGH)
#define isEPD_W21_BUSY digitalRead(BUSY_Pin)
////////FUNCTION//////
void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char command);
void EPD_W21_WriteCMD(unsigned char command);
//EPD
void EPD_W21_Init(void);
void EPD_init(void);
void EPD_sleep(void);
void EPD_refresh(void);
void lcd_chkstatus(void);
void PIC_display_Clean(void);
void PIC_display (const unsigned char* picData_BW,const unsigned char* picData_R);
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
    EPD_init(); //EPD init
    PIC_display(gImage_BW1,gImage_R1);//EPD_picture1
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    delay(2000); //2s  
    
    
    //PICTURE Clean
    EPD_init(); //EPD init
    PIC_display_Clean();
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    while(1);

  }


}

//////////////////////SPI///////////////////////////////////

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
    delay(10);//At least 10ms delay 
    EPD_W21_RST_1;
    delay(10);//At least 10ms delay 
}
void EPD_init(void)
{
    EPD_W21_Init();     //Electronic paper IC reset 
  
    EPD_W21_WriteCMD(0x01);     //POWER SETTING
    EPD_W21_WriteDATA (0x07);
    EPD_W21_WriteDATA (0x07);    //VGH=20V,VGL=-20V
    EPD_W21_WriteDATA (0x3f);   //VDH=15V
    EPD_W21_WriteDATA (0x3f);   //VDL=-15V
    //Enhanced display drive(Add 0x06 command)
    EPD_W21_WriteCMD(0x06);     //Booster Soft Start 
    EPD_W21_WriteDATA (0x17);
    EPD_W21_WriteDATA (0x17);   
    EPD_W21_WriteDATA (0x28);   
    EPD_W21_WriteDATA (0x17); 

    EPD_W21_WriteCMD(0x04); //POWER ON
    delay(100);  
    lcd_chkstatus();        //waiting for the electronic paper IC to release the idle signal
  
    EPD_W21_WriteCMD(0X00);     //PANNEL SETTING
    EPD_W21_WriteDATA(0x0F);   //KW-3f   KWR-2F BWROTP 0f BWOTP 1f

    EPD_W21_WriteCMD(0x61);         //tres      
    EPD_W21_WriteDATA (0x03);   //source 800
    EPD_W21_WriteDATA (0x20);
    EPD_W21_WriteDATA (0x01);   //gate 480
    EPD_W21_WriteDATA (0xE0);
  
    EPD_W21_WriteCMD(0X15);   
    EPD_W21_WriteDATA(0x00);    

    EPD_W21_WriteCMD(0X50);     //VCOM AND DATA INTERVAL SETTING
    EPD_W21_WriteDATA(0x11);
    EPD_W21_WriteDATA(0x07);

    EPD_W21_WriteCMD(0X60);     //TCON SETTING
    EPD_W21_WriteDATA(0x22);
}
void EPD_refresh(void)
{
    EPD_W21_WriteCMD(0x12);     //DISPLAY REFRESH   
    delay(10);          //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();
} 
void EPD_sleep(void)
{
    EPD_W21_WriteCMD(0X02);   //power off
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal
    delay(100);      //Power off time delay   
    EPD_W21_WriteCMD(0X07);   //deep sleep
    EPD_W21_WriteDATA(0xA5);
}

void PIC_display (const unsigned char* picData_BW,const unsigned char* picData_R)
{
    unsigned int i;
    EPD_W21_WriteCMD(0x10);      
    for(i=0;i<48000;i++)       
     EPD_W21_WriteDATA(~pgm_read_byte(&picData_BW[i])); 
   
    EPD_W21_WriteCMD(0x13);      
    for(i=0;i<48000;i++)       
    EPD_W21_WriteDATA(pgm_read_byte(&picData_R[i]));     
        //Refresh
    EPD_W21_WriteCMD(0x12);   //DISPLAY REFRESH   
    delay(10);    //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal 

}
void PIC_display_Clean(void)
{
    unsigned int i;
    EPD_W21_WriteCMD(0x10);        //Transfer old data
    for(i=0;i<48000;i++)       
  {
    EPD_W21_WriteDATA(0xff);
  }
  
    EPD_W21_WriteCMD(0x13);        //Transfer new data
    for(i=0;i<48000;i++)       
  {
    EPD_W21_WriteDATA(0x00);
  }
     //Refresh
    EPD_W21_WriteCMD(0x12);   //DISPLAY REFRESH   
    delay(10);    //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal

}
void lcd_chkstatus(void)
{
  while(!isEPD_W21_BUSY);   //0:BUSY, 1:FREE                     
}
