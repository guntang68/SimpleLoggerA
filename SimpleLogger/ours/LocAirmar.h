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

typedef struct{
	double bp;
	double at;
	double ws;
	double wd;
} airmarReading_t;



class LocAirmar {
private:
	String 	_airmarRaw;
	void _processAirmarData();
public:
	LocAirmar(int core);
	static void loop(void* parameter);
};

#endif /* OURS_LOCAIRMAR_H_ */
