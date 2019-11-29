/*
 * graph.h
 *
 *  Created on: Nov 23, 2019
 *      Author: annuar
 */

#ifndef OURS_GRAPH_H_
#define OURS_GRAPH_H_

#include "Arduino.h"
#include "simplot.h"

typedef struct {
	unsigned char	Max;
	unsigned char	Point;
} graph_t;


class graph {
private:
	std::vector<unsigned char> dataRun;
//	std::vector<int> dataRun;
	std::vector<unsigned char> histo;
	int _size;
	int _addedData;
	unsigned char _max;
	unsigned char _maxPointedTo;
public:
	graph(int size){
		this->_size = size;
		this->histo.resize(256,0);
		this->_addedData = 0;
		this->_max = 0;
		this->_maxPointedTo = 0;
	}

	graph_t transferToHisto(){
		graph_t res;
		unsigned char i;

		for(int y = 0; y < this->histo.size(); y++){
			this->histo.at(y) = 0;
		}


		for(int y = 0; y < this->_addedData; y ++){
			i = this->dataRun.at(y);
			this->histo.at(i)++;
			if(this->histo.at(i) > 254){
				this->histo.at(i) = 253;
			}
		}
		this->_max = 0;
		for(int z=0; z < this->histo.size(); z ++){
			if(this->histo.at(z) > this->_max){
				this->_max = this->histo.at(z);
				this->_maxPointedTo = z;
			}
		}
		res.Max = this->_max;
		res.Point = this->_maxPointedTo;

//		log_i("max = %d; pointed to %d", this->_max, this->_maxPointedTo);

		return res;
	}

	void add(unsigned char data){
		this->dataRun.insert(this->dataRun.begin(), data);
		this->dataRun.resize(this->_size, 0);
		this->_addedData++;
		if(this->_addedData > this->_size){
			this->_addedData = this->_size;
		}
	}

	void typeIt(){
		int i = 10;

		for(int y = 0; y < this->histo.size(); y ++){
			if(y & 8){
				i = 0;
			}
			else{
				i = -20;
			}

			if(y <= 1){
				i = 100;
			}
			plot2(Serial, (int) 10 * this->histo.at(y), i);
			delay(1);
//			plot2(Serial, (int) 10 * this->histo.at(y), i);
//			delay(1);
//			plot2(Serial, (int) 10 * this->histo.at(y), i);
//			delay(1);
//			plot2(Serial, (int) 10 * this->histo.at(y), i);
//			delay(1);
		}
	}

};

#endif /* OURS_GRAPH_H_ */


