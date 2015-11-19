#include "Directions.h"

void DIR::init(const LFLOAT x, const LFLOAT y, const LFLOAT z, const LFLOAT theta, const LFLOAT phi)
{
	this->xyz = glm::vec3(x, y, z);
	this->theta = theta;
	this->phi = phi;

	calculateShValues();
}

void DIR::calculateShValues()
{
	SHEvalFunc SHEval[] = { SHEval3, SHEval3, SHEval3, SHEval3, SHEval4, SHEval5, SHEval6, SHEval7, SHEval8, SHEval9, SHEval10 };
	SHEval[BAND_NUM](xyz.x, xyz.y, xyz.z, this->shValues);
}

void Directions::generateDir(const int sqrtDirNum)
{

	this->directions.resize(sqrtDirNum * sqrtDirNum);

	unsigned index = 0;
	for (int i = 0; i < sqrtDirNum; ++i)
	{
		for (int j = 0; j < sqrtDirNum; ++j)
		{
			double a, b, theta, phi, x, y, z;
			
			a = (i + ((double)rand() / RAND_MAX)) / (double)sqrtDirNum;
			b = (j + ((double)rand() / RAND_MAX)) / (double)sqrtDirNum;

			theta = 2 * acos(sqrt(1 - a));
			phi = 2 * M_PI * b;
			x = sin(theta) * cos(phi);
			y = sin(theta) * sin(phi);
			z = cos(theta);
	
			directions[index].init(x, y, z, theta, phi);

			++index;
		}
		
	}
}

int Directions::size()
{
	return directions.size();
}