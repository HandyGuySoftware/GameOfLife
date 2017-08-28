/*
* Hardware class definition file for Adafruit 16x32 LED panel.
* For use in Life game simulator.
*/

#include "hardware_Adafruit_LED_16X32.h"

RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// Define Arduino I/O pins for button management
#define PIN_SETUP 11
#define PIN_OPTION 12
#define PIN_LED 13

hardware::hardware() {}

hardware::~hardware() {}

void hardware::initialize()
{
	matrix.begin();
	pinMode(PIN_SETUP, INPUT);		// Mode setting pin
	pinMode(PIN_OPTION, INPUT);	// Set intividual values
	pinMode(PIN_LED, OUTPUT);		// Mode indicator - ON=setup mode  OFF=Running mode
}

void hardware::softwareReset() // Restarts program from beginning but does not reset the peripherals and registers
{
	asm volatile ("  jmp 0");
}

int hardware::fillRect(int c, int r, int w, int h, bool ran)
{
	if (ran) // set a random color
		setPixelColor(random(0, 256), random(0, 256), random(0, 256));
	matrix.fillRect(c, r, w, h, matrix.Color888(pixelRed, pixelGreen, pixelBlue));
}

int hardware::drawPixel(int c, int r, bool rand)
{
	if (rand) // set a random color
		setPixelColor(random(0, 256), random(0, 256), random(0, 256));
	matrix.drawPixel(c, r, matrix.Color888(pixelRed, pixelGreen, pixelBlue));
	return 0;
}

int hardware::clearPixel(int c, int r)
{
	matrix.drawPixel(c, r, matrix.Color888(0, 0, 0));
	return 0;
}

uint16_t hardware::setPixelColor(uint8_t r, uint8_t g, uint8_t b)
{
	pixelRed = r;
	pixelGreen = g;
	pixelBlue = b;
}

int hardware::setupButtonPressed()
{
	return digitalRead(PIN_SETUP);
}

int hardware::optionButtonPressed()
{
	return digitalRead(PIN_OPTION);
}

void hardware::ledPinLight(bool mode)
{
	if (mode)
		digitalWrite(PIN_LED, HIGH);
	else
		digitalWrite(PIN_LED, LOW);

	return;
}

void hardware::optionScreenSetup(char *s1, char *s2, char *s3)
{

	while (setupButtonPressed() == LOW) {} // Wait until pressing stops.
	
	matrix.fillRect(0, 0, MAXCOLUMNS, MAXROWS, matrix.Color888(0, 0, 0)); // Clear screen
	matrix.setTextSize(1);    // size 1 == 8 pixels high
	matrix.setTextColor(matrix.Color333(0, 7, 7));

	matrix.setCursor(0, 0);   // start at top left, with one pixel of spacing
	matrix.print(s1);
	matrix.setCursor(0, 8);
	if (s2 != NULL)
		matrix.print(s2);
	if (s3 != NULL)
	{
		matrix.setCursor(24, 4);
		matrix.setTextColor(matrix.Color333(7, 0, 0));
		matrix.print(s3);
	}
}

bool hardware::optionScreenRandomPixels(bool pr)
{
	bool newPr = pr;
	
	optionScreenSetup("Rnd", "Pixl", (newPr ? "Y" : "N"));
	
	while (setupButtonPressed() == HIGH) 
	{
		if (optionButtonPressed() == LOW)
		{
			newPr = newPr ? false : true;
			matrix.fillRect(24, 4, 8, 8, matrix.Color333(0, 0, 0));
			matrix.setCursor(24, 4);
			matrix.setTextColor(matrix.Color333(7, 0, 0));
			matrix.print(newPr ? "Y" : "N");
			while (optionButtonPressed() == LOW) {}
		}
	}
	return newPr;
}


bool hardware::optionScreenRandomGames(bool gr)
{
	bool newGr = gr;

	optionScreenSetup("Rnd", "Game", (newGr ? "Y" : "N"));

	while (setupButtonPressed() == HIGH)
	{
		if (optionButtonPressed() == LOW)
		{
			newGr = newGr ? false : true;
			matrix.fillRect(24, 4, 8, 8, matrix.Color333(0, 0, 0));
			matrix.setCursor(24, 4);
			matrix.setTextColor(matrix.Color333(7, 0, 0));
			matrix.print(newGr ? "Y" : "N");
			while (optionButtonPressed() == LOW) {}
		}
	}
	return newGr;
}

int hardware::optionScreenGameDelay(int dl)
{
	int newDl = dl, maxSpeeds = 6;

	optionScreenSetup("Delay", NULL, NULL);

	matrix.setCursor(13, 8);
	matrix.setTextColor(matrix.Color333(7, 0, 0));
	matrix.print(newDl);

		while (setupButtonPressed() == HIGH)
	{
		if (optionButtonPressed() == LOW)
		{
			if (++newDl == maxSpeeds)
				newDl = 0;
			matrix.fillRect(13, 8, 8, 8, matrix.Color333(0, 0, 0));
			matrix.setCursor(13, 8);
			matrix.setTextColor(matrix.Color333(7, 0, 0));
			matrix.print(newDl);
			while (optionButtonPressed() == LOW) {}
		}
	}
	return newDl;
}

int hardware::optionScreenTimeout(int to)
{
	int newTo = to, increment = 100, maxTo = 1000;

	optionScreenSetup("MxLup", NULL, NULL);
	
	matrix.setCursor(6, 8);
	matrix.setTextColor(matrix.Color333(7, 0, 0));
	matrix.print(newTo);

	while (setupButtonPressed() == HIGH)
	{
		if (optionButtonPressed() == LOW)
		{
			if ((newTo += increment) > maxTo)
				newTo = 100;
			matrix.fillRect(0, 8, 32, 8, matrix.Color333(0, 0, 0));
			matrix.setCursor(6, 8);
			matrix.setTextColor(matrix.Color333(7, 0, 0));
			matrix.print(newTo);
			while (optionButtonPressed() == LOW) {}
		}
	}
	return newTo;
}

bool hardware::optionScreenWrap(bool wp)
{
	bool newWp = wp;

	optionScreenSetup("Wrap", "Brd", (newWp ? "Y" : "N"));
	
	while (setupButtonPressed() == HIGH)
	{
		if (optionButtonPressed() == LOW)
		{
			newWp = newWp ? false : true;
			matrix.fillRect(24, 4, 8, 8, matrix.Color333(0, 0, 0));
			matrix.setCursor(24, 4);
			matrix.setTextColor(matrix.Color333(7, 0, 0));
			matrix.print(newWp ? "Y" : "N");
			while (optionButtonPressed() == LOW) {}
		}
	}
	return newWp;
}

bool hardware::optionScreenShowGens(bool sh)
{
	bool newSh = sh;

	optionScreenSetup("Show", "Gens", (newSh ? "Y" : "N"));

	while (setupButtonPressed() == HIGH)
	{
		if (optionButtonPressed() == LOW)
		{
			newSh = newSh ? false : true;
			matrix.fillRect(24, 4, 8, 8, matrix.Color333(0, 0, 0));
			matrix.setCursor(24, 4);
			matrix.setTextColor(matrix.Color333(7, 0, 0));
			matrix.print(newSh ? "Y" : "N");
			while (optionButtonPressed() == LOW) {}
		}
	}
	return newSh;
}


bool hardware::optionScreenReset(void)
{
	optionScreenSetup("Reset", "Dflt?", NULL);
	
	while (setupButtonPressed() == HIGH)
	{
		if (optionButtonPressed() == LOW)
			return true;	// Send signal to reset defaults in EEPROM
	}
	return false;	// Send signal to NOT reset defaults in EEPROM
}

void hardware::screenScore(int n)
{
	matrix.fillRect(0, 0, MAXCOLUMNS, MAXROWS, matrix.Color888(0, 0, 0)); // Clear screen
	matrix.setTextSize(1);    // size 1 == 8 pixels high
	matrix.setTextColor(matrix.Color888(pixelRed, pixelGreen, pixelBlue));

	matrix.setCursor(0, 0);   // start at top left, with one pixel of spacing
	matrix.print(n);
	matrix.setCursor(0, 8);
	matrix.print("Gen's");

	return;
}
