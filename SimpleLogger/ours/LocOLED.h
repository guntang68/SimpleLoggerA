/*
 * LocOLED.h
 *
 *  Created on: Nov 6, 2019
 *      Author: annuar
 */

#ifndef OURS_LOCOLED_H_
#define OURS_LOCOLED_H_

#include "Arduino.h"
#include <Wire.h>
#include <SSD1306Spi.h>
#include <TimeLib.h>
#include <OLEDDisplayUi.h>
#include "images.h"
#include <WiFi.h>


//OLED Display pin
#define pOutDRes 25
#define pOutDDC 26
#define pOutDCS 27

const int screenW = 128;
const int screenH = 64;

const int clockCenterX = screenW/2;
const int clockCenterY = ((screenH-16)/2)+16;   // top yellow part is 16 px height
const int clockRadius = 23;

const int spacing = 12;

typedef struct{
	double bp;
	double at;
	double ws;
	double wd;
} airmarReading_t;


class LocOLED {
private:
	bool _pause;
public:
	LocOLED(int core);
	static void loop(void * parameter);
	void setProgress(double value);
	double sonarDistance;
	bool done();
	void pause(bool pause);
	void setDisplay(int set);
	std::vector<airmarReading_t> readingList;
	airmarReading_t reading;

};

#endif /* OURS_LOCOLED_H_ */
