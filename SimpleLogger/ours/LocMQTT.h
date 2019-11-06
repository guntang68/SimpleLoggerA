/*
 * LocMQTT.h
 *
 *  Created on: Oct 17, 2019
 *      Author: annuar
 */

#ifndef OURS_LOCMQTT_H_
#define OURS_LOCMQTT_H_

#include "Arduino.h"
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <TimeLib.h>
#include <WiFi.h>

class LocMQTT {
private:
	String *_MAC;
public:
	LocMQTT(String *mac);
	void update();
	void hantar(String t, String m);
	static void callback(char* topic, byte* message, unsigned int length);
	static void reconnect();
};

#endif /* OURS_LOCMQTT_H_ */
