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
#include <Ticker.h>
//#include <digitalize.h>
//#include <graph.h>

#include <SPI.h>
#include <SD.h>

LocWiFi			*locWiFi;
LocMQTT			*locMqtt;
LocDirectOTA	*locDirectOTA;
LocOLED			*locOLED;
LocMando		*locMando;
LocSonar		*locSonar;
LocAirmar		*locAirmar;

Ticker	*tickWiFi;
Ticker	*tickNyamuk;
Ticker	*tickSonar;

int gValWiFi = lw_wifi_off;
int gValDirectOTA = 0;
String gMAC = "";

void _setupSPIFFiles(bool format);

// for MQTT
//void _tickNyamuk();
time_t gtickNyamukTime = 0;
bool gtockBeat;
bool gmqttEnabled=true;

timer_t gTimer1 =0;
bool tick1 = false;


DynamicJsonDocument jDoc(4000);
String katun="";


File root;



void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

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


//	Serial.print("Initializing SD card...");
//
//	if (!SD.begin(32)) {
//		Serial.println("initialization failed!");
//		while (1);
//	}
//	Serial.println("initialization done.");
//
//	root = SD.open("/");
//
//	printDirectory(root, 0);
//
//	Serial.println("done!");
//
//
//
//	// re-open the file for reading:
////	File myFile = SD.open("/config.txt");
////	if (myFile) {
////		while (myFile.available()) {
////		Serial.write(myFile.read());
////		}
////		myFile.close();
////	} else {
////
////	Serial.println("error opening test.txt");
////	}
//
//	SD.end();


	//--------------------------------------------------------------------------------------------

//	while(true){
//		delay(1000);
//	}


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


	tickWiFi = new Ticker(15000, 0);
	tickNyamuk = new Ticker(60000, 0);
	tickSonar = new Ticker(20000,0);


	gTimer1 = millis();
}

//=================================================================================================

int cnt=0;

void loop()
{
	locMqtt->update();

	tickWiFi->Update();
	if(tickWiFi->set & !WiFi.isConnected()){
		tickWiFi->Reset();
		gValWiFi = lw_wifi_apsta;
	}

	if(tickNyamuk->Update()){
		gtockBeat = !gtockBeat;
		locMqtt->hantar(gMAC, gtockBeat?"1":"0");
	}

	if(tickSonar->Update()){
		locMando->PortMando(false);
		locSonar->PortSonar(true);
	}

	if(locSonar->done){
		locSonar->PortSonar(false);
		locMando->PortMando(true);

		airmarReading_t r = locAirmar->GetReading();
		jDoc.clear();
		jDoc["ts"] = now();
		jDoc["ws"] = r.ws;
		jDoc["wd"] = r.wd;
		jDoc["at"] = r.at;
		jDoc["bp"] = r.bp;
		jDoc["wl"] = locSonar->GetDistance();

		katun = "";
		serializeJsonPretty(jDoc, katun);

		locMqtt->hantar("tick data", katun);

		locOLED->pause(true);
		while(!locOLED->done()){
			delay(10);
		}

		if (SD.begin(32)) {
			log_i("sini a");
			File dataFile = SD.open("/datalog.txt", FILE_APPEND);
			log_i("sini b");

			// if the file is available, write to it:
			if (dataFile) {
				dataFile.println(katun);
				Serial.println(dataFile.size());
				dataFile.close();
				// print to the serial port too:
	//			Serial.println(katun);
			}
			// if the file isn't open, pop up an error:
			else {
				Serial.println("error opening datalog.txt");
			}

			SD.end();
		}

		locOLED->pause(false);



//		delay(200);
//
//		locOLED->pause(true);
//		log_i("sini A");
//		while(!locOLED->done()){
//			delay(10);
//		}
//
//		delay(200);
//
//		if (SD.begin(32)) {
//			log_i("sini a");
//
//
//
//			File dataFile = SD.open("/datalog.txt", FILE_APPEND);
//			log_i("sini b");
//
//			// if the file is available, write to it:
//			if (dataFile) {
//				dataFile.println(katun);
//				Serial.println(dataFile.size());
//				dataFile.close();
//				// print to the serial port too:
//	//			Serial.println(katun);
//			}
//			// if the file isn't open, pop up an error:
//			else {
//				Serial.println("error opening datalog.txt");
//			}
//
//
//
//			SD.end();
//
//			digitalWrite(32, HIGH);
//
//			delay(1000);
//
//		}
//
//		locOLED->pause(false);
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






//// re-open the file for reading:
//File myFile = SD.open("/datalog.txt");
//if (myFile) {
////				while (myFile.available()) {
////					Serial.write(myFile.read());
////				}
//
//	Serial.println(myFile.size());
//	myFile.close();
//}
//else {
//	Serial.println("error opening datalog.txt");
//}
