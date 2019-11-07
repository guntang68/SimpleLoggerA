/*
 * ManNMEA.cpp
 *
 *  Created on: Jul 29, 2019
 *      Author: annuar
 */

#include <ManNMEA.h>

double ManNMEA::toDegree(String snum){
	float num = snum.toFloat();

	float temp = num / 100;
	int a = (int) temp;			//holding deg int
	float b = temp - a;			//holding minute
	b *= 100;
	b /= 60;
	b += a;

	return b;



}

String ManNMEA::extractCol(String data, int index){
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = data.length()-1;

	for(int i=0; i<=maxIndex && found<=index; i++){
		if(data.charAt(i)==',' || i==maxIndex){
			found++;
			strIndex[0] = strIndex[1]+1;
			strIndex[1] = (i == maxIndex) ? i+1 : i;
		}
	}
	return found>index ? data.substring(strIndex[0], strIndex[1]) : "";



}

int ManNMEA::checksumNmea(String data){
	int c = 0;
	char t;

	for(int i= 0; i < data.length(); i++){
		t = data.charAt(i);
		if(t == '!'){
			t = 0;
		}
		if(t == '$'){
			t = 1;
		}
		c ^= t;
	}
    return c;
}

bool ManNMEA::ValidCheckSum(String word){
	int f, resultCS=0;	//, x=0;
	String calculate="";
	bool result=false, pass=false;

	String testCS="";

	word.trim();
	String test = word.substring(1);
	//!AIVDO,1,1,,,B>eq`d@001l=N!AIVDO,1,1,,,B>eq`d@001l=N!AIVDO,1,1,,,B>eq`d@001l=N!AIVDO,1,1,,,B>eq`d@001l=N$GNRMC,070415.00,A,0305.37072,N,10132.77692,E,0.003,,260419,,,D*6C
	//!AIVDO,1,1,,,B>eq`d@001l=N!AIVDO,1,1,,,B>eq`d@001l=N$GNRMC,072014.00,A,0305.37058,N,10132.77695,E,0.003,,260419,,,D*64
	//!AIVDM,2,1,1,B,58155>p2>ktEKLhSL00<50F0I84pdhTp0000001?C`><=5!AIVDM,2,2,1,B,00000000000,2*26
	//!AIVDO,1,1,,,B>eq`d@001l=N!AIVDO,1,1,,,B>eq`d@001l=N!AIVDO,1,1,,,B>eq`d@001l=N!AIVDO,1,1,,,B>eq`d@001l=N<0LB@;Q3wjPl000,0*6D
	f = word.indexOf("*");
	if(f>=0 && pass==false){
		testCS = test.substring(f);
		test = test.substring(0,f-1);
		resultCS = checksumNmea(test);

		calculate = String(resultCS,HEX);
		calculate.toUpperCase();

		if(calculate.length()==1){
			calculate = "0" + calculate;
		}

		if(calculate == testCS){
			result = true;
		}
	}
	return result;
}

ManNMEA::ManNMEA() {
	// TODO Auto-generated constructor stub

}

ManNMEA::~ManNMEA() {
	// TODO Auto-generated destructor stub
}

