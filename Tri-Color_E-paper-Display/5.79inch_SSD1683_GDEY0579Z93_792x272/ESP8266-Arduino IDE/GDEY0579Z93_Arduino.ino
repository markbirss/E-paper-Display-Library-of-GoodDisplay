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


#define MONOMSB_MODE 1
#define MONOLSB_MODE 2 
#define RED_MODE     3

#define ALLSCREEN_GRAGHBYTES  27200/2

#define Source_BYTES    400/8
#define Gate_BITS  272
#define ALLSCREEN_BYTES   Source_BYTES*Gate_BITS

////////FUNCTION//////
//EPD
void EPD_init(void);
void EPD_init_GUI(void);
void PIC_display(const unsigned char *BW_datas,const unsigned char *R_datas);
void EPD_Update(void);
void EPD_sleep(void);
void Epaper_READBUSY(void);
void PIC_display_Clear(void);

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
    EPD_init(); //EPD init
    PIC_display(gImage_BW1,gImage_R1);//EPD_picture1
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    delay(3000); //2s    
 
    //Clear
    EPD_init(); //EPD init
    PIC_display_Clear();//EPD Clear
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!! 
  while(1) 
    {
     Sys_run();//System run
     LED_run();//Breathing lamp
    }
}







///////////////////EXTERNAL FUNCTION////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////////////////////////////
//SSD1683
void EPD_init(void)
{  
    EPD_W21_RST_0;    // Module reset
    delay(10);//At least 10ms delay 
    EPD_W21_RST_1;
    delay(10);//At least 10ms delay 

    Epaper_READBUSY(); //waiting for the electronic paper IC to release the idle signal 
    EPD_W21_WriteCMD(0x12);     //SWRESET
    Epaper_READBUSY(); //waiting for the electronic paper IC to release the idle signal

}
void EPD_init_GUI(void)
{ 
    EPD_W21_RST_0;    // Module reset
    delay(10);//At least 10ms delay 
    EPD_W21_RST_1;
    delay(10);//At least 10ms delay 

    Epaper_READBUSY(); //waiting for the electronic paper IC to release the idle signal 
    EPD_W21_WriteCMD(0x12);     //SWRESET
    Epaper_READBUSY(); //waiting for the electronic paper IC to release the idle signal

    EPD_W21_WriteCMD(0x01);  // Set MUX as 300
    EPD_W21_WriteDATA(0x17);           
    EPD_W21_WriteDATA(0x03);
    EPD_W21_WriteDATA(0x00); 

    EPD_W21_WriteCMD(0x11);  // Data entry mode
    EPD_W21_WriteDATA(0x01);   
    
    EPD_W21_WriteCMD(0x44); 
    EPD_W21_WriteDATA(0x00); // RAM x address start at 0
    EPD_W21_WriteDATA(0x1F); // RAM x address end at 1Fh(31+1)*8->272
    EPD_W21_WriteCMD(0x45); 
    EPD_W21_WriteDATA(0x17);   // RAM y address start at 12Bh  0317h(792-1)--792    
    EPD_W21_WriteDATA(0x03);
    EPD_W21_WriteDATA(0x00); // RAM y address end at 00h     
    EPD_W21_WriteDATA(0x00);
    
    EPD_W21_WriteCMD(0x3C); // board
    EPD_W21_WriteDATA(0x01); // 

    EPD_W21_WriteCMD(0x18);
    EPD_W21_WriteDATA(0X80);

    EPD_W21_WriteCMD(0x4E); 
    EPD_W21_WriteDATA(0x00);
    EPD_W21_WriteCMD(0x4F); 
    EPD_W21_WriteDATA(0x17);
    EPD_W21_WriteDATA(0x03);
} 
void EPD_Update(void)
{   
  EPD_W21_WriteCMD(0x22); //Display Update Control
  EPD_W21_WriteDATA(0xF7);   
  EPD_W21_WriteCMD(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();   

}
void EPD_sleep(void)
{
  EPD_W21_WriteCMD(0x10); //enter deep sleep
  EPD_W21_WriteDATA(0x01); 
  delay(100);
}
void Set_ramMP(void) // Set RAM X - address Start / End position  ; Set RAM Y - address Start / End position  -MASTER
{
  EPD_W21_WriteCMD(0x11);  // Data Entry mode setting
  EPD_W21_WriteDATA(0x05);     // 1 ¨CY decrement, X increment
  EPD_W21_WriteCMD(0x44);              // Set Ram X- address Start / End position
  EPD_W21_WriteDATA(0x00);                 // XStart, POR = 00h
  EPD_W21_WriteDATA(0x31); //400/8-1
  EPD_W21_WriteCMD(0x45);                   // Set Ram Y- address  Start / End position 
  EPD_W21_WriteDATA(0x0f);  
  EPD_W21_WriteDATA(0x01);  //300-1 
  EPD_W21_WriteDATA(0x00);                      // YEnd L
  EPD_W21_WriteDATA(0x00);                      // YEnd H 
      
}

void Set_ramMA(void)  // Set RAM X address counter ;  Set RAM Y address counter                             -MASTER
{
  EPD_W21_WriteCMD(0x4e);              
  EPD_W21_WriteDATA(0x00);  
  EPD_W21_WriteCMD(0x4f);  
  EPD_W21_WriteDATA(0x0f);  
  EPD_W21_WriteDATA(0x01);  
}

void Set_ramSP(void)  // Set RAM X - address Start / End position  ; Set RAM Y - address Start / End position  -SLAVE
{
  EPD_W21_WriteCMD(0x91);              
  EPD_W21_WriteDATA(0x04); 
  EPD_W21_WriteCMD(0xc4);              // Set Ram X- address Start / End position
  EPD_W21_WriteDATA(0x31);                 // XStart, POR = 00h
  EPD_W21_WriteDATA(0x00); 
  EPD_W21_WriteCMD(0xc5);                   // Set Ram Y- address  Start / End position 
  EPD_W21_WriteDATA(0x0f);  
  EPD_W21_WriteDATA(0x01);    
  EPD_W21_WriteDATA(0x00);                      // YEnd L
  EPD_W21_WriteDATA(0x00);                      // YEnd H   
}

void Set_ramSA(void)   // Set RAM X address counter ;  Set RAM Y address counter                             -SLAVE
{
  EPD_W21_WriteCMD(0xce);              
  EPD_W21_WriteDATA(0x31); 
  EPD_W21_WriteCMD(0xcf);  
  EPD_W21_WriteDATA(0x0f);  
  EPD_W21_WriteDATA(0x01);  
}

void PIC_display(const unsigned char *BW_datas,const unsigned char *R_datas)
{
  unsigned int i; 
  unsigned char tempOriginal;      
  unsigned int tempcol=0;
  unsigned int templine=0;
  
  //BW
  Set_ramMP(); 
  Set_ramMA();  
  EPD_W21_WriteCMD(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     tempOriginal=pgm_read_byte(&BW_datas[templine*Source_BYTES*2+tempcol]);
     templine++;
     if(templine>=Gate_BITS)
     {
       tempcol++;
       templine=0;
     }     
     EPD_W21_WriteDATA(~tempOriginal);
   }
   
  Set_ramSP();
  Set_ramSA();
  tempcol=tempcol-1; //Byte dislocation processing
  templine=0;  
  EPD_W21_WriteCMD(0xa4);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     tempOriginal=pgm_read_byte(&BW_datas[templine*Source_BYTES*2+tempcol]);
     templine++;
     if(templine>=Gate_BITS)
     {
       tempcol++;
       templine=0;
     }     
     EPD_W21_WriteDATA(~tempOriginal);
   }

  //R
  Set_ramMP(); 
  Set_ramMA();  
  tempcol=0; //Byte dislocation processing
  templine=0;  
  EPD_W21_WriteCMD(0x26);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     tempOriginal=pgm_read_byte(&R_datas[templine*Source_BYTES*2+tempcol]);
     templine++;
     if(templine>=Gate_BITS)
     {
       tempcol++;
       templine=0;
     }     
     EPD_W21_WriteDATA(tempOriginal);
   }

   
  Set_ramSP();
  Set_ramSA();
  tempcol=tempcol-1; //Byte dislocation processing
  templine=0;  
  EPD_W21_WriteCMD(0xa6);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     tempOriginal=pgm_read_byte(&R_datas[templine*Source_BYTES*2+tempcol]);
     templine++;
     if(templine>=Gate_BITS)
     {
       tempcol++;
       templine=0;
     }     
     EPD_W21_WriteDATA(tempOriginal);
   }   
    
  //update
  EPD_Update();  
}
void PIC_display_Clear(void)
{
   unsigned int i;  
  //BW
  Set_ramMP(); 
  Set_ramMA();  
  EPD_W21_WriteCMD(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     EPD_W21_WriteDATA(0xff);
   }
   
  Set_ramSA();     
  EPD_W21_WriteCMD(0x26);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     EPD_W21_WriteDATA(0x00);
   }

  //R
  Set_ramSP();
  Set_ramSA();  
  EPD_W21_WriteCMD(0xa4);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     EPD_W21_WriteDATA(0xff);
   }
   
   Set_ramSA();    
  EPD_W21_WriteCMD(0xa6);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     EPD_W21_WriteDATA(0x00);
   }   
   
  //update
  EPD_Update();

}

void Epaper_READBUSY(void)
{ 
  while(1)
  {  //=1 BUSY
     if(isEPD_W21_BUSY==0) break;
     ESP.wdtFeed(); //Feed dog to prevent system reset
  }  
}





//////////////////////////////////END//////////////////////////////////////////////////
