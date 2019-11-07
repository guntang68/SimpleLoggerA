/*
 * LocDirectOTA.cpp
 *
 *  Created on: Nov 1, 2019
 *      Author: annuar
 */

#include <LocDirectOTA.h>
#include <WebServer.h>


LocDirectOTA	*iniDirectOTA;
TaskHandle_t 	loopDirectOTA= NULL;



WebServer server(80);
JsonHandler *jsonHandler;




LocDirectOTA::LocDirectOTA(int core, int loopDelay, int *lookVal)
{



	iniDirectOTA = this;
	iniDirectOTA->_lookVal = lookVal;
	iniDirectOTA->_loopDelay = loopDelay;

	iniDirectOTA->siniLocMando = NULL;

	jsonHandler = new JsonHandler;

	xTaskCreatePinnedToCore(iniDirectOTA->loop, "loopDirectOTA", 3072, NULL, 1, &loopDirectOTA, core);
}

void LocDirectOTA::loop(void* parameter) {
	log_i("require init");
	while(true){
		if((WiFi.getMode() == WIFI_MODE_AP) || (WiFi.getMode() == WIFI_MODE_APSTA)){
			if( !WiFi.softAPIP().toString().equals("0.0.0.0")){
				break;
			}
		}
		log_i("NO AP");
		delay(iniDirectOTA->_loopDelay);
	}

	log_i("Local softAPIP = %s", WiFi.softAPIP().toString().c_str());

	MDNS.begin("nine");
	server.on("/stat", WebServer::THandlerFunction(iniDirectOTA->_StatusViaWiFi));

	server.on("/adTech", WebServer::THandlerFunction(iniDirectOTA->_adTech));

	server.on("/reboot", HTTP_GET, WebServer::THandlerFunction([]() {
		server.sendHeader("Connection", "close");
		server.send(200, "text/html", "done");
		delay(1000);
		ESP.restart();
		}));

	server.on("/clearfs", WebServer::THandlerFunction([]() {
		iniDirectOTA->_clearFile();
		iniDirectOTA->_adTech();
	}));

	server.on("/config", WebServer::THandlerFunction(iniDirectOTA->_handleConfDDMS));

	server.on("/ota", HTTP_GET, WebServer::THandlerFunction([]() {
		server.sendHeader("Connection", "close");
		server.send(200, "text/html", ota);}));

	server.on("/update", HTTP_POST, WebServer::THandlerFunction([]() {
		server.sendHeader("Connection", "close");
		server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
		delay(1000);
		ESP.restart();}),
			WebServer::THandlerFunction([]() {
			HTTPUpload& upload = server.upload();
			if (upload.status == UPLOAD_FILE_START) {
				Serial.printf("Update: %s\n", upload.filename.c_str());
				if (!Update.begin(UPDATE_SIZE_UNKNOWN,U_FLASH,2,1)) { //start with max available size
					Update.printError(Serial);
				}
			}
			else if (upload.status == UPLOAD_FILE_WRITE) {
				/* flashing firmware to ESP*/
				if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
					Update.printError(Serial);
				}
			}
			else if (upload.status == UPLOAD_FILE_END) {
				if (Update.end(true)) { //true to set the size to the current progress
					Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
				}
				else {
					Update.printError(Serial);
				}
			}
		}));


	server.begin();

	while(true){
		server.handleClient();
		delay(iniDirectOTA->_loopDelay);
	}
}

void LocDirectOTA::_handleConfDDMS()
{
	String msg = inputB;
	if (server.args()==0) {
		msg.replace("[minn]", "66655");
		msg.replace("[maxx]", "33333");
		msg.replace("[note]", "");
	}
	else {
		if (server.arg(0)==0 || server.arg(1)==0) {
			msg.replace("[minn]", server.arg(0));
			msg.replace("[maxx]", server.arg(1));
			msg.replace("[note]", "please fill in the blank(s)");
		}
		else {
			msg.replace("[minn]", server.arg(0));
			msg.replace("[maxx]", server.arg(1));
			msg.replace("[note]", "Saved");
		}
	}
	server.send(200, "text/html", msg);
}

void LocDirectOTA::_StatusViaWiFi(void) {
	String msg="";

	long rssi = WiFi.RSSI();

	msg  = "MAC = " + iniDirectOTA->getMAC() + "</br>";
	msg += "IP = " + WiFi.localIP().toString() + "</br>";
	msg += "Live = " + String(millis()/1000) + "</br>";
	msg += "Time = xxx</br>";
	msg += "RSSI = " + String(rssi) + "</br>";
	msg += "<hr>";

	msg += iniDirectOTA->siniLocMando->dapatMandoText;


	iniDirectOTA->siniLocMando->dapatMandoText = "";

	server.sendHeader("Connection", "close");
	server.send(200, "text/html", msg);

	delay(50);
}

bool LocDirectOTA::_adTech() {
	LocSpiff 	*locSpiff;
	locSpiff = new LocSpiff("_adTech");


	String msg = inputA;
	if (server.args()==0) {
		msg.replace("[note]", "");
		msg.replace("[mID]", jsonHandler->mID);
		msg.replace("[mac]", iniDirectOTA->getMAC());
	}
	else {
		if (server.argName(0) == "clear") {
//			spiffsHandler->mulaFiles(true);
			locSpiff->mulaFiles(true);
			msg.replace("[mac]", iniDirectOTA->getMAC());
			msg.replace("[note]", "FS Cleared");
			msg.replace("[mID]", jsonHandler->mID);
		} else if (server.arg(0).length() < 6) {
			msg.replace("[mac]", iniDirectOTA->getMAC());
			msg.replace("[note]", "Require 6 digit alphanumeric id");
			msg.replace("[mID]", server.arg(0));
		}
		else {
//			spiffsHandler->writeToSpiffs("m", server.arg(0));
			locSpiff->writeFile("/m", server.arg(0).c_str());


			jsonHandler->mID = server.arg(0);
			msg.replace("[mac]", iniDirectOTA->getMAC());
			msg.replace("[note]", "Configuration sent.");
			msg.replace("[mID]", server.arg(0));
		}
	}
	delete locSpiff;

	server.send(200, "text/html", msg);

	return true;
}

void LocDirectOTA::_clearFile() {
	LocSpiff 	*locSpiff;
	locSpiff = new LocSpiff("_clearFile");

	locSpiff->mulaFiles(true);


	delete locSpiff;
}

String LocDirectOTA::getMAC() {
	uint8_t mac[6];
	WiFi.macAddress(mac);

	String macID = "";


	uint8_t num;
	for(int i=0; i < 5; i++){
		num = mac[i];
		if(num < 16) macID += "0";
		macID += String(mac[i], HEX) + ":";
	}
	macID += String(mac[5], HEX);



	macID.toUpperCase();

	return macID;
}

