//-------| src/dot.c |-------//
#include "dot.h"


static unsigned short dot_hexadecimal[16][MAX_DOT] = {
	{0x7F, 0x41, 0x41, 0x41, 0x7F}, // 0
	{0x00, 0x00, 0x7F, 0x00, 0x00}, // 1
	{0x4F, 0x49, 0x49, 0x49, 0x79}, // 2
	{0x49, 0x49, 0x49, 0x49, 0x7F}, // 3
	{0x78, 0x08, 0x08, 0x7F, 0x08}, // 4
	{0x79, 0x49, 0x49, 0x49, 0x4F}, // 5
	{0x7F, 0x49, 0x49, 0x49, 0x4F}, // 6
	{0x40, 0x40, 0x40, 0x40, 0x7F}, // 7
	{0x7F, 0x49, 0x49, 0x49, 0x7F}, // 8
	{0x78, 0x48, 0x48, 0x48, 0x7F}, // 9
	{0x1F, 0x28, 0x48, 0x28, 0x1F}, // A
	{0x7F, 0x49, 0x49, 0x49, 0x36}, // B
	{0x3E, 0x41, 0x41, 0x41, 0x22}, // C
	{0x7F, 0x41, 0x41, 0x41, 0x3E}, // D
	{0x7F, 0x49, 0x49, 0x49, 0x49}, // E
	{0x7F, 0x48, 0x48, 0x48, 0x48}  // F
};

static short * dot[MAX_DOT];


void init_dot(short * address[]) {
	int i;
	for( i=0; i<MAX_DOT; i++ ) {
		dot[i] = address[i];
	}
}

void dot_clear() {
	int i;
	for(i=0; i<MAX_DOT; i++){
		*dot[i] = 0;
	}
	usleep(0); // for Ximulator
}

void dot_write(int number) {
	int i;
	for(i=0; i<MAX_DOT; i++){
		*dot[i] = dot_hexadecimal[number][i];
	}
	usleep(0); // for Ximulator
}

void dot_arrow() {
	*dot[0] = 0x04;
	*dot[1] = 0x04;
	*dot[2] = 0x49;
	*dot[3] = 0x2A;
	*dot[4] = 0x1B;
}
