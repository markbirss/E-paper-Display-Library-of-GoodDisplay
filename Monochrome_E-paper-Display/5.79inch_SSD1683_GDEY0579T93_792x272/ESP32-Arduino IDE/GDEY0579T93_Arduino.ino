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
   
#define EPD_WIDTH   272
#define EPD_HEIGHT  400

#define Source_BYTES    400/8
#define Gate_BITS  272
#define ALLSCREEN_BYTES   Source_BYTES*Gate_BITS

#define NUM_LINE_BYTES 8// =64/8  
#define NUM_COLUMN_BYTES  30 

#define PART_LINE_BYTES 8// =64/8
#define PART_COLUMN_BYTES  150
//EPD
void Epaper_READBUSY(void);
void Epaper_Spi_WriteByte(unsigned char TxData);
void Epaper_Write_Command(unsigned char cmd);
void Epaper_Write_Data(unsigned char data);


void Epaper_HW_SW_RESET(void);
void EPD_HW_Init(void); //Electronic paper initialization
void EPD_Update(void);

void EPD_Part_Init(void);//Local refresh initialization
void EPD_Part_Update(void); 

void EPD_WhiteScreen_White(void);
void EPD_DeepSleep(void);
//Display 
void EPD_SetRAMValue_BaseMap(const unsigned char * datas);
void EPD_Dis_Part(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE);
void EPD_Dis_Part_myself_M(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
                         unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
                         unsigned int x_startC,unsigned int y_startC,const unsigned char * datasC,
                         unsigned int x_startD,unsigned int y_startD,const unsigned char * datasD,
                         unsigned int x_startE,unsigned int y_startE,const unsigned char * datasE,
                         unsigned int PART_COLUMN,unsigned int PART_LINE
                        );
void EPD_Dis_Part_myself_S(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
                         unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
                         unsigned int x_startC,unsigned int y_startC,const unsigned char * datasC,
                         unsigned int x_startD,unsigned int y_startD,const unsigned char * datasD,
                         unsigned int x_startE,unsigned int y_startE,const unsigned char * datasE,
                         unsigned int PART_COLUMN,unsigned int PART_LINE
                        );
void EPD_Dis_Part_myself_All(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
                         unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
                         unsigned int x_startC,unsigned int y_startC,const unsigned char * datasC,
                         unsigned int x_startD,unsigned int y_startD,const unsigned char * datasD,
                         unsigned int x_startE,unsigned int y_startE,const unsigned char * datasE,
                         unsigned int PART_COLUMN,unsigned int PART_LINE
                        );
//Display canvas function
void EPD_HW_Init_GUI(void); //EPD init GUI
void EPD_Display(unsigned char *Image); 
void EPD_Standby(void);

void EPD_HW_Init_Fast(void);
void EPD_WhiteScreen_ALL_Fast(const unsigned char *datas);
  
void EPD_HW_Init_Part(void);                         
void EPD_Dis_Part_M(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE);                        
void EPD_Dis_Part_S(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE);                        

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
/*When the electronic paper is refreshed in full screen, the picture flicker is a normal phenomenon, and the main function is to clear the display afterimage in the previous picture.
  When the local refresh is performed, the screen does not flash.*/
/*When you need to transplant the driver, you only need to change the corresponding IO. The BUSY pin is the input mode and the others are the output mode. */
  
void loop() {
    unsigned char fen_L,fen_H,miao_L,miao_H; 

    /************Fast picture display(1.5s)*******************/
    EPD_HW_Init_Fast(); //EPD init Fast
    EPD_WhiteScreen_ALL_Fast(gImage_1);//EPD_picture1
    EPD_DeepSleep();//EPD_DeepSleep,Sleep instruction is necessary, please do not delete!!!
    delay(2000); //2s  
    EPD_HW_Init_Fast(); //EPD init Fast
    EPD_WhiteScreen_ALL_Fast(gImage_2);//EPD_picture1
    EPD_DeepSleep();//EPD_DeepSleep,Sleep instruction is necessary, please do not delete!!!
    delay(2000); //2s      
  
//////////////////////Partial refresh time demo/////////////////////////////////////    
    EPD_HW_Init(); //Electronic paper initialization
    EPD_SetRAMValue_BaseMap(gImage_basemap);
    EPD_HW_Init_Part();

    for(fen_H=0;fen_H<6;fen_H++)
    {
    for(fen_L=0;fen_L<10;fen_L++)
    {
    for(miao_H=0;miao_H<6;miao_H++)   
    {
    for(miao_L=0;miao_L<10;miao_L++)
    {
        EPD_Dis_Part_myself_S(64,80,Num1[fen_H],         //x-A,y-A,DATA-A
                            64+48,80,Num1[fen_L],         //x-B,y-B,DATA-B
                            64+48*2,80,gImage_dot1,       //x-C,y-C,DATA-C
                            64+48*3,80,Num1[miao_H],       //x-D,y-D,DATA-D
                            64+48*4,80,Num1[miao_L],104,48);   //x-D,y-D,DATA-D,Resolution 32*64                        
                            if((fen_L==0)&&(miao_H==0)&&(miao_L==5))
                            goto Clear;  
       } 
      }
     }  
    } 
    
    
    Clear:
    delay(1000); //1s        
    EPD_HW_Init(); //Electronic paper initialization
    EPD_WhiteScreen_White(); //Show all white
    EPD_DeepSleep(); //Enter deep sleep,Sleep instruction is necessary, please do not delete!!!
    delay(2000); //2s        
    while(1);
}












///////////////////EXTERNAL FUNCTION////////////////////////////////////////////////////////////////////////

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
  
/////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
void Epaper_READBUSY(void)
{ 
  while(1)
  {   //=1 BUSY
     if(isEPD_W21_BUSY==0) break;;
  }  
}

//SSD1683
void EPD_HW_Init(void)
{
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
  
  Epaper_READBUSY();   
  Epaper_Write_Command(0x12);  //SWRESET
  Epaper_READBUSY();   
  
  
}
void EPD_HW_Init_Fast(void)
{
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
  
  Epaper_Write_Command(0x12);  //SWRESET
  Epaper_READBUSY();   
  
  Epaper_Write_Command(0x18); //Read built-in temperature sensor
  Epaper_Write_Data(0x80);  
      
  Epaper_Write_Command(0x22); // Load temperature value
  Epaper_Write_Data(0xB1);    
  Epaper_Write_Command(0x20); 
  Epaper_READBUSY();   

  Epaper_Write_Command(0x1A); // Write to temperature register
  Epaper_Write_Data(0x64);    
  Epaper_Write_Data(0x00);  
            
  Epaper_Write_Command(0x22); // Load temperature value
  Epaper_Write_Data(0x91);    
  Epaper_Write_Command(0x20); 
  Epaper_READBUSY();   
}

/////////////////////////////////////////////////////////////////////////////////////////
/*When the electronic paper screen is updated, do not unplug the electronic paper to avoid damage to the screen*/

void EPD_Update(void)
{   
  Epaper_Write_Command(0x22); //Display Update Control
  Epaper_Write_Data(0xF7);   
  Epaper_Write_Command(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();   
  
}
void EPD_Update_Fast(void)
{   
  Epaper_Write_Command(0x22); //Display Update Control
  Epaper_Write_Data(0xC7);   
  Epaper_Write_Command(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();   

}
/*When the electronic paper screen is updated, do not unplug the electronic paper to avoid damage to the screen*/
void EPD_Part_Update(void)
{
  Epaper_Write_Command(0x22); //Display Update Control
  Epaper_Write_Data(0xFF);   
  Epaper_Write_Command(0x20); //Activate Display Update Sequence
  Epaper_READBUSY();      
}

void Set_ramMP(void) // Set RAM X - address Start / End position  ; Set RAM Y - address Start / End position  -MASTER
{
  Epaper_Write_Command(0x11);  // Data Entry mode setting
  Epaper_Write_Data(0x05);     // 1 ¨CY decrement, X increment
  Epaper_Write_Command(0x44);              // Set Ram X- address Start / End position
  Epaper_Write_Data(0x00);                 // XStart, POR = 00h
  Epaper_Write_Data(0x31); //400/8-1
  Epaper_Write_Command(0x45);                   // Set Ram Y- address  Start / End position 
  Epaper_Write_Data(0x0f);  
  Epaper_Write_Data(0x01);  //300-1 
  Epaper_Write_Data(0x00);                      // YEnd L
  Epaper_Write_Data(0x00);                      // YEnd H 
      
}

void Set_ramMA(void)  // Set RAM X address counter ;  Set RAM Y address counter                             -MASTER
{
  Epaper_Write_Command(0x4e);              
  Epaper_Write_Data(0x00);  
  Epaper_Write_Command(0x4f);  
  Epaper_Write_Data(0x0f);  
  Epaper_Write_Data(0x01);  
}

void Set_ramSP(void)  // Set RAM X - address Start / End position  ; Set RAM Y - address Start / End position  -SLAVE
{
  Epaper_Write_Command(0x91);              
  Epaper_Write_Data(0x04); 
  Epaper_Write_Command(0xc4);              // Set Ram X- address Start / End position
  Epaper_Write_Data(0x30);   //392/8-1               // XStart, POR = 00h
  Epaper_Write_Data(0x00); 
  Epaper_Write_Command(0xc5);                   // Set Ram Y- address  Start / End position 
  Epaper_Write_Data(0x0f);  
  Epaper_Write_Data(0x01);    
  Epaper_Write_Data(0x00);                      // YEnd L
  Epaper_Write_Data(0x00);                      // YEnd H   
}

void Set_ramSA(void)   // Set RAM X address counter ;  Set RAM Y address counter                             -SLAVE
{
  Epaper_Write_Command(0xce);              
  Epaper_Write_Data(0x31); 
  Epaper_Write_Command(0xcf);  
  Epaper_Write_Data(0x0f);  
  Epaper_Write_Data(0x01);  
}

//////////////////////////////All screen update////////////////////////////////////////////

//Horizontal scanning, from right to left, from bottom to top
void EPD_WhiteScreen_ALL_Fast(const unsigned char *datas)
{
  unsigned int i; 
  unsigned char tempOriginal;      
  unsigned int tempcol=0;
  unsigned int templine=0;

    Epaper_Write_Command(0x11);     
    Epaper_Write_Data(0x05); 
  
    Epaper_Write_Command(0x44); //set Ram-X address start/end position   
    Epaper_Write_Data(0x00);
    Epaper_Write_Data(0x31);    //0x12-->(18+1)*8=152
  
    Epaper_Write_Command(0x45); //set Ram-Y address start/end position          
    Epaper_Write_Data(0x0F);   //0x97-->(151+1)=152      
    Epaper_Write_Data(0x01);
    Epaper_Write_Data(0x00);
    Epaper_Write_Data(0x00); 

    Epaper_Write_Command(0x4E);     
    Epaper_Write_Data(0x00);  
    Epaper_Write_Command(0x4F);       
    Epaper_Write_Data(0x0F);
    Epaper_Write_Data(0x01);  
  
  Epaper_READBUSY();
    Epaper_Write_Command(0x24);   //write RAM for black(0)/white (1)  
  for(i=0;i<Source_BYTES*Gate_BITS;i++)
   {          
     tempOriginal=*(datas+templine*Source_BYTES*2+tempcol);
     templine++;
     if(templine>=Gate_BITS)
     {
       tempcol++;
       templine=0;
     }     
     Epaper_Write_Data(~tempOriginal);
   } 
   
    Epaper_Write_Command(0x26);   //write RAM for black(0)/white (1)  
  for(i=0;i<Source_BYTES*Gate_BITS;i++)
   {            
     Epaper_Write_Data(0X00);
   }   
   

    Epaper_Write_Command(0x91);     
    Epaper_Write_Data(0x04); 
  
    Epaper_Write_Command(0xC4); //set Ram-X address start/end position   
    Epaper_Write_Data(0x31);
    Epaper_Write_Data(0x00);    //0x12-->(18+1)*8=152
  
    Epaper_Write_Command(0xC5); //set Ram-Y address start/end position          
    Epaper_Write_Data(0x0F);   //0x97-->(151+1)=152  ÐÞ¸ÄµÄ    
    Epaper_Write_Data(0x01);
    Epaper_Write_Data(0x00);
    Epaper_Write_Data(0x00); 
    
    Epaper_Write_Command(0xCE);     
    Epaper_Write_Data(0x31);  
    Epaper_Write_Command(0xCF);       
    Epaper_Write_Data(0x0F);
    Epaper_Write_Data(0x01);  
  
    Epaper_READBUSY();
    tempcol=tempcol-1; //Byte dislocation processing
    templine=0;
    Epaper_Write_Command(0xa4);   //write RAM for black(0)/white (1)
  for(i=0;i<Source_BYTES*Gate_BITS;i++)
   {          
     tempOriginal=*(datas+templine*Source_BYTES*2+tempcol);
     templine++;
     if(templine>=Gate_BITS)
     {
       tempcol++;
       templine=0;
     }     
     Epaper_Write_Data(~tempOriginal);
   } 
   
    Epaper_Write_Command(0xa6);   //write RAM for black(0)/white (1)  
  for(i=0;i<Source_BYTES*Gate_BITS;i++)
   {            
     Epaper_Write_Data(0X00);
   }  
   
   EPD_Update_Fast();  
}
///////////////////////////Part update//////////////////////////////////////////////
//The x axis is reduced by one byte, and the y axis is reduced by one pixel.
void EPD_SetRAMValue_BaseMap( const unsigned char * datas)
{
  unsigned int i; 
    unsigned char tempOriginal;      
  unsigned int tempcol=0;
  unsigned int templine=0;
Set_ramMP();
Set_ramMA();
  Epaper_Write_Command(0x24);   
  for(i=0;i<Source_BYTES*Gate_BITS;i++)
  {
     tempOriginal=*(datas+templine*Source_BYTES*2+tempcol);
     templine++;
     if(templine>=Gate_BITS)
     {
       tempcol++;
       templine=0;
     }     
     Epaper_Write_Data(~tempOriginal);
     //Epaper_Write_Data(0xff);
  }

Set_ramMA();
  Epaper_Write_Command(0x26);   
  for(i=0;i<Source_BYTES*Gate_BITS;i++)
  {
      Epaper_Write_Data(0x00);
  }

tempcol=tempcol-1; //Byte dislocation processing
templine=0;
Set_ramSP();
Set_ramSA();
  Epaper_Write_Command(0xA4);   
  for(i=0;i<Source_BYTES*Gate_BITS;i++)
  {
     tempOriginal=*(datas+templine*Source_BYTES*2+tempcol);
     templine++;
     if(templine>=Gate_BITS)
     {
       tempcol++;
       templine=0;
     }     
     Epaper_Write_Data(~tempOriginal); 
    // Epaper_Write_Data(0xff);    
  }

Set_ramSA();
  Epaper_Write_Command(0xA6);   
  for(i=0;i<Source_BYTES*Gate_BITS;i++)
  {
      Epaper_Write_Data(0x00);
  } 
  EPD_Update();
  
  
  
   //basemap  
Set_ramMA();
  tempcol=0;
  templine=0;
  Epaper_Write_Command(0x26);   
  for(i=0;i<Source_BYTES*Gate_BITS;i++)
  {
     tempOriginal=*(datas+templine*Source_BYTES*2+tempcol);
     templine++;
     if(templine>=Gate_BITS)
     {
       tempcol++;
       templine=0;
     }     
     Epaper_Write_Data(~tempOriginal);
     //Epaper_Write_Data(0xff);
  }
Set_ramSA();
  tempcol=tempcol-1; //Byte dislocation processing
  templine=0;
  Epaper_Write_Command(0xa6);   
  for(i=0;i<Source_BYTES*Gate_BITS;i++)
  {
     tempOriginal=*(datas+templine*Source_BYTES*2+tempcol);
     templine++;
     if(templine>=Gate_BITS)
     {
       tempcol++;
       templine=0;
     }     
     Epaper_Write_Data(~tempOriginal);
     //Epaper_Write_Data(0xff);
  } 
}


void EPD_DeepSleep(void)
{   
  Epaper_Write_Command(0x10); //enter deep sleep
  Epaper_Write_Data(0x01); 
  delay(100);
}



/////////////////////////////////Single display////////////////////////////////////////////////

void EPD_WhiteScreen_White(void)

{
  unsigned int i; 
  
Set_ramMP();
Set_ramMA();
  Epaper_Write_Command(0x24);   
  for(i=0;i<Source_BYTES*Gate_BITS;i++)
  {
      Epaper_Write_Data(0xff);
  }

Set_ramMA();
  Epaper_Write_Command(0x26);   
  for(i=0;i<Source_BYTES*Gate_BITS;i++)
  {
      Epaper_Write_Data(0x00);
  }

    
Set_ramSP();
Set_ramSA();
  Epaper_Write_Command(0xA4);   
  for(i=0;i<Source_BYTES*Gate_BITS;i++)
  {
      Epaper_Write_Data(0xff);
  }

Set_ramSA();
  Epaper_Write_Command(0xA6);   
  for(i=0;i<Source_BYTES*Gate_BITS;i++)
  {
      Epaper_Write_Data(0x00);
  } 
  EPD_Update();
  

}
void EPD_HW_Init_Part(void)
{
  EPD_W21_RST_0;  // Module reset   
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10); //At least 10ms delay 
  
  Epaper_READBUSY();   
  Epaper_Write_Command(0x12);  //SWRESET
  Epaper_READBUSY();   

  Epaper_Write_Command(0x11);  // Data Entry mode setting
  Epaper_Write_Data(0x03);     // 1 ¨CY decrement, X increment
    
  Epaper_Write_Command(0x3C); //BorderWavefrom
  Epaper_Write_Data(0x80);    
  
}


//Horizontal scanning
void EPD_Dis_Part_M(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE)
{
  unsigned int i,j;  
  unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
  
  char tempData,data1;

  
  
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
  
  Epaper_Write_Command(0x22); 
  Epaper_Write_Data(0xc0);   
  Epaper_Write_Command(0x20); 
  Epaper_READBUSY();  
  
//  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_start);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    // ????=0 


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_start); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN;i++)
      for(j=0;j<PART_LINE/8;j++)
       {   
         //Byte image processing
         tempData=datas[i*(PART_LINE/8)+(PART_LINE/8)-j-1];  
         data1=(tempData>>7&0x01)+(tempData>>5&0x02)+(tempData>>3&0x04)+(tempData>>1&0x08)+(tempData<<7&0x80)+(tempData<<5&0x40)+(tempData<<3&0x20)+(tempData<<1&0x10); //×Ö½ÚÄÚÊý¾Ý·´Ðò
         Epaper_Write_Data(~data1); 
         
       }

   EPD_Part_Update();

}
void EPD_Dis_Part_S(unsigned int x_start,unsigned int y_start,const unsigned char * datas,unsigned int PART_COLUMN,unsigned int PART_LINE)
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

  
  Epaper_Write_Command(0x22); 
  Epaper_Write_Data(0xc0);   
  Epaper_Write_Command(0x20); 
  Epaper_READBUSY();  
  
//
  Epaper_Write_Command(0x91);                
  Epaper_Write_Data(0x03); 
  Epaper_Write_Command(0xC4);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_start);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0xC5);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    // ????=0 


  Epaper_Write_Command(0xCE);   // set RAM x address count to 0;
  Epaper_Write_Data(x_start); 
  Epaper_Write_Command(0xCF);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0xA4);   //Write Black and White image to RAM
   
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {   
     Epaper_Write_Data(~datas[i]);
   }

   EPD_Part_Update();

}

/////////////////////////////////////TIME///////////////////////////////////////////////////
void EPD_Dis_Part_myself_M(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
                         unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
                         unsigned int x_startC,unsigned int y_startC,const unsigned char * datasC,
                         unsigned int x_startD,unsigned int y_startD,const unsigned char * datasD,
                         unsigned int x_startE,unsigned int y_startE,const unsigned char * datasE,
                         unsigned int PART_COLUMN,unsigned int PART_LINE
                        )
{
  unsigned int i,j;  
  unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
  
  char tempData,data1;

  
  //Data A//////////////////////////////
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

//
  Epaper_Write_Command(0x22); 
  Epaper_Write_Data(0xc0);   
  Epaper_Write_Command(0x20); 
  Epaper_READBUSY();  
  
//  
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startA);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startA); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN;i++)
      for(j=0;j<PART_LINE/8;j++)
       {   
         //Byte image processing   
         tempData=datasA[i*(PART_LINE/8)+(PART_LINE/8)-j-1];   
         data1=(tempData>>7&0x01)+(tempData>>5&0x02)+(tempData>>3&0x04)+(tempData>>1&0x08)+(tempData<<7&0x80)+(tempData<<5&0x40)+(tempData<<3&0x20)+(tempData<<1&0x10); //×Ö½ÚÄÚÊý¾Ý·´Ðò
         Epaper_Write_Data(~data1); 
         
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
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startB);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);   


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startB); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN;i++)
      for(j=0;j<PART_LINE/8;j++)
       {   
         //Byte image processing 
         tempData=datasB[i*(PART_LINE/8)+(PART_LINE/8)-j-1];   
         data1=(tempData>>7&0x01)+(tempData>>5&0x02)+(tempData>>3&0x04)+(tempData>>1&0x08)+(tempData<<7&0x80)+(tempData<<5&0x40)+(tempData<<3&0x20)+(tempData<<1&0x10); //×Ö½ÚÄÚÊý¾Ý·´Ðò
         Epaper_Write_Data(~data1); 
         
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
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startC);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);   


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startC); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN;i++)
      for(j=0;j<PART_LINE/8;j++)
       {   
         //Byte image processing 
         tempData=datasC[i*(PART_LINE/8)+(PART_LINE/8)-j-1];   
         data1=(tempData>>7&0x01)+(tempData>>5&0x02)+(tempData>>3&0x04)+(tempData>>1&0x08)+(tempData<<7&0x80)+(tempData<<5&0x40)+(tempData<<3&0x20)+(tempData<<1&0x10); //×Ö½ÚÄÚÊý¾Ý·´Ðò
         Epaper_Write_Data(~data1); 
         
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
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startD);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);        // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startD); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN;i++)
      for(j=0;j<PART_LINE/8;j++)
       {   
         //Byte image processing
         tempData=datasD[i*(PART_LINE/8)+(PART_LINE/8)-j-1];   
         data1=(tempData>>7&0x01)+(tempData>>5&0x02)+(tempData>>3&0x04)+(tempData>>1&0x08)+(tempData<<7&0x80)+(tempData<<5&0x40)+(tempData<<3&0x20)+(tempData<<1&0x10); //×Ö½ÚÄÚÊý¾Ý·´Ðò
         Epaper_Write_Data(~data1); 
         
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
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startE);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startE); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN;i++)
      for(j=0;j<PART_LINE/8;j++)
       {   
         //Byte image processing     
         tempData=datasE[i*(PART_LINE/8)+(PART_LINE/8)-j-1];   
         data1=(tempData>>7&0x01)+(tempData>>5&0x02)+(tempData>>3&0x04)+(tempData>>1&0x08)+(tempData<<7&0x80)+(tempData<<5&0x40)+(tempData<<3&0x20)+(tempData<<1&0x10); //×Ö½ÚÄÚÊý¾Ý·´Ðò
         Epaper_Write_Data(~data1); 
         
       }  
   EPD_Part_Update(); 

}

void EPD_Dis_Part_myself_S(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
                         unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
                         unsigned int x_startC,unsigned int y_startC,const unsigned char * datasC,
                         unsigned int x_startD,unsigned int y_startD,const unsigned char * datasD,
                         unsigned int x_startE,unsigned int y_startE,const unsigned char * datasE,
                         unsigned int PART_COLUMN,unsigned int PART_LINE
                        )
{
  unsigned int i;  
  unsigned int x_end,y_start1,y_start2,y_end1,y_end2;


  //Data A//////////////////////////////
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

//
  Epaper_Write_Command(0x22); 
  Epaper_Write_Data(0xc0);   
  Epaper_Write_Command(0x20); 
  Epaper_READBUSY();  

  Epaper_Write_Command(0x91);                
  Epaper_Write_Data(0x03);  
//  
  
  Epaper_Write_Command(0xC4);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startA);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0xC5);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    


  Epaper_Write_Command(0xCE);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startA); 
  Epaper_Write_Command(0xCF);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0xA4);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {   
     Epaper_Write_Data(~datasA[i]);
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
  
  Epaper_Write_Command(0xC4);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startB);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0xC5);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);   


  Epaper_Write_Command(0xCE);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startB); 
  Epaper_Write_Command(0xCF);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0xA4);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {   
     Epaper_Write_Data(~datasB[i]);
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
  
  Epaper_Write_Command(0xC4);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startC);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0xC5);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);   


  Epaper_Write_Command(0xCE);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startC); 
  Epaper_Write_Command(0xCF);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0xA4);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {   
     Epaper_Write_Data(~datasC[i]);
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
  
  Epaper_Write_Command(0xC4);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startD);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);        // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0xC5);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    


  Epaper_Write_Command(0xCE);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startD); 
  Epaper_Write_Command(0xCF);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0xA4);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {   
     Epaper_Write_Data(~datasD[i]);
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
  
  Epaper_Write_Command(0xC4);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startE);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0xC5);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    


  Epaper_Write_Command(0xCE);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startE); 
  Epaper_Write_Command(0xCF);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0xA4);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {   
     Epaper_Write_Data(~datasE[i]);
   }
 
   EPD_Part_Update(); 

}

void EPD_Dis_Part_myself_All(unsigned int x_startA,unsigned int y_startA,const unsigned char * datasA,
                         unsigned int x_startB,unsigned int y_startB,const unsigned char * datasB,
                         unsigned int x_startC,unsigned int y_startC,const unsigned char * datasC,
                         unsigned int x_startD,unsigned int y_startD,const unsigned char * datasD,
                         unsigned int x_startE,unsigned int y_startE,const unsigned char * datasE,
                         unsigned int PART_COLUMN,unsigned int PART_LINE
                        )
{
  unsigned int i,j;  
  unsigned int x_end,y_start1,y_start2,y_end1,y_end2;
  char tempData,data1;

  
  
//
  Epaper_Write_Command(0x22); 
  Epaper_Write_Data(0xc0);   
  Epaper_Write_Command(0x20); 
  Epaper_READBUSY();  

  Epaper_Write_Command(0x91);                
  Epaper_Write_Data(0x03);  
//  
  
  //Data A//////////////////////////////
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
  
  
  Epaper_Write_Command(0xC4);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startA);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0xC5);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    


  Epaper_Write_Command(0xCE);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startA); 
  Epaper_Write_Command(0xCF);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0xA4);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {   
     Epaper_Write_Data(~datasA[i]);
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
  
  Epaper_Write_Command(0xC4);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startB);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0xC5);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);   


  Epaper_Write_Command(0xCE);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startB); 
  Epaper_Write_Command(0xCF);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0xA4);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {   
     Epaper_Write_Data(~datasB[i]);
   }
   
  //Data C/////////////////////////////////////

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
  
  Epaper_Write_Command(0xC4);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startC);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0xC5);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);   


  Epaper_Write_Command(0xCE);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startC); 
  Epaper_Write_Command(0xCF);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0xA4);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN*PART_LINE/8;i++)
   {   
     Epaper_Write_Data(~datasC[i]);
   }  
   
  /***************************************************************************/ 
//
  Epaper_Write_Command(0x22); 
  Epaper_Write_Data(0xc0);   
  Epaper_Write_Command(0x20); 
  Epaper_READBUSY();    
//
       
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
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startD);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);    // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);   


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startD); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM
   for(i=0;i<PART_COLUMN;i++)
      for(j=0;j<PART_LINE/8;j++)
       {   
         //Byte image processing   
         tempData=datasD[i*(PART_LINE/8)+(PART_LINE/8)-j-1];   
         data1=(tempData>>7&0x01)+(tempData>>5&0x02)+(tempData>>3&0x04)+(tempData>>1&0x08)+(tempData<<7&0x80)+(tempData<<5&0x40)+(tempData<<3&0x20)+(tempData<<1&0x10); //×Ö½ÚÄÚÊý¾Ý·´Ðò
         Epaper_Write_Data(~data1); 
         
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
  
  Epaper_Write_Command(0x44);       // set RAM x address start/end, in page 35
  Epaper_Write_Data(x_startE);    // RAM x address start at 00h;
  Epaper_Write_Data(x_end);        // RAM x address end at 0fh(15+1)*8->128 
  Epaper_Write_Command(0x45);       // set RAM y address start/end, in page 35
  Epaper_Write_Data(y_start2);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_start1);    // RAM y address start at 0127h;
  Epaper_Write_Data(y_end2);    // RAM y address end at 00h;
  Epaper_Write_Data(y_end1);    


  Epaper_Write_Command(0x4E);   // set RAM x address count to 0;
  Epaper_Write_Data(x_startE); 
  Epaper_Write_Command(0x4F);   // set RAM y address count to 0X127;    
  Epaper_Write_Data(y_start2);
  Epaper_Write_Data(y_start1);
  
  
   Epaper_Write_Command(0x24);   //Write Black and White image to RAM

   for(i=0;i<PART_COLUMN;i++)
      for(j=0;j<PART_LINE/8;j++)
       {   
         //Byte image processing 
         tempData=datasE[i*(PART_LINE/8)+(PART_LINE/8)-j-1];   
         data1=(tempData>>7&0x01)+(tempData>>5&0x02)+(tempData>>3&0x04)+(tempData>>1&0x08)+(tempData<<7&0x80)+(tempData<<5&0x40)+(tempData<<3&0x20)+(tempData<<1&0x10); //×Ö½ÚÄÚÊý¾Ý·´Ðò
         Epaper_Write_Data(~data1); 
         
       }
   EPD_Part_Update(); 

}

//////////////////////////////////END//////////////////////////////////////////////////
