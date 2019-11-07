/*
 * LocWiFi.cpp
 *
 *  Created on: Oct 13, 2019
 *      Author: annuar
 */

#include <LocWiFi.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPmDNS.h>

LocWiFi	*iniWiFi;
TaskHandle_t loopLocWiFi= NULL;
WiFiMulti 	*wifiMulti;

LocWiFi::LocWiFi(int core, int loopDelay, int *lookVal) {
	iniWiFi = this;
	iniWiFi->_loopDelay = loopDelay;
	iniWiFi->_lookVal = lookVal;
	iniWiFi->_connectionIs = lookVal;

	wifiMulti = new WiFiMulti;
	wifiMulti->APlistClean();

	xTaskCreatePinnedToCore(iniWiFi->loop, "loopLocWiFi", 3072, NULL, 1, &loopLocWiFi, core);
}

void LocWiFi::_openConnection(int as) {
	log_i("LocWiFi::_readSsid");
	LocSpiff *ls;
	bool staFlag=false;

	ls = new LocSpiff("LocWiFi::_openConnection");
	_sList.clear();
	_sList = ls->readCSV("/ssid.txt");
	delete ls;

	wifiMulti->APlistClean();

	log_i("INITIAL SSID=%s IP=%s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());

	if( (as == lw_wifi_sta) || (as == lw_wifi_apsta) || (as == lw_wifi_ap) ){
		for(int x=0; x < _sList.size(); x++){
			auto data = _sList.at(x);

			if( ((data.at(0) == "apsta") || (data.at(0) == "sta")) && (as != lw_wifi_ap) ){
				log_i("STA");
				wifiMulti->addAP(data.at(1).c_str(), data.at(2).c_str());
				staFlag = true;
			}
			if( ((data.at(0) == "apsta") || (data.at(0) == "ap")) && (as != lw_wifi_sta) ){
				log_i("AP ...");
				WiFi.softAP(data.at(1).c_str(), data.at(2).c_str());
				log_i("IP=%s", WiFi.softAPIP().toString().c_str());
			}
		}
		if(staFlag){
			log_i("Start ...");
			wifiMulti->run();

			log_i("... End SSID=%s IP=%s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
		}
	}

//	log_i("Memory = %d", String(esp_get_free_heap_size()).c_str());
}

void LocWiFi::loop(void* parameter) {
	//init




	while(true){
		switch (*iniWiFi->_lookVal) {
			case lw_wifi_ap:
				iniWiFi->_openConnection(lw_wifi_ap);
				break;
			case lw_wifi_sta:
				iniWiFi->_openConnection(lw_wifi_sta);
				break;
			case lw_wifi_apsta:
				iniWiFi->_openConnection(lw_wifi_apsta);
				break;
			case lw_wifi_off:
				break;
			default:
				break;
		}

		*iniWiFi->_lookVal = 0;
		delay(iniWiFi->_loopDelay);
	}
}


LocWiFi::~LocWiFi() {


}

