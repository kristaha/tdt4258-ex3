#include <stdio.h>
#include <stdlib.h>
#include "display.h"

FILE* gamepad;

/* Prototypes */
int initGamepad();
void movePaddle1(int direction); // direction == 1 --> up, direction == -1 --> down
void movePaddle2(int direction); // direction == 1 --> up, direction == -1 --> down
/*
moveBall directions:
xDirextion == 1 --> right, xDirection == -1 --> left
yDirection == 1 --> up, yDirection == 0 --> no change, yDirection == -1 down
*/
void moveBall(int xDirection, int yDirection); 

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
			directionTest = -1;
		}else if (directionTest == -1 && paddle1PositionY >= 190){
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
		paddle1PositionY -= 5;
	}else if (direction == -1 && paddle1PositionY <= 190){
		paddle1PositionY += 5;
	}	
}
void movePaddle2(int direction){	
	if(direction == 1 && paddle2PositionY >= 0){
		paddle2PositionY -= 5;
	}else if (direction == -1 && paddle2PositionY <= 190){
		paddle2PositionY += 5;
	}	
}

void moveBall(int xDirection, int yDirection){
	if(xDirection == 1 && yDirection == 1){         // Move right, up
		ballPositionX += 5;
		ballPositionY -= 5;
	} else if(xDirection == 1 && yDirection == 0){  // Move right
		ballPositionX += 5;

	} else if(xDirection == 0 && yDirection == 1){  // Move left, up
		ballPositionX -= 5;
		ballPositionY -= 5;
	} else if(xDirection == 0 && yDirection == 0){  // Move left
		ballPositionX -= 5;
		
	} else if(xDirection == 0 && yDirection == -1){ // Move left, down
		ballPositionX -= 5;
		ballPositionY += 5;
	} else if(xDirection == 1 && yDirection == -1){ // Move right, down
		ballPositionX += 5;
		ballPositionY += 5;
	}









}

	
