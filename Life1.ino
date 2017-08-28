/*
* Conway's Game of Life by Stephen Fried
* February 15th, 2017
*
* Please feel free to modify and distribute this code, however, I do not accept liability
* for any concequences of its use.
*
*/

#include "gameBoard.h"

void setup() {
	Serial.begin(9600);
	randomSeed(analogRead(A0) * analogRead(A1));  // Create a random seed from analog "noise" on an unused port
	delay(10);
}

void loop() 
{
	bool gameOver = false; //This is what breaks the game loop
	gameBoard gameField;

	gameField.initialize();

	while (!gameOver) 
	{
		gameField.displayGeneration();
		gameOver = gameField.incrementGeneration();
	}
	//Serial.println("Game Over!!!");
}

