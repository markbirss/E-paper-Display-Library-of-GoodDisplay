#include <SPI.h>
#include"Ap_29demo.h"
//IO settings
int BUSY_Pin = 4; 
int RES_Pin = 5; 
int DC_Pin = 6; 
int CS_Pin = 7; 

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
void PIC_display(const unsigned char* picData);

void setup() {
   pinMode(BUSY_Pin, INPUT); 
   pinMode(RES_Pin, OUTPUT);  
   pinMode(DC_Pin, OUTPUT);    
   pinMode(CS_Pin, OUTPUT);   
   //SPI
   SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0)); 
   SPI.begin ();  
}
////////Partial refresh schematic////////////////

/////Y/// (0,0)               /---/(x,y)
          //                 /---/
          //                /---/  
          //x
          //
          //
//Tips//
/*
1.When the e-paper is refreshed in full screen, the picture flicker is a normal phenomenon, and the main function is to clear the display afterimage in the previous picture.
2.When the partial refresh is performed, the screen does not flash.
3.After the e-paper is refreshed, you need to put it into sleep mode, please do not delete the sleep command.
4.Please do not take out the electronic paper when power is on.
5.Wake up from sleep, need to re-initialize the e-paper.
6.When you need to transplant the driver, you only need to change the corresponding IO. The BUSY pin is the input mode and the others are the output mode.
*/ 
void loop() {
  while(1)
  {
    EPD_init(); //EPD init
    PIC_display(gImage_1);//EPD_picture1
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    delay(2000); //2s  
    
    //PICTURE2
    EPD_init(); //EPD init
    PIC_display(gImage_2);//EPD_picture1
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
    EPD_W21_Init();//Electronic paper IC reset
  
    EPD_W21_WriteCMD(0x00);       
    EPD_W21_WriteDATA (0x5f); // otp
  
    EPD_W21_WriteCMD(0x2A);     
    EPD_W21_WriteDATA(0x00); 
    EPD_W21_WriteDATA(0x00);
    
    EPD_W21_WriteCMD(0x04);  //Power on
    lcd_chkstatus();        //waiting for the electronic paper IC to release the idle signal

    //Add the following code
    EPD_W21_WriteCMD(0x50);     //Solve some black paper black border problems  
    EPD_W21_WriteDATA (0x97);  
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
    //Part2 Increase the time delay
    delay(1000);      //Power off time delay, this is  necessary!!!     
    EPD_W21_WriteCMD(0X07);   //deep sleep
    EPD_W21_WriteDATA(0xA5);
}

void PIC_display(const unsigned char* picData)
{
    unsigned int i;
    EPD_W21_WriteCMD(0x10);        //Transfer old data
    for(i=0;i<1280;i++)      
    EPD_W21_WriteDATA(0xff); 
  
    EPD_W21_WriteCMD(0x13);        //Transfer new data
    for(i=0;i<1280;i++)      
     EPD_W21_WriteDATA(pgm_read_byte(&picData[i]));

       //Refresh
    EPD_W21_WriteCMD(0x12);   //DISPLAY REFRESH   
    delay(10);    //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal

}

void PIC_display_Clean(void)
{
    unsigned int i;
    EPD_W21_WriteCMD(0x10);        //Transfer old data
    for(i=0;i<1280;i++)       
  {
    EPD_W21_WriteDATA(0xff);
  }
  
    EPD_W21_WriteCMD(0x13);        //Transfer new data
    for(i=0;i<1280;i++)       
  {
    EPD_W21_WriteDATA(0xff);
  }
     //Refresh
    EPD_W21_WriteCMD(0x12);   //DISPLAY REFRESH   
    delay(10);    //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal

}
void lcd_chkstatus(void)
{  
     while(1)
  {   //=0 BUSY
     if(isEPD_W21_BUSY==1) break;
  }  
}
