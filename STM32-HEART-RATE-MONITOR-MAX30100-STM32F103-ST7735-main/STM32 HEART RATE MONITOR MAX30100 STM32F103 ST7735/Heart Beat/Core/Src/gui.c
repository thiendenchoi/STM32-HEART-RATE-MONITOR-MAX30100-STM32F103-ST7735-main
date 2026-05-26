
#include "stm32f1xx_hal.h"
#include "ST7735.h"
#include "GUI.h"
#include "font.h"
ֵ
uint16_t LCD_BGR2RGB(uint16_t c)
{
  uint16_t  r,g,b,rgb;   
  b=(c>>0)&0x1f;
  g=(c>>5)&0x3f;
  r=(c>>11)&0x1f;	 
  rgb=(b<<11)+(g<<5)+(r<<0);		 
  return(rgb);

}

void Gui_Circle(uint16_t X,uint16_t Y,uint16_t R,uint16_t fc) 
{
    unsigned short  a,b; 
    int c; 
    a=0; 
    b=R; 
    c=3-2*R; 
    while (a<b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc);     //        7 
        Gui_DrawPoint(X-a,Y+b,fc);     //        6 
        Gui_DrawPoint(X+a,Y-b,fc);     //        2 
        Gui_DrawPoint(X-a,Y-b,fc);     //        3 
        Gui_DrawPoint(X+b,Y+a,fc);     //        8 
        Gui_DrawPoint(X-b,Y+a,fc);     //        5 
        Gui_DrawPoint(X+b,Y-a,fc);     //        1 
        Gui_DrawPoint(X-b,Y-a,fc);     //        4 

        if(c<0) c=c+4*a+6; 
        else 
        { 
            c=c+4*(a-b)+10; 
            b-=1; 
        } 
       a+=1; 
    } 
    if (a==b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y-b,fc); 
        Gui_DrawPoint(X-a,Y-b,fc); 
        Gui_DrawPoint(X+b,Y+a,fc); 
        Gui_DrawPoint(X-b,Y+a,fc); 
        Gui_DrawPoint(X+b,Y-a,fc); 
        Gui_DrawPoint(X-b,Y-a,fc); 
    } 
	
} 
void Gui_DrawLine(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1,uint16_t Color)   
{
int dx,
    dy,
    dx2,
    dy2, 
    x_inc,
    y_inc,
    error,
    index;


	Lcd_SetXY(x0,y0);
	dx = x1-x0;
	dy = y1-y0;

	if (dx>=0)
	{
		x_inc = 1;
	}
	else
	{
		x_inc = -1;
		dx    = -dx;  
	} 
	
	if (dy>=0)
	{
		y_inc = 1;
	} 
	else
	{
		y_inc = -1;
		dy    = -dy; 
	} 

	dx2 = dx << 1;
	dy2 = dy << 1;

	if (dx > dy)
	{
		error = dy2 - dx; 

		for (index=0; index <= dx; index++)
		{

			Gui_DrawPoint(x0,y0,Color);
			
			if (error >= 0) ֵ
			{
				error-=dx2;

				y0+=y_inc;ֵ
			}

			error+=dy2;

			x0+=x_inc;
		}
	}
	else
	{
		error = dx2 - dy; 

		for (index=0; index <= dy; index++)
		{
			Gui_DrawPoint(x0,y0,Color);

			if (error >= 0)
			{
				error-=dy2;

				x0+=x_inc;
			}

			error+=dx2;

			y0+=y_inc;
		}
	}
}

void gui_draw_Block(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t lineColor,uint16_t blockColor)
{
	Gui_DrawLine(x0,y0,x0,y1,lineColor); 
	Gui_DrawLine(x0,y0,x1,y0,lineColor); 
	Gui_DrawLine(x1,y1,x0,y1,lineColor); 
	Gui_DrawLine(x1,y1,x1,y0,lineColor); 
	
	Lcd_Block(x0 + 1,y0 + 1,x1 - 1,y1 - 1,blockColor);
}
void gui_draw_square(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t lineColor)
{
	Gui_DrawLine(x0,y0,x0,y1,lineColor); 
	Gui_DrawLine(x0,y0,x1,y0,lineColor); 
	Gui_DrawLine(x1,y1,x0,y1,lineColor); 
	Gui_DrawLine(x1,y1,x1,y0,lineColor); 
}

void Gui_DrawFont_GBK16(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s)
{
	unsigned char i,j;
	unsigned short k,x0;
	x0=x;

	while(*s) 
	{	
		if((*s) < 128)
		{
			k=*s;
			if (k==13) 
			{
				x=x0;
				y+=16;
			}
			else 
			{
				if (k>32) k-=32; else k=0;
	
			    for(i=0;i<16;i++)
				for(j=0;j<8;j++) 
					{
				    	if(asc16[k*16+i]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
						else 
						{
							if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
						}
					}
				x+=8;
			}
			s++;
		}
			
		else
		{
			/*
			for (k=0;k<hz16_num;k++) 
			{
			  if ((hz16[k].Index[0]==*(s))&&(hz16[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<16;i++)
				    {
						for(j=0;j<8;j++) 
							{
						    	if(hz16[k].Msk[i*2]&(0x80>>j))	Gui_DrawPoint(x+j,y+i,fc);
								else {
									if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz16[k].Msk[i*2+1]&(0x80>>j))	Gui_DrawPoint(x+j+8,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
								}
							}
				    }
				}
			  }
			s+=2;x+=16;
			*/
		} 
	}
}

void Gui_DrawFont_GBK24(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s)
{
	unsigned char i,j;
	unsigned short k;

	while(*s) 
	{
		if( *s < 0x80 )
		{
			k=*s;
			if (k>32) k-=32; else k=0;

		  for(i=0;i<16;i++)
				for(j=0;j<8;j++) 
				{
			    if(asc16[k*16+i]&(0x80>>j))	
					{
						Gui_DrawPoint(x+j,y+i,fc);
					}
					else 
					{
						if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
					}
				}
			s++;x+=8;
		}
		else //GBK24
		{
			/*
			for (k=0;k<hz24_num;k++) 
			{
			  if ((hz24[k].Index[0]==*(s))&&(hz24[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<24;i++)
				    {
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3]&(0x80>>j))
								Gui_DrawPoint(x+j,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3+1]&(0x80>>j))	Gui_DrawPoint(x+j+8,y+i,fc);
								else {
									if (fc!=bc) Gui_DrawPoint(x+j+8,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3+2]&(0x80>>j))	
								Gui_DrawPoint(x+j+16,y+i,fc);
								else 
								{
									if (fc!=bc) Gui_DrawPoint(x+j+16,y+i,bc);
								}
							}
				    }
			  }
			}
			s+=2;x+=24;
			*/
		}
	}
}

void Gui_DrawFont_Num32(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint16_t num)
{
	unsigned char i,j,k,c;
  for(i=0;i<32;i++)
	{
		for(j=0;j<4;j++) 
		{
			c=*(sz32+num*32*4+i*4+j);
			for (k=0;k<8;k++)	
			{
		    	if(c&(0x80>>k))
					{
						Gui_DrawPoint(x+j*8+k,y+i,fc);
					}
					else 
					{
						if (fc!=bc)
						{
							Gui_DrawPoint(x+j*8+k,y+i,bc);
						}
					}
			}
		}
	}
}


