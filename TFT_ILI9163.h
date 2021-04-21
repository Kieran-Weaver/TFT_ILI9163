/***************************************************
  Arduino TFT graphics library targetted at the UNO
  and Mega boards.

  This library has been derived from the Adafruit_GFX
  library and the associated driver library. See text
  at the end of this file.

  This is a standalone library that contains the
  hardware driver, the graphics funtions and the
  proportional fonts.

  The larger fonts are Run Length Encoded to reduce
  their FLASH footprint.

 ****************************************************/

// Include header file that defines the fonts loaded and the pins to be used
#include <User_Setup.h>

// Include header file from the FastLED library for fast pin toggling using direct port access
#include <TFT_FastPin.h>

// Stop fonts being loaded multiple times
#ifndef _TFT_ILI9163H_
#define _TFT_ILI9163H_

// Only load the fonts defined in User_Setup.h (to save space)
// Set flag so RLE rendering code is optionally compiled
#ifdef LOAD_GLCD
  #include <Fonts/glcdfont.c>
#endif

#ifdef LOAD_FONT2
  #include <Fonts/Font16.h>
#endif

#ifdef LOAD_FONT4
  #include <Fonts/Font32rle.h>
  #define LOAD_RLE
#endif

#ifdef LOAD_FONT6
  #include <Fonts/Font64rle.h>
  #ifndef LOAD_RLE
    #define LOAD_RLE
  #endif
#endif

#ifdef LOAD_FONT7
  #include <Fonts/Font7srle.h>
  #ifndef LOAD_RLE
    #define LOAD_RLE
  #endif
#endif

#ifdef LOAD_FONT8
  #include <Fonts/Font72rle.h>
  #ifndef LOAD_RLE
    #define LOAD_RLE
  #endif
#endif

#include <Arduino.h>
#include <Print.h>

#include <avr/pgmspace.h>

// Swap any type
template <typename T> static inline void
tftswap(T& a, T& b) { T t = a; a = b; b = t; }

//These define the ports and port bits used for the chip select (CS) and data/command (DC) lines
#define TFT_CS_L	FastPin<TFT_CS>::lo()
#define TFT_DC_C 	FastPin<TFT_DC>::lo()
#define TFT_CS_H	FastPin<TFT_CS>::hi()
#define TFT_DC_D 	FastPin<TFT_DC>::hi()

//These enumerate the text plotting alignment (reference datum point)
#define TL_DATUM 0 // Top left (default)
#define TC_DATUM 1 // Top centre
#define TR_DATUM 2 // Top right
#define ML_DATUM 3 // Middle left
#define CL_DATUM 3 // Centre left, same as above
#define MC_DATUM 4 // Middle centre
#define CC_DATUM 4 // Centre centre, same as above
#define MR_DATUM 5 // Middle right
#define CR_DATUM 5 // Centre right, same as above
#define BL_DATUM 6 // Bottom left
#define BC_DATUM 7 // Bottom centre
#define BR_DATUM 8 // Bottom right


// Change the width and height if required (defined in portrait mode)
// or use the constructor to over-ride defaults

#define ILI9163_INIT_DELAY 0x80

// These are the ILI9163 control registers

#define ILI9163_TFTWIDTH  128
#define ILI9163_TFTHEIGHT 160

#if ILI9163_TFTWIDTH > 255
typedef uint16_t UINTX;
#define SIZEOF_UINTX 2
#else
typedef uint8_t UINTX;
#define SIZEOF_UINTX 1
#endif

#if ILI9163_TFTHEIGHT > 255
typedef uint16_t UINTY;
#define SIZEOF_UINTY 2
#else
typedef uint8_t UINTY;
#define SIZEOF_UINTX 1
#endif

// These are the ILI9163 control registers

#define ILI9163_INVOFF  0x20
#define ILI9163_INVON   0x21
#define ILI9163_CASET   0x2A
#define ILI9163_RASET   0x2B
#define ILI9163_RAMWR   0x2C
#define ILI9163_RAMRD   0x2E

#define ILI9163_PTLAR   0x30
#define ILI9163_VSCDEF  0x33

#define ILI9163_MADCTL  0x36

#define ILI9163_MADCTL_MY  0x80
#define ILI9163_MADCTL_MX  0x40
#define ILI9163_MADCTL_MV  0x20
#define ILI9163_MADCTL_ML  0x10
#define ILI9163_MADCTL_RGB 0x00
#define ILI9163_MADCTL_BGR 0x08
#define ILI9163_MADCTL_MH  0x04


// New color definitions use for all my libraries
#define TFT_BLACK       0x0000      /*   0,   0,   0 */
#define TFT_NAVY        0x000F      /*   0,   0, 128 */
#define TFT_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define TFT_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define TFT_MAROON      0x7800      /* 128,   0,   0 */
#define TFT_PURPLE      0x780F      /* 128,   0, 128 */
#define TFT_OLIVE       0x7BE0      /* 128, 128,   0 */
#define TFT_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define TFT_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define TFT_BLUE        0x001F      /*   0,   0, 255 */
#define TFT_GREEN       0x07E0      /*   0, 255,   0 */
#define TFT_CYAN        0x07FF      /*   0, 255, 255 */
#define TFT_RED         0xF800      /* 255,   0,   0 */
#define TFT_MAGENTA     0xF81F      /* 255,   0, 255 */
#define TFT_YELLOW      0xFFE0      /* 255, 255,   0 */
#define TFT_WHITE       0xFFFF      /* 255, 255, 255 */
#define TFT_ORANGE      0xFD20      /* 255, 165,   0 */
#define TFT_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define TFT_PINK        0xF81F

// Color definitions for backwards compatibility
#define ILI9163_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9163_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9163_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9163_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9163_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9163_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9163_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9163_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9163_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9163_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9163_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9163_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9163_RED         0xF800      /* 255,   0,   0 */
#define ILI9163_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9163_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9163_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9163_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9163_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9163_PINK        0xF81F

typedef struct {
	const unsigned char *chartbl;
	const unsigned char *widthtbl;
	unsigned       char height;
	} fontinfo;

// This is a structure to conveniently hold infomation on the fonts
// Stores font character image address pointer, width table and height

const PROGMEM fontinfo fontdata [] = {
   { 0, 0, 0 },

   { 0, 0, 8 },

  #ifdef LOAD_FONT2
   { (const unsigned char *)chrtbl_f16, widtbl_f16, chr_hgt_f16},
  #else
   { 0, 0, 0 },
  #endif

   { 0, 0, 0 },

  #ifdef LOAD_FONT4
   { (const unsigned char *)chrtbl_f32, widtbl_f32, chr_hgt_f32},
  #else
   { 0, 0, 0 },
  #endif

   { 0, 0, 0 },

  #ifdef LOAD_FONT6
   { (const unsigned char *)chrtbl_f64, widtbl_f64, chr_hgt_f64},
  #else
   { 0, 0, 0 },
  #endif

  #ifdef LOAD_FONT7
   { (const unsigned char *)chrtbl_f7s, widtbl_f7s, chr_hgt_f7s},
  #else
   { 0, 0, 0 },
  #endif

  #ifdef LOAD_FONT8
   { (const unsigned char *)chrtbl_f72, widtbl_f72, chr_hgt_f72}
  #else
   { 0, 0, 0 }
  #endif
};


// Class functions and variables
class TFT_ILI9163 : public Print {

 public:

  TFT_ILI9163(UINTX _W = ILI9163_TFTWIDTH, UINTY _H = ILI9163_TFTHEIGHT);

  void     init(void), begin(void), // Same - begin included for backwards compatibility

           drawPixel(UINTX x, UINTY y, uint16_t color),
           fastSetup(void),

           drawChar(UINTX x, UINTY y, unsigned char c, uint16_t color, uint16_t bg, uint8_t font),
           setAddrWindow(UINTX x0, UINTY y0, UINTX x1, UINTY y1),

           pushColor(uint16_t color),
           pushColor(uint16_t color, uint16_t len),

           pushColors(uint16_t *data, uint8_t len),
           pushColors(uint8_t  *data, uint16_t len),

           fillScreen(uint16_t color),

           writeEnd(void),
           backupSPCR(void),
           restoreSPCR(void),

           drawLine(UINTX x0, UINTY y0, UINTX x1, UINTY y1, uint16_t color),
           drawFastVLine(UINTX x, UINTY y, UINTY h, uint16_t color),
           drawFastHLine(UINTX x, UINTY y, UINTX w, uint16_t color),

           drawRect(UINTX x, UINTY y, UINTX w, UINTY h, uint16_t color),
           fillRect(UINTX x, UINTY y, UINTX w, UINTY h, uint16_t color),
           drawRoundRect(UINTX x0, UINTY y0, UINTX w, UINTY h, int16_t radius, uint16_t color),
           fillRoundRect(UINTX x0, UINTY y0, UINTX w, UINTY h, int16_t radius, uint16_t color),

           setRotation(uint8_t r),
           invertDisplay(boolean i),

           drawCircle(UINTX x0, UINTY y0, int16_t r, uint16_t color),
           drawCircleHelper(UINTX x0, UINTY y0, int16_t r, uint8_t cornername, uint16_t color),
           fillCircle(UINTX x0, UINTY y0, int16_t r, uint16_t color),
           fillCircleHelper(UINTX x0, UINTY y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color),

           drawEllipse(UINTX x0, UINTY y0, UINTX rx, UINTY ry, uint16_t color),
           fillEllipse(UINTX x0, UINTY y0, UINTX rx, UINTY ry, uint16_t color),

           drawTriangle(UINTX x0, UINTY y0, UINTX x1, UINTY y1, UINTX x2, UINTY y2, uint16_t color),
           fillTriangle(UINTX x0, UINTY y0, UINTX x1, UINTY y1, UINTX x2, UINTY y2, uint16_t color),

           drawBitmap(UINTX x, UINTY y, const uint8_t *bitmap, UINTX w, UINTY h, uint16_t color),

           setCursor(UINTX x, UINTY y),
           setCursor(UINTX x, UINTY y, uint8_t font),
           setTextColor(uint16_t color),
           setTextColor(uint16_t fgcolor, uint16_t bgcolor),
           setTextSize(uint8_t size),
           setTextFont(uint8_t font),
           setTextWrap(boolean wrap),
           setTextDatum(uint8_t datum),
           setTextPadding(uint16_t x_width),

           spiwrite(uint8_t),
           writecommand(uint8_t c),
           writedata(uint8_t d),
           commandList(const uint8_t *addr);

  uint8_t  getRotation(void);

  uint16_t fontsLoaded(void),
           color565(uint8_t r, uint8_t g, uint8_t b);

  int16_t  drawChar(unsigned int uniCode, UINTX x, UINTY y, int font),
           drawNumber(long long_num, UINTX poX, UINTY poY, int font),
           drawFloat(float floatNumber,int decimal, UINTX poX, UINTY poY, int font),

           drawString(char *string, UINTX poX, UINTY poY, int font),
           drawCentreString(char *string, UINTX dX, UINTY poY, int font),
           drawRightString(char *string, UINTX dX, UINTY poY, int font),

           height(void),
           width(void),
           textWidth(char *string, int font),
           fontHeight(int font);

    void   setWindow(UINTX x0, UINTY y0, UINTX x1, UINTY y1);

 virtual  size_t write(uint8_t);

 private:

  uint8_t  tabcolor,
           colstart, rowstart; // some displays need this changed

  boolean  hwSPI;

  uint8_t  mySPCR, savedSPCR;

 protected:

  int16_t  _width, _height, // Display w/h as modified by current rotation
           cursor_x, cursor_y, padX;

  uint16_t textcolor, textbgcolor, fontsloaded;

  UINTX    addr_row, win_xe;
  UINTY    addr_col, win_ye;

  uint8_t  textfont,
           textsize,
           textdatum,
           rotation;

  boolean  textwrap; // If set, 'wrap' text at right edge of display

};

#endif

/***************************************************

  ORIGINAL LIBRARY HEADER

  This is our library for the Adafruit  ILI9163 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution

  Updated with new functions by Bodmer 14/4/15
 ****************************************************/
