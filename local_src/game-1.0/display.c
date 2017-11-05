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
	paintBall(155, 115);
	ioctl(fp, 0x4680, &rect);

	return 1;
}

void paintRect(int x, int y){
	//printf("I am a rectangle");
	for(int i = x; i < x + 10; i++){
		for (int j = y; j < y + 50; j++){
			pixelValue[j*320 + i] = 0xFF0F;
		}
	}
}

void paintBall(int x, int y){
	//printf("I am a ball");
	for(int i = x; i < x + 10; i++){
		for (int j = y; j < y + 10; j++){
			pixelValue[j*320 + i] = 0x5555;
		}
	}
}

void updateDisplay(int paddle1Y, int paddle2Y, int direction){
	paintRect(paddle1PositionX, paddle1Y);	//Paddle 1
	blackOutAfterPaddle(paddle1PositionX, paddle1Y, direction);
	paintRect(paddle2PositionX, paddle2Y);	//Paddle 2
	blackOutAfterPaddle(paddle2PositionX, paddle2Y, direction);
	
	ioctl(fp, 0x4680, &rect); // updates the display
}

void blackOutAfterPaddle(int x, int y, int direction){ // direction == 1 --> up, direction == 0 --> down
	if(direction == 0){
		for(int i = x; i < x + 10; i++){
			pixelValue[y*320 + i] = 0x0000;
		}
	}else if (direction == 1){
		for(int i = x; i < x + 10; i++){
			pixelValue[(y+50)*320 + i] = 0x0000;
		}
	}
}
