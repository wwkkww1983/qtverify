#include <QtCore/QDebug>
#include "alg.h"

AlgClass::AlgClass()
{

}

AlgClass::~AlgClass()
{

}

float AlgClass::calc(float a, float b)
{
	float sum = a + b;
	qDebug("%.2f + %.2f = %.2f \n", a, b, sum);
	return sum;
}
