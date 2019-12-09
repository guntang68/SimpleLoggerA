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
#include <digitalize.h>
#include <graph.h>

class LocSonar {
private:
	bool 	enable;
	String 	_sonarRaw;
	float 	_getDistance(String str);
	double 	_sonarDistance;

	digitalize 		*digiSonar;
	graph			*graphSonar;

	int _getNumber();
public:
	LocSonar(int core, double Max, double Min);
	void PortSonar(bool select);
	static void loop(void * parameter);
	LocMQTT *siniLocMQTT;
	LocOLED *siniLocOLED;
	double GetDistance();
	bool done;

};

#endif /* OURS_LOCSONAR_H_ */
