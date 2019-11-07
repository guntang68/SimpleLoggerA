/*
 * LocMando.cpp
 *
 *  Created on: Nov 6, 2019
 *      Author: annuar
 */

#include <LocMando.h>
#include <ManNMEA.h>
#include			<TimeLib.h>


LocMando		*iniLocMando;
TaskHandle_t 	loopLocMando= NULL;

timer_t timeGap=0;

LocMando::LocMando(int core) {
	iniLocMando = this;

	iniLocMando->siniLocMQTT = NULL;

	iniLocMando->dapatMandoText = "";
	iniLocMando->_mandoRaw = "";

	iniLocMando->enable = false;

	xTaskCreatePinnedToCore(iniLocMando->loop, "loopLocMando", 3072, NULL, 1, &loopLocMando, core);
}

void LocMando::PortMando(bool select)
{
	pinMode(MUX_S0, OUTPUT);
	pinMode(MUX_S1, OUTPUT);
	pinMode(MUX_S2, OUTPUT);
	if(select){
		digitalWrite(MUX_S0, LOW);			//mando 0x04
		digitalWrite(MUX_S1, LOW);
		digitalWrite(MUX_S2, HIGH);
		iniLocMando->enable = true;
	}
	else{
		iniLocMando->enable = false;
	}
}

void LocMando::_processMandoData() {
	ManNMEA *manNmea;
	String header="";
	manNmea = new ManNMEA;

	if((iniLocMando->_mandoRaw.charAt(0) == '$') | (iniLocMando->_mandoRaw.charAt(0) == '!')){
		if(manNmea->ValidCheckSum(iniLocMando->_mandoRaw)){
			header = manNmea->extractCol(iniLocMando->_mandoRaw, 0).substring(1);
			iniLocMando->dapatMandoText += header + "\t" + iniLocMando->_mandoRaw + "<br />";

			if((header == "GPRMC") || (header == "GNRMC")){
				//$GNRMC,072014.00,A,0305.37058,N,10132.77695,E,0.003,,260419,,,D*64
				if(manNmea->extractCol(iniLocMando->_mandoRaw, 2) == "A"){

					iniLocMando->LAT = manNmea->toDegree(manNmea->extractCol(iniLocMando->_mandoRaw, 3));
					if(manNmea->extractCol(iniLocMando->_mandoRaw, 4) == "S"){
						iniLocMando->LAT *= -1;
					}
					iniLocMando->LNG = manNmea->toDegree(manNmea->extractCol(iniLocMando->_mandoRaw, 5));
					if(manNmea->extractCol(iniLocMando->_mandoRaw, 6) == "W"){
						iniLocMando->LNG *= -1;
					}

					if(millis() > timeGap){
						timeGap = millis() + 180000;
						String sTime = manNmea->extractCol(iniLocMando->_mandoRaw, 1);
						String sDate = manNmea->extractCol(iniLocMando->_mandoRaw, 9);

						setTime(sTime.substring(0,2).toInt(), sTime.substring(2,4).toInt(), sTime.substring(4,6).toInt(), sDate.substring(0,2).toInt(), sDate.substring(2,4).toInt(), sDate.substring(4,6).toInt());
						adjustTime(3600*8);
						iniLocMando->siniLocMQTT->hantar("RMC", "Dapat Masa Baru");
					}
				}
			}
			else if(header == "AIABK"){
				String abk = manNmea->extractCol(iniLocMando->_mandoRaw,5).substring(0, 1);
				iniLocMando->siniLocMQTT->hantar("abk", abk);
			}


			if(iniLocMando->dapatMandoText.length() > 4000){
				iniLocMando->dapatMandoText = "...";
			}
		}
	}
	delete manNmea;
}

void LocMando::loop(void* parameter)
{
	char cr;
//	iniLocMando->PortMando(true);

	while(true){
		if(iniLocMando->enable){
			while(Serial.available() > 0){
				cr = Serial.read();
				if(cr >= 32 && cr <= 126 ){			//printable char only
					iniLocMando->_mandoRaw.concat(cr);
				}
				if(cr == 13){
					iniLocMando->_processMandoData();
					iniLocMando->_mandoRaw = "";
				}
			}
		}
		delay(20);
	}
}

