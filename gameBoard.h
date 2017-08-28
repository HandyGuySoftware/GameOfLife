#pragma once

#ifndef _GAMEBOARD_H_
#define _GAMEBOARD_H_

#include "hardware_Adafruit_LED_16X32.h"
#include <EEPROM.h>

class gameBoard
{
public:
	gameBoard();
	~gameBoard();
	void initialize(void);
	void refreshDisplay(void);
	void displayGeneration(void);
	bool incrementGeneration(void);

private:
		hardware display;
		uint32_t cgrid[MAXROWS], ngrid[MAXROWS];
		uint32_t sgrid1[MAXROWS], sgrid2[MAXROWS], sgrid3[MAXROWS], sgrid4[MAXROWS], sgrid5[MAXROWS];
		bool randomPixelColor;							// Do we want all pixels to be random colors? Read from EEPROM
		bool randomGameColor;							// For non-random pixels, do we want each game to choose a random color? Read from EEPROM
		int maxLoops;									// Maximum number of same-count loops before giving up (indicating an endless loop)
		bool wrap;										// Do you want to wrap around the screen edges?
		int delayVal;									// Delay between generations. values from 0-5 Read from EEPROM
		uint8_t pixelRed, pixelBlue, pixelGreen;		// RGB for standard pixel colors
		int numGenerations;								// Count of generations used in game
		int lastPixCount[6];							// Number of lit pixels in last six generation. Used for endless loop prevention
		int repeatCount;								// Number of times pixel count is repeated. Used for endless loop prevention
		bool showGens;

		uint32_t setBit(uint32_t num, int bit);			// Set bit in 32-bit value
		uint32_t clearBit(uint32_t num, int bit);		// Clear bit in 32-bit value
		bool getBit(uint32_t num, int bit);				// return bit in a 32-bit value
		int countNeighbors(int r, int c);				// Count number of neighbors a pixel has

		int optionManager();							// Manage game options

		int eepromReadDefaults(int *, bool *, bool *, int *, bool *, bool *);	// Read values from EEPROM storage
		int eepromWriteDefaults(int, bool, bool, int, bool, bool);			// Write values to EEPROM storage
};


#endif //_GAMEBOARD_H_
