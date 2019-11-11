/*
 * LocDirectOTA.h
 *
 *  Created on: Nov 1, 2019
 *      Author: annuar
 */

#include	"Arduino.h"
#include	<WiFi.h>
#include	<WebServer.h>
#include	<ESPmDNS.h>
#include	<JsonHandler.h>
#include	<pages.h>
#include	<LocSpiff.h>
#include	<Update.h>
#include	<LocMando.h>
#include	<TimeLib.h>


#ifndef OURS_LOCDIRECTOTA_H_
#define OURS_LOCDIRECTOTA_H_

class LocDirectOTA {
private:
	int 	_loopDelay;
	int * 	_lookVal;
//	static bool	_nine();
	static void _handleConfDDMS();
	static void _StatusViaWiFi(void);
	static bool _adTech();
	static void _clearFile();


public:
	LocDirectOTA(int core, int loopDelay, int *lookVal);
	static void loop(void * parameter);
	String getMAC();
	LocMando *siniLocMando;



};

#endif /* OURS_LOCDIRECTOTA_H_ */
