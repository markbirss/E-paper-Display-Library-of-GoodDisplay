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
void PIC_display(const unsigned char *old_data,const unsigned char *new_data,unsigned char mode);// mode0:Refresh picture1,mode1:Refresh picture2... ,mode2¡¢3:Clear
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
      //Clear
      EPD_init(); //EPD init
      PIC_display_Clean();//EPD Clear
      EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
      delay(2000); //2s  
        
      EPD_init(); //EPD init
      PIC_display(gImage_1,gImage_1,0);//EPD_picture1
      EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
      delay(2000); //2s  
      
      EPD_init(); //EPD init
      PIC_display(gImage_1,gImage_2,1);//EPD_picture1
      EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
      delay(2000); //2s   
      //Clear
      EPD_init(); //EPD init
      PIC_display_Clean();//EPD Clear
      EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
      delay(2000); //2s  
  while(1) 
    {
     Sys_run();//System run
     LED_run();//Breathing lamp
    }
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
    EPD_W21_Init();
    delay(100);         //reset IC and select BUS 

    EPD_W21_WriteCMD(0x00);             //panel setting
    EPD_W21_WriteDATA (0xDf);
    EPD_W21_WriteDATA (0x0e); 
  
    EPD_W21_WriteCMD(0x4D);             //FITIinternal code
    EPD_W21_WriteDATA (0x55);

    EPD_W21_WriteCMD(0xaa);             
    EPD_W21_WriteDATA (0x0f);
    
    EPD_W21_WriteCMD(0xE9);             
    EPD_W21_WriteDATA (0x02);

    EPD_W21_WriteCMD(0xb6);             
    EPD_W21_WriteDATA (0x11);
    
    EPD_W21_WriteCMD(0xF3);             
    EPD_W21_WriteDATA (0x0a);     

    EPD_W21_WriteCMD(0x61);     //resolution setting
    EPD_W21_WriteDATA (0xc8);
    EPD_W21_WriteDATA (0x00);   
    EPD_W21_WriteDATA (0xc8);    
    
    EPD_W21_WriteCMD(0x60);          //Tcon setting   
    EPD_W21_WriteDATA (0x00);
    
    EPD_W21_WriteCMD(0X50);           
    EPD_W21_WriteDATA(0x97);//
    
    EPD_W21_WriteCMD(0XE3);           
    EPD_W21_WriteDATA(0x00);
    
    EPD_W21_WriteCMD(0x04);//Power on
    delay(100);
    lcd_chkstatus();  
    
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
void PIC_display(const unsigned char *old_data,const unsigned char *new_data,unsigned char mode)// mode0:Refresh picture1,mode1:Refresh picture2... ,mode2¡¢3:Clear
{    
    unsigned int i;
    
    if(mode==0)  //mode0:Refresh picture1
    {
      EPD_W21_WriteCMD(0x10);
      for(i=0;i<5000;i++)      
      {
          EPD_W21_WriteDATA(0xff);  
      }  
      EPD_W21_WriteCMD(0x13);
      for(i=0;i<5000;i++)      
      {
          EPD_W21_WriteDATA(pgm_read_byte(&new_data[i]));  
      }  
    }
    
    else if(mode==1)  //mode0:Refresh picture2...
    {
      EPD_W21_WriteCMD(0x10);
      for(i=0;i<5000;i++)      
      {
          EPD_W21_WriteDATA(pgm_read_byte(&old_data[i]));  
      }  
      EPD_W21_WriteCMD(0x13);
      for(i=0;i<5000;i++)      
      {
          EPD_W21_WriteDATA(pgm_read_byte(&new_data[i]));  
      }  
    }
    
   else if(mode==2) 
    {
      EPD_W21_WriteCMD(0x10);
      for(i=0;i<5000;i++)      
      {
          EPD_W21_WriteDATA(pgm_read_byte(&old_data[i]));  
      }  
      EPD_W21_WriteCMD(0x13);
      for(i=0;i<5000;i++)      
      {
          EPD_W21_WriteDATA(0xff);  
      }  
    }  
   else if(mode==3) 
    {
      EPD_W21_WriteCMD(0x10);
      for(i=0;i<5000;i++)      
      {
          EPD_W21_WriteDATA(0xff); 
      }  
      EPD_W21_WriteCMD(0x13);
      for(i=0;i<5000;i++)      
      {
          EPD_W21_WriteDATA(0xff);  
      }  
    }

    EPD_W21_WriteCMD(0x12);     //DISPLAY REFRESH   
    delay(10);     //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();
}
void PIC_display_Clean(void)
{
    unsigned int i,j; 
    for(j=0;j<2;j++)
    {
      EPD_W21_WriteCMD(0x10);
      for(i=0;i<5000;i++)      
      {
          EPD_W21_WriteDATA(0x00);  
      }  
      EPD_W21_WriteCMD(0x13);
      for(i=0;i<5000;i++)      
      {
          EPD_W21_WriteDATA(0xff);  
      }     
      EPD_W21_WriteCMD(0x12);     //DISPLAY REFRESH   
      delay(10);     //!!!The delay here is necessary, 200uS at least!!!     
      lcd_chkstatus();   //waiting for the electronic paper IC to release the idle signal
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
