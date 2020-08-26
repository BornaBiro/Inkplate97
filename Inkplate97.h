/*************************************************** 
This library is used for controling ED097OC1 epaper panel on e-radionica's Inkplate10 dev board (we are still working on it!).
If you don't know what Inkplate is, check it out here: https://inkplate.io/

Author: Borna Biro ( https://github.com/BornaBiro/ )
Organization: e-radionica.com / TAVU

This library uses Adafruit GFX library (https://github.com/adafruit/Adafruit-GFX-Library) made by Adafruit Industries.

NOTE: This library is still heavily in progress, so there is still some bugs. Use it on your own risk!
 ****************************************************/

#ifndef __INKPLATE97_H__
#define __INKPLATE97_H__
#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifndef ARDUINO_ESP32_DEV
#error "Wrong board selected! Select ESP32 Wrover from board menu!"
#endif

#include "Adafruit_GFX.h"
#include "Wire.h"
#include "SPI.h"
//#include "Adafruit_MCP23017.h"
#include "SdFat.h"

#define MCP23017_INT_ADDR		0x20
#define MCP23017_EXT_ADDR		0x27
#define MCP23017_INT_PORTA      0x00
#define MCP23017_INT_PORTB      0x01
#define MCP23017_INT_NO_MIRROR  false
#define MCP23017_INT_MIRROR     true
#define MCP23017_INT_PUSHPULL   false
#define MCP23017_INT_OPENDRAIN  true
#define MCP23017_INT_ACTLOW     false
#define MCP23017_INT_ACTHIGH    true

#define MCP23017_IODIRA 0x00
#define MCP23017_IPOLA 0x02
#define MCP23017_GPINTENA 0x04
#define MCP23017_DEFVALA 0x06
#define MCP23017_INTCONA 0x08
#define MCP23017_IOCONA 0x0A
#define MCP23017_GPPUA 0x0C
#define MCP23017_INTFA 0x0E
#define MCP23017_INTCAPA 0x10
#define MCP23017_GPIOA 0x12
#define MCP23017_OLATA 0x14

#define MCP23017_IODIRB 0x01
#define MCP23017_IPOLB 0x03
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALB 0x07
#define MCP23017_INTCONB 0x09
#define MCP23017_IOCONB 0x0B
#define MCP23017_GPPUB 0x0D
#define MCP23017_INTFB 0x0F
#define MCP23017_INTCAPB 0x11
#define MCP23017_GPIOB 0x13
#define MCP23017_OLATB 0x15


#define E_INK_WIDTH 		1200
#define E_INK_HEIGHT 		825
#define BLACK 				1
#define WHITE 				0
#define GPIO0_ENABLE 		8
#define INKPLATE_1BIT 		0
#define INKPLATE_3BIT 		1
#define PAD1 				0
#define PAD2 				1
#define PAD3 				2

#define DATA    		0x0E8C0030   //D0-D7 = GPIO4 GPIO5 GPIO18 GPIO19 GPIO23 GPIO25 GPIO26 GPIO27

#define CL        		0x01    //GPIO0
#define CL_SET    		{GPIO.out_w1ts = CL;}
#define CL_CLEAR  		{GPIO.out_w1tc = CL;}

#define LE        		0x04     //GPIO2
#define LE_SET    		{GPIO.out_w1ts = LE;}
#define LE_CLEAR  		{GPIO.out_w1tc = LE;}

#define CKV       		0x01   //GPIO32
#define CKV_SET   		{GPIO.out1_w1ts.val = CKV;}
#define CKV_CLEAR 		{GPIO.out1_w1tc.val = CKV;}

#define SPH         	0x02   //GPIO33
#define SPH_SET     	{GPIO.out1_w1ts.val = SPH;}
#define SPH_CLEAR   	{GPIO.out1_w1tc.val = SPH;}

//I/O Expander - A Channel
#define GMOD       1    //GPIOA1
#define GMOD_SET    	{digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, GMOD, HIGH);}
#define GMOD_CLEAR  	{digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, GMOD, LOW);}

#define OE          	0    //GPIOA0
#define OE_SET      	{digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, OE, HIGH);}
#define OE_CLEAR    	{digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, OE, LOW);}

#define SPV         	2   //GPIOA5
#define SPV_SET     	{digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, SPV, HIGH);}
#define SPV_CLEAR   	{digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, SPV, LOW);}

#define WAKEUP         	3   //GPIOA3
#define WAKEUP_SET     	{digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, WAKEUP, HIGH);}
#define WAKEUP_CLEAR   	{digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, WAKEUP, LOW);}

#define PWRUP         	4   //GPIOA4
#define PWRUP_SET     	{digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, PWRUP, HIGH);}
#define PWRUP_CLEAR   	{digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, PWRUP, LOW);}

#define VCOM         	5   //GPIOA6
#define VCOM_SET     	{digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, VCOM, HIGH);}
#define VCOM_CLEAR   	{digitalWriteInternal(MCP23017_INT_ADDR, mcpRegsInt, VCOM, LOW);}

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

//extern Adafruit_MCP23017 mcp;
extern SPIClass spi2;
extern SdFat sd;

class Inkplate : public Adafruit_GFX {
  public:
    uint8_t* D_memory_new;
    uint8_t* _partial;
    uint8_t* D_memory4Bit;
    uint8_t * _pBuffer;
    const uint8_t LUT2[16] = {B10101010, B10101001, B10100110, B10100101, B10011010, B10011001, B10010110, B10010101, B01101010, B01101001, B01100110, B01100101, B01011010, B01011001, B01010110, B01010101};
    const uint8_t LUTW[16] = {B11111111, B11111110, B11111011, B11111010, B11101111, B11101110, B11101011, B11101010, B10111111, B10111110, B10111011, B10111010, B10101111, B10101110, B10101011, B10101010};
    const uint8_t LUTB[16] = {B11111111, B11111101, B11110111, B11110101, B11011111, B11011101, B11010111, B11010101, B01111111, B01111101, B01110111, B01110101, B01011111, B01011101, B01010111, B01010101};
    const uint8_t pixelMaskLUT[8] = {B00000001, B00000010, B00000100, B00001000, B00010000, B00100000, B01000000, B10000000};
    const uint8_t pixelMaskGLUT[2] = {B00001111, B11110000};
    const uint8_t discharge[16] = {B11111111, B11111100, B11110011, B11110000, B11001111, B11001100, B11000011, B11000000, B00111111, B00111100, B00110011, B00110000, B00001111, B00001100, B00000011, B00000000};
    //BLACK->WHITE
	//THIS IS OKAYISH WAVEFORM FOR GRAYSCALE. IT CAN BE MUCH BETTER.
	const uint8_t waveform3Bit[8][8] = {{0, 0, 0, 0, 1, 1, 1, 0}, {1, 2, 2, 2, 1, 1, 1, 0}, {0, 1, 2, 1, 2, 2, 1, 0}, {0, 0, 1, 1, 1, 1, 2, 0}, {0, 0, 0, 1, 1, 1, 2, 0}, {2, 1, 1, 1, 2, 1, 2, 0}, {1, 1, 1, 2, 1, 2, 2, 0}, {0, 0, 0, 0, 0, 0, 2, 0}};
    //const uint8_t waveform3Bit[8][9] = {{0, 0, 0, 0, 1, 1, 1, 0, 3}, {0, 1, 2, 1, 1, 2, 1, 0, 3}, {0, 2, 2, 1, 1, 2, 1, 0, 3}, {1, 1, 1, 1, 1, 1, 2, 0, 3}, {0, 2, 1, 1, 1, 1, 2, 0, 3} , {2, 1, 1, 1, 2, 1, 2, 0, 3}, {1, 1, 1, 2, 1, 2, 2, 0, 3}, {0, 0, 0, 0, 0, 0, 2, 0, 3}};
	//const uint8_t waveform3Bit[8][7] = {{0, 0, 0, 0, 1, 1, 1}, {0, 0, 1, 1, 1, 2, 1}, {0, 1, 1, 2, 1, 2, 1}, {0, 0, 1, 1, 2, 1, 2}, {1, 1, 1, 2, 2, 1, 2}, {0, 0, 1, 1, 1, 2, 2}, {0, 1, 1, 2, 1, 2, 2}, {0, 0, 0, 0, 0, 0, 2}};
	
	
	//const uint8_t waveform3Bit[8][12] = {{3,3,3,1,1,1,1,1,1,1,2,0}, {3,3,3,3,1,1,1,1,1,1,2,0}, {3,3,3,3,3,1,1,1,1,1,2,0}, {3,3,3,3,3,3,1,1,1,1,2,0}, {3,3,3,3,3,3,3,1,1,1,2,0}, {3,3,3,3,3,3,3,2,1,1,2,0}, {3,3,3,3,3,3,3,3,3,1,2,0}, {3,3,3,3,3,3,3,3,3,3,2,0}};
	//const uint8_t waveform3Bit[16][12] = {{0,0,0,0,0,0,1,2,1,1,0,3},{0,0,1,1,1,2,2,2,1,1,0,3},{0,0,0,1,1,2,2,2,1,1,0,3},  {0,0,0,1,2,1,2,1,2,1,3}, {0,0,2,1,2,1,2,1,2,1,3}, {0,0,1,2,2,1,1,1,1,2,0,3}, {0,0,0,2,1,1,1,1,0,2,0,3}, {0,0,2,1,2,2,1,1,1,2,0,3}, {0,0,0,2,2,2,1,1,1,2,0,3}, {0,0,0,0,0,0,2,1,1,2,0,3}, {0,0,0,0,0,2,2,1,1,2,0,3}, {0,0,0,0,0,1,1,1,2,2,0,3}, {0,0,0,0,1,2,1,2,1,2,0,3}, {0,0,0,0,1,1,2,2,1,2,0,3},{0,0,0,0,1,1,1,2,2,2,0,3}, {0,0,0,0,0,0,0,0,0,2,0,3}};
	//PVI waveform for cleaning screen, not sure if it is correct, but it cleans screen properly.
    const uint32_t waveform[50] = {0x00000008, 0x00000008, 0x00200408, 0x80281888, 0x60a81898, 0x60a8a8a8, 0x60a8a8a8, 0x6068a868, 0x6868a868, 0x6868a868, 0x68686868, 0x6a686868, 0x5a686868, 0x5a686868, 0x5a586a68, 0x5a5a6a68, 0x5a5a6a68, 0x55566a68, 0x55565a64, 0x55555654, 0x55555556, 0x55555556, 0x55555556, 0x55555516, 0x55555596, 0x15555595, 0x95955595, 0x95959595, 0x95949495, 0x94949495, 0x94949495, 0xa4949494, 0x9494a4a4, 0x84a49494, 0x84948484, 0x84848484, 0x84848484, 0x84848484, 0xa5a48484, 0xa9a4a4a8, 0xa9a8a8a8, 0xa5a9a9a4, 0xa5a5a5a4, 0xa1a5a5a1, 0xa9a9a9a9, 0xa9a9a9a9, 0xa9a9a9a9, 0xa9a9a9a9, 0x15151515, 0x11111111};
	
	struct bitmapHeader {
		uint16_t signature;
		uint32_t fileSize;
		uint32_t startRAW;
		uint32_t dibHeaderSize;
		uint32_t width;
		uint32_t height;
		uint16_t color;
		uint32_t compression;
	};
  
    Inkplate(uint8_t _mode);
	void begin(void);
    void drawPixel(int16_t x0, int16_t y0, uint16_t color);
    void clearDisplay();
    void display();
    void partialUpdate();
	void drawBitmap3Bit(int16_t _x, int16_t _y, const unsigned char* _p, int16_t _w, int16_t _h);
	void setRotation(uint8_t);
    void einkOff(void);
    void einkOn(void);
    void selectDisplayMode(uint8_t _mode);
	uint8_t getDisplayMode();
	int drawBitmapFromSD(SdFile* p, int x, int y);
	int drawBitmapFromSD(char* fileName, int x, int y);
	int sdCardInit();
	SdFat getSdFat();
	SPIClass getSPI();
	uint8_t getPanelState();
    uint8_t readTouchpad(uint8_t);
    int8_t readTemperature();
    double readBattery();
	void vscan_start();
	void vscan_write();
	void hscan_start(uint32_t _d = 0);
	void vscan_end();
	void clean();
    void cleanFast(uint8_t c, uint8_t rep);
    void cleanFast2(uint8_t c, uint8_t n, uint16_t d);
    void pinsZstate();
    void pinsAsOutputs();
	
	void pinModeMCP(uint8_t _pin, uint8_t _mode);
	void digitalWriteMCP(uint8_t _pin, uint8_t _state);
	uint8_t digitalReadMCP(uint8_t _pin);
	void setIntOutput(uint8_t intPort, uint8_t mirroring, uint8_t openDrain, uint8_t polarity);
	void setIntPin(uint8_t _pin, uint8_t _mode);
	void removeIntPin(uint8_t _pin);
	uint16_t getINT();
	uint16_t getINTstate();
	void setPorts(uint16_t _d);
	uint16_t getPorts();

  private:
	uint8_t mcpRegsInt[22], mcpRegsEx[22];
    int8_t _temperature;
    uint8_t _panelOn = 0;
    uint8_t _rotation = 0;
    uint8_t _displayMode = 0; //By default, 1 bit mode is used
	int sdCardOk = 0;
	uint8_t _blockPartial = 1;
	uint8_t _beginDone = 0;
	
	void display1b();
    void display3b();
	uint32_t read32(uint8_t* c);
	uint16_t read16(uint8_t* c);
	void readBmpHeader(SdFile *_f, struct bitmapHeader *_h);
	int drawMonochromeBitmap(SdFile *f, struct bitmapHeader bmpHeader, int x, int y);
	int drawGrayscaleBitmap24(SdFile *f, struct bitmapHeader bmpHeader, int x, int y);
	
	bool mcpBegin(uint8_t _addr, uint8_t* _r);
	void readMCPRegisters(uint8_t _addr, uint8_t *k);
	void readMCPRegisters(uint8_t _addr, uint8_t _regName, uint8_t *k, uint8_t _n);
	void readMCPRegister(uint8_t _addr, uint8_t _regName, uint8_t *k);
	void updateAllRegisters(uint8_t _addr, uint8_t *k);
	void updateRegister(uint8_t _addr, uint8_t _regName, uint8_t _d);
	void updateRegister(uint8_t _addr, uint8_t _regName, uint8_t *k, uint8_t _n);
	void pinModeInternal(uint8_t _addr, uint8_t* _r, uint8_t _pin, uint8_t _mode);
	void digitalWriteInternal(uint8_t _addr, uint8_t* _r, uint8_t _pin, uint8_t _state);
	uint8_t digitalReadInternal(uint8_t _addr, uint8_t* _r, uint8_t _pin);
	void setIntOutputInternal(uint8_t _addr, uint8_t* _r, uint8_t intPort, uint8_t mirroring, uint8_t openDrain, uint8_t polarity);
	void setIntPinInternal(uint8_t _addr, uint8_t* _r, uint8_t _pin, uint8_t _mode);
	void removeIntPinInternal(uint8_t _addr, uint8_t* _r, uint8_t _pin);
	uint16_t getINTInternal(uint8_t _addr, uint8_t* _r);
	uint16_t getINTstateInternal(uint8_t _addr, uint8_t* _r);
	void setPortsInternal(uint8_t _addr, uint8_t* _r, uint16_t _d);
	uint16_t getPortsInternal(uint8_t _addr, uint8_t* _r);
};

#endif