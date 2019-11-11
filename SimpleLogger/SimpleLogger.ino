#include "Arduino.h"
#include <LocSpiff.h>
#include <LocWiFi.h>
#include <LocMQTT.h>
#include <LocDirectOTA.h>
#include <ArduinoJson.h>
#include <LocOLED.h>
#include <LocMando.h>
#include <LocSonar.h>

//TODO	need to detect WiFi lost connection

LocWiFi			*locWiFi;
LocMQTT			*locMqtt;
LocDirectOTA	*locDirectOTA;
LocOLED			*locOLED;
LocMando		*locMando;
LocSonar		*locSonar;

int gValWiFi = lw_wifi_off;
int gValDirectOTA = 0;
String gMAC = "";

void _setupSPIFFiles(bool format);

// for MQTT
void _tickNyamuk();
time_t gtickNyamukTime = 0;
bool gtockBeat;
bool gmqttEnabled=true;




//boolean xisNumeric(String str)
//{
//    unsigned int stringLength = str.length();
//    boolean seenDecimal = false;
//
//    if (stringLength == 0) {
//        return false;
//    }
//
//    for(unsigned int i = 0; i < stringLength; ++i) {
//        if (isDigit(str.charAt(i))) {
//            continue;
//        }
//
//        if (str.charAt(i) == '.') {
//            if (seenDecimal) {
//                return false;
//            }
//            seenDecimal = true;
//            continue;
//        }
//        return false;
//    }
//    return true;
//}


//double getSTD(int data[])
//{
//
//	int i, sum = 0;
//	double avg;
//
//	for (i = 0; i < 5; ++i) {
//		sum += data[i];
//	}
//	avg = double(sum) / 5;
//
//	return avg;
//
//
//}

//void DataFromString(String *str)
//{
//
//	int size = 36;
//	int item[size], start=0, end=0, pos=0;
//	int index = 0, tempNum=0;
//	bool pass;
//
//	String Temp;
//
//	item[0]=0;
//
//	//tukarka ke dalam array
//	while(true){
//		pass = false;
//		start = str->indexOf("R",pos);
//		if(start != -1){
//			pos = start + 1;
//			end = str->indexOf("R",pos);
//			if(end != -1){
//				Temp = str->substring(start+1,end);
//				if(Temp.length() > 0 && xisNumeric(Temp)){
//
//					tempNum = Temp.toInt();
//
//					if((tempNum != 500) && (tempNum != 4999) && (tempNum != 9999)){
//						log_i("Index = %d: Start = %d, End = %d, dapat = %s", index, start, end, Temp.c_str());
//
//						item[index] =tempNum;
//						index ++;
//						item[index] = 0;
//						if(index == size-1){
//							break;
//						}
//					}
//					pass = true;
//				}
//			}
//		}
//		if(!pass){
//			break;
//		}
//	}
//
//	double sum=0, mean=0;
//	int i, below=0, above=0;
//
//	for(i=0; i<index; i++){
//		sum += item[i];
//	}
//	mean = sum / i;
//
//	for(i=0; i<index; i++){
//		if(item[i] > mean){
//			above++;
//		}
//		else{
//			below++;
//		}
//	}
//	log_i("above %d; below %d", above, below);
//
//	int j;
//
//	if(below > above){
//		sum = 0;
//		j = 0;
//		for(i=0; i<index; i++){
//			if(item[i] < mean){
//				j++;
//				sum += item[i];
//			}
//		}
//		mean = sum / j;
//		log_i("avg on below %f", mean);
//	}
//	else{
//		sum = 0;
//		j = 0;
//		for(i=0; i<index; i++){
//			if(item[i] > mean){
//				j++;
//				sum += item[i];
//			}
//		}
//		mean = sum / j;
//		log_i("avg on above %f", mean);
//	}
//
//}

//=================================================================================================
void setup()
{

//	Serial.begin(921600, SERIAL_8N1, 3, 1); //Mando dan Sonar
	Serial.begin(9600, SERIAL_8N1, 3, 1); //Mando dan Sonar
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

//	String dataRaw = "empIR500R5642R5640R5638R5638R5636R5635R5634R5632R5634R5634R5634R5634R5633R5635R5635R6589R6588R6586R6585R6580R6578R5634R5632R5633R5632R5632R5632R5631R5631R5631R5631R5631";
//	String dataRaw = "empIR5642R5640R5632R5652R5641R5642R5640R5632R5652R5641R5642R5640R5632R5652R5641R5642R5640R5632R5652R5641R5642R5640R5632R5652R5641R5642R5640R5632R5652R5641R5642R5640R5632R5652R5641";

//	String dataRaw = "R4820R5244R5138R5186R4787R4890R5110R5004R5062R5131R5157R4997R4935R4798R4777R5208R5242R4785R4945R5043R5046R3943R4199R3892R4065R4055R4237R3874R4151R4060R3789R3784R";

//	DataFromString(&dataRaw);


//	while(1){
//
//	}

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
	locSonar = new LocSonar(0);

	locDirectOTA->siniLocMando = locMando;
	locMando->siniLocMQTT = locMqtt;
	locSonar->siniLocMQTT = locMqtt;



	locMando->PortMando(true);

}

//=================================================================================================

int cnt=0;

void loop()
{
	delay(1000);
	cnt++;
//	if(cnt == 15){
//		locMqtt->hantar("mando", "kasi enable");
//		locMando->PortMando(true);
//	}
//	if(cnt == 30){
//		gValWiFi = lw_wifi_apsta;
//	}


	if((!WiFi.isConnected()) & (cnt > 15)){
		cnt = 0;
		gValWiFi = lw_wifi_apsta;

	}

	if((cnt % 20) == 0){
		locMando->PortMando(false);
		locSonar->PortSonar(true);
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
	log_i("aaa");
	locSpiff->listAllFiles();
	log_i("bbb");
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
