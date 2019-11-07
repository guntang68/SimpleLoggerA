/*
 * LocSonar.cpp
 *
 *  Created on: Nov 7, 2019
 *      Author: annuar
 */

#include <LocSonar.h>

LocSonar		*iniLocSonar;
TaskHandle_t 	loopLocSonar= NULL;


timer_t sonarReadTime=0;

boolean isNumeric(String str);


LocSonar::LocSonar(int core) {
	iniLocSonar = this;



	iniLocSonar->enable = false;
	iniLocSonar->_sonarRaw = "";
	iniLocSonar->_sonarDistance = 0;


	xTaskCreatePinnedToCore(iniLocSonar->loop, "loopLocSonar", 3072, NULL, 1, &loopLocSonar, core);
}

void LocSonar::PortSonar(bool select) {
	pinMode(MUX_S0, OUTPUT);
	pinMode(MUX_S1, OUTPUT);
	pinMode(MUX_S2, OUTPUT);

	pinMode(13, OUTPUT);

	iniLocSonar->_sonarRaw = "";
	iniLocSonar->enable = select;

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


float LocSonar::_getDistance(String data)
{
	int Start = 0, Found=0;
	int reading[36], i=0;
	String Temp="";

	int max=0, min=9999;
	long jumlah=0;

	do{
		Found = data.indexOf("R",Start);
		if(Found >= 0){
			Temp = data.substring(Found+1,Found + 5);

			if(Temp.length() == 4 && isNumeric(Temp)){
				reading[i] = Temp.toInt();
				i++;
				reading[i] = 0;
			}
			Start = Found + 3;
		}
	}while(Found >= 0);

	if(i > 4){
		int y;
		for(y = 0; y < i; y ++){

			if(reading[y] == 0) break;
			//get max
			if(reading[y] > max) max = reading[y];

			//get min
			if(reading[y] < min) min = reading[y];

			//get total
			jumlah += reading[y];
		}
		//get average
		jumlah /= y;
//		Serial.println(jumlah);
//		sonarDistance = jumlah;
	}
	return jumlah;
}

void LocSonar::loop(void* parameter) {
	char cr;

	while(true){
		if(iniLocSonar->enable){
			while(Serial.available() > 0){
				cr = Serial.read();

				iniLocSonar->_sonarRaw.concat(cr);
				if((millis()-sonarReadTime)<1000){
					iniLocSonar->_sonarRaw = ""; //discard initial reading
				}
				if((millis()-sonarReadTime)>5500){
					iniLocSonar->_sonarDistance = iniLocSonar->_getDistance(iniLocSonar->_sonarRaw);


				}


			}
		}




		delay(10);
	}
}
