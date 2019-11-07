/*
 * LocMQTT.cpp
 *
 *  Created on: Oct 17, 2019
 *      Author: annuar
 *      ANNUAR 02:56:00 ~: $ sudo ufw allow 1883
 *
 *      Node-Red:
 *      sudo apt install curl

		curl -sL https://deb.nodesource.com/setup_8.x | sudo bash -

		sudo apt-get install -y nodejs
 */

#include <LocMQTT.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <TimeLib.h>
#include <WiFi.h>

//const char* mqtt_server = "103.75.190.251";
//const char* mqtt_server = "192.168.43.114";
//const char* mqtt_server =   "127.0.0.1";
const char* mqtt_server = "172.105.117.29";




String message="";
String topic="";
char tempString[120];
char tempTopic[36];

WiFiClient espClient;
PubSubClient client(espClient);

LocMQTT *iniMQTT;


LocMQTT::LocMQTT(String *mac) {
	log_i("================== LocMQTT::LocMQTT()=============");
	iniMQTT = this;
	client.setServer(mqtt_server, 1883);
	client.setCallback(iniMQTT->callback);

	iniMQTT->_MAC = mac;
}

void LocMQTT::update() {
	if (!client.connected()) {
		delay(1000);
		iniMQTT->reconnect();
	}
	else{
		client.loop();
	}

	if(!WiFi.isConnected()){
		log_w("Wifi need to reconnect");
		WiFi.reconnect();
		iniMQTT->reconnect();

	}
}

void LocMQTT::hantar(String t, String m) {
	topic = t;
	message = m;

	if(client.connected() && m.length()>0){
		m.toCharArray(tempString, m.length()+1);
		t.toCharArray(tempTopic, t.length()+1);
		client.publish(tempTopic, tempString);

	}
}

void LocMQTT::callback(char* topic, byte* message, unsigned int length) {
	String messageTemp;

	for (int i = 0; i < length; i++) {
		messageTemp += (char)message[i];
	}

	if (String(topic) == "esp32/output") {
		if(messageTemp == "on"){

			log_i("+++++++++++++++++++++++++++++++ON");
		}
		else if(messageTemp == "off"){
			log_i("+++++++++++++++++++++++++++++++OFF");
		}
		else{
			// terima dalam bentuk JSON
			log_i("%s",messageTemp.c_str());
		}
	}
}

void LocMQTT::reconnect() {
	while (!client.connected() && WiFi.isConnected()) {
		yield();
		delay(20);
		if (client.connect("ESP8266Client")) {
			client.subscribe("esp32/output");
			break;
		} else {
			log_w("reconnect failed");
			WiFi.reconnect();
			break;
//			delay(5000);
//			log_w("retry");

		}
	}
}


