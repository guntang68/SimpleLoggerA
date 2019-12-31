/*
 * ManOTA.h
 *
 *  Created on: Jul 1, 2019
 *      Author: annuar
 */

#ifndef MAN_MANOTA_H_
#define MAN_MANOTA_H_

#include	<LocSpiff.h>
#include	"Arduino.h"
#include	<WiFi.h>
#include 	<WebServer.h>
#include	"HTTPClient.h"
#include 	"TimeLib.h"
#include	"ESP32httpUpdate.h"

#include	"esp_task_wdt.h"


enum{
	manOTA_StartDownload=2,
	manOTA_EndDownload,
	manOTA_FailDownload
};

class ManOTA {

public:
	ManOTA();
	virtual ~ManOTA();
	static void setLoopDelay(int16_t masa);
	static void setFileLookUp(String filename);
	static void serverOTA(void * parameter);
};

#endif /* MAN_MANOTA_H_ */


///*
// * ManOTA.h
// *
// *  Created on: Jul 1, 2019
// *      Author: annuar
// */
//
//#ifndef MAN_MANOTA_H_
//#define MAN_MANOTA_H_
//
//#include	<Spiff.h>
//#include	"Arduino.h"
//#include	<WiFi.h>
//#include 	<WebServer.h>
//#include	"HTTPClient.h"
//#include 	"TimeLib.h"
//#include	"ESP32httpUpdate.h"
//#include	"han.h"
////#include 	"esp_task_wdt.h"
//#include	"esp_task_wdt.h"
//
//
//enum{
//	manOTA_StartDownload=2,
//	manOTA_EndDownload,
//	manOTA_FailDownload
//};
//
//class ManOTA {
//
//public:
//	ManOTA();
//	virtual ~ManOTA();
//	static void setLoopDelay(int16_t masa);
//	static void setFileLookUp(String filename);
//	static void serverOTA(void * parameter);
//};
//
//#endif /* MAN_MANOTA_H_ */
