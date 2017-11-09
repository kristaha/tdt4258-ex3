#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include "display.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>

/* Prototypes for funtions only to be used in this file */
void blackOutAfterPaddle(int x, int y, int direction);
void blackOutAfterBall(int xPosition, int yPosition, int xDirection, int yDirection);

struct fb_copyarea rect;
short* pixelValue;
int fp;

static int paddle1PositionX = 30;
static int paddle2PositionX = 280;
int initDisplay(){

	rect.dx = 0;
	rect.dy = 0;
	rect.width = 320;
	rect.height = 240;

	fp = open("/dev/fb0", O_RDWR);

	if(!fp){
		printf("Failure open fb");
		return 0;
	}

	printf("File successfully opened!\n");
//	printf("%d\n ", write(fp, initDisplay, 2*320*240));
	pixelValue = mmap(NULL, 2*320*240, PROT_WRITE|PROT_READ, MAP_SHARED, fp, 0);
	printf("%p\n", pixelValue);
	paintRect(paddle1PositionX,90);
	paintRect(paddle2PositionX,90);
	paintBall(155, 110);
	ioctl(fp, 0x4680, &rect);

	return 1;
}

void paintRect(int x, int y){
	for(int i = x; i < x + 10; i++){
		for (int j = y; j < y + 50; j++){
			pixelValue[j*320 + i] = 0xFF0F;
		}
	}
}

void paintBall(int x, int y){
	for(int i = x -5; i < x + 15; i++){
		for (int j = y-5; j < y + 15; j++){
			if((i < x || i > x+10) && (j < y || j > y +10 )){
				pixelValue[j*320 + i] = 0x0000;
			}else {
				pixelValue[j*320 + i] = 0x5555;
			}
		}
	}
}

void updateDisplay(int paddle1Y, int paddle2Y, int paddle1Direction, int paddle2Direction, int ballX, int ballY, int ballDirectionX, int ballDirectionY){
	/* Paddle 1 */
	paintRect(paddle1PositionX, paddle1Y);	
	blackOutAfterPaddle(paddle1PositionX, paddle1Y, paddle1Direction);
	/* Paddle 2 */
	paintRect(paddle2PositionX, paddle2Y);
	blackOutAfterPaddle(paddle2PositionX, paddle2Y, paddle2Direction);
	/* Ball */
	paintBall(ballX, ballY);
	//blackOutAfterBall(ballX, ballY, ballDirectionX, ballDirectionY);
	
	ioctl(fp, 0x4680, &rect); // updates the display
}

void blackOutAfterPaddle(int x, int y, int direction){ // direction == 1 --> up, direction == -1 --> down
	if(direction == -1){
		for(int i = x; i < x + 10; i++){
			for(int j = y; j < y + 5; j++){
				pixelValue[j*320 + i] = 0x0000;
			}
		}
	}else if (direction == 1){
		for(int i = x; i < x + 10; i++){
			for(int j = y - 5; j < y; j++){
				pixelValue[(j+50)*320 + i] = 0x0000;
			}
		}
	}
}

void blackOutAfterBall(int xPosition, int yPosition, int xDirection, int yDirection){
	if(xDirection == 1 && yDirection == 0){	// Only right
		for(int i = xPosition - 5; i < xPosition; i++){
			for(int j = yPosition; j < yPosition + 10; j++){
				pixelValue[j*320 + i] = 0x0000;
			}
		}
	}else if (xDirection == -1 && yDirection == 0){	//Only left 
		for(int i = xPosition + 10; i < xPosition + 15; i++){
			for(int j = yPosition; j < yPosition + 10; j++){
				pixelValue[j*320 + i] = 0x0000;
			}
		}	
	}else if (xDirection == 1 && yDirection == 1){
		for(int i = xPosition - 5; i < xPosition + 10; i++){
			for(int j = yPosition-5; j < yPosition + 10; j++){
				pixelValue[j*320 + i] = 0x0000;
			}
		}
	}else if (xDirection == 1 && yDirection == -1){
		for(int i = xPosition; i < xPosition + 5; i++){
			for(int j = yPosition; j < yPosition + 10; j++){
				pixelValue[j*320 + i] = 0x0000;
			}
		}
	}else if (xDirection == -1 && yDirection == 1){

	}else if (xDirection == -1 && yDirection == -1){

	}



}
