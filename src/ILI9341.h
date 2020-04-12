/*
 *
 *
 * @file ILI9341.h
 *
 *
 */

#ifndef __ILI9341_H
#define __ILI9341_H
#endif

#include "stm32f30x.h"
#include "stm32f30x_spi.h"
#include "Fonts.h"


#define DATA            GPIO_BSRR_BS_3;
#define COMMAND         GPIO_BSRR_BR_3;
#define CSEL            GPIO_BSRR_BR_4;
#define _CSEL           GPIO_BSRR_BS_4;
#define RST             GPIO_BSRR_BR_2;
#define _RST            GPIO_BSRR_BS_2;

/*
#define DebugLedON   GPIOE->BSRR |= GPIO_BSRR_BS_15
#define DebugLedOFF  GPIOE->BSRR |= GPIO_BSRR_BR_15
*/
//-------------------COMMANDS---------------------------------------------

#define ILI9341_TFTWIDTH  320  ///< ILI9341 max TFT width
#define ILI9341_TFTHEIGHT 240 ///< ILI9341 max TFT height

#define ILI9341_NOP 0x00     ///< No-op register
#define ILI9341_SWRESET 0x01 ///< Software reset register
#define ILI9341_RDDID 0x04   ///< Read display identification information
#define ILI9341_RDDST 0x09   ///< Read Display Status

#define ILI9341_SLPIN 0x10  ///< Enter Sleep Mode
#define ILI9341_SLPOUT 0x11 ///< Sleep Out
#define ILI9341_PTLON 0x12  ///< Partial Mode ON
#define ILI9341_NORON 0x13  ///< Normal Display Mode ON

#define ILI9341_RDMODE 0x0A     ///< Read Display Power Mode
#define ILI9341_RDMADCTL 0x0B   ///< Read Display MADCTL
#define ILI9341_RDPIXFMT 0x0C   ///< Read Display Pixel Format
#define ILI9341_RDIMGFMT 0x0D   ///< Read Display Image Format
#define ILI9341_RDSELFDIAG 0x0F ///< Read Display Self-Diagnostic Result

#define ILI9341_INVOFF 0x20   ///< Display Inversion OFF
#define ILI9341_INVON 0x21    ///< Display Inversion ON
#define ILI9341_GAMMASET 0x26 ///< Gamma Set
#define ILI9341_DISPOFF 0x28  ///< Display OFF
#define ILI9341_DISPON 0x29   ///< Display ON

#define ILI9341_CASET 0x2A ///< Column Address Set
#define ILI9341_PASET 0x2B ///< Page Address Set
#define ILI9341_RAMWR 0x2C ///< Memory Write
#define ILI9341_RAMRD 0x2E ///< Memory Read

#define ILI9341_PTLAR 0x30    ///< Partial Area
#define ILI9341_VSCRDEF 0x33  ///< Vertical Scrolling Definition
#define ILI9341_MADCTL 0x36   ///< Memory Access Control
        #define MADCTL_MY               0x80
        #define MADCTL_MX               0x40
        #define MADCTL_MV               0x20
        #define MADCTL_BGR              0x08
        #define MADCTL_RGB              0x00
        #define MADCTL_PARAMETR         MADCTL_MX | MADCTL_MY| MADCTL_MV | MADCTL_BGR
#define ILI9341_VSCRSADD 0x37 ///< Vertical Scrolling Start Address
#define ILI9341_PIXFMT 0x3A   ///< COLMOD: Pixel Format Set

#define ILI9341_FRMCTR1                                                        \
  0xB1 ///< Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9341_FRMCTR2 0xB2 ///< Frame Rate Control (In Idle Mode/8 colors)
#define ILI9341_FRMCTR3 0xB3 ///< Frame Rate control (In Partial Mode/Full Colors)
#define ILI9341_INVCTR 0xB4  ///< Display Inversion Control
#define ILI9341_DFUNCTR 0xB6 ///< Display Function Control

#define ILI9341_PWCTR1 0xC0 ///< Power Control 1
#define ILI9341_PWCTR2 0xC1 ///< Power Control 2
#define ILI9341_PWCTR3 0xC2 ///< Power Control 3
#define ILI9341_PWCTR4 0xC3 ///< Power Control 4
#define ILI9341_PWCTR5 0xC4 ///< Power Control 5
#define ILI9341_VMCTR1 0xC5 ///< VCOM Control 1
#define ILI9341_VMCTR2 0xC7 ///< VCOM Control 2

#define ILI9341_RDID1 0xDA ///< Read ID 1
#define ILI9341_RDID2 0xDB ///< Read ID 2
#define ILI9341_RDID3 0xDC ///< Read ID 3
#define ILI9341_RDID4 0xDD ///< Read ID 4

#define ILI9341_GMCTRP1 0xE0 ///< Positive Gamma Correction
#define ILI9341_GMCTRN1 0xE1 ///< Negative Gamma Correction

#define ClMODE16BIT     0x55
#define ClMODE18BIT     0x66


//---------------COLORS-----------------------------------
#define BLACK                       0x0000
#define BLUE                        0x001F
#define RED                         0xF800
#define GREEN                       0x07E0
#define CYAN                        0x07FF
#define MAGENTA                     0xF81F
#define YELLOW                      0xFFE0
#define WHITE                       0xFFFF

namespace ILI9341
{
 typedef union
 {
  uint8_t _byte;
  struct
   {
 	 uint8_t NotUsed1:1;
 	 uint8_t NotUsed2:1;
 	 uint8_t MH:1;   //LCD horizontal refreshing direction control
 	 uint8_t BGR:1;  //Color selector switch control (0=RGB color filter panel, 1=BGR color filter panel)
 	 uint8_t ML:1;   //LCD vertical refresh direction control
 	 uint8_t MV:1;   //These bit controls MCU to memory write/read direction
 	 uint8_t MX:1;   //These bit controls MCU to memory write/read direction
 	 uint8_t MY:1;   //These bit controls MCU to memory write/read direction

   };
 } MADCTL_Param;

  class TFTdisplay
  {
    public:
	 const uint32_t TFTWidth;
	 const uint32_t TFTHeight;
     void SleepOut();
     void DisplayON();
     void DisplayInvertionOff();
     void DisplayInvertionOn();
     void MemoryAccessCtl();
     void WriteData(uint8_t* data, uint16_t datasize);
     void WritePixelArray(uint16_t* data, uint16_t datasize);
     void SoftReset();
     void ColorMode16();
     void ColorMode18();
     uint16_t SetCanvasSize(uint16_t x, uint16_t y, uint16_t dx, uint16_t dy);
     uint16_t SetCanvasStart(uint16_t x, uint16_t y);
     void FullScreen();
     void DrawPixel(uint16_t x,uint16_t y, uint16_t color);
     void WritePixel(uint16_t color);
     void Flood(uint16_t Color);
     void JPG24Frame(const uint8_t* VideoBuffer, uint32_t size);
     void Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
     void Circle(int x0, int y0, int radius, uint16_t color);
     void Rectangle(uint16_t Start_x, uint16_t Start_y, uint16_t Width, uint16_t Heidth, uint16_t color);
     void WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);
     void WriteString(uint16_t x, uint16_t y, /*const*/ char* str, FontDef font, uint16_t color, uint16_t bgcolor);
     static TFTdisplay& getInstance(GPIO_TypeDef* iop);
     void initDisplay();
     void deinitDisplay();

    private:
     GPIO_TypeDef* ioPort;
     TFTdisplay(GPIO_TypeDef* iop);
     TFTdisplay(const TFTdisplay&) = delete;
     TFTdisplay& operator=(TFTdisplay&) = delete;
     void WriteByte(uint8_t data);
     void WriteCommand(uint8_t cmdcode);

    protected:
  };
}

