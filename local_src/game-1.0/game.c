#include <stdio.h>
#include <stdlib.h>
#include "display.h"

int initGamepad();

int main(int argc, char *argv[])
{
	printf("Hello World, I'm game!\n");
	initDisplay();
	exit(EXIT_SUCCESS);
}

int initGamepad(){
	gamepad = fopen("/dev/gamepad", "rb");
	if(!gamepad){
		return EXIT_FAILURE;
	}
}
