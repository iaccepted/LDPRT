#include "GradSH.h"


double P(int l, int m, double x)
{
	//First generate the value of P(m, m) at x
	double pmm = 1.0;

	if (m>0)
	{
		double sqrtOneMinusX2 = sqrt(1.0 - x*x);

		double fact = 1.0;

		for (int i = 1; i <= m; ++i)
		{
			pmm *= (-fact)*sqrtOneMinusX2;
			fact += 2.0;
		}
	}

	//If l==m, P(l, m)==P(m, m)
	if (l == m)
		return pmm;

	//Use rule 3 to calculate P(m+1, m) from P(m, m)
	double pmp1m = x*(2.0*m + 1.0)*pmm;

	//If l==m+1, P(l, m)==P(m+1, m)
	if (l == m + 1)
		return pmp1m;

	//Otherwise, l>m+1.
	//Iterate rule 1 to get the result
	double plm = 0.0;

	for (int i = m + 2; i <= l; ++i)
	{
		plm = ((2.0*i - 1.0)*x*pmp1m - (i + m - 1.0)*pmm) / (i - m);
		pmm = pmp1m;
		pmp1m = plm;
	}

	return plm;
}

//Evaluate an Associated Legendre Polynomial P(l, m) at x-------x is cos(theta), i.e the gradient of cos(theta)
double gradP(int l, int m, double x)
{
	//First generate the value of P(m, m) at x
	double pmm=1.0;
	double gradPmm = 1.0;

	if(m>0)
	{
		double sqrtOneMinusX2=sqrt(1.0-x*x);

		double fact=1.0;

		for(int i=1; i<=m; ++i)
		{
			pmm*=(-fact)*sqrtOneMinusX2;
			fact+=2.0;
		}

		gradPmm = pmm;
		gradPmm *= -1.0;
		gradPmm *= m;
		gradPmm *= (x / sqrtOneMinusX2);
	}

	//If l==m, P(l, m)==P(m, m)
	if (l == m)
		return gradPmm;

	//Use rule 3 to calculate P(m+1, m) from P(m, m)
	double pmp1m=x*(2.0*m+1.0)*pmm;
	double gradPmp1m = (2 * m + 1) * pmm + x * (2 * m + 1) * gradPmm;

	//If l==m+1, gradient of P(l, m)== gradient of P(m+1, m)
	if (l == m + 1)
		return gradPmp1m;

	//Otherwise, l>m+1.
	//Iterate rule 1 to get the result
	double plm=0.0;
	double gradPlm = 0.0;

	for(int i=m+2; i<=l; ++i)
	{
		plm=((2.0*i-1.0)*x*pmp1m-(i+m-1.0)*pmm)/(i-m);
		gradPlm = ((2 * i - 1) * pmp1m + x * (2 * i - 1) * gradPmp1m - (i + m - 1.0) * gradPmm) / (i - m);
		//pmm=pmp1m;
		gradPmm = gradPmp1m;
		pmp1m=plm;
		gradPmp1m = gradPlm;
	}

	return gradPlm;
}

//Calculate the normalisation constant for an SH function
//No need to use |m| since SH always passes positive m
double K(int l, int m)
{
	double temp=((2.0*l+1.0)*Factorial(l-m))/((4.0*M_PI)*Factorial(l+m));
	
	return sqrt(temp);
}



//Sample a spherical harmonic basis function Y(l, m) at a point on the unit sphere
double SH(int l, int m, double theta, double phi)
{
	const double sqrt2=sqrt(2.0);

	if(m==0)
		return K(l, 0)*P(l, m, cos(theta));

	if(m>0)
		return sqrt2*K(l, m)*cos(m*phi)*P(l, m, cos(theta));

	//m<0
	return sqrt2*K(l,-m)*sin(-m*phi)*P(l, -m, cos(theta));
}

//calculate the gradient of phi about ylm
LFLOAT gradPhi(int l, int m, LFLOAT theta, LFLOAT phi)
{
	const LFLOAT sqrt2 = sqrt(2.0);

	if (m == 0)
		return 0;

	if (m > 0)
		return  sqrt2 * K(l, m) * P(l, m, cos(theta)) * -1.0 * sin(m * phi) * m;

	return sqrt2 * K(l, -m) * P(l, -m, cos(theta)) * -1.0 * sin(-m * phi) * (-m);
}

//calculate the gradient of theta about ylm
LFLOAT gradTheta(int l, int m, LFLOAT theta, LFLOAT phi)
{
	const double sqrt2 = sqrt(2.0f);

	if (m == 0)
		return K(l, 0) * gradP(l, m, cos(theta)) * (-sin(theta));
	
	if (m > 0)
		return sqrt2 * K(l, m) * cos(m * phi) * gradP(l, m, cos(theta)) * (-sin(theta));

	//m < 0
	return sqrt2 * K(l, -m) * sin(-m * phi) * gradP(l, -m, cos(theta)) * (-sin(theta));
}




//Calculate n! (n>=0)
int Factorial(int n)
{
	if(n<=1)
		return 1;

	int result=n;

	while(--n > 1)
		result*=n;

	return result;
}
