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
//960*640///////////////////////////////////////

#define MONOMSB_MODE 1
#define MONOLSB_MODE 2 
#define RED_MODE     3

#define MAX_LINE_BYTES 120 // =960/8
#define MAX_COLUMN_BYTES  640

#define ALLSCREEN_GRAGHBYTES  76800

////////FUNCTION//////
void driver_delay_us(unsigned int xus);
void driver_delay_xms(unsigned long xms);
void DELAY_S(unsigned int delaytime);     
void SPI_Delay(unsigned char xrate);
void SPI_Write(unsigned char value);
void Epaper_Write_CMD(unsigned char command);
void Epaper_Write_DATA(unsigned char command);
//EPD
void Epaper_READBUSY(void);


void EPD_HW_Init(void); //Electronic paper initialization
void EPD_Update(void);

void EPD_WhiteScreen_Black(void);
void EPD_WhiteScreen_White(void);
void EPD_DeepSleep(void);
//Display 
void EPD_WhiteScreen_ALL(const unsigned char *datas);

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
    EPD_WhiteScreen_ALL(gImage_1); //Refresh the picture in full screen
    EPD_DeepSleep();  //Enter deep sleep,Sleep instruction is necessary, please do not delete!!!
    delay(3000);
  
  ////////////////////////////////////////////////////////////////////////  
      //Clean
    EPD_HW_Init();//Electronic paper initialization
    EPD_WhiteScreen_White();  //Show all white
    EPD_DeepSleep();  //Enter deep sleep,Sleep instruction is necessary, please do not delete!!!
    while(1); 
}












///////////////////EXTERNAL FUNCTION////////////////////////////////////////////////////////////////////////
void SPI_Write(unsigned char value)                                    
{                                                           
    SPI.transfer(value);

}

void Epaper_Write_CMD(unsigned char command)
{
  EPD_W21_CS_0;                   
  EPD_W21_DC_0;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}
void Epaper_Write_DATA(unsigned char command)
{
  EPD_W21_CS_0;                   
  EPD_W21_DC_1;   // command write
  SPI_Write(command);
  EPD_W21_CS_1;
}

/////////////////EPD settings Functions/////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
//SSD1677
void EPD_HW_Init(void)
{
    EPD_W21_RST_0;  // Module reset      
    delay(10); //At least 10ms delay 
    EPD_W21_RST_1; 
    delay(10); //At least 10ms delay  
    
    Epaper_Write_CMD(0x12);       //SWRESET
    Epaper_READBUSY();        //waiting for the electronic paper IC to release the idle signal
    
    Epaper_Write_CMD(0x0C);  // Soft start setting
    Epaper_Write_DATA(0xAE);
    Epaper_Write_DATA(0xC7);
    Epaper_Write_DATA(0xC3);
    Epaper_Write_DATA(0xC0);
    Epaper_Write_DATA(0xFF);   

    Epaper_Write_CMD(0x01);  // Set MUX as 527
    Epaper_Write_DATA(0x7F);
    Epaper_Write_DATA(0x02);
    Epaper_Write_DATA(0x00);

    Epaper_Write_CMD(0x11);  // Data entry mode
    Epaper_Write_DATA(0x01);
    
    Epaper_Write_CMD(0x44);
    Epaper_Write_DATA(0x00); // RAM x address start at 0
    Epaper_Write_DATA(0x00);
    Epaper_Write_DATA(0xBF); // RAM x address end at 36Fh -> 879
    Epaper_Write_DATA(0x03);
    Epaper_Write_CMD(0x45);
    Epaper_Write_DATA(0x7F); // RAM y address start at 20Fh;
    Epaper_Write_DATA(0x02);
    Epaper_Write_DATA(0x00); // RAM y address end at 00h;
    Epaper_Write_DATA(0x00);

    Epaper_Write_CMD(0x3C); // VBD
    Epaper_Write_DATA(0x01); // LUT1, for white

    Epaper_Write_CMD(0x18);
    Epaper_Write_DATA(0X80);

    Epaper_Write_CMD(0x4E); 
    Epaper_Write_DATA(0x00);
    Epaper_Write_DATA(0x00);
    Epaper_Write_CMD(0x4F); 
    Epaper_Write_DATA(0x7F);
    Epaper_Write_DATA(0x02);
  
}
//////////////////////////////All screen update////////////////////////////////////////////
void EPD_WhiteScreen_ALL(const unsigned char *datas)
{
   unsigned int i;  
  Epaper_Write_CMD(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     Epaper_Write_DATA(pgm_read_byte(&datas[i]));
   }
   EPD_Update();   
}

/////////////////////////////////////////////////////////////////////////////////////////
void EPD_Update(void)
{
  Epaper_Write_CMD(0x22); //Display Update Control
  Epaper_Write_DATA(0xF7);   
  Epaper_Write_CMD(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();    

}

void EPD_DeepSleep(void)
{  
  Epaper_Write_CMD(0x10); //enter deep sleep
  Epaper_Write_DATA(0x01); 
  delay(100);
}
void Epaper_READBUSY(void)
{ 
  while(1)
  {   //=1 BUSY
     if(isEPD_W21_BUSY==0) break;
  }  
}
/////////////////////////////////Single display////////////////////////////////////////////////

void EPD_WhiteScreen_White(void)

{
   unsigned int i,k;
   Epaper_Write_CMD(0x24);   //write RAM for black(0)/white (1)
  for(k=0;k<960;k++)
  {
    for(i=0;i<80;i++)
    {
      Epaper_Write_DATA(0xff);
      }
  }
  EPD_Update();
}
void EPD_WhiteScreen_Black(void)

{
   unsigned int i,k;
   Epaper_Write_CMD(0x24);   //write RAM for black(0)/white (1)
  for(k=0;k<960;k++)
  {
    for(i=0;i<80;i++)
    {
      Epaper_Write_DATA(0x00);
      }
  }
  EPD_Update();
}



//////////////////////////////////END//////////////////////////////////////////////////
