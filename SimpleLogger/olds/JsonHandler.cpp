/*
 * JsonHandler.cpp
 *
 *  Created on: Jun 24, 2019
 *      Author: suhaimi
 */

#include "JsonHandler.h"

//extern SpiffsHandler *spiffsHandler;
//extern ais Ais;
//extern board Board;
String JSONHopperCount = "0,1,2";
String JSONSonar = "Installed,Not installed";

JsonHandler::JsonHandler() {


}

//jDoc["i"] = "1";
//	jDoc["n"] = "UNKNOWN";
//	jDoc["j"] = "d";
//	jDoc["c"] = "I4N2IP.png";
//	jDoc["s"] = "";
//	jDoc["m"] = "A00000";
//	jDoc["e"] = "10800000";
//
//	JsonArray g = jDoc.createNestedArray("g");
//	g.add("ABCDEF");
////	g.add("G1OFJV");

void JsonHandler::processAdTech(String json)
{
	const char* e1;
	const char* m="";
	const char* gr;
	String g="";
	JsonArray groups;

	bool ada = false;
	DynamicJsonDocument jDoc(2048);
	deserializeJson(jDoc, json);
	JsonArray rows = jDoc["rows"];

	LocSpiff 	*locSpiff;
	locSpiff = new LocSpiff("processAdTech");


	for (JsonObject aRow : rows) {
		e1 = aRow["element"][0]["e1"][0];

		if (!strcmp(e1,"Machine ID")) {
			m = aRow["element"][1]["e2"][0];
			ada = true;
		}
		else if (!strcmp(e1, "Group")) {
			groups = aRow["element"][1]["e2"];
			ada = true;
		}
	}

	if (ada) {
		if (m && m[0]) {
			mID = m;
//			spiffsHandler->writeToSpiffs("m", m);
			locSpiff->writeFile("/m", m);
		}

		if (groups.size() > 0) {
			for (int i=0; i<groups.size(); i++) {
				gr = groups[i];
				g += gr;
				if (groups.size() > 1 && i < groups.size()-1) g += ",";
			}
			mGroup = g;
//			spiffsHandler->writeToSpiffs("g", g);
			locSpiff->writeFile("/g", g.c_str());
		}

	}

	delete locSpiff;


}

String JsonHandler::cariDanTukar(String data, char separator, String cari)
{
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;
  int ioff = -1;
  bool ada = false;
  String temp = "";

  for (int i = 0; i <= maxIndex; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
      ioff = data.substring(strIndex[0], strIndex[1]).indexOf(cari);
      if (ioff >= 0) {
        ada = true;
        break;
      }
    }
  }

  if (ada) {
    temp = data.substring(strIndex[0], strIndex[1]);
    data.remove(strIndex[1] > maxIndex ? strIndex[0] - 1 : strIndex[0] , temp.length() + 1 );
    temp += ",";
    temp += data;
  }

  return ada ? temp : data;
}

void JsonHandler::writeSdata(String key, String value)
{
	LocSpiff 	*locSpiff;
	locSpiff = new LocSpiff("writeSdata");


	DynamicJsonDocument jDoc(2048);
//	String conf = spiffsHandler->readFromSpiffs("sData");
	String conf = locSpiff->readFile("/sData");
	jDoc.clear();
	deserializeJson(jDoc, conf);
	jDoc[key] = value;
	conf="";
	serializeJson(jDoc, conf);
//	spiffsHandler->writeToSpiffs("sData", conf);
	locSpiff->writeFile("/sData", conf.c_str());

	delete locSpiff;
}

String JsonHandler::getSdata(String apa)
{
	String res = "";
	LocSpiff 	*locSpiff;
	locSpiff = new LocSpiff("writeSdata");


	DynamicJsonDocument jDoc(2048);
	deserializeJson(jDoc, locSpiff->readFile("/sData"));

	delete locSpiff;

	return jDoc[apa];
}

void JsonHandler::processSubmitJSON(String json)
{
	const char* e1;
	const char* e2;
	String minn="", maxx="", hopp="", sonn="", nn="";
	bool ada = false;
	DynamicJsonDocument jDoc(2048);

	LocSpiff 	*locSpiff;
	locSpiff = new LocSpiff("processSubmitJSON");


	deserializeJson(jDoc, json);
	JsonArray rows = jDoc["rows"];
	for (JsonObject aRow : rows) {
		e1 = aRow["element"][0]["e1"][0];
		e2 = aRow["element"][1]["e2"][0];
		if (!strcmp(e1,"Min")) {
			minn = e2;
			ada = true;
		}
		else if (!strcmp(e1, "Max")) {
			maxx = e2;
			ada = true;
		}else if (!strcmp(e1, "Hopper")) {
			hopp = e2;
			ada = true;
		}else if (!strcmp(e1, "Sonar")) {
			sonn = e2;
			ada = true;
		}else if (!strcmp(e1, "Name")) {
			nn = e2;
			ada = true;
		}
	}

	if (ada) {
		if (minn != "") {
			locSpiff->writeFile("/min", minn.c_str());
//			Ais.min = minn.toInt();
		}
		if (maxx != "") {
			locSpiff->writeFile("/max", maxx.c_str());
//			Ais.max = maxx.toInt();
		}
		if (hopp != "") {
			locSpiff->writeFile("/hopper", hopp.c_str());
//			Ais.hopperCount = hopp.toInt();
		}
		if (sonn != "") {
			if (sonn == "Installed") {
				sonn = "1";
			}else {
				sonn = "0";
			}
			locSpiff->writeFile("/sonar", sonn.c_str());
//			Ais.sonarCount = sonn.toInt();
		}
		if (nn != "") {
			locSpiff->writeFile("/n",	 nn.c_str());
			mName = nn;
		}
	}

	delete locSpiff;
}

DynamicJsonDocument JsonHandler::makeJson() {
	DynamicJsonDocument jDoc(2048);


	jDoc["m"] = mID; // XF7TDK
	jDoc["n"] = mName;
	jDoc["j"] = "d";

	JsonArray g = jDoc.createNestedArray("g");
	g.add("GF7TDK");
	g.add("G1OFJV");

	JsonObject r = jDoc.createNestedObject("r");
	r["i"] = "Update List";
	r["f"] = "##0.00";      //number format
	r["c"] = "RM";        //currency
	r["u"] = "0";       //currency

	JsonObject tab = jDoc.createNestedObject("tabs");
	tab["1"] = "Config";

	JsonArray rows = jDoc.createNestedArray("rows");

	tambahRow(rows, "1", "0", "Machine Detail", "");
//	tambahRow(rows, "1", "2", "MAC", Board.macID);
	tambahRow(rows, "1", "2", "MAC", "12122112121");
	tambahRow(rows, "1", "3", "Name", mName);

	tambahRow(rows, "1", "0", "Sonar Sensor", "");
	String son = "no";

//	if (Ais.sonarCount) {
//		son = "Installed";
//	}
//	else{
//		son = "Not installed";
//	}


	tambahRow(rows, "1", "7", "Sonar", cariDanTukar(JSONSonar, ',', son));
//	tambahRow(rows, "1", "4", "Min", String(Ais.min));
//	tambahRow(rows, "1", "4", "Max", String(Ais.max));
	tambahRow(rows, "1", "4", "Min", "1234");
	tambahRow(rows, "1", "4", "Max", "4321");

	tambahRow(rows, "1", "0", "Proximity Sensor", "");
//	tambahRow(rows, "1", "7", "Hopper", cariDanTukar(JSONHopperCount, ',', String(Ais.hopperCount)));
	tambahRow(rows, "1", "7", "Hopper", cariDanTukar(JSONHopperCount, ',', "2"));


	return jDoc;

}

DynamicJsonDocument JsonHandler::getInit()
{
	DynamicJsonDocument jDoc(2048);
	LocSpiff 	*locSpiff;
	locSpiff = new LocSpiff("processSubmitJSON");


//	deserializeJson(jDoc, spiffsHandler->readFromSpiffs("init"));
	deserializeJson(jDoc, locSpiff->readFile("/init"));

	delete locSpiff;

	return jDoc;
}

DynamicJsonDocument JsonHandler::makeInit() {
	DynamicJsonDocument jDoc(2048);

	jDoc["i"] = "1";
	jDoc["n"] = mName;
	jDoc["j"] = "d";
	jDoc["c"] = "I4N2IP.png";
	jDoc["s"] = "";
	jDoc["m"] = mID;
	jDoc["e"] = "10800000";

	JsonArray g = jDoc.createNestedArray("g");
	g.add("GF7TDK");
	g.add("G1OFJV");

	return jDoc;
}

void JsonHandler::tambahRow(JsonArray root, String tab, String type, String e1, String e2)
{
	String temp;
	int x;

	JsonObject r = root.createNestedObject();
	r["tab"] = tab;
	r["type"] = type;

	JsonArray element = r.createNestedArray("element");


	JsonObject e = element.createNestedObject();
	JsonArray arg1 = e.createNestedArray("e1");
	arg1.add(e1);

	JsonObject f = element.createNestedObject();
	JsonArray arg2 = f.createNestedArray("e2");

	if (e2.length() > 0) {
		x = 0;
		while (true) {
			temp = getValue(e2, ',', x);
			if (temp != "") {
				arg2.add(temp);
			}
			else break;
			x++;
		}
	}

}

String JsonHandler::getValue(String data, char separator, int index)
{
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = data.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++) {
		if (data.charAt(i) == separator || i == maxIndex) {
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}

	return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

