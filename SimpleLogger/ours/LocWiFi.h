/*
 * LocWiFi.h
 *
 *  Created on: Oct 13, 2019
 *      Author: annuar
 */

#ifndef OURS_LOCWIFI_H_
#define OURS_LOCWIFI_H_

#include "Arduino.h"
#include <LocSpiff.h>



enum{
	lw_wifi_sta = 40,
	lw_wifi_ap,
	lw_wifi_apsta,
	lw_wifi_off,
	lw_wifi_is_sta,
	lw_wifi_is_ap,
	lw_wifi_is_apsta,
	lw_wifi_is_off
};

class LocWiFi {
private:
	void _openConnection(int as);
	std::vector<std::vector<String>> _sList;
	int 	_loopDelay;
	int * 	_lookVal;
public:
	LocWiFi(int core, int loopDelay, int *lookVal);
	virtual ~LocWiFi();
	static void loop(void * parameter);
};

#endif /* OURS_LOCWIFI_H_ */
