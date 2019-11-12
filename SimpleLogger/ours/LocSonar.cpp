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


				if((millis()-sonarReadTime)>6000 && !iniLocSonar->done){
					digitalWrite(13, LOW);			//Sonar POWER OFF
					iniLocSonar->_sonarDistance = iniLocSonar->_getDistance(iniLocSonar->_sonarRaw);
					iniLocSonar->done = true;
					iniLocSonar->enable = false;
					iniLocSonar->siniLocMQTT->hantar("Sonar", String(iniLocSonar->_sonarDistance));
					iniLocSonar->siniLocOLED->sonarDistance = iniLocSonar->_sonarDistance;

				}

				delay(10);


			}
		}




		delay(10);
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


float LocSonar::_getDistance(String str)
{
	int size = 36;
	int item[size], start=0, end=0, pos=0;
	int index = 0, tempNum=0;
	bool pass;

	String Temp;

	item[0]=0;

	//tukarka ke dalam array
	log_i("Raw Sonar %s", str.c_str());

	while(true){
		pass = false;
		start = str.indexOf("R",pos);
//		log_i("Start %d", start);
		if(start != -1){
			pos = start + 1;
			end = str.indexOf("R",pos);
			if(end != -1){
//				pos = end+1;




				Temp = str.substring(start+1,end);
				Temp.trim();


				if((Temp.length() > 0) && isNumeric(Temp)){

					tempNum = Temp.toInt();

					if((tempNum != 500) && (tempNum != 4999) && (tempNum != 9999)){
//						log_i("Index = %d: Start = %d, End = %d, dapat = %s", index, start, end, Temp.c_str());

						item[index] =tempNum;
						index ++;
						item[index] = 0;
						if(index == size-1){
							break;
						}
					}
					pass = true;
				}

				if(!isNumeric(Temp)){
					pass = true;
				}

			}
		}
		if(!pass){
			break;
		}
	}

	double sum=0, mean=0;
	int i, below=0, above=0;

	for(i=0; i<index; i++){
		sum += item[i];
	}
	mean = sum / i;

	for(i=0; i<index; i++){
		if(item[i] > mean){
			above++;
		}
		else{
			below++;
		}
	}
	log_i("above %d; below %d", above, below);

	int j;

	if(below > above){
		sum = 0;
		j = 0;
		for(i=0; i<index; i++){
			if(item[i] < mean){
				j++;
				sum += item[i];
			}
		}
		mean = sum / j;
		log_i("avg on below %f", mean);
	}
	else{
		sum = 0;
		j = 0;
		for(i=0; i<index; i++){
			if(item[i] > mean){
				j++;
				sum += item[i];
			}
		}
		mean = sum / j;
		log_i("avg on above %f", mean);
	}

	return mean;


}

LocSonar::LocSonar(int core) {
	iniLocSonar = this;

	iniLocSonar->siniLocMQTT = NULL;
	iniLocSonar->siniLocOLED = NULL;
	iniLocSonar->enable = false;
	iniLocSonar->_sonarRaw = "";
	iniLocSonar->_sonarDistance = 0;
	iniLocSonar->done = false;

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





//float LocSonar::_getDistance(String data)
//{
//	int Start = 0, Found=0;
//	int reading[36], i=0;
//	String Temp="";
//
//
//	int max=0, min=99999;
//	long jumlah=0;
//
//
//	int maxLimit = 9999;
//	int minLimit = 500;
//
//	do{
//		Found = data.indexOf("R",Start);
//		if(Found >= 0){
//			Temp = data.substring(Found+1,Found + 5);
//
//			if(Temp.length() == 4 && isNumeric(Temp)){
//				reading[i] = Temp.toInt();
//				i++;
//				reading[i] = 0;
//			}
//			Start = Found + 3;
//		}
//	}while(Found >= 0);
//
//	if(i > 4){
//		int y;
//		for(y = 0; y < i; y ++){
//
//			if(reading[y] == 0) break;
//			//get max
//			if(reading[y] > max) max = reading[y];
//
//			//get min
//			if(reading[y] < min) min = reading[y];
//
//			//get total
//			jumlah += reading[y];
//		}
//		//get average
//		jumlah /= y;
//
//
//		//============================================================================
//
//		for(y = 0; y < i; y ++){
//			if(reading[y] == 0) break;
//			if((reading[y] != maxLimit) && (reading[y] != minLimit)){
//
//
//
//
//			}
//		}
//
//
//
//
//
//
//		//============================================================================
//
//		log_i("min=%d, max=%d, avg=%d", min, max, jumlah);
////		Serial.println(jumlah);
////		sonarDistance = jumlah;
//	}
//	return jumlah;
//}

