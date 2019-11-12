/*
 * LocSonar.h
 *
 *  Created on: Nov 7, 2019
 *      Author: annuar
 */

#ifndef OURS_LOCSONAR_H_
#define OURS_LOCSONAR_H_

#include "Arduino.h"
#include <LocMQTT.h>
#include <LocOLED.h>
#include <pins.h>

class LocSonar {
private:
	bool 	enable;
	String 	_sonarRaw;
	float 	_getDistance(String str);
	double 	_sonarDistance;
public:
	LocSonar(int core);
	void PortSonar(bool select);
	static void loop(void * parameter);
	LocMQTT *siniLocMQTT;
	LocOLED *siniLocOLED;
	bool done;

};

#endif /* OURS_LOCSONAR_H_ */
