#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include "display.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>

/* Decleration of variables only to be used in this file*/
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
		for (int j = y - 5; j < y + 55; j++){
			if(j <= y || j >= y + 50){
				pixelValue[j*320 + i] = 0x0000;
			}else {
				pixelValue[j*320 + i] = 0xFF0F;
			}
		}
	}
}

void paintBall(int x, int y){
	for(int i = x - 5; i < x + 15; i++){
		for (int j = y-5; j < y + 15; j++){
			if((i < x || i >= x + 10) || (j < y || j >= y + 10)){
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
	/* Paddle 2 */
	paintRect(paddle2PositionX, paddle2Y);
	/* Ball */
	paintBall(ballX, ballY);
	/* Update display */	
	ioctl(fp, 0x4680, &rect); // updates the display
}

void newGameDisplay(){
	for(int i = 0; i < 41; i++){
		for(int j = 0; j < 241; j ++){
			pixelValue[j*320 + i] = 0x0000;
		}
	}
	
	for(int i = 269; i < 341; i++){
		for(int j = 0; j < 241; j ++){
			pixelValue[j*320 + i] = 0x0000;
		}
	}
}
