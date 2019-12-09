/*
 * LocOLED.cpp
 *
 *  Created on: Nov 6, 2019
 *      Author: annuar
 */

#include <LocOLED.h>

LocOLED	*iniLocOLED;
TaskHandle_t 	loopLocOLED= NULL;

bool MandoWake = false;
bool mandoMissing = false;



String PlainType1Text="PlainType1Text";
String PlainType2Text="PlainType2Text";
double dProgress=0;

//dari airmar
//std::vector<airmarReading_t> readingList;
//airmarReading_t reading;

double BP=0, AT=0, WS=0, WD=0;

SSD1306Spi display(pOutDRes, pOutDDC, pOutDCS);
OLEDDisplayUi ui ( &display );

String makeTwoDigits(int digits);
void displayIcons(OLEDDisplay* display);

void aTitle(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void aPlainType1(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void aPlainType2(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void aMeteorological(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void aProgress(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void aGraph(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);


bool frameScroll = false;
bool freshDone = false;
bool done = false;
int8_t currentSet=0;
int8_t newSet=0;
int frameCount = 3;

bool enUpdate=true;

FrameCallback frames1[] = {aTitle, aPlainType1, aPlainType2};
FrameCallback frames2[] = {aMeteorological};
FrameCallback frames3[] = {aProgress};
FrameCallback frames4[] = {aTitle, aGraph};

LocOLED::LocOLED(int core) {
	iniLocOLED = this;

	iniLocOLED->_pause = false;

	ui = &display;

	ui.setTargetFPS(20);
	ui.setFrameAnimation(SLIDE_LEFT);
	ui.setFrames(frames4, 2);
	ui.enableAutoTransition();
	ui.disableAllIndicators();


	display.init();


	display.setTextAlignment(TEXT_ALIGN_LEFT);
	display.flipScreenVertically();

	iniLocOLED->sonarDistance = 1200;


	frameScroll = true;

	xTaskCreatePinnedToCore(iniLocOLED->loop, "loopLocOLED", 3072, NULL, 1, &loopLocOLED, core);

}

bool LocOLED::done() {

	return freshDone;
}


void LocOLED::pause(bool pause) {
	iniLocOLED->_pause = pause;
	log_i("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< %d", pause);
	if(pause == false){
		enUpdate = true;
	}
}

void LocOLED::loop(void* parameter) {
	int remainingTimeBudget;
	ui.disableAllIndicators();



	pinMode(2, OUTPUT);

	while(1){
		if(enUpdate){
			remainingTimeBudget = ui.update();
			if(remainingTimeBudget > 0){
				delay(remainingTimeBudget);
			}

			if(ui.getUiState()->frameState == IN_TRANSITION){

				freshDone = false;

				if(!frameScroll){
					frameScroll = true;
					digitalWrite(2, HIGH);
				}

			}
			else{


				if(frameScroll){
					frameScroll = false;
					digitalWrite(2, LOW);
					if(iniLocOLED->_pause){
						iniLocOLED->_pause = false;
						log_i(">>>>>>>>>>>>>>>>>>>>>>>>>>>");
						enUpdate = false;
						freshDone = true;
					}
				}



			}


			if(currentSet != newSet){
				currentSet = newSet;
				switch (currentSet) {
					case 1:
						frameCount = 3;
						ui.setFrames(frames1, frameCount);
						break;
					case 2:
						frameCount = 1;
						ui.setFrames(frames2, frameCount);
						ui.disableAutoTransition();
						break;
					case 3:
						frameCount = 1;
						ui.setFrames(frames3, frameCount);
						ui.disableAutoTransition();
						break;
					case 4:
						log_i("SINI");
						frameCount = 2;
						ui.setFrames(frames4, frameCount);
						ui.disableAutoTransition();
						break;
					default:
						break;
				}
			}
		}

		delay(10);
	}
}



//		if(iniLocOLED->_pause == false){
//			enUpdate = true;
//			freshDone = true;
////			digitalWrite(pOutDCS, LOW);
//		}




inline void displayIcons(OLEDDisplay* display) {





	switch (WiFi.getMode()) {
		case 1:
			display->drawFastImage(48, 0, 8, 8, sta);
			break;
		case 2:
			display->drawFastImage(48, 0, 8, 8, ap);
			break;
		case 3:
			display->drawFastImage(48, 0, 8, 8, both);
			break;
		default:
			display->drawFastImage(48, 0, 8, 8, n);
			break;
	}

	display->drawFastImage(58, 0, 8, 8, nOpen);

//		if(sDoorOpen){
//			display->drawFastImage(58, 0, 8, 8, nOpen);
//		}
//		else{
//			display->drawFastImage(58, 0, 8, 8, nClose);
//		}

	if(MandoWake){
		display->drawFastImage(68, 0, 8, 8, aton);
	}
	else{
		display->drawFastImage(68, 0, 8, 8, atonsleep);
	}

	if(mandoMissing){
		display->drawFastImage(78, 0, 8, 8, offline);

	}
	else{
		display->drawFastImage(78, 0, 8, 8, online);
	}

}
inline String makeTwoDigits(int digits)
{
	if(digits < 10) {
		String i = '0'+String(digits);
		return i;
	}
	else {
		return String(digits);
	}
}

inline void aTitle(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->setFont(ArialMT_Plain_10);

	String timenow = makeTwoDigits(hour())+":"+makeTwoDigits(minute())+":"+makeTwoDigits(second());
	display->drawString(0 , 0, timenow );

//	timenow = "SPMS V5.0";
//	timenow = "GFLOGER";
	timenow = "EARTLOG";
	display->setTextAlignment(TEXT_ALIGN_CENTER);
	display->setFont(ArialMT_Plain_24);
	display->drawString(clockCenterX + x , clockCenterY/2 + y - 2, timenow );

//	timenow = "GreenFinder";
	timenow = "GeoAIS-Comm";
	display->setFont(ArialMT_Plain_16);
	display->drawString(clockCenterX + x , clockCenterY + y + 4, timenow );

	displayIcons(display);
}

inline void aPlainType1(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->setFont(ArialMT_Plain_10);

	display->drawString(0 ,  12, PlainType1Text );

	displayIcons(display);
	String timenow = makeTwoDigits(hour())+":"+makeTwoDigits(minute())+":"+makeTwoDigits(second());
	display->drawString(0 , 0, timenow );

}

inline void aPlainType2(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->setFont(ArialMT_Plain_10);

	display->drawString(0 ,  12, PlainType2Text );

	displayIcons(display);
	String timenow = makeTwoDigits(hour())+":"+makeTwoDigits(minute())+":"+makeTwoDigits(second());
	display->drawString(0 , 0, timenow );
}

inline void aMeteorological(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
	String text = "";
	int offY = 0;

	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->setFont(ArialMT_Plain_10);

	text = "AT : ";// + String(AT);
	offY += spacing;
	display->drawString(x ,  y + offY , text );

	text = "BP : ";// + String(BP);
	offY += spacing;
	display->drawString(x ,  y + offY , text );


	text = "WS : ";// + String(WS);
	offY += spacing;
	display->drawString(x ,  y + offY , text );

	text = "WD : ";// + String(WD);
	offY += spacing;
	display->drawString(x ,  y + offY , text );


	displayIcons(display);
	String timenow = makeTwoDigits(hour())+":"+makeTwoDigits(minute())+":"+makeTwoDigits(second());
	display->drawString(0 , 0, timenow );
}

inline void aProgress(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->setFont(ArialMT_Plain_10);

	int progress = (int) dProgress % 101;
	// draw the progress bar
	display->drawProgressBar(0, 32, 120, 10, progress);

	// draw the percentage as String
	display->setTextAlignment(TEXT_ALIGN_CENTER);
	display->drawString(64, 15, String(dProgress) + "%");

	display->drawString(64 , 50 , "New firmware ..." );
	String timenow = makeTwoDigits(hour())+":"+makeTwoDigits(minute())+":"+makeTwoDigits(second());

	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->drawString(0 , 0, timenow );

}

void LocOLED::setProgress(double value) {
	dProgress = value;
}

inline void aGraph(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
	double maxws=0, wdws=0;


	display->setTextAlignment(TEXT_ALIGN_LEFT);
	display->setFont(ArialMT_Plain_10);

	display->drawRect(0, 13, 62, 40);

	int xp=0, yp=0, yt=0;

	if(iniLocOLED->readingList.size()>0){
		airmarReading_t v = iniLocOLED->readingList.at(0);

		yp = 60 - (v.ws*2 +15);

		for(int x = 1; x< iniLocOLED->readingList.size(); x++){
			v = iniLocOLED->readingList.at(x);

			if(v.ws > maxws){
				maxws = v.ws;
				wdws = v.wd;
			}

			yt = 60 - (v.ws*2 +15);
			if(yt < 5) yt = 5;

			display->drawLine(xp, yp, x,yt);
			xp = x;
			yp = yt;
		}
	}



	display->drawString(72 , 50, String(iniLocOLED->sonarDistance,0) + "mm");
	display->drawCircle(94, 25, 13);

	double rad = wdws*3.14/180;
	double dX = 15 * sin(rad);
	double dY = 15 * cos(rad) * -1;

	display->drawLine(94, 25, 94+dX, 25+dY);

	String timenow = makeTwoDigits(hour())+":"+makeTwoDigits(minute())+":"+makeTwoDigits(second());
	display->drawString(0 , 0, timenow );

	displayIcons(display);
}




//
//						case 4:
////							hantu("SINI");
////							frameCount = 2;
////							ui.setFrames(frames4, frameCount);
////							ui.disableAutoTransition();
//							break;
