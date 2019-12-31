/*
 * LocOTA.h
 *
 *  Created on: Dec 10, 2019
 *      Author: annuar
 */

#ifndef OURS_LOCOTA_H_
#define OURS_LOCOTA_H_

#include 			"Arduino.h"
#include 			<LocSpiff.h>
#include			<WiFi.h>
#include 			<HTTPClient.h>
#include 			<ESP32httpUpdate.h>


//#include	"ESP32httpUpdate.h"

enum OTAStatus {
	ota_start = 10,
	ota_failed,
	ota_no_update,
	ota_update_ok
};


class LocOTA {
private:
	String _filename;
	String _macID;
	String _machineID;
public:
	LocOTA(String filename, int core);
	static void loop(void* parameter);
	String getMAC();
	String getMachineID(String mac);
	int Status;
	bool newFile();

};

#endif /* OURS_LOCOTA_H_ */
