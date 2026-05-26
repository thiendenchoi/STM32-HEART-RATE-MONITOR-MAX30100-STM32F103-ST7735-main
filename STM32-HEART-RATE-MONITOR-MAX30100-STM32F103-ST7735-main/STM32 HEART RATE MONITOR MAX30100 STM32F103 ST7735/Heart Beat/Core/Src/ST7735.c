#include <ST7735.h>
#include "stm32f1xx_hal.h"              // Keil::Device:STM32Cube HAL:Common

//**********************Start of ST7735 LCD APIs*********************/


extern SPI_HandleTypeDef hspi2;


	
void lcd7735_senddata(unsigned char data) {
  HAL_SPI_Transmit(&hspi2, &data,1,0x1);
}
void lcd7735_send16bData(uint8_t msb,uint8_t lsb) {
	uint8_t masData[]={lsb,msb};
	HAL_SPI_Transmit(&hspi2,masData,1,0x1);
}





static int16_t _width = ST7735_TFTWIDTH;
static int16_t _height = ST7735_TFTHEIGHT;
uint32_t StX=0; 
uint32_t StY=0; 
uint16_t StTextColor = YELLOW;
static uint8_t ColStart, RowStart; 
void static writedata(uint8_t d);
void static setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void static writecommand(uint8_t c) ;

void ST7735_SetCursor(uint32_t newX, uint32_t newY);
void standard_Init_Cmd(void);


void ST7735_Init(void) {


 standard_Init_Cmd();
	
  ST7735_SetCursor(0,0);
  StTextColor = YELLOW;
  ST7735_FillScreen(BLACK);        
}

void ST7735_SetCursor(uint32_t newX, uint32_t newY){
  if((newX > 20) || (newY > 15)){       
    return;                           
  }
  StX = newX;
  StY = newY;
}



void ST7735_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  uint8_t hi = color >> 8, lo = color;

  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width)  w = _width  - x;
  if((y + h - 1) >= _height) h = _height - y;

  setAddrWindow(x, y, x+w-1, y+h-1);

  for(y=h; y>0; y--) {
    for(x=w; x>0; x--) {
      writedata(hi);
      writedata(lo);
    }
  }
}


void ST7735_FillScreen(uint16_t color) {
  ST7735_FillRect(0, 0, _width, _height, color);  
}

void static writecommand(uint8_t c) {
  LCD_DC0;
  lcd7735_senddata(c);
}
void static writedata(uint8_t d) {
  LCD_DC1;/
   lcd7735_senddata(d);
}

void static setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {

  writecommand(ST7735_CASET);
  writedata(0x00);
  writedata(x0+ColStart);
  writedata(0x00);
  writedata(x1+ColStart);

  writecommand(ST7735_RASET);
  writedata(0x00);
  writedata(y0+RowStart);
  writedata(0x00);
  writedata(y1+RowStart);

  writecommand(ST7735_RAMWR);
}


void standard_Init_Cmd(void)
{
  LCD_CS0;
  LCD_RST0;

  HAL_Delay(10);      

  LCD_RST1;
  HAL_Delay(10);      

   
   lcd7735_sendCmd(0x11); 

   HAL_Delay(120);      

   lcd7735_sendCmd (0x3A);
   lcd7735_sendData(0x05);
	 lcd7735_sendCmd (0x36);
	 lcd7735_sendData(0x14);
	 lcd7735_sendCmd (0x29);
}

void static pushColor(uint16_t color) {
  writedata((uint8_t)(color >> 8));
  writedata((uint8_t)color);
}

void ST7735_DrawPixel(int16_t x, int16_t y, uint16_t color) {

  if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;

  setAddrWindow(x,y,x,y);

  pushColor(color);
}



void ST7735_DrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  uint8_t hi = color >> 8, lo = color;


  if((x >= _width) || (y >= _height)) return;
  if((y+h-1) >= _height) h = _height-y;
  setAddrWindow(x, y, x, y+h-1);

  while (h--) {
    writedata(hi);
    writedata(lo);
  }
}

void ST7735_DrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  uint8_t hi = color >> 8, lo = color;


  if((x >= _width) || (y >= _height)) return;
  if((x+w-1) >= _width)  w = _width-x;
  setAddrWindow(x, y, x+w-1, y);

  while (w--) {
    writedata(hi);
    writedata(lo);
  }
}

void ST7735_DrawBitmap(int16_t x, int16_t y, const uint16_t *image, int16_t w, int16_t h){
  int16_t skipC = 0;                      
  int16_t originalWidth = w;              
  int i = w*(h - 1);

  if((x >= _width) || ((y - h + 1) >= _height) || ((x + w) <= 0) || (y < 0)){
    return;                             
  }
  if((w > _width) || (h > _height)){    
   
    return;
  }
  if((x + w - 1) >= _width){            
    skipC = (x + w) - _width;           
    w = _width - x;
  }
  if((y - h + 1) < 0){                  
    i = i - (h - y - 1)*originalWidth;  
    h = y + 1;
  }
  if(x < 0){                            
    w = w + x;
    skipC = -1*x;                       
    i = i - x;                          
    x = 0;
  }
  if(y >= _height){                     
    h = h - (y - _height + 1);
    y = _height - 1;
  }

  setAddrWindow(x, y-h+1, x+w-1, y);

  for(y=0; y<h; y=y+1){
    for(x=0; x<w; x=x+1){
                                        
      writedata((uint8_t)(image[i] >> 8));
                                        
      writedata((uint8_t)image[i]);
      i = i + 1;                       
    }
    i = i + skipC;
    i = i - 2*originalWidth;
  }
}

void ST7735_DrawCharS(int16_t x, int16_t y, char c, int16_t textColor, int16_t bgColor, uint8_t size){
  uint8_t line; 
  int32_t i, j;
  if((x >= _width)            || 
     (y >= _height)           || 
     ((x + 5 * size - 1) < 0) || 
     ((y + 8 * size - 1) < 0))   
    return;

  for (i=0; i<6; i++ ) {
    if (i == 5)
      line = 0x0;
    else
      line = Font[(c*5)+i];
    for (j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) 
          ST7735_DrawPixel(x+i, y+j, textColor);
        else {  
          ST7735_FillRect(x+(i*size), y+(j*size), size, size, textColor);
        }
      } else if (bgColor != textColor) {
        if (size == 1) // default size
          ST7735_DrawPixel(x+i, y+j, bgColor);
        else {  // big size
          ST7735_FillRect(x+i*size, y+j*size, size, size, bgColor);
        }
      }
      line >>= 1;
    }
  }
}

void ST7735_DrawChar(int16_t x, int16_t y, char c, int16_t textColor, int16_t bgColor, uint8_t size){
  uint8_t line;
  int32_t col, row, i, j;
  if(((x + 5*size - 1) >= _width)  ||
     ((y + 8*size - 1) >= _height) ||
     ((x + 5*size - 1) < 0)        ||
     ((y + 8*size - 1) < 0)){
    return;
  }

  setAddrWindow(x, y, x+6*size-1, y+8*size-1);

  line = 0x01;
  for(row=0; row<8; row=row+1){
    for(i=0; i<size; i=i+1){

      for(col=0; col<5; col=col+1){
        if(Font[(c*5)+col]&line){

          for(j=0; j<size; j=j+1){
            pushColor(textColor);
          }
        } else{

          for(j=0; j<size; j=j+1){
            pushColor(bgColor);
          }
        }
      }

      for(j=0; j<size; j=j+1){
        pushColor(bgColor);
      }
    }
    line = line<<1;
  }
}
uint16_t ST7735_Color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);
}

uint32_t ST7735_DrawString(uint16_t x, uint16_t y, char *pt, int16_t textColor){
  uint32_t count = 0;
  if(y>15) return 0;
  while(*pt){
    ST7735_DrawCharS(x*6, y*10, *pt, textColor, BLACK, 1);
    pt++;
    x = x+1;
    if(x>20) return count;
    count++;
  }
  return count;
}

int32_t Ymax,Ymin,X;
int32_t Yrange;


void ST7735_PlotClear(int32_t ymin, int32_t ymax){
  ST7735_FillRect(0, 32, 128, 128, ST7735_Color565(228,228,228));
  if(ymax>ymin){
    Ymax = ymax;
    Ymin = ymin;
    Yrange = ymax-ymin;
  } else{
    Ymax = ymin;
    Ymin = ymax;
    Yrange = ymax-ymin;
  }

  X = 0;
}
int TimeIndex;               
int32_t Ymax, Ymin, Yrange;  
uint16_t PlotBGColor; 

void ST7735_SimplePlotPoint(int32_t y)
{
		int32_t j;
  if(y<Ymin) y=Ymin;
  if(y>Ymax) y=Ymax;
  j = 32+(127*(Ymax-y))/Yrange;
  if(j<32) j = 32;
  if(j>159) j = 159;
  ST7735_DrawPixel(X,   j,  BLUE);
  ST7735_DrawPixel(X+1, j,  BLUE);
  ST7735_DrawPixel(X,   j+1,BLUE);
  ST7735_DrawPixel(X+1, j+1,BLUE);
}

void ST7735_PlotPoint(int32_t data1, uint16_t color1){
  data1 = ((data1 - Ymin)*100)/Yrange;
  if(data1 > 98){
    data1 = 98;
    color1 = RED;
  }
  if(data1 < 0){
    data1 = 0;
    color1 = RED;
  }
  ST7735_DrawPixel(TimeIndex + 11, 116 - data1, color1);
  ST7735_DrawPixel(TimeIndex + 11, 115 - data1, color1);
}
void ST7735_PlotIncrement(void){
  TimeIndex = TimeIndex + 1;
  if(TimeIndex > 99){
    TimeIndex = 0;
  }
  ST7735_DrawFastVLine(TimeIndex + 11, 17, 100, PlotBGColor);
}

int32_t lastj=0;

void ST7735_PlotLine(int32_t y)
{
	int32_t i,j;
  if(y<Ymin) y=Ymin;
  if(y>Ymax) y=Ymax;

  j = 32+(127*(Ymax-y))/Yrange;
  if(j < 32) j = 32;
  if(j > 159) j = 159;
  if(lastj < 32) lastj = j;
  if(lastj > 159) lastj = j;
  if(lastj < j){
    for(i = lastj+1; i<=j ; i++){
      ST7735_DrawPixel(X,   i,   BLUE) ;
      ST7735_DrawPixel(X+1, i,   BLUE) ;
    }
  }else if(lastj > j){
    for(i = j; i<lastj ; i++){
      ST7735_DrawPixel(X,   i,   BLUE) ;
      ST7735_DrawPixel(X+1, i,   BLUE) ;
    }
  }else{
    ST7735_DrawPixel(X,   j,   BLUE) ;
    ST7735_DrawPixel(X+1, j,   BLUE) ;
  }
  lastj = j;
}

void ST7735_PlotBar(int32_t y){
int32_t j;
  if(y<Ymin) y=Ymin;
  if(y>Ymax) y=Ymax;

  j = 32+(127*(Ymax-y))/Yrange;
  ST7735_DrawFastVLine(X, j, 159-j,BLACK);

}

void ST7735_PlotNext(void){
  if(X==127){
    X = 0;
  } else{
    X++;
  }
}

void lcd7735_sendCmd(unsigned char cmd) {
   LCD_DC0;
   lcd7735_senddata(cmd);
}

void lcd7735_sendData(unsigned char data) {
   LCD_DC1;
   lcd7735_senddata(data);
}
void lcd7735_at(unsigned char startX, unsigned char startY, unsigned char stopX, unsigned char stopY) {
	lcd7735_sendCmd(0x2A);
	LCD_DC1;
	lcd7735_senddata(0x00);
	lcd7735_senddata(startX);
	lcd7735_senddata(0x00);
	lcd7735_senddata(stopX);

	lcd7735_sendCmd(0x2B);
	LCD_DC1;
	lcd7735_senddata(0x00);
	lcd7735_senddata(startY);
	lcd7735_senddata(0x00); 
	lcd7735_senddata(stopY);
}




void ST7735_Drawaxes(uint16_t axisColor, uint16_t bgColor, char *xLabel,char *yLabel1, uint16_t label1Color, char *yLabel2, uint16_t label2Color,int32_t ymax, int32_t ymin)
	{
  int i;

  Ymax = ymax;
  Ymin = ymin;
  Yrange = Ymax - Ymin;
  TimeIndex = 0;
  PlotBGColor = bgColor;
  ST7735_FillRect(0, 0, 128, 160, bgColor);
  ST7735_DrawFastHLine(10, 140, 101, axisColor);
  ST7735_DrawFastVLine(10, 17, 124, axisColor);
  for(i=20; i<=110; i=i+10){
    ST7735_DrawPixel(i, 141, axisColor);
  }
  for(i=17; i<120; i=i+10){
    ST7735_DrawPixel(9, i, axisColor);
  }
  i = 50;
  while((*xLabel) && (i < 100)){
    ST7735_DrawChar(i, 145, *xLabel, axisColor, bgColor, 1);
    i = i + 6;
    xLabel++;
  }
  if(*yLabel2){
    i = 26;
    while((*yLabel2) && (i < 50)){
      ST7735_DrawChar(0, i, *yLabel2, label2Color, bgColor, 1);
      i = i + 8;
      yLabel2++;
    }
    i = 82;
  }else{
    i = 42;
  }
  while((*yLabel1) && (i < 120)){
   ST7735_DrawChar(0, i, *yLabel1, label1Color, bgColor, 1);
    i = i + 8;
    yLabel1++;
  }
}


void  SPI_WriteData(uint8_t Data)
{


	  LCD_DC1;
	   lcd7735_senddata(Data);

}

void Lcd_WriteIndex(uint8_t Index)
{
	  LCD_DC0;
	   lcd7735_senddata(Index);

}

void Lcd_WriteData(uint8_t Data)
{
	  LCD_DC1;
	   lcd7735_senddata(Data);
}
void LCD_WriteData_16Bit(uint16_t Data)
{
	  LCD_DC1;//Set DC HIGH
	   lcd7735_senddata(Data>>8);
	   lcd7735_senddata(Data);
;
}


void Lcd_WriteReg(uint8_t Index,uint8_t Data)
{
	Lcd_WriteIndex(Index);
  Lcd_WriteData(Data);
}

void Lcd_Reset(void)
{
//	LCD_RST_CLR;
	LCD_RST0;
	HAL_Delay(100);
//	LCD_RST_SET;
	LCD_RST1;
	HAL_Delay(50);
}

void Lcd_Init(void)
{




	Lcd_Reset();
	Lcd_WriteIndex(0x11);
	HAL_Delay (120);


	Lcd_WriteIndex(0xB1);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x2C);
	Lcd_WriteData(0x2D);

	Lcd_WriteIndex(0xB2);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x2C);
	Lcd_WriteData(0x2D);

	Lcd_WriteIndex(0xB3);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x2C);
	Lcd_WriteData(0x2D);
	Lcd_WriteData(0x01);
	Lcd_WriteData(0x2C);
	Lcd_WriteData(0x2D);

	Lcd_WriteIndex(0xB4); //Column inversion
	Lcd_WriteData(0x07);

	//ST7735R Power Sequence
	Lcd_WriteIndex(0xC0);
	Lcd_WriteData(0xA2);
	Lcd_WriteData(0x02);
	Lcd_WriteData(0x84);
	Lcd_WriteIndex(0xC1);
	Lcd_WriteData(0xC5);

	Lcd_WriteIndex(0xC2);
	Lcd_WriteData(0x0A);
	Lcd_WriteData(0x00);

	Lcd_WriteIndex(0xC3);
	Lcd_WriteData(0x8A);
	Lcd_WriteData(0x2A);
	Lcd_WriteIndex(0xC4);
	Lcd_WriteData(0x8A);
	Lcd_WriteData(0xEE);

	Lcd_WriteIndex(0xC5); //VCOM
	Lcd_WriteData(0x0E);

	Lcd_WriteIndex(0x36); //MX, MY, RGB mode
	Lcd_WriteData(0xC8);

	//ST7735R Gamma Sequence
	Lcd_WriteIndex(0xe0);
	Lcd_WriteData(0x0f);
	Lcd_WriteData(0x1a);
	Lcd_WriteData(0x0f);
	Lcd_WriteData(0x18);
	Lcd_WriteData(0x2f);
	Lcd_WriteData(0x28);
	Lcd_WriteData(0x20);
	Lcd_WriteData(0x22);
	Lcd_WriteData(0x1f);
	Lcd_WriteData(0x1b);
	Lcd_WriteData(0x23);
	Lcd_WriteData(0x37);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x07);
	Lcd_WriteData(0x02);
	Lcd_WriteData(0x10);

	Lcd_WriteIndex(0xe1);
	Lcd_WriteData(0x0f);
	Lcd_WriteData(0x1b);
	Lcd_WriteData(0x0f);
	Lcd_WriteData(0x17);
	Lcd_WriteData(0x33);
	Lcd_WriteData(0x2c);
	Lcd_WriteData(0x29);
	Lcd_WriteData(0x2e);
	Lcd_WriteData(0x30);
	Lcd_WriteData(0x30);
	Lcd_WriteData(0x39);
	Lcd_WriteData(0x3f);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x07);
	Lcd_WriteData(0x03);
	Lcd_WriteData(0x10);

	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x7f);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x9f);

	Lcd_WriteIndex(0xF0); //Enable test command
	Lcd_WriteData(0x01);
	Lcd_WriteIndex(0xF6); //Disable ram power save mode
	Lcd_WriteData(0x00);

	Lcd_WriteIndex(0x3A); //65k mode
	Lcd_WriteData(0x05);


	Lcd_WriteIndex(0x29);//Display on
}




void Lcd_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end)
{
	Lcd_WriteIndex(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_start+2);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_end+2);

	Lcd_WriteIndex(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_start+3);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_end+3);

	Lcd_WriteIndex(0x2c);

}

void Lcd_SetXY(uint16_t x,uint16_t y)
{
  	Lcd_SetRegion(x,y,x,y);
}


void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data)
{
	uint16_t tmp;
	tmp = x;
	x = 128 - y;
	y = tmp;

	Lcd_SetRegion(x,y,x+1,y+1);
	LCD_WriteData_16Bit(Data);

}

unsigned int Lcd_ReadPoint(uint16_t x,uint16_t y)
{
  unsigned int Data;
  Lcd_SetXY(x,y);

  Lcd_WriteData(Data);
  return Data;
}

void Lcd_Clear(uint16_t Color)
{
   unsigned int i,m;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
   Lcd_WriteIndex(0x2C);
   for(i=0;i<X_MAX_PIXEL;i++)
   for(m=0;m<Y_MAX_PIXEL;m++)
   {
	  	LCD_WriteData_16Bit(Color);
   }
}
void Lcd_Block(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t color)
{
   unsigned int i,m;
   Lcd_SetRegion(128 - y1,x0,128 - y0,x1);
   Lcd_WriteIndex(0x2C);
   for(i=0;i<(x1 - x0 + 1);i++)
   for(m=0;m<(y1 - y0 + 1);m++)
   {
	  	LCD_WriteData_16Bit(color);
   }

   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
   Lcd_WriteIndex(0x2C);
}
