#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

FILE* gamepad;

/* Prototypes */
int initGamepad();
void exitGamepad();
void saHandler(int signalNumber);
void mapInput(int input);
void movePaddle1(); 
void movePaddle2(); 
void moveBall();
void hitPaddle1();
void hitPaddle2();
void hitEdges();
int  checkIfScore();
void newGame(); 

/* Positions values */
int paddle1PositionY; 
int paddle2PositionY; 
int ballPositionX; 
int ballPositionY; 

/* **Direction Values**
*Paddle directions*
paddleNDirection == 1 --> north, paddleNDirection == -1 --> south, else --> no movement

*Ball directions*
xDirection == 1 --> east, xDirection == -1 --> west
yDirection == 1 --> north, yDirection == 0 --> no change, yDirection == -1 south
*/
int paddle1Direction; 
int paddle2Direction; 
int ballDirectionY; 
int ballDirectionX;

/*Structs*/
struct sigaction gameSigaction = {
	.sa_handler = saHandler, 
};


int main(int argc, char *argv[]){
	initDisplay();
	initGamepad();	
	newGame();	
	int a = 1;
	while(a == 1){
		if (checkIfScore() == 1){
			newGame();
		}else{
			if(ballPositionX <= 50 || ballPositionX >= 260){
				hitPaddle1();
				hitPaddle2();
				updateDisplayPaddles(paddle1PositionY, paddle2PositionY);
			}
			if(ballPositionY <= 20 || ballPositionY >= 210){
				hitEdges();
			}
			moveBall();
			updateDisplayBall( ballPositionX, ballPositionY);
		}
	usleep(33000);
	}
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
	if(sigaction(SIGIO, &gameSigaction, NULL) != 0){ 
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

/* Signal handler */
void saHandler(int signalNumber){
	int input;
	if(signalNumber == SIGIO){
//		input = fgetc(gamepad);
		read(fileno(gamepad), &input, 1);
		printf("%d\n", input);
		mapInput(input);
		movePaddle1();
		movePaddle2();	
		updateDisplayPaddles(paddle1PositionY, paddle2PositionY);
	}	

}

void mapInput(int input) {
	switch(input){
		case 255: // No input 
		paddle1Direction = 0;
		paddle2Direction = 0;
		break;

		case 253: // SW 2 pressed
		paddle1Direction = 1;
		paddle2Direction = 0;
		break;
		
		case 247: // SW 4 pressed
		paddle1Direction = -1;
		paddle2Direction = 0;
		break;

		case 223: // SW 6 pressed
		paddle1Direction = 0;
		paddle2Direction = 1;	
		break;

		case 127: // SW 8 pressed
		paddle1Direction = 0;
		paddle2Direction = -1;
		break;

		case 221: // SW 2 and SW 6 pressed
		paddle1Direction = 1;
		paddle2Direction = 1;
		break;	

		case 215: // SW 4 and SW 6 pressed
		paddle1Direction = -1;
		paddle2Direction = 1;
		break;

		case 125: // SW 2 and SW 8 pressed
		paddle1Direction = 1;
		paddle2Direction = -1;
		break;

		case 119: // SW 4 and SW 8 pressed
		paddle1Direction = -1;
		paddle2Direction = -1;
		break;	
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
	if(ballDirectionX == 1 && ballDirectionY == 1){         // Move northeast
		ballPositionX += 5;
		ballPositionY -= 5;
	} else if(ballDirectionX == 1 && ballDirectionY == 0){  // Move east
		ballPositionX += 5;

	} else if(ballDirectionX == -1 && ballDirectionY == 1){  // Move northwest
		ballPositionX -= 5;
		ballPositionY -= 5;
	} else if(ballDirectionX == -1 && ballDirectionY == 0){  // Move west
		ballPositionX -= 5;
		
	} else if(ballDirectionX == -1 && ballDirectionY == -1){ // Move sothwest
		ballPositionX -= 5;
		ballPositionY += 5;
	} else if(ballDirectionX == 1 && ballDirectionY == -1){ // Move southeast
		ballPositionX += 5;
		ballPositionY += 5;
	}

}

/* Functions that determines if paddles or edges of the screen is hit */	
void hitPaddle1(){
	if(ballPositionY >= (paddle1PositionY - 10) && ballPositionY <= paddle1PositionY + 15 && ballPositionX <= 42){
		ballDirectionX = 1;
		ballDirectionY = 1;
	}else if(ballPositionY > (paddle1PositionY + 15) && ballPositionY < paddle1PositionY + 35 && ballPositionX <= 42){
		ballDirectionX = 1;
		ballDirectionY = 0;
	}else if(ballPositionY >= (paddle1PositionY + 35) && ballPositionY <= paddle1PositionY + 50 && ballPositionX <= 42){
		ballDirectionX = 1;
		ballDirectionY = -1;
	}
}
void hitPaddle2(){
	if(ballPositionY >= (paddle2PositionY - 10) && ballPositionY <= paddle2PositionY + 15 && ballPositionX >= 268){
		ballDirectionX = -1;
		ballDirectionY = 1;
	}else if(ballPositionY > (paddle2PositionY + 15) && ballPositionY < paddle2PositionY + 35 && ballPositionX >= 268){
		ballDirectionX = -1;
		ballDirectionY = 0;
	}else if(ballPositionY >= (paddle2PositionY + 35) && ballPositionY <= paddle2PositionY + 50 && ballPositionX >= 268){
		ballDirectionX = -1;
		ballDirectionY = -1;
	}
}
void hitEdges(){
	if(ballPositionY <= 15){
		ballDirectionY = -1;
	}else if(ballPositionY >= 215){
		ballDirectionY = 1;
	}
}

/* Checks if the ball has moved passed the x position of any of the paddles. This is the definition of win in Pong*/
int  checkIfScore(){
	if(ballPositionX >= 315 || ballPositionX < 25){
		return 1;
	}
	return 0;
}

/* Initiates a new game with paddles and the ball in their starting positions */
void newGame(){
	paddle1PositionY = 90;
	paddle2PositionY = 90;
	ballPositionX = 155;
	ballPositionY = 110;
	paddle1Direction = 0;
	paddle2Direction = 0;
	ballDirectionY = 0;
	ballDirectionX = 1;
	newGameDisplay();
	updateDisplayPaddles(paddle1PositionY, paddle2PositionY);
	updateDisplayBall(ballPositionX, ballPositionY);
} 
