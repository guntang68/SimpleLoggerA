/*
 * digitalize.h
 *
 *  Created on: Nov 21, 2019
 *      Author: annuar
 */

#ifndef OURS_DIGITALIZE_H_
#define OURS_DIGITALIZE_H_

#include "Arduino.h"

class digitalize {
private:
	double	_max;
	double	_min;
	double 	_range;
	double	_gap;

public:
	digitalize(double Max, double Min){
		this->_max = Max;
		this->_min = Min;
		this->_range = Max - Min;
		this->_gap = this->_range / 256;
	}

	double toDouble(unsigned char value){
		double res;

		res = value * this->_gap;
		res += this->_min;

		return res;
	}

	unsigned char toChar(double value){
		unsigned char res = 0;
		int perkali = 128;
		double comparator = 0;
		int polarity = 1;
		double digitWeight;

		double offset = value - this->_min;
		double collection=0;

		for(int i = 0; i < 8; i++){
			comparator = comparator + (polarity * perkali * this->_gap);
			digitWeight = perkali * this->_gap;

			if(offset > comparator){
				polarity = 1;
				collection += digitWeight;
				res += perkali;
			}
			else{
				polarity = -1;
			}
			perkali /= 2;
		}

//		log_i("result %d" , res);
		return res;
	}
};

#endif /* OURS_DIGITALIZE_H_ */
