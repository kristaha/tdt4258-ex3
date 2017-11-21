#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include "display.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>

/* Prototypes of functions only to be used in this file */
void paintPaddle(int x, int y);
void paintBall(int x, int y);
void drawToDisplay(int dx, int dy, int h, int w);

/* Decleration of variables only to be used in this file*/
struct fb_copyarea rect;
short* pixelValue;
int fb;
static int paddle1PositionX = 30;
static int paddle2PositionX = 280;

/* Initiate the display */
int initDisplay(){
	
	/* Get access to framebuffer device which represent the graphic memory */
	fb = open("/dev/fb0", O_RDWR);

	if(!fb){
		printf("Failure open fb\n");
		return 0;
	}
	printf("File successfully opened!\n");
	
	/* Memorymaps driver to array in the memory */
	pixelValue = mmap(NULL, 2*320*240, PROT_WRITE|PROT_READ, MAP_SHARED, fb, 0);
	
	/* Update display */
	ioctl(fb, 0x4680, &rect);
	return 1;
}

/* Writes values to display to view a rectangle with upper left corner at position (x,y) */
void paintPaddle(int x, int y){
	for(int i = x; i < x + 10; i++){
		for (int j = y - 10; j < y + 65; j++){
			if(j <= y || j >= y + 50){
				pixelValue[j*320 + i] = 0x0000;
			}else {
				pixelValue[j*320 + i] = 0xFF0F;
			}
		}
	}
	drawToDisplay(x, y - 22, 82, 10);
}

/* Writes values to display to view the ball with upper left corner at position (x,y) */
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
	drawToDisplay(x-6, y-6, 22, 22);
}

/* Updates the display to view the game state */
void updateDisplayPaddles(int paddle1PositionY, int paddle2PositionY){
	/* Paddle 1 */
	paintPaddle(paddle1PositionX, paddle1PositionY);	
	/* Paddle 2 */
	paintPaddle(paddle2PositionX, paddle2PositionY);
}

void updateDisplayBall(int ballPositionX, int ballPositionY){
	paintBall(ballPositionX, ballPositionY);
}
/* Specifies the area on the display we want to update */
void drawToDisplay(int dx, int dy, int h, int w) {
	rect.dx = dx;
	rect.dy = dy;
	rect.height = h;
	rect.width = w;

	ioctl(fb, 0x4680, &rect);	
}

/* Blackes out previous visualizations from last game 
   Called when a new game is initiated                */
void newGameDisplay(){
	for(int i = 0; i < 150; i++){
		for(int j = 0; j < 241; j ++){
			pixelValue[j*320 + i] = 0x0000;
		}
	}
	
	for(int i = 269; i < 341; i++){
		for(int j = 0; j < 241; j ++){
			pixelValue[j*320 + i] = 0x0000;
		}
	}
	drawToDisplay(0,0,240,320);
}

