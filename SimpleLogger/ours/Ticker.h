/*
 * Ticker.h
 *
 *  Created on: Dec 2, 2019
 *      Author: annuar
 */

#ifndef OURS_TICKER_H_
#define OURS_TICKER_H_

#include "Arduino.h"

class Ticker {
private:
	unsigned long _gap;
	unsigned long _lock;
public:
	bool set;
	Ticker(unsigned long gap, unsigned long lock){
		this->_gap = gap;
		this->_lock = lock;
		this->set = false;
	}
	bool Update(){
		bool ret = false;
		if((millis() - this->_lock) >= this->_gap){
			this->_lock = millis();
			ret = true;
			this->set = true;
		}
		return ret;
	}

	void Reset(){
		this->_lock = millis();
		this->set = false;
	}

};

#endif /* OURS_TICKER_H_ */


//if((millis()-gtickNyamukTime) >= 60000){
//	gtickNyamukTime = millis();
//	gtockBeat = !gtockBeat;
//	locMqtt->hantar(gMAC, gtockBeat?"1":"0");
//}
