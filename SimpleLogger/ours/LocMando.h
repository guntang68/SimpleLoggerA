/*
 * LocMando.h
 *
 *  Created on: Nov 6, 2019
 *      Author: annuar
 */

#ifndef OURS_LOCMANDO_H_
#define OURS_LOCMANDO_H_

#include "Arduino.h"
#include <LocMQTT.h>

#define MUX_S0 5
#define MUX_S1 17
#define MUX_S2 16

class LocMando {
private:
	void 	_processMandoData();
	String 	_mandoRaw;
	bool enable;
public:
	LocMando(int core);
	void PortMando(bool select);
	static void loop(void * parameter);
	String dapatMandoText;
	double LAT=0;
	double LNG=0;
	LocMQTT *siniLocMQTT;

};

#endif /* OURS_LOCMANDO_H_ */
