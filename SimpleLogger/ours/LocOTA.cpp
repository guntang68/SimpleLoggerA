/*
 * LocOTA.cpp
 *
 *  Created on: Dec 10, 2019
 *      Author: annuar
 */

#include <LocOTA.h>
//#include <WiFiClient.h>


LocOTA			*iniLocOTA;
TaskHandle_t 	loopLocOTA= NULL;

LocSpiff		*iniLocSpiff;

HTTPClient		http;


bool LocOTA::newFile() {
	bool ret = false;
	String payload="", tsFile="";
	bool sama = true;

	log_i("mula++++++");
	if(!WiFi.isConnected()){
		log_i("No wifi");
		return ret;
	}

	http.begin("http://172.105.117.29/nbe/update.php?file=" + iniLocOTA->_filename + "&m=" + iniLocOTA->_machineID);

	int httpCode = http.GET();

	log_i("=== %d", httpCode);
	if(httpCode == 200){
		payload = http.getString();	//533662722

		log_i("apa? %s", payload.c_str());
		if(payload.length() > 9){
			iniLocSpiff = new LocSpiff("LocOTA::newFile()");
			tsFile = iniLocSpiff->readFile("/latestUpdate.txt");
			log_i("tsFile %s", tsFile.c_str());

			delete iniLocSpiff;

			if(payload != tsFile){
				sama = false;
				log_i("Tak Sama");
			}
		}
	}
	http.end();

	if(!sama){
		esp_task_wdt_reset();
		ESPhttpUpdate.rebootOnUpdate(false);

		iniLocOTA->Status = ota_start;

		t_httpUpdate_return ret = ESPhttpUpdate.update("http://172.105.117.29/nbe/" + iniLocOTA->_filename + "?m=" + iniLocOTA->_machineID);

		switch(ret) {
			case HTTP_UPDATE_FAILED:
//				Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
//				ManOTA_events(manOTA_EndDownload);
				iniLocOTA->Status = ota_failed;
				delay(1000);
				break;

			case HTTP_UPDATE_NO_UPDATES:
//				Serial.println("HTTP_UPDATE_NO_UPDATES");
				iniLocOTA->Status = ota_no_update;
				delay(1000);
				break;

			case HTTP_UPDATE_OK:

				http.begin("http://172.105.117.29/nbe/update.php?file=" + iniLocOTA->_filename + "&m=" + iniLocOTA->_machineID + "&s=ok"); 												//Specify the URL
				http.GET();
				http.end(); //Free the resources

//				Serial.println("HTTP_UPDATE_OK");
				iniLocOTA->Status = ota_update_ok;

				iniLocSpiff = new LocSpiff("LocOTA::newFile() HTTP_UPDATE_OK");
				if(iniLocSpiff->writeFile("/latestUpdate.txt", payload.c_str())){
					log_i("Upadeted into log");
				}
				else{
					log_i("fail to update log");
				}
				delete iniLocSpiff;

//				ManOTA_events(manOTA_EndDownload);
				delay(1000);
				ESP.restart();
				break;
		}




	}




	return ret;
}

void LocOTA::loop(void* parameter) {

//	delay(30000);

	while(true){


		iniLocOTA->getMachineID(iniLocOTA->_macID);

		iniLocOTA->newFile();
		log_i("try tengok");



		delay(15000);
	}
}

String LocOTA::getMAC() {
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

String LocOTA::getMachineID(String mac) {
	String mID="";


	if(iniLocOTA->_machineID.length() == 6){
		log_i("-----------1");
		mID = iniLocOTA->_machineID;
		return mID;
	}

	iniLocSpiff = new LocSpiff("LocOTA::getMachineID");
	mID = iniLocSpiff->readFile("/machineid.txt");
	if(mID.length() == 6){
		iniLocOTA->_machineID = mID;
		delete iniLocSpiff;
		log_i("-----------2");
		return mID;
	}
	delete iniLocSpiff;

	http.begin("http://172.105.117.29/nbe/gen.php?mac=" + mac);
	int httpCode = http.GET();
	if(httpCode == 200){
		String payload = http.getString();

		payload.trim();
		log_i("Payload = %s %s", payload.c_str(), mac.c_str());

		mID = payload;

		iniLocSpiff = new LocSpiff("LocOTA::getMachineID Part Save");
		iniLocSpiff->writeFile("/machineid.txt", payload.c_str());
		delete iniLocSpiff;


	}
	http.end();


	return mID;
}

//		http.begin("http://172.105.117.29/nbe/gen.php?mac=" + iniLocOTA->_macID);
//		int httpCode = http.GET();
//
//
//		log_i("http= %d filename=%s; MAC=%s", httpCode, iniLocOTA->_filename.c_str(), iniLocOTA->_macID.c_str());
//
//		http.end();


LocOTA::LocOTA(String filename, int core) {
	iniLocOTA = this;
	iniLocOTA->_filename = filename.substring(3,filename.length()-3) + "bin";
	iniLocOTA->_macID = iniLocOTA->getMAC();
	iniLocOTA->_machineID = "";
	iniLocOTA->Status = ota_no_update;

	xTaskCreatePinnedToCore(iniLocOTA->loop, "loopLocOTA", 10000, NULL, 1, &loopLocOTA, core);
}
