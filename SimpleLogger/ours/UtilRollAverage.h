/*
 * UtilRollAverage.h
 *
 *  Created on: Nov 13, 2019
 *      Author: annuar
 */

#ifndef OURS_UTILROLLAVERAGE_H_
#define OURS_UTILROLLAVERAGE_H_

class UtilRollAverage {
private:
	double	_prev;
	int		_roll;
public:

	UtilRollAverage(int roll){
		this->_roll = roll;
		this->_prev = 0;
	}

	double Update(double newData){
		double x = 0;
		x = this->_prev * this->_roll;
		x -= this->_prev;
		x += newData;
		x /= this->_roll;

		this->_prev =  x;

		return this->_prev;
	}

	double Get(){
		return this->_prev;
	}

	void Reset(){
		this->_prev = 0;
	}

};

#endif /* OURS_UTILROLLAVERAGE_H_ */



//float rollAverage(float prev, float new, int roll)
//{
//	float X;
//////	Y = prev  (prev/roll) + (new/roll)
////	prev -= (prev / roll);
////	prev += (new /roll);
////
////	X =(P.R -P + N)/R
//	X = prev * roll;
//	X -= prev;
//	X += new;
//	X /= roll;
//
//	return X;
//}
