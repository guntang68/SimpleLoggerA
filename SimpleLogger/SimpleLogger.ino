#include "Arduino.h"
#include <LocSpiff.h>
#include <LocWiFi.h>
#include <LocMQTT.h>
#include <LocDirectOTA.h>
#include <ArduinoJson.h>
#include <LocOLED.h>
#include <LocMando.h>
#include <LocSonar.h>
#include <LocAirmar.h>
//#include <digitalize.h>
//#include <graph.h>



//TODO	need to detect WiFi lost connection

LocWiFi			*locWiFi;
LocMQTT			*locMqtt;
LocDirectOTA	*locDirectOTA;
LocOLED			*locOLED;
LocMando		*locMando;
LocSonar		*locSonar;
LocAirmar		*locAirmar;

int gValWiFi = lw_wifi_off;
int gValDirectOTA = 0;
String gMAC = "";

void _setupSPIFFiles(bool format);

// for MQTT
void _tickNyamuk();
time_t gtickNyamukTime = 0;
bool gtockBeat;
bool gmqttEnabled=true;


DynamicJsonDocument jDoc(4000);
String katun="";

//=================================================================================================
void setup()
{

//	Serial.begin(921600, SERIAL_8N1,  3,  1);	//Mando dan Sonar
	Serial.begin(  9600, SERIAL_8N1,  3,  1);	//Mando dan Sonar
	Serial2.begin( 4800, SERIAL_8N1, 33, 64);	//Airmar


	delay(300);
	log_i("\n\n\n\nSalam Dunia dari %s\n\n\n\n", __FILE__);
	log_i("Memory = %d", String(esp_get_free_heap_size()).c_str());
	//	Memory = 1073447996

	pinMode(2, OUTPUT);

	for(int w=0; w<5;w++){
		digitalWrite(2, HIGH);
		delay(100);
		digitalWrite(2, LOW);
		delay(100);
	}

	//============================================================================================



	jDoc["1"] = "satu"; // XF7TDK
	jDoc["2"] = "xxxxxxxxxxxxxxxxxxxxx";
	jDoc["3"] = "tiga";


	serializeJsonPretty(jDoc, katun);

	//============================================================================================


	_setupSPIFFiles(false);	//if true -> delete all files & create default file

	locWiFi = new LocWiFi(0,3000, &gValWiFi);
	gValWiFi = lw_wifi_apsta;
	//	Memory = 1073426524

	while(WiFi.macAddress().length() < 5){
		delay(500);
	}
	gMAC = WiFi.macAddress();
	log_i("------------------------------MAC = %s", gMAC.c_str());

	locMqtt = new LocMQTT(&gMAC);

	locDirectOTA = new LocDirectOTA(0,10, &gValDirectOTA);
	locOLED = new LocOLED(0);
	locMando = new LocMando(0);
	locSonar = new LocSonar(0, 10000, 500);
	locAirmar = new LocAirmar(0);

	locDirectOTA->siniLocMando = locMando;
	locMando->siniLocMQTT = locMqtt;
	locSonar->siniLocMQTT = locMqtt;
	locSonar->siniLocOLED = locOLED;
	locAirmar->siniLocOLED = locOLED;



	locMando->PortMando(true);
}

//=================================================================================================

int cnt=0;

void loop()
{
	delay(1000);
	cnt++;

	if((!WiFi.isConnected()) & (cnt > 15)){
		cnt = 0;
		log_w("wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww??????");
		gValWiFi = lw_wifi_apsta;

	}

	if((cnt % 20) == 0){
		locMando->PortMando(false);
		locSonar->PortSonar(true);
		locMqtt->hantar("jsondata", katun);
	}

	if(locSonar->done){
		locSonar->PortSonar(false);
		locMando->PortMando(true);
	}

	_tickNyamuk();
}

//=================================================================================================
inline void _tickNyamuk() {
	locMqtt->update();
	if((millis()-gtickNyamukTime) > 60000){
		gtickNyamukTime = millis();
		gtockBeat = !gtockBeat;
		locMqtt->hantar(gMAC, gtockBeat?"1":"0");
//		log_i("beat ---------------------------------> %s", gMAC.c_str());
	}
}
//=================================================================================================
inline void _setupSPIFFiles(bool format) {
	LocSpiff 	*locSpiff;
	FileInfo_t	info;

	locSpiff = new LocSpiff("_setupSPIFFiles");

	locSpiff->listAllFiles();

	if(format){
		log_i("deleting files ...");
		locSpiff->format();
		log_i("all files deleted");

		//create time stamp file for Online OTA
		locSpiff->appendFile("/timestamp.txt", "NONE");

		//create SSID credentials
		locSpiff->appendFile("/ssid.txt", "sta,ideapad,sawabatik1\n");
		locSpiff->appendFile("/ssid.txt", "sta,AndroidAP,sawabatik\n");
		locSpiff->appendFile("/ssid.txt", "sta,GF_Wifi_2.4GHz,Gr33nF1nd3r2018\n");
		locSpiff->appendFile("/ssid.txt", "ap,NiNe,AsamBoiqqq\n");
//		locSpiff->appendFile("/ssid.txt", "ap,GreenFinderIOT,0xadezcsw1\n");

		log_i("Default files created");
	}
	delete locSpiff;

	// Sistem will halt if format==true
	if(format){
		log_i("\n\n\n\n\n\n\n\n\nSistem Halt: Please change set SPIFF -> false\n\n\n");
		while(true){
			delay(100);
		}
	}
}
