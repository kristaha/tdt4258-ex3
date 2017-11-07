#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

FILE* gamepad;

/* Prototypes */
int initGamepad();
void exitGamepad();
void movePaddle1(int direction); // direction == 1 --> up, direction == -1 --> down
void movePaddle2(int direction); // direction == 1 --> up, direction == -1 --> down
void saHandler(int signalNumber);
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

/*Structs*/
struct sigaction gameSigaction = {
	.sa_handler = saHandler, 
};



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
		moveBall(directionTest, 0);
		updateDisplay(paddle1PositionY, paddle2PositionY, directionTest, ballPositionX, ballPositionY, directionTest,0);
	}
	exit(EXIT_SUCCESS);
}

int initGamepad(){
	long oflags;

	gamepad = fopen("/dev/gamepad", "rb");

	if(!gamepad){
		return EXIT_FAILURE;
	}
	if(sigaction(SIGIO, *gameSigaction, NULL) != 0){ // bruker sigaction siden signal er utdatert
		return EXIT_FAILURE;	
	}	
	if(fcntl(fileno(gamepad), F_SETOWN, getpid()) != 0){
		return EXIT_FAILURE;
	}
	oflags = fcntl(fileno(gamepad), F_GETFL);		
	if(fcntl(fileno(gamepad), F_SETFL, oflags | FASYNC) != 0){
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

void exitGamepad(){
	flose(gamepad);
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

	} else if(xDirection == -1 && yDirection == 1){  // Move left, up
		ballPositionX -= 5;
		ballPositionY -= 5;
	} else if(xDirection == -1 && yDirection == 0){  // Move left
		ballPositionX -= 5;
		
	} else if(xDirection == -1 && yDirection == -1){ // Move left, down
		ballPositionX -= 5;
		ballPositionY += 5;
	} else if(xDirection == 1 && yDirection == -1){ // Move right, down
		ballPositionX += 5;
		ballPositionY += 5;
	}

}

	
