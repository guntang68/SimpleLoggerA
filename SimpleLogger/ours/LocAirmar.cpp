/*
 * LocAirmar.cpp
 *
 *  Created on: Nov 12, 2019
 *      Author: annuar
 */

#include <LocAirmar.h>

LocAirmar		*iniLocAirmar;
TaskHandle_t 	loopLocAirmar= NULL;

//double BP=0, AT=0, WS=0, WD=0;

std::vector<airmarReading_t> readingList;
airmarReading_t reading;

LocAirmar::LocAirmar(int core) {

	iniLocAirmar = this;
	iniLocAirmar->_airmarRaw = "";

	pinMode(12, OUTPUT);
	digitalWrite(12, HIGH);			//Airmar POWER

	xTaskCreatePinnedToCore(iniLocAirmar->loop, "loopLocAirmar", 3072, NULL, 1, &loopLocAirmar, core);
}

void LocAirmar::_processAirmarData() {
//	int x;
//	String header="";
//
//	ManNMEA *manNmea;
//
//	x = iniLocAirmar->_airmarRaw.indexOf('*', 0);
//	if(x >= 0){
//		String cs = iniLocAirmar->_airmarRaw.substring(x+1,x+3);
//		int y = iniLocAirmar->_airmarRaw.lastIndexOf('!',x);
//		int z = iniLocAirmar->_airmarRaw.lastIndexOf('$',x);
//
//		if((y>=0) || (z>=0)){
//			if(z > y) y = z;
//			String word = iniLocAirmar->_airmarRaw.substring(y, x+3);
//
//
//			manNmea = new ManNMEA;
//			if(manNmea->ValidCheckSum(iniLocAirmar->_airmarRaw)){
//				header = manNmea->extractCol(iniLocAirmar->_airmarRaw, 0).substring(1);
//
//				if(header == "WIMDA"){
//					BP = manNmea->extractCol(iniLocAirmar->_airmarRaw,3).toDouble() * 1000;
//					AT = manNmea->extractCol(iniLocAirmar->_airmarRaw, 5).toDouble();
//					reading.bp = BP;
//					reading.at = AT;
//				}
//				else if(header == "WIMWV"){
//					WS = manNmea->extractCol(iniLocAirmar->_airmarRaw,3).toDouble();
//					WD = manNmea->extractCol(iniLocAirmar->_airmarRaw, 1).toDouble();
//					reading.ws = WS;
//					reading.wd = WD;
//				}
//				else if(header == "TIROT"){
////					hantu(String(BP) + "\t" + String(AT) + "\t" + String(WS) + "\t" + String(WD) );
//					if(readingList.size() > 60){
//						readingList.erase(readingList.begin());
//					}
//					readingList.push_back(reading);
//				}
//
//			}
//			manNmea->~ManNMEA();
//			delete manNmea;
//		}
//	}
}

void LocAirmar::loop(void* parameter) {
	char cr;


	while(true){

		while(Serial2.available() > 0){
			cr = Serial2.read();
			if(cr >= 32 && cr <= 126 ){			//printable char only
				iniLocAirmar->_airmarRaw.concat(cr);
			}
			else if(cr >= 9 && cr <= 13){			//control char only
				iniLocAirmar->_airmarRaw.concat(cr);
			}

			if(cr == 13){

				iniLocAirmar->_airmarRaw.trim();


//				processAirmarData();
//				log_i("%s", iniLocAirmar->_airmarRaw.c_str());

				iniLocAirmar->_airmarRaw = "";
			}


		}


		delay(10);
	}
}
