/*
 * ManNMEA.h
 *
 *  Created on: Jul 29, 2019
 *      Author: annuar
 */

#ifndef MAN_MANNMEA_H_
#define MAN_MANNMEA_H_

#include			"Arduino.h"

class ManNMEA {
public:
	ManNMEA();
	virtual ~ManNMEA();
	bool ValidCheckSum(String word);
	int checksumNmea(String data);
	String extractCol(String data, int index);
	double toDegree(String snum);
};

#endif /* MAN_MANNMEA_H_ */
