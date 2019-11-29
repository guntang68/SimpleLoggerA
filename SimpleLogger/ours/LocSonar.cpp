/*
 * LocSonar.cpp
 *
 *  Created on: Nov 7, 2019
 *      Author: annuar
 */

#include <LocSonar.h>
//#include <UtilRollAverage.h>



LocSonar		*iniLocSonar;
TaskHandle_t 	loopLocSonar= NULL;


timer_t sonarReadTime=0;

boolean isNumeric(String str);

//UtilRollAverage *distAvg;
//UtilRollAverage *distAvgSlow;
//UtilRollAverage *distAvgFast;

void LocSonar::loop(void* parameter) {
	char cr;
	int RFound=0;
	double distance=0;

	unsigned char tempChar;

	while(true){
		if(iniLocSonar->enable){
			while(Serial.available() > 0){
				cr = Serial.read();
				if(cr >= 32 && cr <= 126 ){			//printable char only
					iniLocSonar->_sonarRaw.concat(cr);
				}

				if(cr == 'R'){
					distance = iniLocSonar->_getNumber();
				}

				if(distance > 0){
					RFound ++;

					tempChar = iniLocSonar->digiSonar->toChar(distance);
					iniLocSonar->graphSonar->add(tempChar);
				}

				if(RFound > 24 && !iniLocSonar->done){
					RFound = 0;
					digitalWrite(13, LOW);			//Sonar POWER OFF


					graph_t ddd = iniLocSonar->graphSonar->transferToHisto();

#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
					iniLocSonar->graphSonar->typeIt();
					log_i("hh");
					log_i("hh");
					log_i("hh");
					log_i("%d %d %f", ddd.Max, ddd.Point, iniLocSonar->digiSonar->toDouble(ddd.Point));
#endif

//					iniLocSonar->_sonarDistance = distAvg->Get();
					iniLocSonar->_sonarDistance = iniLocSonar->digiSonar->toDouble(ddd.Point);
					iniLocSonar->done = true;
					iniLocSonar->enable = false;
					iniLocSonar->siniLocMQTT->hantar("Sonar", String(iniLocSonar->_sonarDistance));
					iniLocSonar->siniLocOLED->sonarDistance = iniLocSonar->_sonarDistance;
					iniLocSonar->_sonarRaw = "";
				}

				distance  = 0;
				delay(10);
			}
		}
		delay(10);
	}
}

LocSonar::LocSonar(int core, double Max, double Min) {
	iniLocSonar = this;

	iniLocSonar->siniLocMQTT = NULL;
	iniLocSonar->siniLocOLED = NULL;
	iniLocSonar->enable = false;
	iniLocSonar->_sonarRaw = "";
	iniLocSonar->_sonarDistance = 0;
	iniLocSonar->done = false;


	iniLocSonar->digiSonar = new digitalize(Max,Min);
	iniLocSonar->graphSonar = new graph(64);

	//	digiSonar = new digitalize(10000, 2000);
	//	graphSonar = new graph(256);

	xTaskCreatePinnedToCore(iniLocSonar->loop, "loopLocSonar", 3072, NULL, 1, &loopLocSonar, core);
}

void LocSonar::PortSonar(bool select) {
	pinMode(MUX_S0, OUTPUT);
	pinMode(MUX_S1, OUTPUT);
	pinMode(MUX_S2, OUTPUT);

	pinMode(13, OUTPUT);

	iniLocSonar->_sonarRaw = "";
	iniLocSonar->enable = select;
	iniLocSonar->done = false;

	if(select){
		digitalWrite(MUX_S0, LOW);			//sonar 0x02
		digitalWrite(MUX_S1, HIGH);
		digitalWrite(MUX_S2, LOW);
		digitalWrite(13, HIGH);			//Sonar POWER on masa nak baca sahaja
		sonarReadTime = millis();
	}
	else{
		digitalWrite(13, LOW);			//Sonar POWER on masa nak baca sahaja
	}

}

boolean isNumeric(String str)
{
    unsigned int stringLength = str.length();
    boolean seenDecimal = false;


//    log_i("panjang = %d", stringLength);

    if (stringLength == 0) {
        return false;
    }

    for(unsigned int i = 0; i < stringLength; ++i) {
        if (isDigit(str.charAt(i))) {
            continue;
        }

        if (str.charAt(i) == '.') {
            if (seenDecimal) {
                return false;
            }
            seenDecimal = true;
            continue;
        }
        return false;
    }
    return true;
}


int LocSonar::_getNumber() {
	int start=0, end=0, pos = 0, tempNum = 0;
	String Temp = "";

	start = iniLocSonar->_sonarRaw.indexOf("R",pos);
	if(start != -1){
		pos = start + 1;
		end = iniLocSonar->_sonarRaw.indexOf("R",pos);
		if(end != -1){
			Temp = iniLocSonar->_sonarRaw.substring(start+1,end);

			Temp.trim();

			iniLocSonar->_sonarRaw = iniLocSonar->_sonarRaw.substring(start+1);
//			log_i("Temp = %s", Temp.c_str());

			if((Temp.length() > 0) && isNumeric(Temp)){
				tempNum = Temp.toInt();
				if((tempNum == 500) || (tempNum == 4999) || (tempNum == 9999)){
					tempNum = 0;
				}
			}
		}
	}


	return tempNum;
}
