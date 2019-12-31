/*
 * ManOTA.cpp
 *
 *  Created on: Jul 1, 2019
 *      Author: annuar
 */

#include 	"ManOTA.h"
#include	<LocSpiff.h>
#include	"esp_task_wdt.h"
#include 	<WiFiClient.h>


extern String machineID;
String macID="";

int16_t loopDelay;
String Site;
HTTPClient http;
Spiff *spiff;
String tsFile = "";


void getMachineID();
void getMAC();


ManOTA::ManOTA() {
	loopDelay = 60000;
	Site = "";
}

void getMAC(){
	uint8_t mac[6];
	WiFi.macAddress(mac);

	macID = "";


	uint8_t num;
	for(int i=0; i < 5; i++){
		num = mac[i];
		if(num < 16) macID += "0";
		macID += String(mac[i], HEX) + ":";
	}
	macID += String(mac[5], HEX);

	macID.toUpperCase();

}


void getMachineID(){
	String payload="";



	if(machineID.length()<3){
		spiff = new Spiff;
		machineID = spiff->readFromSpiffs("machineid");
		free(spiff);

		if(machineID.length()<3){
			//get it from server
			getMAC();
			http.begin("http://172.105.117.29/nbe/gen.php?mac=" + macID);
			int httpCode = http.GET();										//Make the request

			if (httpCode > 0) { //Check for the returning code
				payload = http.getString();

				payload.trim();

				if(payload.length() == 6){
	                spiff = new Spiff;
	                spiff->writeToSpiffs("machineid", payload);
	                free(spiff);

				}

				hantu(payload);
				hantu(String(payload.length()));


			}

			http.end(); //Free the resources

		}

	}

	log_i("machineID = " , machineID.c_str());


}





void ManOTA::serverOTA(void * parameter) {
	int16_t loop;

	while(true){
		bool dapat=false;
		String payload="";

		int16_t currentDelay = loopDelay;

		loop = currentDelay / 1000;
		for(int x = 0; x < loop; x++){
			delay(1000);
			if(currentDelay != loopDelay) break;
			esp_task_wdt_reset();
		}



		if ((WiFi.status() == WL_CONNECTED) && (Site.length()>2)) { //Check the current connection status
			getMachineID();

			if(machineID.length() == 6){
				http.begin("http://172.105.117.29/nbe/update.php?file=" + Site + "&m=" + machineID); 												//Specify the URL
				int httpCode = http.GET();										//Make the request

				if (httpCode == 200) { //Check for the returning code
//					Serial.println(String(httpCode));
					if(tsFile == ""){
						spiff = new Spiff;
						tsFile = spiff->readFromSpiffs("latestUpdate");
						free(spiff);
					}

					payload = http.getString();	//533662722
					if(payload != tsFile){
						dapat=true;
					}
				}
				else {
					Serial.println("Error on HTTP request aza");


				}

				http.end(); //Free the resources

				if(dapat){
					esp_task_wdt_reset();
					Serial.println("Downloading firmware ...");

					delay(10);

					ManOTA_events(manOTA_StartDownload);
					ESPhttpUpdate.rebootOnUpdate(false);
					t_httpUpdate_return ret = ESPhttpUpdate.update("http://172.105.117.29/nbe/" + Site + "?m=" + machineID);

//					WiFiClient client;
//					t_httpUpdate_return ret = ESPhttpUpdate.update(client, "http://103.75.190.251/nbe/" + Site + "?m=" + machineID);

					switch(ret) {
						case HTTP_UPDATE_FAILED:
							Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
							ManOTA_events(manOTA_EndDownload);
							delay(1000);
							break;

						case HTTP_UPDATE_NO_UPDATES:
							Serial.println("HTTP_UPDATE_NO_UPDATES");
							delay(1000);
							break;

						case HTTP_UPDATE_OK:

							http.begin("http://172.105.117.29/nbe/update.php?file=" + Site + "&m=" + machineID + "&s=ok"); 												//Specify the URL
							http.GET();
							http.end(); //Free the resources



							Serial.println("HTTP_UPDATE_OK");
							spiff = new Spiff;
							spiff->writeToSpiffs("latestUpdate", payload);
							free(spiff);
							ManOTA_events(manOTA_EndDownload);
							delay(1000);
							ESP.restart();
							break;
					}
					dapat = false;	// mana laaaaaa ;-)
				}

			}

		}

	}


}

ManOTA::~ManOTA() {
	// TODO Auto-generated destructor stub
}

void ManOTA::setLoopDelay(int16_t masa) {
	loopDelay = masa;
}

void ManOTA::setFileLookUp(String filename) {
	Site = filename;
}



//
//
//
//
///*
// * ManOTA.cpp
// *
// *  Created on: Jul 1, 2019
// *      Author: annuar
// */
//
//#include "ManOTA.h"
//#include	"esp_task_wdt.h"
//
//
//int16_t loopDelay;
//String Site;
//HTTPClient http;
//Spiff *spiff;
//String tsFile = "";
//
//ManOTA::ManOTA() {
//	loopDelay = 60000;
//	Site = "";
//}
//
//void ManOTA::serverOTA(void * parameter) {
//	int16_t loop;
//
//	while(true){
//		bool dapat=false;
//		String payload="";
//
//		int16_t currentDelay = loopDelay;
//
//		loop = currentDelay / 1000;
//		for(int x = 0; x < loop; x++){
//			delay(1000);
//			if(currentDelay != loopDelay) break;
//			esp_task_wdt_reset();
////			hantu("clrwdt@manOTA:" + String(esp_task_wdt_reset()));
//		}
//
//
//
//		if ((WiFi.status() == WL_CONNECTED) && (Site.length()>2)) { //Check the current connection status
//			hantu("Check server for update");
//			http.begin("http://103.75.190.251/nbe/update.php?file=" + Site); 												//Specify the URL
//			int httpCode = http.GET();										//Make the request
//
//			if (httpCode > 0) { //Check for the returning code
//				spiff = new Spiff;
//				tsFile = spiff->readFromSpiffs("latestUpdate");
//				free(spiff);
//				payload = http.getString();
//				if(payload != tsFile){
//					dapat=true;
//				}
//			}
//
//			else {
//				Serial.println("Error on HTTP request aza");
//			}
//
//			http.end(); //Free the resources
//
//			if(dapat){
//				esp_task_wdt_reset();
//				Serial.println("Downloading firmware ...");
//
//				delay(10);
//
//				ManOTA_events(manOTA_StartDownload);
//		        ESPhttpUpdate.rebootOnUpdate(false);
//		        t_httpUpdate_return ret = ESPhttpUpdate.update("http://103.75.190.251/nbe/" + Site);
//
//		        switch(ret) {
//		            case HTTP_UPDATE_FAILED:
//		                Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
//		                ManOTA_events(manOTA_EndDownload);
//		                delay(1000);
//		                break;
//
//		            case HTTP_UPDATE_NO_UPDATES:
//		                Serial.println("HTTP_UPDATE_NO_UPDATES");
//		                delay(1000);
//		                break;
//
//		            case HTTP_UPDATE_OK:
//
//		                Serial.println("HTTP_UPDATE_OK");
//		                spiff = new Spiff;
//		                spiff->writeToSpiffs("latestUpdate", payload);
//		                free(spiff);
//		                ManOTA_events(manOTA_EndDownload);
//		                delay(1000);
//		                ESP.restart();
//		                break;
//		        }
//				dapat = false;	// mana laaaaaa ;-)
//			}
//		}
//
//	}
//
//
//}
//
//ManOTA::~ManOTA() {
//	// TODO Auto-generated destructor stub
//}
//
//void ManOTA::setLoopDelay(int16_t masa) {
//	loopDelay = masa;
//}
//
//void ManOTA::setFileLookUp(String filename) {
//	Site = filename;
//}
