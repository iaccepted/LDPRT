//////////////////////////////////////////////////////////////////////////////////////////
//GradSH.h

//¼ÆËãshµÄÌÝ¶È
//////////////////////////////////////////////////////////////////////////////////////////	

#ifndef __GRADSH_H__
#define __GRADSH_H__

#include "Global.h"

//Evaluate an Associated Legendre Polynomial P(l, m) at x
double P(int l, int m, double x);

//Calculate the normalisation constant for an SH function
double K(int l, int m);

//Sample a spherical harmonic basis function Y(l, m) at a point on the unit sphere
double SH(int l, int m, double theta, double phi);


//Calculate n!
int Factorial(int n);

LFLOAT gradTheta(int l, int m, LFLOAT theta, LFLOAT phi);
LFLOAT gradPhi(int l, int m, LFLOAT theta, LFLOAT phi);

#endif