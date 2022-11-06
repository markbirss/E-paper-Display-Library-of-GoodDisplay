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
//400*300///////////////////////////////////////

#define MONOMSB_MODE 1
#define MONOLSB_MODE 2 
#define RED_MODE     3

#define MAX_LINE_BYTES 50// =400/8
#define MAX_COLUMN_BYTES  300

#define ALLSCREEN_GRAGHBYTES  15000

////////FUNCTION//////
void driver_delay_us(unsigned int xus);
void driver_delay(unsigned long xms);
void DELAY_S(unsigned int delaytime);     
void SPI_Delay(unsigned char xrate);
void SPI_Write(unsigned char value);
void EPD_W21_WriteDATA(unsigned char command);
void EPD_W21_WriteCMD(unsigned char command);
//EPD
void EPD_init(void);//Electronic paper initialization
void EPD_Update(void);
void EPD_sleep(void);

//Display 
void EPD_Display(unsigned char *BW_Image);
void PIC_display_Clear(void);
void EPD_SetRAMValue_BaseMap(const unsigned char * datas);
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE);
void EPD_Dis_Part_myself(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
                         unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
                         unsigned int x_startC,unsigned int y_startC,const unsigned char * datasC,
                         unsigned int x_startD,unsigned int y_startD,const unsigned char * datasD,
                         unsigned int x_startE,unsigned int y_startE,const unsigned char * datasE,
                         unsigned int PART_COLUMN,unsigned int PART_LINE
                        );
void EPD_HW_Init_Fast(void);
void EPD_WhiteScreen_ALL_Fast(const unsigned char *datas);
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
    /************Fast picture display(1.5s)*******************/
    EPD_HW_Init_Fast(); //EPD init Fast
    EPD_WhiteScreen_ALL_Fast(gImage_1);//EPD_picture1
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    delay(2000); //2s  
    
    EPD_init(); //Electronic paper initialization  
    EPD_SetRAMValue_BaseMap(gImage_basemap); //Partial refresh background color      
    unsigned char fen_L,fen_H,miao_L,miao_H;
    for(fen_H=0;fen_H<6;fen_H++)
    {
    for(fen_L=0;fen_L<10;fen_L++)
    {
    for(miao_H=0;miao_H<6;miao_H++)   
    {
    for(miao_L=0;miao_L<10;miao_L++)
    {
        EPD_Dis_Part_myself(176,110,(unsigned char *)&Num[miao_L],         //x-A,y-A,DATA-A
                            176,142,(unsigned char *)&Num[miao_H],         //x-B,y-B,DATA-B
                            176,174,(unsigned char *)gImage_numdot,       //x-C,y-C,DATA-C
                            176,206,(unsigned char *)&Num[fen_L],         //x-D,y-D,DATA-D
                            176,238,(unsigned char *)&Num[fen_H],32,64);   //x-E,y-E,DATA-E,Resolution 32*64
                            
                            if((fen_L==0)&&(miao_H==0)&&(miao_L==5))
                            goto Clear;
      }
    }
    }
    
    }       
  
    //Clean
    Clear:
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
void EPD_W21_WriteDATA(unsigned char data)
{
  EPD_W21_CS_0;                   
  EPD_W21_DC_1;   // data write
  SPI_Write(data);
  EPD_W21_CS_1;
}


/////////////////EPD settings Functions/////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
//SSD1683
void EPD_init(void)
{
    EPD_W21_RST_0;  // Module reset      
    delay(10); //At least 10ms delay 
    EPD_W21_RST_1; 
    delay(10); //At least 10ms delay  
    
    Epaper_READBUSY();    //waiting for the electronic paper IC to release the idle signal
    EPD_W21_WriteCMD(0x12);     //SWRESET
    Epaper_READBUSY();  //waiting for the electronic paper IC to release the idle signal
  
    EPD_W21_WriteCMD(0x01);  // Set MUX as 300
    EPD_W21_WriteDATA(0x2B);           
    EPD_W21_WriteDATA(0x01);
    EPD_W21_WriteDATA(0x00); 

    EPD_W21_WriteCMD(0x11);  // Data entry mode
    EPD_W21_WriteDATA(0x01);   
    
    EPD_W21_WriteCMD(0x44); 
    EPD_W21_WriteDATA(0x00); // RAM x address start at 0
    EPD_W21_WriteDATA(0x31); // RAM x address end at 31h(49+1)*8->400
    EPD_W21_WriteCMD(0x45); 
    EPD_W21_WriteDATA(0x2B);   // RAM y address start at 12Bh     
    EPD_W21_WriteDATA(0x01);
    EPD_W21_WriteDATA(0x00); // RAM y address end at 00h     
    EPD_W21_WriteDATA(0x00);
    
    EPD_W21_WriteCMD(0x3C); // board
    EPD_W21_WriteDATA(0x01); // 

    EPD_W21_WriteCMD(0x18);
    EPD_W21_WriteDATA(0X80);

    EPD_W21_WriteCMD(0x4E); 
    EPD_W21_WriteDATA(0x00);
    EPD_W21_WriteCMD(0x4F); 
    EPD_W21_WriteDATA(0x2B);
    EPD_W21_WriteDATA(0x01);
}
void EPD_HW_Init_Fast(void)
{
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
  
  EPD_W21_WriteCMD(0x12);  //SWRESET
  Epaper_READBUSY();   
 
  EPD_W21_WriteCMD(0x3C); // board
  EPD_W21_WriteDATA(0x01); 
  
  EPD_W21_WriteCMD(0x18); //Read built-in temperature sensor
  EPD_W21_WriteDATA(0x80);  
      
  EPD_W21_WriteCMD(0x22); // Load temperature value
  EPD_W21_WriteDATA(0xB1);    
  EPD_W21_WriteCMD(0x20); 
  Epaper_READBUSY();   

  EPD_W21_WriteCMD(0x1A); // Write to temperature register
  EPD_W21_WriteDATA(0x64);    
  EPD_W21_WriteDATA(0x00);    
            
  EPD_W21_WriteCMD(0x22); // Load temperature value
  EPD_W21_WriteDATA(0x91);    
  EPD_W21_WriteCMD(0x20); 
  Epaper_READBUSY();   
}
//////////////////////////////All screen update////////////////////////////////////////////


void PIC_display(const unsigned char *BW_datas)
{
   unsigned int i;  
  EPD_W21_WriteCMD(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     EPD_W21_WriteDATA(pgm_read_byte(&BW_datas[i]));
   }
  EPD_W21_WriteCMD(0x26);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     EPD_W21_WriteDATA(0x00);
   }
  //update
  EPD_Update();  
}
void EPD_WhiteScreen_ALL_Fast(const unsigned char *datas)
{
   unsigned int i;  
  EPD_W21_WriteCMD(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
   } 
  EPD_W21_WriteCMD(0x26);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     EPD_W21_WriteDATA(0x00);
   }   
   EPD_Update_Fast();  
}
/////////////////////////////////////////////////////////////////////////////////////////
void EPD_Update(void)
{   
  EPD_W21_WriteCMD(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();   

}
void EPD_Update_Fast(void)
{   
  EPD_W21_WriteCMD(0x22); //Display Update Control
  EPD_W21_WriteDATA(0xC7);   
  EPD_W21_WriteCMD(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();   

}
void EPD_sleep(void)
{  
 EPD_W21_WriteCMD(0x10); //enter deep sleep
 EPD_W21_WriteDATA(0x01); 
  delay(100);
}
void Epaper_READBUSY(void)
{ 
  while(1)
  {   //=1 BUSY
     if(isEPD_W21_BUSY==0) break;
     ESP.wdtFeed(); //Feed dog to prevent system reset
  }  
}

void PIC_display_Clear(void)
{
   unsigned int i;  
  EPD_W21_WriteCMD(0x24);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     EPD_W21_WriteDATA(0xff);
   }
  EPD_W21_WriteCMD(0x26);   //write RAM for black(0)/white (1)
   for(i=0;i<ALLSCREEN_GRAGHBYTES;i++)
   {               
     EPD_W21_WriteDATA(0x00);
   }
  //update
  EPD_Update(); 
}

void EPD_SetRAMValue_BaseMap( const unsigned char * datas)
{
  unsigned int i;   
  const unsigned char  *datas_flag;   
  datas_flag=datas;

  
  EPD_W21_WriteCMD(0x24);   //Write Black and White image to RAM
   for(i=0;i<15000;i++)
   {               
     EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));

   }
  
  EPD_W21_WriteCMD(0x26);   //Write Black and White image to RAM
   for(i=0;i<15000;i++)
   {               
     EPD_W21_WriteDATA(0x00);
   }
   EPD_Update_basemap();  
  EPD_W21_WriteCMD(0x26);   //Write Black and White image to RAM
   for(i=0;i<15000;i++)
   {               
     EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));

   }   
   
}
void EPD_Update_basemap(void)
{   
 EPD_W21_WriteCMD(0x22); //Display Update Control
  EPD_W21_WriteDATA(0xF4);  
  EPD_W21_WriteCMD(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();   

}

void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE)
{
  unsigned int i;  
  unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
  x_start=x_start/8;
  x_end=x_start+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_start;
  if(y_start>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_start+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   

//Reset
  EPD_W21_RST_0;  // Module reset   
  driver_delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  driver_delay(10); //At least 10ms delay 
    
  EPD_W21_WriteCMD(0x3C); //BorderWavefrom
  EPD_W21_WriteDATA(0x80);  
//
  
  EPD_W21_WriteCMD(0x44);       // set RAM x address start/end, in page 35
  EPD_W21_WriteDATA(x_start);    // RAM x address start at 00h;
  EPD_W21_WriteDATA(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  EPD_W21_WriteCMD(0x45);       // set RAM y address start/end, in page 35
  EPD_W21_WriteDATA(y_start2);    // RAM y address start at 0127h;
  EPD_W21_WriteDATA(y_start1);    // RAM y address start at 0127h;
  EPD_W21_WriteDATA(y_end2);    // RAM y address end at 00h;
  EPD_W21_WriteDATA(y_end1);    // ????=0 


  EPD_W21_WriteCMD(0x4E);   // set RAM x address count to 0;
  EPD_W21_WriteDATA(x_start); 
  EPD_W21_WriteCMD(0x4F);   // set RAM y address count to 0X127;    
  EPD_W21_WriteDATA(y_start2);
  EPD_W21_WriteDATA(y_start1);
  
  
   EPD_W21_WriteCMD(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     EPD_W21_WriteDATA(pgm_read_byte(&datas[i]));
   } 
   EPD_Part_Update();

}


void EPD_Dis_Part_myself(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
                         unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
                         unsigned int x_startC,unsigned int y_startC,const unsigned char * datasC,
                         unsigned int x_startD,unsigned int y_startD,const unsigned char * datasD,
                         unsigned int x_startE,unsigned int y_startE,const unsigned char * datasE,
                         unsigned int PART_COLUMN,unsigned int PART_LINE
                        )
{
  unsigned int i;  
  unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
  
  //Data A////////////////////////////
  x_startA=x_startA/8;//Convert to byte
  x_end=x_startA+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startA-1;
  if(y_startA>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startA+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   

//Reset
/*  EPD_W21_RST_0;  // Module reset   
  driver_delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  driver_delay(10); //At least 10ms delay 
    
  EPD_W21_WriteCMD(0x3C); //BorderWavefrom
  EPD_W21_WriteDATA(0x80);  */
//
  
  EPD_W21_WriteCMD(0x44);       // set RAM x address start/end, in page 35
  EPD_W21_WriteDATA(x_startA);    // RAM x address start at 00h;
  EPD_W21_WriteDATA(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  EPD_W21_WriteCMD(0x45);       // set RAM y address start/end, in page 35
  EPD_W21_WriteDATA(y_start2);    // RAM y address start at 0127h;
  EPD_W21_WriteDATA(y_start1);    // RAM y address start at 0127h;
  EPD_W21_WriteDATA(y_end2);    // RAM y address end at 00h;
  EPD_W21_WriteDATA(y_end1);    


  EPD_W21_WriteCMD(0x4E);   // set RAM x address count to 0;
  EPD_W21_WriteDATA(x_startA); 
  EPD_W21_WriteCMD(0x4F);   // set RAM y address count to 0X127;    
  EPD_W21_WriteDATA(y_start2);
  EPD_W21_WriteDATA(y_start1);
  
  
   EPD_W21_WriteCMD(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     EPD_W21_WriteDATA(pgm_read_byte(&datasA[i]));
   } 
  //Data B/////////////////////////////////////
  x_startB=x_startB/8;//Convert to byte
  x_end=x_startB+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startB-1;
  if(y_startB>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startB+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  
  EPD_W21_WriteCMD(0x44);       // set RAM x address start/end, in page 35
  EPD_W21_WriteDATA(x_startB);    // RAM x address start at 00h;
  EPD_W21_WriteDATA(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  EPD_W21_WriteCMD(0x45);       // set RAM y address start/end, in page 35
  EPD_W21_WriteDATA(y_start2);    // RAM y address start at 0127h;
  EPD_W21_WriteDATA(y_start1);    // RAM y address start at 0127h;
  EPD_W21_WriteDATA(y_end2);    // RAM y address end at 00h;
  EPD_W21_WriteDATA(y_end1);   


  EPD_W21_WriteCMD(0x4E);   // set RAM x address count to 0;
  EPD_W21_WriteDATA(x_startB); 
  EPD_W21_WriteCMD(0x4F);   // set RAM y address count to 0X127;    
  EPD_W21_WriteDATA(y_start2);
  EPD_W21_WriteDATA(y_start1);
  
  
   EPD_W21_WriteCMD(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     EPD_W21_WriteDATA(pgm_read_byte(&datasB[i]));
   }   
   
  //Data C//////////////////////////////////////
  x_startC=x_startC/8;//Convert to byte
  x_end=x_startC+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startC-1;
  if(y_startC>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startC+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  
  EPD_W21_WriteCMD(0x44);       // set RAM x address start/end, in page 35
  EPD_W21_WriteDATA(x_startC);    // RAM x address start at 00h;
  EPD_W21_WriteDATA(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  EPD_W21_WriteCMD(0x45);       // set RAM y address start/end, in page 35
  EPD_W21_WriteDATA(y_start2);    // RAM y address start at 0127h;
  EPD_W21_WriteDATA(y_start1);    // RAM y address start at 0127h;
  EPD_W21_WriteDATA(y_end2);    // RAM y address end at 00h;
  EPD_W21_WriteDATA(y_end1);   


  EPD_W21_WriteCMD(0x4E);   // set RAM x address count to 0;
  EPD_W21_WriteDATA(x_startC); 
  EPD_W21_WriteCMD(0x4F);   // set RAM y address count to 0X127;    
  EPD_W21_WriteDATA(y_start2);
  EPD_W21_WriteDATA(y_start1);
  
  
   EPD_W21_WriteCMD(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     EPD_W21_WriteDATA(pgm_read_byte(&datasC[i]));
   }     
   
  //Data D//////////////////////////////////////
  x_startD=x_startD/8;//Convert to byte
  x_end=x_startD+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startD-1;
  if(y_startD>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startD+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  
  EPD_W21_WriteCMD(0x44);       // set RAM x address start/end, in page 35
  EPD_W21_WriteDATA(x_startD);    // RAM x address start at 00h;
  EPD_W21_WriteDATA(x_end);        // RAM x address end at 0fh(15+1)*8->128 
  EPD_W21_WriteCMD(0x45);       // set RAM y address start/end, in page 35
  EPD_W21_WriteDATA(y_start2);    // RAM y address start at 0127h;
  EPD_W21_WriteDATA(y_start1);    // RAM y address start at 0127h;
  EPD_W21_WriteDATA(y_end2);    // RAM y address end at 00h;
  EPD_W21_WriteDATA(y_end1);    


  EPD_W21_WriteCMD(0x4E);   // set RAM x address count to 0;
  EPD_W21_WriteDATA(x_startD); 
  EPD_W21_WriteCMD(0x4F);   // set RAM y address count to 0X127;    
  EPD_W21_WriteDATA(y_start2);
  EPD_W21_WriteDATA(y_start1);
  
  
   EPD_W21_WriteCMD(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
       EPD_W21_WriteDATA(pgm_read_byte(&datasD[i]));
   } 
  //Data E//////////////////////////////////////
  x_startE=x_startE/8;//Convert to byte
  x_end=x_startE+PART_LINE/8-1; 
  
  y_start1=0;
  y_start2=y_startE-1;
  if(y_startE>=256)
  {
    y_start1=y_start2/256;
    y_start2=y_start2%256;
  }
  y_end1=0;
  y_end2=y_startE+PART_COLUMN-1;
  if(y_end2>=256)
  {
    y_end1=y_end2/256;
    y_end2=y_end2%256;    
  }   
  
  EPD_W21_WriteCMD(0x44);       // set RAM x address start/end, in page 35
  EPD_W21_WriteDATA(x_startE);    // RAM x address start at 00h;
  EPD_W21_WriteDATA(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  EPD_W21_WriteCMD(0x45);       // set RAM y address start/end, in page 35
  EPD_W21_WriteDATA(y_start2);    // RAM y address start at 0127h;
  EPD_W21_WriteDATA(y_start1);    // RAM y address start at 0127h;
  EPD_W21_WriteDATA(y_end2);    // RAM y address end at 00h;
  EPD_W21_WriteDATA(y_end1);    


  EPD_W21_WriteCMD(0x4E);   // set RAM x address count to 0;
  EPD_W21_WriteDATA(x_startE); 
  EPD_W21_WriteCMD(0x4F);   // set RAM y address count to 0X127;    
  EPD_W21_WriteDATA(y_start2);
  EPD_W21_WriteDATA(y_start1);
  
  
   EPD_W21_WriteCMD(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {                         
     EPD_W21_WriteDATA(pgm_read_byte(&datasE[i]));
   }    
   EPD_Part_Update();

}
/*When the electronic paper screen is updated, do not unplug the electronic paper to avoid damage to the screen*/
void EPD_Part_Update(void)
{
  EPD_W21_WriteCMD(0x22); //Display Update Control
  EPD_W21_WriteDATA(0x1C);   
  EPD_W21_WriteCMD(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();       
}
//////////////////////////////////END//////////////////////////////////////////////////
//////////////////////////////////END//////////////////////////////////////////////////
