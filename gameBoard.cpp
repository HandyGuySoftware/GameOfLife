#include "gameBoard.h"
#include "hardware_Adafruit_LED_16X32.h"

const unsigned int EepromStartAddress = 10;
const unsigned int EepromSignature = 0x33;

gameBoard::gameBoard() {}

gameBoard::~gameBoard() {}

void gameBoard::initialize(void)
{
	int row, col;

	display.initialize();
	numGenerations = 1;

	/*
	// Need to read EEPROM  to get default values
	// - Game delay (0-5 = 0, 200, 400, 600, 800, 1000 ms) 
	// - Random Pixel color (T/F)
	// - Random game color (T/F) (If not random pixels, each full game board has a random color.)
	// - Maximum loop rounds for endless loops
	// - Wrap screen ( Wrap pixels around edges of screen)
	*/
	if (eepromReadDefaults(&delayVal, &randomPixelColor, &randomGameColor, &maxLoops, &wrap, &showGens) != 0)  // EEEPROM config not programmed yet, Use default values.
	{
		Serial.println("Generating new default values");
		delayVal = 0;
		randomPixelColor = false;
		randomGameColor = true;
		maxLoops = 200;
		wrap = false;
		showGens = false;

		// Write the default values to EEPROM with header signature
		eepromWriteDefaults(delayVal, randomPixelColor, randomGameColor, maxLoops, wrap, showGens);
	}
	/*
	else
	{
		Serial.println("Read old default values");
		Serial.print("d-p-g-t-w-s = ");
		Serial.print(delayVal); Serial.print("-");
		Serial.print(randomPixelColor); Serial.print("-");
		Serial.print(randomGameColor); Serial.print("-");
		Serial.print(maxLoops); Serial.print("-");
		Serial.print(wrap);Serial.print("-");
		Serial.println(showGens);

	}
	*/

	for (row=0; row < 6; ++row)
		lastPixCount[row] = 0;
	repeatCount = 0;

	// Logic to set game colors
	if (!randomGameColor && !randomPixelColor)	// Just plain black & white, please
		display.setPixelColor(255, 255, 255);
	else
		display.setPixelColor(random(0, 256), random(0, 256), random(0, 256)); // Set random color for randomGameColor. randomPixelColor=true will override this during the game

	// Initialize random grid
	for (row = 0; row < display.getRows(); ++row) //This block initializes the game grid with random cells
	{
		for (col = 0; col < display.getCols(); ++col)
		{
			if (random(4) > 2)	// Initialize 25% of pixels to "on"
				cgrid[row] = setBit(cgrid[row], col);
			else
				cgrid[row] = clearBit(cgrid[row], col);
		}
		ngrid[row] = sgrid1[row] = sgrid2[row] = sgrid3[row] = sgrid4[row] = sgrid5[row] = (uint32_t)0;	// Initialize next grid and backup grids to all 0's
	}

	/*
	// Test Case - Glider
	cgrid[5] = setBit(cgrid[5], 2);
	cgrid[5] = setBit(cgrid[5], 3);
	cgrid[6] = setBit(cgrid[6], 1);
	cgrid[6] = setBit(cgrid[6], 3);
	cgrid[7] = setBit(cgrid[7], 3);
	*/
		
	/*
	// Test Case - LWSS
	cgrid[5] = setBit(cgrid[5], 3);
	cgrid[5] = setBit(cgrid[5], 4);
	cgrid[5] = setBit(cgrid[5], 5);
	cgrid[6] = setBit(cgrid[6], 2);
	cgrid[6] = setBit(cgrid[6], 5);
	cgrid[7] = setBit(cgrid[7], 5);
	cgrid[8] = setBit(cgrid[8], 1);
	cgrid[8] = setBit(cgrid[8], 5);
	cgrid[9] = setBit(cgrid[9], 5);
	cgrid[10] = setBit(cgrid[10], 2);
	cgrid[10] = setBit(cgrid[10], 4);
	*/
	
	refreshDisplay();
}

void gameBoard::refreshDisplay()
{
	int row, col;

	for (col = 0; col < display.getCols(); ++col)
	{
		// Draw vertical bar that wipes across the screen to reveal initial pixels. A neat visual effect.
		display.fillRect(col, 0, 1, display.getRows(), (randomPixelColor ? true : false));   
		if (col != 0)
		{
			for (row = 0; row < display.getRows(); ++row)
			{
				if (getBit(cgrid[row], col - 1))
					display.drawPixel(col - 1, row, (randomPixelColor?true:false));
				else
					display.clearPixel(col - 1, row);
			}
		}
		delay(25);
	}
}

uint32_t gameBoard::setBit(uint32_t num, int bit)
{
	return (num |= ((uint32_t)0x80000000 >> bit));
}

uint32_t gameBoard::clearBit(uint32_t num, int bit)
{
	return (num &= ~((uint32_t)0x80000000 >> bit));
}

bool gameBoard::getBit(uint32_t num, int bit)
{
	return (num &= ((uint32_t)0x80000000 >> bit));
}

int gameBoard::countNeighbors(int r, int c)
{
	int neighbors = 0, x, y;
	int chkRow, chkCol;

	for (x = -1; x < 2; ++x)	// from center pixel, check left & right 1 pixel
	{
		chkCol = c + x;	// The column of the pixel you're currently checking

		if (wrap)	// Wrapping around board edges
		{
			if (chkCol < 0)							// Off left edge of screen
				chkCol = (display.getCols() - 1);	// Set to right edge of screen
			else if (chkCol >= display.getCols())	// Off right edge of screen
				chkCol = 0;							// Set to left edge of screen
		}
		else // Not wrapping around board edges
		{
			if ((chkCol < 0) || (chkCol >= display.getCols()))  // Neighbor is off left/right side of board
				continue;
		}

		for (y = -1; y < 2; ++y) // from center pixel , check up & down 1 pixel
		{
			chkRow = r + y; // The row of the pixel you're currently checking
			if ((x == 0) && (y == 0)) // This is the center pixel. We're counting the center's neighbors, not itself.
				continue;

			if (wrap) // Wrapping around board edges
			{
				if (chkRow < 0)							// Off top edge of screen
					chkRow = (display.getRows() - 1);	// Set to bottom edge of screen
				else if (chkRow >= display.getRows())	// Off bottom edge of screen
					chkRow = 0;							// Set to top edge of screen
			}
			else // Not wrapping around board edges
			{
				if ((chkRow < 0) || (chkRow >= display.getRows()))  // Neighbor is off top/bottom of board
					continue;
			}
			if (getBit(cgrid[chkRow], chkCol) == true)
				neighbors++;
		}
	}
	return neighbors;
}

int gameBoard::optionManager()
{
	randomPixelColor = display.optionScreenRandomPixels(randomPixelColor);
	if ((randomGameColor = display.optionScreenRandomGames(randomGameColor)) == false)
	{
		display.setPixelColor(255, 255, 255); // Just black & while, thank you
	}
	delayVal = display.optionScreenGameDelay(delayVal);
	maxLoops = display.optionScreenTimeout(maxLoops);
	wrap = display.optionScreenWrap(wrap);
	showGens= display.optionScreenShowGens(showGens);
	if (display.optionScreenReset())
	{
		EEPROM.update(EepromStartAddress, 0x00);	// Delete header signature
		display.softwareReset();
	}

	// Now, store all the new options
	eepromWriteDefaults(delayVal, randomPixelColor, randomGameColor, maxLoops, wrap, showGens);

	return 0;
}

int gameBoard::eepromReadDefaults(int *d, bool *p, bool *g, int *t, bool *w, bool *s)
{
	int tTmp;

	int sig = EEPROM.read(EepromStartAddress);
	if (sig != 0x30)
		return -1;	// No signature - use default values
		
	*d = EEPROM.read(EepromStartAddress + 1);	// Game delay (0-5)
	*p = EEPROM.read(EepromStartAddress + 2);	// Random Pixel color (T/F)
	*g = EEPROM.read(EepromStartAddress + 3);	// Random game color (T/f)
	tTmp = EEPROM.read(EepromStartAddress + 4) << 8;	// Maximum endless loop count - bits 0-7
	tTmp |= EEPROM.read(EepromStartAddress + 5);	// Maximum endless loop count - Bits 8-15
	*t = tTmp;
	*w = EEPROM.read(EepromStartAddress + 6);	// Wrap screen edges (T/F)
	*s = EEPROM.read(EepromStartAddress + 7);	// Show # of generations at end of game

	return 0;
}

int gameBoard::eepromWriteDefaults(int d, bool p, bool g, int t, bool w, bool s)
{
	/*
	Serial.println("Writing new default values");
	Serial.print("d-p-g-t-w-s = ");
	Serial.print(d); Serial.print("-");
	Serial.print(p); Serial.print("-");
	Serial.print(g); Serial.print("-");
	Serial.print(t); Serial.print("-");
	Serial.print(w); Serial.print("-");
	Serial.println(s);
	*/
	
	EEPROM.update(EepromStartAddress, 0x30);					// header signature
	EEPROM.update(EepromStartAddress + 1, d);					// Game delay (0-5)
	EEPROM.update(EepromStartAddress + 2, p);					// Random Pixel color (T/F)
	EEPROM.update(EepromStartAddress + 3, g);					// Random game color (T/f)
	EEPROM.update(EepromStartAddress + 4, ((t & 0xff00) >> 8));	// Maximum endless loop count - bits 0-7
	EEPROM.update(EepromStartAddress + 5, (t & 0xff));			// Maximum endless loop count - Bits 8-15
	EEPROM.update(EepromStartAddress + 6, w);					// Wrap screen edges (T/F)
	EEPROM.update(EepromStartAddress + 7, s);					// Show # of generations at end of game

	return 0;
}

void gameBoard::displayGeneration()
{
	int row, col, pixCount=0;

	for (row = 0; row < display.getRows(); ++row)
	{
		for (col = 0; col < display.getCols(); ++col)
		{
			if (getBit(cgrid[row], col))
			{
				display.drawPixel(col, row, (randomPixelColor ? true : false));
				++pixCount;
			}
			else
				display.clearPixel(col, row);		// Black
		}
	}
	
	//Serial.print("PixCount="); Serial.print(pixCount); Serial.print("="); Serial.print(((float)pixCount / 512) * 100); Serial.println("%");
	
	if (pixCount == lastPixCount[0])	//You're starting/in an endless loop
	{
		//Serial.print("Same pixCount:"); Serial.print(pixCount); Serial.print("   repeatCount:"); Serial.println(repeatCount);
		++repeatCount;
	}
	else if ((pixCount == lastPixCount[1]) && (pixCount == lastPixCount[3]) && (pixCount == lastPixCount[5]))	// You're starting a repeating pattern
	{
		//Serial.print("Pattern pixCount:"); Serial.print(pixCount); Serial.print("   repeatCount:"); Serial.println(repeatCount);
		++repeatCount;
	}
	else // You're not starting/in an endless loop. Reset counts
	{
		repeatCount = 0;
	}
	
	// Save last pixel countt
	for (row = 5; row > 0; --row)
		lastPixCount[row] = lastPixCount[row - 1];
	lastPixCount[0] = pixCount;

	/*
	Serial.print("lastPixCount: ");
	for (row = 0; row < 6; ++row)
	{
		Serial.print(lastPixCount[row]); Serial.print("-");
	}
	Serial.println("X");
	*/
	
	delay(delayVal * 200); // Speed setting is in increments. Each increment = 200 ms
}

// Increment to next generation. Returns false (game not over) or true (game over - no more moves)
bool gameBoard::incrementGeneration()
{
	int row, col, neighbors;
	bool gameOver = true;	// Start by assuming the gamne is over

	// Check for setup indicators
	if (display.setupButtonPressed() == LOW)
	{
		display.ledPinLight(true);
		optionManager();
	}
	else
		display.ledPinLight(false);

	if (repeatCount != maxLoops)		// Haven't maxed out of endless loop yet
	{
		for (row = 0; row < display.getRows() ; ++row)
		{
			for (col = 0; col < display.getCols(); ++col)
			{
				neighbors = countNeighbors(row, col);

				if (getBit(cgrid[row], col) == true) //If a cell is 'alive', game is not over, apply alive cell rules.
				{
					gameOver = false;
					if (neighbors < 2)
						ngrid[row] = clearBit(ngrid[row], col);
					else if ((neighbors == 2) || (neighbors == 3))
						ngrid[row] = setBit(ngrid[row], col);
					else if (neighbors > 3)
						ngrid[row] = clearBit(ngrid[row], col);
				}
				else //If cell is not alive, apply dead cell rule.
				{
					if (neighbors == 3)
						ngrid[row] = setBit(ngrid[row], col);
				}
			}
		}

		// If current grid same as next grid OR current grid same as saved grid(s), we're caught in a loop. End.
		if ((memcmp(cgrid, ngrid, sizeof(cgrid)) == 0)
			|| (memcmp(sgrid1, ngrid, sizeof(cgrid)) == 0)
			|| (memcmp(sgrid2, ngrid, sizeof(cgrid)) == 0)
			|| (memcmp(sgrid3, ngrid, sizeof(cgrid)) == 0)
			|| (memcmp(sgrid4, ngrid, sizeof(cgrid)) == 0)
			|| (memcmp(sgrid5, ngrid, sizeof(cgrid)) == 0))
		{
			delay(5000);	// Pause to display final board
			gameOver = true;
		}
		else
		{
			// Save game board state for later comparison
			// Eliminates "zombie" boards that loop endlessly between several patterns
			memcpy(sgrid5, sgrid4, sizeof(cgrid));    //Saved grid 4 becomes saved grid 5.
			memcpy(sgrid4, sgrid3, sizeof(cgrid));    //Saved grid 3 becomes saved grid 4.
			memcpy(sgrid3, sgrid2, sizeof(cgrid));    //Saved grid 2 becomes saved grid 3.
			memcpy(sgrid2, sgrid1, sizeof(cgrid));    //Saved grid 1 becomes saved grid 2
			memcpy(sgrid1, cgrid, sizeof(cgrid));    //Current grid becomes saved grid1.
			memcpy(cgrid, ngrid, sizeof(cgrid));    //Next grid becomes current grid.
		}
	}

	if (gameOver && showGens)
	{
		//Serial.println("gameOver && showGens");
		display.screenScore(numGenerations);
		delay(5000);	// Pause to display final score
	}
	
	++numGenerations;
	return gameOver;
}
