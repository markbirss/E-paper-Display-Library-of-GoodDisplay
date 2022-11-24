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
//648x480///////////////////////////////////////

unsigned char Old_data[38880];
////////FUNCTION//////
//EPD
//EPD
void EPD_W21_Init(void);
void EPD_init(void);
void PIC_display (const unsigned char* picData);
void EPD_sleep(void);
void EPD_refresh(void);
void lcd_chkstatus(void);
void PIC_display_Clear(void);
//Display canvas function

void EPD_init_GUI(void);        
void EPD_Refresh(void);     
void EPD_display_init(void);    
void EPD_partial_display(int x_start,int y_start,const unsigned char *new_data,unsigned int PART_COLUMN,unsigned int PART_LINE,unsigned char mode); //partial display 
void EPD_init_Fast(void); 
void PIC_display_Part(const unsigned char* picData);  

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
  ///////////////////////////Normal picture display/////////////////////////////////////////////////////////////////
    /************Normal picture display*******************/
    EPD_init(); //EPD init
    PIC_display(gImage_1);//EPD_picture1
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    delay(2000); //2s  

    EPD_init_Fast(); //EPD init Fast
    PIC_display(gImage_s1);//EPD_picture1
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!! 
    delay(1000); //1s  
    EPD_init_Fast(); //EPD init Fast
    PIC_display(gImage_basemap);//EPD_picture1
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!!
    //Partial refresh display 
    EPD_display_init();
    EPD_partial_display(304,180,gImage_00,104,48,1);  //x,y,old_data,new_data,W,L,mode 
    EPD_partial_display(304,180,gImage_11,104,48,1);  //x,y,old_data,new_data,W,L,mode 
    EPD_partial_display(304,180,gImage_22,104,48,1);  //x,y,old_data,new_data,W,L,mode 
    EPD_partial_display(304,180,gImage_33,104,48,1);  //x,y,old_data,new_data,W,L,mode 
    EPD_partial_display(304,180,gImage_44,104,48,1);  //x,y,old_data,new_data,W,L,mode 
    EPD_partial_display(304,180,gImage_55,104,48,1);  //x,y,old_data,new_data,W,L,mode 
    EPD_partial_display(304,180,gImage_77,104,48,1);  //x,y,old_data,new_data,W,L,mode 
    EPD_partial_display(304,180,gImage_88,104,48,1);  //x,y,old_data,new_data,W,L,mode 
    EPD_partial_display(304,180,gImage_99,104,48,1);  //x,y,old_data,new_data,W,L,mode 

    //Clear
    EPD_init_Fast(); //EPD init Fast
    PIC_display_Clear();//EPD Clear
    EPD_sleep();//EPD_sleep,Sleep instruction is necessary, please do not delete!!! 
    while(1);
  
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


void EPD_W21_Init(void)
{
  EPD_W21_RST_0;    // Module reset
  delay(10);//At least 10ms delay 
  EPD_W21_RST_1;
  delay(10);//At least 10ms delay 
  
}
void EPD_Refresh(void)
{
  EPD_W21_WriteCMD(0x92);   //Enter normal mode
  //Refresh
  EPD_W21_WriteCMD(0x12);   //DISPLAY REFRESH   
  delay(1);              //!!!The delay here is necessary, 200uS at least!!!     
  lcd_chkstatus();   
}


//UC8179
void EPD_init(void)
{ 
    EPD_W21_Init();     //Electronic paper IC reset 
    delay(100);         //reset IC and select BUS   

    EPD_W21_WriteCMD(0x00);
    EPD_W21_WriteDATA(0x1F);
    
    EPD_W21_WriteCMD(0x04); //POWER ON
    delay(100);  
    lcd_chkstatus();
    EPD_W21_WriteCMD(0X50);     //VCOM AND DATA INTERVAL SETTING
    EPD_W21_WriteDATA(0x21);
    EPD_W21_WriteDATA(0x07);
    
    
}
void EPD_init_Fast(void)
{ 
    EPD_W21_Init();     //Electronic paper IC reset 
    delay(100);         //reset IC and select BUS   
    EPD_W21_WriteCMD(0x00);
    EPD_W21_WriteDATA(0x1F); 
    EPD_W21_WriteCMD(0x04); //POWER ON
    delay(100);  
    lcd_chkstatus();
    EPD_W21_WriteCMD(0X50);     //VCOM AND DATA INTERVAL SETTING
    EPD_W21_WriteDATA(0x29);
    EPD_W21_WriteDATA(0x07);
    
    EPD_W21_WriteCMD(0xE0);
    EPD_W21_WriteDATA(0x02);
    EPD_W21_WriteCMD(0xE5);
    EPD_W21_WriteDATA(0x5A);
    
    
}
void EPD_display_init(void)
{ 
    EPD_W21_Init();   
    delay(100);         //reset IC and select BUS 

    EPD_W21_WriteCMD(0x00);
    EPD_W21_WriteDATA(0x1F);
  
    EPD_W21_WriteCMD(0x04); //POWER ON
    delay(100);  
    lcd_chkstatus();

    EPD_W21_WriteCMD(0xE0);
    EPD_W21_WriteDATA(0x02);
    EPD_W21_WriteCMD(0xE5);
    EPD_W21_WriteDATA(0x6E);

}

void EPD_partial_display(int x_start,int y_start,const unsigned char *new_data,unsigned int PART_COLUMN,unsigned int PART_LINE,unsigned char mode) //partial display 
{
  unsigned int i,count;  
  unsigned int x_end,y_end;
  
  x_end=x_start+PART_LINE-1; 
  y_end=y_start+PART_COLUMN-1;
    count=PART_COLUMN*PART_LINE/8;  
  
    EPD_W21_WriteCMD(0x50);
    EPD_W21_WriteDATA(0xA9);
    EPD_W21_WriteDATA(0x07);
  
    EPD_W21_WriteCMD(0x91);   //This command makes the display enter partial mode
    EPD_W21_WriteCMD(0x90);   //resolution setting
    EPD_W21_WriteDATA (x_start/256);
    EPD_W21_WriteDATA (x_start%256);   //x-start    
    
    EPD_W21_WriteDATA (x_end/256);    
    EPD_W21_WriteDATA (x_end%256-1);  //x-end 
  
    EPD_W21_WriteDATA (y_start/256);  //
    EPD_W21_WriteDATA (y_start%256);   //y-start    
    
    EPD_W21_WriteDATA (y_end/256);    
    EPD_W21_WriteDATA (y_end%256-1);  //y-end
    EPD_W21_WriteDATA (0x01); 

  EPD_W21_WriteCMD(0x13);        //writes New data to SRAM.
    for(i=0;i<count;i++)       
   {
    EPD_W21_WriteDATA(new_data[i]);  
   }
    
  
      
  EPD_W21_WriteCMD(0x12);    //DISPLAY REFRESH                 
  delay(10);     //!!!The delay here is necessary, 200uS at least!!!     
  lcd_chkstatus();
  EPD_W21_WriteCMD(0X92);   //This command makes the display exit partial mode and enter normal mode. 
 
} 
//UC8179
void EPD_init_GUI(void)
{ 
    unsigned int i;
    EPD_W21_Init();     //Electronic paper IC reset 

    EPD_W21_WriteCMD(0x04); //POWER ON
    lcd_chkstatus();        //waiting for the electronic paper IC to release the idle signal
  
    EPD_W21_WriteCMD(0X00);     //PANNEL SETTING
    EPD_W21_WriteDATA(0x0F);   //KW-3f   KWR-2F BWROTP 0f BWOTP 1f
  

    EPD_W21_WriteCMD(0X50);     //VCOM AND DATA INTERVAL SETTING
    EPD_W21_WriteDATA(0x20);
    EPD_W21_WriteDATA(0x07);
    
    EPD_W21_WriteCMD(0x10);        //Transfer old data
    for(i=0;i<38880;i++)    
    { 
    EPD_W21_WriteDATA(0x00); 
    }
    EPD_W21_WriteCMD(0x13);        //Transfer new data
    for(i=0;i<38880;i++)       
    {
    EPD_W21_WriteDATA(0x00);  //Transfer the actual displayed data
    }
}
void EPD_sleep(void)
{
    EPD_W21_WriteCMD(0X50);  //VCOM AND DATA INTERVAL SETTING     
    EPD_W21_WriteDATA(0xf7); //WBmode:VBDF 17|D7 VBDW 97 VBDB 57    WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7  

    EPD_W21_WriteCMD(0X02);   //power off
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal
    EPD_W21_WriteCMD(0X07);   //deep sleep
    EPD_W21_WriteDATA(0xA5);
}



void PIC_display(const unsigned char* picData)
{
    unsigned int i;
    //Write Data
    EPD_W21_WriteCMD(0x10);      
    for(i=0;i<38880;i++)       
    {
      EPD_W21_WriteDATA(0xff); 
    }
    EPD_W21_WriteCMD(0x13);      
    for(i=0;i<38880;i++)       
    {   
      
      EPD_W21_WriteDATA(picData[i]);        
    }
   //Refresh
    EPD_W21_WriteCMD(0x12);   //DISPLAY REFRESH   
    delay(1);   //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal


}

void PIC_display_Part(const unsigned char* picData)
{
    unsigned int i;
    //Write Data
    EPD_W21_WriteCMD(0x10);      
    for(i=0;i<38880;i++)       
    {
      EPD_W21_WriteDATA(Old_data[i]); 
    }
    EPD_W21_WriteCMD(0x13);      
    for(i=0;i<38880;i++)       
    {   
      
      EPD_W21_WriteDATA(picData[i]);   
      Old_data[i]=picData[i];     
    }
   //Refresh
    EPD_W21_WriteCMD(0x12);   //DISPLAY REFRESH   
    delay(1);   //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal


}
void PIC_display_Clear(void)
{
    unsigned int i;
    //Write Data
    EPD_W21_WriteCMD(0x10);      
    for(i=0;i<38880;i++)       
    {
      EPD_W21_WriteDATA(0xff);  
    }
    EPD_W21_WriteCMD(0x13);      
    for(i=0;i<38880;i++)       
    {
      EPD_W21_WriteDATA(0xff);  
    }
    //Refresh
    EPD_W21_WriteCMD(0x12);   //DISPLAY REFRESH   
    delay(1);              //!!!The delay here is necessary, 200uS at least!!!     
    lcd_chkstatus();          //waiting for the electronic paper IC to release the idle signal

}

void lcd_chkstatus(void)
{
  while(!isEPD_W21_BUSY); //0:BUSY, 1:FREE                     
}





//////////////////////////////////END//////////////////////////////////////////////////
