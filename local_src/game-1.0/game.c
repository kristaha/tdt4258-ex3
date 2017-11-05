#include <stdio.h>
#include <stdlib.h>
#include "display.h"

FILE* gamepad;

/* Prototypes */
int initGamepad();
void movePaddle1(int direction); // direction == 1 --> up, direction == 0 --> down
void movePaddle2(int direction); // direction == 1 --> up, direction == 0 --> down

/* Positions values */
int paddle1PositionY = 90;
int paddle2PositionY = 90;
int ballPositionX = 155;
int ballPositionY = 115;

int main(int argc, char *argv[])
{
	printf("Hello World, I'm game!\n");
	initDisplay();
	int directionTest = 1;
	for(int i = 0; i < 500; i++){
		if(directionTest == 1 && paddle1PositionY <= 0 ){
			directionTest = 0;
		}else if (directionTest == 0 && paddle1PositionY >= 190){
			directionTest = 1;
		}

		movePaddle1(directionTest);
		movePaddle2(directionTest);
		updateDisplay(paddle1PositionY, paddle2PositionY, directionTest);
	}
	exit(EXIT_SUCCESS);
}

int initGamepad(){
	gamepad = fopen("/dev/gamepad", "rb");
	if(!gamepad){
		return EXIT_FAILURE;
	}
	return 0;
}

void movePaddle1(int direction){
	if(direction == 1 && paddle1PositionY >= 0){
		paddle1PositionY -= 1;
	}else if (direction == 0 && paddle1PositionY <= 190){
		paddle1PositionY += 1;
	}	
}
void movePaddle2(int direction){	
	if(direction == 1 && paddle2PositionY >= 0){
		paddle2PositionY -= 1;
	}else if (direction == 0 && paddle2PositionY <= 190){
		paddle2PositionY += 1;
	}	
}

	
