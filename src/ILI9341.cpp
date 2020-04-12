/*
 * ILI9341.c
 *
 *  Created on: 12 џэт. 2020 у.
 *      Author: Garick
 */

#include "ILI9341.h"
#include "stdlib.h"

using namespace ILI9341;

/* --------------------------------------------------------  */
 TFTdisplay::TFTdisplay(GPIO_TypeDef* iop):TFTWidth(ILI9341_TFTWIDTH),TFTHeight(ILI9341_TFTHEIGHT)
{
  this->ioPort = iop;
  this->initDisplay();
}

 /*-----------------------------------------------------------------------------*/
 TFTdisplay& TFTdisplay::getInstance(GPIO_TypeDef* iop)
 {
     static TFTdisplay  instance(iop);
     return instance;
 }
 /* --------------------------------------------------------  */
 void TFTdisplay::initDisplay()
 {
	    //Hardware reset
	 	uint32_t delaytime;
	 	this->ioPort->BSRR |= CSEL;
	 	for(delaytime=0;delaytime<200000;delaytime++)
	    {;}
	 	this->ioPort->BSRR |= RST;
	 	for(delaytime=0;delaytime<200000;delaytime++)
	 	{;}
	 	this->ioPort->BSRR |= _RST;
	 	for(delaytime=0;delaytime<200000;delaytime++)
	 	{;}
        this->SoftReset();
        for(delaytime=0;delaytime<200000;delaytime++)
        {;}
        this->ColorMode16();
        for(delaytime=0;delaytime<200000;delaytime++)
        {;}
        this->MemoryAccessCtl();
        for(delaytime=0;delaytime<200000;delaytime++)
        {;}
        this->SleepOut();
        for(delaytime=0;delaytime<200000;delaytime++)
        {;}
        this->DisplayON();
        for(delaytime=0;delaytime<200000;delaytime++)
        {;}
 }

 /* --------------------------------------------------------  */
 void TFTdisplay::deinitDisplay()
 {
   this->ioPort->BSRR |= _CSEL;
 }

 /* --------------------------------------------------------  */
 void TFTdisplay::WriteByte(uint8_t data)
 {
	 //Sending data
  this->ioPort->BSRR |= DATA;
  while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_TXE) == RESET){;};
  SPI_SendData8(SPI1,data);
 }

 /* --------------------------------------------------------  */
 void TFTdisplay::WriteCommand(uint8_t cmdcode)
 {
 	//Sending command
	this->ioPort->BSRR |= COMMAND;
 	while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_TXE) == RESET){;};
 	SPI_SendData8(SPI1,cmdcode);
 }

 /* --------------------------------------------------------  */
 void TFTdisplay::SleepOut()
 {
 	WriteCommand(ILI9341_SLPOUT);
 }

 /* --------------------------------------------------------  */
 void TFTdisplay::DisplayON()
 {
 	WriteCommand(ILI9341_DISPON);
 }

 /* --------------------------------------------------------  */
 void TFTdisplay::DisplayInvertionOff()
 {
 	WriteCommand(ILI9341_INVOFF);
 }

 /* --------------------------------------------------------  */
 void TFTdisplay::DisplayInvertionOn()
 {
 	WriteCommand(ILI9341_INVON);
 }

 /* --------------------------------------------------------  */
 void TFTdisplay::MemoryAccessCtl()
 {
 	MADCTL_Param madctlparam;
 	madctlparam.NotUsed1 = 0;
 	madctlparam.NotUsed2 = 0;
 	madctlparam.BGR = 1;
 	madctlparam.MX = 1;
 	madctlparam.MY = 1;
 	madctlparam.MV = 1;
 	madctlparam.MH = 1;
 	madctlparam.ML = 1;

 	WriteCommand(ILI9341_MADCTL);
 	while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
 	WriteByte(madctlparam._byte);
 }
 /* --------------------------------------------------------  */
 void TFTdisplay::WriteData(uint8_t* data, uint16_t datasize)
 {
 	uint32_t ByteCounter = 0;
 	WriteCommand(ILI9341_RAMWR);
 	while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
     for(ByteCounter=0;ByteCounter<datasize;ByteCounter++)
        {
     	WriteByte(data[ByteCounter] & 0xFF);
        }
     while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
 }


 //--------------------------------------------------------------------
 void TFTdisplay::WritePixelArray(uint16_t* data, uint16_t datasize)
 {
 	uint32_t ByteCounter = 0;
 	WriteCommand(ILI9341_RAMWR);
 	while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
     for(ByteCounter=0;ByteCounter<datasize;ByteCounter++)
        {
     	WriteByte((data[ByteCounter]>>8) & 0xFF);
     	WriteByte(data[ByteCounter] & 0xFF);
        }
     while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
 }

 //--------------------------------------------------------------------
 void TFTdisplay::SoftReset()
 {
 	WriteCommand(ILI9341_SWRESET);
 }

 //--------------------------------------------------------------------
 void TFTdisplay::ColorMode16(void)
 {
 	WriteCommand(ILI9341_PIXFMT);
 	while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
 	WriteByte(ClMODE16BIT);
 }

 //--------------------------------------------------------------------
 void TFTdisplay::ColorMode18(void)
 {
 	WriteCommand(ILI9341_PIXFMT);
 	while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
 	WriteByte(ClMODE18BIT);
 }

 //--------------------------------------------------------------------
 uint16_t TFTdisplay::SetCanvasSize(uint16_t x, uint16_t y, uint16_t dx, uint16_t dy)
 {
 	if(((x+dx)>(TFTWidth-1))||((y+dy)>(TFTHeight-1))) return 0;
 	else
 	{
 		WriteCommand(ILI9341_CASET);
 		while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};

 		WriteByte(x>>8 & 0xFF);WriteByte(x & 0xFF);// start pixel column
 		WriteByte(((x+dx)>>8) & 0xFF);WriteByte((x+dx) & 0xFF);// end pixel column

 		while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
 		WriteCommand(ILI9341_PASET);
 		while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};

 		WriteByte(y>>8 & 0xFF);WriteByte(y & 0xFF);// start pixel page
 		WriteByte(((y+dy)>>8) & 0xFF);WriteByte((y+dy) & 0xFF);// end pixel page

 		while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
 		return 1;
 	}
 }

 //--------------------------------------------------------------------
 uint16_t TFTdisplay::SetCanvasStart(uint16_t x, uint16_t y)
 {
 	if((x>(TFTWidth-1))||(y>(TFTHeight-1))) return 0;
 	else
 	{
 		WriteCommand(ILI9341_CASET);
 		while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};

 		WriteByte(x>>8 & 0xFF);WriteByte(x & 0xFF);// start pixel column
 		WriteByte(((TFTWidth-1)>>8) & 0xFF);WriteByte((TFTWidth-1) & 0xFF);// end pixel column

 		while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
 		WriteCommand(ILI9341_PASET);
 		while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};

 		WriteByte(y>>8 & 0xFF);WriteByte(y & 0xFF);// start pixel page
 		WriteByte(((TFTHeight-1)>>8) & 0xFF);WriteByte((TFTHeight-1) & 0xFF);// end pixel page

 		while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
 		return 1;
 	}
 }

 //--------------------------------------------------------------------
 void TFTdisplay::FullScreen()
 {
 	SetCanvasSize(0,0,TFTWidth-1,TFTHeight-1);
 }

 //--------------------------------------------------------------------
 void TFTdisplay::DrawPixel(uint16_t x,uint16_t y, uint16_t color)
 {
 	SetCanvasSize(x,y,1,1);
 	while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
 	WritePixel(color);
 }

 //--------------------------------------------------------------------

 void TFTdisplay::WritePixel(uint16_t color)
 {
     //
 	WriteCommand(ILI9341_RAMWR);
 	while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
     WriteByte((color>>8) & 0xFF);
 	WriteByte(color & 0xFF);
     while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
 }

 //--------------------------------------------------------------------
 void TFTdisplay::Flood(uint16_t Color)
 {
 	uint32_t ByteCounter = 0;
 	WriteCommand(ILI9341_RAMWR);
 	while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
     for(ByteCounter=0;ByteCounter<(TFTWidth*TFTHeight);ByteCounter++)
        {
     	  WriteByte((Color>>8) & 0xFF);WriteByte(Color & 0xFF);
        }
     while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
 }

 //--------------------------------------------------------------------
 void TFTdisplay::JPG24Frame(const uint8_t* VideoBuffer, uint32_t size)
 {
	 uint32_t ByteCounter = 0;
	 uint16_t OutByte;
	 WriteCommand(ILI9341_RAMWR);
	 while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
	      for(ByteCounter=0;ByteCounter<size;ByteCounter = ByteCounter + 3)
	         {
	    	   OutByte = ((VideoBuffer[ByteCounter] & 0xF8) << 8) | ((VideoBuffer[ByteCounter+1]  & 0xFC) << 3) | (VideoBuffer[ByteCounter+2]  >> 3);
	      	   WriteByte((OutByte>>8) & 0xFF);
	      	   WriteByte(OutByte & 0xFF);
	         }
	      while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
 }
 //-------------------Graphic primitives-------------------------------
 //--------------------------------------------------------------------
 void TFTdisplay::Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
 {
 	    const int deltaX = abs(x2 - x1);
 	    const int deltaY = abs(y2 - y1);
 	    const int signX = x1 < x2 ? 1 : -1;
 	    const int signY = y1 < y2 ? 1 : -1;
 	    //
 	    int error = deltaX - deltaY;
 	    //
 	    DrawPixel(x2, y2, color);
 	    while(x1 != x2 || y1 != y2)
 	     {
 	    	DrawPixel(x1, y1, color);
 	        const int error2 = error * 2;
 	        //
 	        if(error2 > -deltaY)
 	        {
 	            error -= deltaY;
 	            x1 += signX;
 	        }
 	        if(error2 < deltaX)
 	        {
 	            error += deltaX;
 	            y1 += signY;
 	        }
           }
 }

 //--------------------------------------------------------------------
 void TFTdisplay::Circle(int x0, int y0, int radius, uint16_t color)
 {
 	    int x = 0;
 		int y = radius;
 		int delta = 1 - 2 * radius;
 		int error = 0;
 		while(y >= 0) {
 			DrawPixel(x0 + x, y0 + y, color);
 			DrawPixel(x0 + x, y0 - y, color);
 			DrawPixel(x0 - x, y0 + y, color);
 			DrawPixel(x0 - x, y0 - y, color);
 			error = 2 * (delta + y) - 1;
 			if(delta < 0 && error <= 0) {
 				++x;
 				delta += 2 * x + 1;
 				continue;
 			}
 			error = 2 * (delta - x) - 1;
 			if(delta > 0 && error > 0) {
 				--y;
 				delta += 1 - 2 * y;
 				continue;
 			}
 			++x;
 			delta += 2 * (x - y);
 			--y;
 		}
 }

 //--------------------------------------------------------------------
 void TFTdisplay::Rectangle(uint16_t Start_x, uint16_t Start_y, uint16_t Width, uint16_t Heidth, uint16_t color)
 {
   uint16_t ColCounter,RowCounter;
   for(ColCounter=0;ColCounter<Width;ColCounter++)
       {
 	    DrawPixel(Start_x+ColCounter,Start_y,color);
 	    DrawPixel(Start_x+ColCounter,Start_y+Heidth-1,color);
       }
   for(RowCounter=0;RowCounter<Heidth;RowCounter++)
       {
   	    DrawPixel(Start_x,Start_y+RowCounter,color);
   	    DrawPixel(Start_x+Width-1,Start_y+RowCounter,color);
       }
 }

 //---------------------------------------------------------------------
 void TFTdisplay::WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
 {
     uint32_t i, b, j;

     SetCanvasSize(x, y, font.width-1, font.height-1);
     WriteCommand(ILI9341_RAMWR);
     while(SPI_I2S_GetFlagStatus(SPI1,SPI_SR_BSY)!=RESET){;};
     for(i = 0; i < font.height; i++)
     {
         b = font.data[(ch - 32) * font.height + i];
         for(j = 0; j < font.width; j++)
         {
             if((b << j) & 0x8000)
             	{
             	    WriteByte((color>>8) & 0xFF);
             		WriteByte(color & 0xFF);
             	}
             else
             	{
             	    WriteByte((bgcolor>>8) & 0xFF);
             		WriteByte(bgcolor & 0xFF);
             	}
         }
     }

 }

 void TFTdisplay::WriteString(uint16_t x, uint16_t y, /*const*/ char* str, FontDef font, uint16_t color, uint16_t bgcolor)
 {
     while(*str) {
         if(x + font.width >= TFTWidth) {
             x = 0;
             y += font.height;
             if(y + font.height >= TFTHeight) {
                 break;
             }

             if(*str == ' ') {
                 // skip spaces in the beginning of the new line
                 str++;
                 continue;
             }
         }

         WriteChar(x, y, *str, font, color, bgcolor);
         x += font.width;
         str++;
     }
 }

