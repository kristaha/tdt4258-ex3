#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include "display.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>

struct fb_copyarea rect;
short* pixelValue;


int initDisplay(){

	rect.dx = 0;
	rect.dy = 0;
	rect.width = 320;
	rect.height = 240;

	int fp = open("/dev/fb0", O_RDWR);

	if(!fp){
		printf("Failure open fb");
		return 0;
	}

	printf("File successfully opened!\n");
//	printf("%d\n ", write(fp, initDisplay, 2*320*240));
	pixelValue = mmap(NULL, 2*320*240, PROT_WRITE|PROT_READ, MAP_SHARED, fp, 0);
	printf("%p\n", pixelValue);
	paintRect(30,90);
	paintRect(280,90);
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
