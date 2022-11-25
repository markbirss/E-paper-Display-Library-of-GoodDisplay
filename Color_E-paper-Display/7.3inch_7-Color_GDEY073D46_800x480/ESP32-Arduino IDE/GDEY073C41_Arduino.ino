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
//EPD
void EPD_W21_Init(void);
void EPD_init(void);
void PIC_display(const unsigned char* picData);
void EPD_sleep(void);
void EPD_refresh(void);
void lcd_chkstatus(void);
void PIC_display_Clear(void);
void EPD_horizontal(void);
void EPD_vertical(void);
void Acep_color(unsigned char color);
void PIC_display_Clear(void);




#define EPD_WIDTH   600
#define EPD_HEIGHT  448

//8bit
#define Black   0x00  /// 000
#define White   0x11  /// 001
#define Green   0x22  /// 010
#define Blue    0x33  /// 011
#define Red     0x44  /// 100
#define Yellow  0x55  /// 101
#define Orange  0x66  /// 110
#define Clean   0x77  /// 111

//4bit
#define black   0x00  /// 000
#define white   0x01  /// 001
#define green   0x02  /// 010
#define blue    0x03  /// 011
#define red     0x04  /// 100
#define yellow  0x05  /// 101
#define orange  0x06  /// 110
#define clean   0x07  /// 111



#define PSR         0x00
#define PWRR         0x01
#define POF         0x02
#define POFS        0x03
#define PON         0x04
#define BTST1       0x05
#define BTST2       0x06
#define DSLP        0x07
#define BTST3       0x08
#define DTM         0x10
#define DRF         0x12
#define PLL         0x30
#define CDI         0x50
#define TCON        0x60
#define TRES        0x61
#define REV         0x70
#define VDCS        0x82
#define T_VDCS      0x84
#define PWS         0xE3


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
    PIC_display(gImage_11);//EPD_picture1
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    delay(3000); //3s  
    
     EPD_init(); //EPD init
    PIC_display(gImage_22);//EPD_picture1
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    delay(3000); //3s     
    //Clear
    EPD_init(); //EPD init
    PIC_display_Clear();//EPD Clear
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
    EPD_W21_Init(); //Electronic paper IC reset
 
  EPD_W21_WriteCMD(0xAA);    // CMDH
  EPD_W21_WriteDATA(0x49);
  EPD_W21_WriteDATA(0x55);
  EPD_W21_WriteDATA(0x20);
  EPD_W21_WriteDATA(0x08);
  EPD_W21_WriteDATA(0x09);
  EPD_W21_WriteDATA(0x18);

  EPD_W21_WriteCMD(PWRR);
  EPD_W21_WriteDATA(0x3F);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x32);
  EPD_W21_WriteDATA(0x2A);
  EPD_W21_WriteDATA(0x0E);
  EPD_W21_WriteDATA(0x2A);
  
  EPD_W21_WriteCMD(PSR);  
  EPD_W21_WriteDATA(0x5F);
  EPD_W21_WriteDATA(0x69);
  
  EPD_W21_WriteCMD(POFS);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x54);
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x44); 

  EPD_W21_WriteCMD(BTST1);
  EPD_W21_WriteDATA(0x40);
  EPD_W21_WriteDATA(0x1F);
  EPD_W21_WriteDATA(0x1F);
  EPD_W21_WriteDATA(0x2C);
  
  EPD_W21_WriteCMD(BTST2);
  EPD_W21_WriteDATA(0x6F);
  EPD_W21_WriteDATA(0x1F);
  EPD_W21_WriteDATA(0x16);
  EPD_W21_WriteDATA(0x25);

  EPD_W21_WriteCMD(BTST3);
  EPD_W21_WriteDATA(0x6F);
  EPD_W21_WriteDATA(0x1F);
  EPD_W21_WriteDATA(0x1F);
  EPD_W21_WriteDATA(0x22);
 
  EPD_W21_WriteCMD(0x13);    // IPC
  EPD_W21_WriteDATA(0x00);
  EPD_W21_WriteDATA(0x04);
  
  EPD_W21_WriteCMD(PLL);
  EPD_W21_WriteDATA(0x02);
  
  EPD_W21_WriteCMD(0x41);     // TSE
  EPD_W21_WriteDATA(0x00);
  
  EPD_W21_WriteCMD(CDI);
  EPD_W21_WriteDATA(0x3F);
  
  EPD_W21_WriteCMD(TCON);
  EPD_W21_WriteDATA(0x02);
  EPD_W21_WriteDATA(0x00);
  
  EPD_W21_WriteCMD(TRES);
  EPD_W21_WriteDATA(0x03);
  EPD_W21_WriteDATA(0x20);
  EPD_W21_WriteDATA(0x01); 
  EPD_W21_WriteDATA(0xE0);
  
  EPD_W21_WriteCMD(VDCS);
  EPD_W21_WriteDATA(0x1E); 

  EPD_W21_WriteCMD(T_VDCS);
  EPD_W21_WriteDATA(0x00);

  EPD_W21_WriteCMD(0x86);    // AGID
  EPD_W21_WriteDATA(0x00);
  
  EPD_W21_WriteCMD(PWS);
  EPD_W21_WriteDATA(0x2F);
 
  EPD_W21_WriteCMD(0xE0);   // CCSET
  EPD_W21_WriteDATA(0x00); 
  
  EPD_W21_WriteCMD(0xE6);   // TSSET
  EPD_W21_WriteDATA(0x00);  

  EPD_W21_WriteCMD(0x04);     //PWR on  
  lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal

}
void EPD_refresh(void)
{
    EPD_W21_WriteCMD(0x12);     //DISPLAY REFRESH   
    delay(10);          //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();
} 
void EPD_sleep(void)
{
    EPD_W21_WriteCMD(0X02);    //power off
    EPD_W21_WriteDATA(0x00);
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal
}

unsigned char Color_get(unsigned char color)
{
  unsigned datas;
  switch(color)
  {
    case 0xFF:
      datas=white;
    break;
    case 0xFC:
      datas=yellow;
    break;
    case 0xF1:
      datas=orange;
    break;        
    case 0xE5:
      datas=red;
    case 0xC5:
      datas=red;
    break;  
    case 0x4A:
      datas=blue;
    case 0x4B:
      datas=blue;
    break;  
    case 0x39:
      datas=green;
    break;  
    case 0x00:
      datas=black;
    break;    
    default:
    break;      
  }
   return datas;
}
void PIC_display(const unsigned char* picData)
{
  unsigned int i,j,k;
  unsigned char temp1,temp2;
  unsigned char data_H,data_L,data;
  

   //Acep_color(White); //Each refresh must be cleaned first   
  EPD_W21_WriteCMD(0x10);        
  for(i=0;i<480;i++)
  { 
    k=0;
    for(j=0;j<800/2;j++)
    {
      
      temp1=picData[i*800+k++]; 
      temp2=picData[i*800+k++];
      data_H=Color_get(temp1)<<4;
      data_L=Color_get(temp2);
      data=data_H|data_L;
      EPD_W21_WriteDATA(data);
    }
  } 
  
   //Refresh
    EPD_W21_WriteCMD(0x12);   //DISPLAY REFRESH   
    EPD_W21_WriteDATA(0x00);
    delay(1);   //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal

}
void PIC_display_Clear(void)
{
   unsigned int i,j;
  //Acep_color(Clean); //Each refresh must be cleaned first 
  EPD_W21_WriteCMD(0x10);        
  for(i=0;i<480;i++)
  {
    for(j=0;j<800/2;j++)
    {
      EPD_W21_WriteDATA(White);
    }
  }
  
  //Refresh
  EPD_W21_WriteCMD(0x12);   //DISPLAY REFRESH   
  EPD_W21_WriteDATA(0x00);
  delay(1);              //!!!The delay here is necessary, 200uS at least!!!     
  lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal

}
void lcd_chkstatus(void)
{
  while(!isEPD_W21_BUSY);   //0:BUSY, 1:FREE                     
}
