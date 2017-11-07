//#ifdef _DISPLAY_H
//#define _DISPLAY_H	1

int initDisplay();
void paintRect(int x, int y);
void paintBall(int x, int y);
void updateDisplay(int paddle1Y, int paddle2Y, int direction, int ballX, int ballY, int ballDirectionX, int ballDirectionY);
//#endif
