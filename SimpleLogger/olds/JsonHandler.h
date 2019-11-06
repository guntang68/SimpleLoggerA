/*
 * JsonHandler.h
 *
 *  Created on: Jun 24, 2019
 *      Author: suhaimi
 */

#ifndef JSONHANDLER_H_
#define JSONHANDLER_H_

#include "ArduinoJson.h"
//#include "SpiffsHandler.h"
//#include "ais.h"
#include <LocSpiff.h>
#include "Arduino.h"

class JsonHandler {
public:
	JsonHandler();
	DynamicJsonDocument makeInit();
	DynamicJsonDocument makeJson();
	void tambahRow(JsonArray root, String tab, String type, String e1, String e2);
	String getValue(String data, char separator, int index);
	void processSubmitJSON(String json);
	String getSdata(String apa);
	void writeSdata(String key, String value);
	String cariDanTukar(String data, char separator, String cari);
	void processAdTech(String json);
	DynamicJsonDocument getInit();

	String mID;
	String mName;
	String mGroup;
};

#endif /* JSONHANDLER_H_ */
