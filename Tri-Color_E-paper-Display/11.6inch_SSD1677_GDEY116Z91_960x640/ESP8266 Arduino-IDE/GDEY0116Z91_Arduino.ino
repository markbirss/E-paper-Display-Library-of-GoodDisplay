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
void Epaper_READBUSY(void);
void SPI_Write(unsigned char value);  
void Epaper_Write_Command(unsigned char command);
void Epaper_Write_Data(unsigned char command);


void EPD_HW_Init(void); //Electronic paper initialization
void EPD_Update(void);    
void EPD_DeepSleep(void);
void EPD_WhiteScreen_ALL(const unsigned char* picData_BW,const unsigned char* picData_R);
void EPD_WhiteScreen_White(void);                        
                         

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
    //Full screen refresh
    EPD_HW_Init(); //Electronic paper initialization
    EPD_WhiteScreen_ALL(gImage_BW1,gImage_R1); //Refresh the picture in full screen
    EPD_DeepSleep(); //Enter deep sleep,Sleep instruction is necessary, please do not delete!!! 
    delay(3000);   
    
    //Clean
    EPD_HW_Init(); //Electronic paper initialization
    EPD_WhiteScreen_White(); //Show all white
    EPD_DeepSleep(); //Enter deep sleep,Sleep instruction is necessary, please do not delete!!!
  while(1) 
    {
     Sys_run();//System run
     LED_run();//Breathing lamp
    }
}












///////////////////EXTERNAL FUNCTION////////////////////////////////////////////////////////////////////////
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
void Epaper_READBUSY(void)
{ 
  while(1)
  {   //=1 BUSY
     if(isEPD_W21_BUSY==0) break;
     ESP.wdtFeed(); //Feed dog to prevent system reset
  }  
}

//SSD1677
void EPD_HW_Init(void)
{ 
    EPD_W21_RST_0;    // Module reset
    delay(10);//At least 10ms delay 
    EPD_W21_RST_1;
    delay(10);//At least 10ms delay 

    Epaper_Write_Command(0x12);       //SWRESET
    Epaper_READBUSY();        //waiting for the electronic paper IC to release the idle signal
    
    Epaper_Write_Command(0x3C); // VBD
    Epaper_Write_Data(0x01);
}


void EPD_Update(void)
{     
  Epaper_Write_Command(0x22); //Display Update Control
  Epaper_Write_Data(0xF7);   
  Epaper_Write_Command(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();  

}

void EPD_DeepSleep(void)
{
  Epaper_Write_Command(0x10); //enter deep sleep
  Epaper_Write_Data(0x01); 
  delay(100);
}

void EPD_WhiteScreen_ALL(const unsigned char* picData_BW,const unsigned char* picData_R)
{
  int i;
  Epaper_Write_Command(0x24);      
  for(i=0;i<76800;i++)       
  {
    Epaper_Write_Data(pgm_read_byte(&picData_BW[i]));
     
  }
  ESP.wdtFeed(); //Feed dog to prevent system reset
  Epaper_Write_Command(0x26);      
  for(i=0;i<76800;i++)       
  {
    Epaper_Write_Data(~pgm_read_byte(&picData_R[i]));    
  }
  //update
  EPD_Update();  
}

void EPD_WhiteScreen_White(void)
{
  int i;
  Epaper_Write_Command(0x24);      
  for(i=0;i<76800;i++)       
  {
    Epaper_Write_Data(0xFF);  
  }
  ESP.wdtFeed(); //Feed dog to prevent system reset
  Epaper_Write_Command(0x26);      
  for(i=0;i<76800;i++)       
  {
    Epaper_Write_Data(0x00);  
  }
  //update
  EPD_Update(); 
}





//////////////////////////////////END//////////////////////////////////////////////////
