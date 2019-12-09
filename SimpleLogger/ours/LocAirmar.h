/*
 * LocAirmar.h
 *
 *  Created on: Nov 12, 2019
 *      Author: annuar
 */

#ifndef OURS_LOCAIRMAR_H_
#define OURS_LOCAIRMAR_H_

#include 			"Arduino.h"
#include 			<ManNMEA.h>
#include 			<LocOLED.h>

//typedef struct{
//	double bp;
//	double at;
//	double ws;
//	double wd;
//} airmarReading_t;



class LocAirmar {
private:
	String 	_airmarRaw;
	void _processAirmarData();
	std::vector<airmarReading_t> readingList;
	airmarReading_t reading;
	airmarReading_t _processed;
public:
	LocAirmar(int core);
	static void loop(void* parameter);
	airmarReading_t GetReading();
	LocOLED *siniLocOLED;
};

#endif /* OURS_LOCAIRMAR_H_ */
