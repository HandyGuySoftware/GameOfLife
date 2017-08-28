#pragma once

/*
* Hardware class definition header file for Adafruit 16x32 LED panel.
* For use in Life game simulator.
*/

#ifndef _HARDWARE_ADAFRUIT_LED_16X30_H
#define _HARDWARE_ADAFRUIT_LED_16X30_H

#include <Arduino.h>
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

#define CLK 8  // MUST be on PORTB! (Use pin 11 on Mega)
#define LAT A3
#define OE  9
#define A   A0
#define B   A1
#define C   A2

#define MAXROWS 16
#define MAXCOLUMNS 32

class hardware
{
public:
	hardware();
	~hardware();
	void initialize();
	void softwareReset();
	int getRows() { return MAXROWS; }
	int getCols() { return MAXCOLUMNS; }
	int fillRect(int c, int r, int w, int h, bool ran);
	int drawPixel(int c, int r, bool rand); 
	int clearPixel(int c, int r);
	uint16_t setPixelColor(uint8_t r, uint8_t g, uint8_t b);

	int setupButtonPressed();
	int optionButtonPressed();
	void ledPinLight(bool mode);

	void optionScreenSetup(char *s1, char *s2, char *s3);
	bool optionScreenRandomPixels(bool pr);
	bool optionScreenRandomGames(bool gr);
	int optionScreenGameDelay(int dl);
	int optionScreenTimeout(int to);
	bool optionScreenWrap(bool wp);
	bool hardware::optionScreenShowGens(bool sh);
	bool optionScreenReset(void);
	void screenScore(int n);

private:
	int pixelRed, pixelGreen, pixelBlue;

};

#endif  //_HARDWARE_ADAFRUIT_LED_16X30_H
