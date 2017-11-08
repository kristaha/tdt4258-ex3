#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
//#include <linux/types.h>
//#include <asm/fcntl.h>
//#include <linux/fcntl.h>

FILE* gamepad;

/* Prototypes */
int initGamepad();
void exitGamepad();
void saHandler(int signalNumber);
void movePaddle1(); 
void movePaddle2(); 
void moveBall();
//void hitPaddle1();
//void hitPaddle2();
int  checkIfScore();
void newGame(); 

/* Positions values */
int paddle1PositionY; // = 90;
int paddle2PositionY; // = 90;
int ballPositionX; // = 155;
int ballPositionY; // = 115;

/* Direction Values
Paddle directions:
paddleNDirection == 1 --> up, paddleNDirection == -1 --> down, else --> no movement
Ball directions:
xDirextion == 1 --> right, xDirection == -1 --> left
yDirection == 1 --> up, yDirection == 0 --> no change, yDirection == -1 down
*/
int paddle1Direction; // = 0;
int paddle2Direction; // = 0;
int ballDirectionY; // = 0;
int ballDirectionX; // = 1;
/*Structs*/
struct sigaction gameSigaction = {
	.sa_handler = saHandler, // ev med &
	 
};



int main(int argc, char *argv[])
{
	printf("Hello World, I'm game!\n");
	initDisplay();
	initGamepad();	
	newGame();	
	/* Display demo*/
	paddle1Direction = 1;
	paddle2Direction = 1;
	
	for(int i = 0; i < 300; i++){
		if(paddle1PositionY <= 10){
			paddle1Direction = -1;
			paddle2Direction = -1;
			ballDirectionX = -1;
		}else if(paddle1PositionY >= 190){
			paddle1Direction = 1;
			paddle2Direction = 1;
			ballDirectionX = 1;
		}
		movePaddle1();
		movePaddle2();
		moveBall();
		updateDisplay(paddle1PositionY, paddle2PositionY, paddle1Direction, paddle2Direction, ballPositionX, ballPositionY, ballDirectionX, ballDirectionY);
	}
	
	/* End Demo*/
	exit(EXIT_SUCCESS);
}

/* Initiation and handeling of the gamepad */
int initGamepad(){
	long oflags;

	gamepad = fopen("/dev/gamepad", "rb");

	if(!gamepad){
		printf("Unable to open driver\n");
		return EXIT_FAILURE;
	}
	if(sigaction(SIGIO, &gameSigaction, NULL) != 0){ // bruker sigaction siden signal er utdatert
		printf("Could not register a signal handler\n");
		return EXIT_FAILURE;	
	}	
	if(fcntl(fileno(gamepad), F_SETOWN, getpid()) != 0){
		printf("Could not set process as owner\n");
		return EXIT_FAILURE;
	}
	oflags = fcntl(fileno(gamepad), F_GETFL);		
	if(fcntl(fileno(gamepad), F_SETFL, oflags | FASYNC) != 0){
		printf("Could not set FASYNC flag\n");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

void exitGamepad(){
	fclose(gamepad);
}

void saHandler(int signalNumber){
	printf("%d", signalNumber);
	printf("SA handler");
	if(signalNumber == SIGIO){
	
	}	

}

/* Functions to move paddles and the ball */
void movePaddle1(){
	if(paddle1Direction == 1 && paddle1PositionY >= 0){
		paddle1PositionY -= 5;
	}else if (paddle1Direction == -1 && paddle1PositionY <= 190){
		paddle1PositionY += 5;
	}	
}
void movePaddle2(){	
	if(paddle2Direction == 1 && paddle2PositionY >= 0){
		paddle2PositionY -= 5;
	}else if (paddle2Direction == -1 && paddle2PositionY <= 190){
		paddle2PositionY += 5;
	}	
}

void moveBall(){
	if(ballDirectionX == 1 && ballDirectionY == 1){         // Move right, up
		ballPositionX += 5;
		ballPositionY -= 5;
	} else if(ballDirectionX == 1 && ballDirectionY == 0){  // Move right
		ballPositionX += 5;

	} else if(ballDirectionX == -1 && ballDirectionY == 1){  // Move left, up
		ballPositionX -= 5;
		ballPositionY -= 5;
	} else if(ballDirectionX == -1 && ballDirectionY == 0){  // Move left
		ballPositionX -= 5;
		
	} else if(ballDirectionX == -1 && ballDirectionY == -1){ // Move left, down
		ballPositionX -= 5;
		ballPositionY += 5;
	} else if(ballDirectionX == 1 && ballDirectionY == -1){ // Move right, down
		ballPositionX += 5;
		ballPositionY += 5;
	}

}

	
//void hitPaddle1();
//void hitPaddle2();
int  checkIfScore(){
	if(ballPositionX >= 330 || ballPositionX < 10){
		return 1;
	}
	return 0;
}
void newGame(){
	paddle1PositionY = 90;
	paddle2PositionY = 90;
	ballPositionX = 155;
	ballPositionY = 115;
	paddle1Direction = 0;
	paddle2Direction = 0;
	ballDirectionY = 0;
	ballDirectionX = 1;
	updateDisplay(paddle1PositionY, paddle2PositionY, paddle1Direction, paddle2Direction, ballPositionX, ballPositionY, ballDirectionX, ballDirectionY);
} 
