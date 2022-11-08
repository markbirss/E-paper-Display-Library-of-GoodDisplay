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
//250*122///////////////////////////////////////

#define MONOMSB_MODE 1
#define MONOLSB_MODE 2 
#define RED_MODE     3

#define MAX_LINE_BYTES 16// =128/8
#define MAX_COLUMN_BYTES  250

#define ALLSCREEN_GRAGHBYTES  4000

////////FUNCTION//////
void driver_delay_us(unsigned int xus);
void driver_delay_xms(unsigned long xms);
void DELAY_S(unsigned int delaytime);     
void SPI_Delay(unsigned char xrate);
void SPI_Write(unsigned char value);
void Epaper_Write_Command(unsigned char command);
void Epaper_Write_Data(unsigned char command);
//EPD
void Epaper_READBUSY(void);
void EPD_HW_Init(void); //Electronic paper initialization
void EPD_Update(void);

void EPD_WhiteScreen_Black(void);
void EPD_WhiteScreen_White(void);
void EPD_DeepSleep(void);

//Display 
void EPD_WhiteScreen_ALL(const unsigned char *BW_datas,const unsigned char *R_datas);
void EPD_WhiteScreen_ALL_Clean(void);

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
    //Full screen refresh
    EPD_HW_Init(); //Electronic paper initialization
    EPD_WhiteScreen_ALL(gImage_BW,gImage_R); //Refresh the picture in full screen
    EPD_DeepSleep(); //Enter deep sleep,Sleep instruction is necessary, please do not delete!!! 
    delay(2000);   
    
    //Clean
    EPD_HW_Init(); //Electronic paper initialization
    EPD_WhiteScreen_ALL_Clean();
    EPD_DeepSleep(); //Enter deep sleep,Sleep instruction is necessary, please do not delete!!!
    delay(2000); 
    while(1);
}












///////////////////EXTERNAL FUNCTION////////////////////////////////////////////////////////////////////////
/////////////////////delay//////////////////////////////////////

//////////////////////SPI///////////////////////////////////

void SPI_Write(unsigned char value)                                    
{                                                           
  SPI.transfer(value);
}

void Epaper_Write_Command(unsigned char command)
{
  EPD_W21_CS_0;                   
  EPD_W21_DC_0;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}
void Epaper_Write_Data(unsigned char command)
{
  EPD_W21_CS_0;                   
  EPD_W21_DC_1;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}

/////////////////EPD settings Functions/////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
//SSD1680
void EPD_HW_Init(void)
{
  EPD_W21_RST_0;  // Module reset      
  delay(10); //At least 10ms delay 
  EPD_W21_RST_1; 
  delay(10); //At least 10ms delay  
    
  Epaper_READBUSY();   
  Epaper_Write_Command(0x12);  //SWRESET
  Epaper_READBUSY();   
    
  Epaper_Write_Command(0x01); //Driver output control      
  Epaper_Write_Data(0xF9);
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x00);

  Epaper_Write_Command(0x11); //data entry mode       
  Epaper_Write_Data(0x01);

  Epaper_Write_Command(0x44); //set Ram-X address start/end position   
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x0F);    //0x0F-->(15+1)*8=128

  Epaper_Write_Command(0x45); //set Ram-Y address start/end position          
  Epaper_Write_Data(0xF9);   //0xF9-->(249+1)=250
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x00);
  Epaper_Write_Data(0x00); 

  Epaper_Write_Command(0x3C); //BorderWavefrom
  Epaper_Write_Data(0x05);  
      
  Epaper_Write_Command(0x18); //Read built-in temperature sensor
  Epaper_Write_Data(0x80);  
  
  Epaper_Write_Command(0x21); //  Display update control
  Epaper_Write_Data(0x00);  
  Epaper_Write_Data(0x80);  

  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(0x00);
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X199;    
  Epaper_Write_Data(0xF9);
  Epaper_Write_Data(0x00);
  Epaper_READBUSY();
  
}
//////////////////////////////All screen update////////////////////////////////////////////
void EPD_WhiteScreen_ALL(const unsigned char *BW_datas,const unsigned char *R_datas)
{
   unsigned int i;  
  Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(pgm_read_byte(&BW_datas[i]));
   }
  Epaper_Write_Command(0x26);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(~(pgm_read_byte(&R_datas[i])));
   }
   EPD_Update();   
}

void EPD_WhiteScreen_ALL_Clean(void)
{
   unsigned int i;  
  Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(0xff);
   }
  Epaper_Write_Command(0x26);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_Data(0x00);
   }
   EPD_Update();   
}

/////////////////////////////////////////////////////////////////////////////////////////
void EPD_Update(void)
{
  Epaper_Write_Command(0x22); //Display Update Control
  Epaper_Write_Data(0xF7);   
  Epaper_Write_Command(0x20);  //Activate Display Update Sequence
  Epaper_READBUSY();   

}

void EPD_DeepSleep(void)
{  
  Epaper_Write_Command(0x10); //enter deep sleep
  Epaper_Write_Data(0x01); 
  delay(100);
}
void Epaper_READBUSY(void)
{ 
  while(1)
  {   //=1 BUSY
     if(isEPD_W21_BUSY==0) break;
  }  
}



//////////////////////////////////END//////////////////////////////////////////////////
