#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "events.h"

#define RED 82
#define GREEN 71
#define BLUE 66

#define LEDS 10

int leds[LEDS];

void printLEDs() {
	int i = 0;
	printf("\r");
	for(; i < LEDS; i++) {
		printf("[%c]", leds[i]);
	}
	printf("\n");	
	fflush(stdout);
	sleep(1);
}

void clearLEDs() {
	int i = 0;
	for(; i < LEDS; i++) {
		leds[i] = 32;
	}
}

void colorRed(unsigned from, unsigned to) {
	int i = from;
	for(;i <= to; i++) {
		leds[i] = RED;
	}
}


void colorBlue(unsigned from, unsigned to) {
	int i = from;
	for(;i <= to; i++) {
		leds[i] = BLUE;
	}
}


void WhoopWhoopSoundOfDaPolice(unsigned from, unsigned to) {
	int i = from;
	for(;i <= to; i++) {
		if(leds[i] == BLUE)
			leds[i] = RED;
		else 
			leds[i] = BLUE;
	}

	recallEvent(2);
}

int main() {
	//Setup LEDs
	clearLEDs();
	colorBlue(0, 4);
	colorRed(5,9);
	//Setup Event & register wvents
	initEvent(printLEDs);
	registerEvent(WhoopWhoopSoundOfDaPolice, 0, 4, 0);
	registerEvent(WhoopWhoopSoundOfDaPolice, 5, 9, 1);
	eventLoop();
	return 0;
}
