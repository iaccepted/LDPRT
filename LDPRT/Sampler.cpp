#include "Sampler.h"
#include "SHEval.h"
#include "Global.h"

SAMPLE::SAMPLE(const SAMPLE &sample)
{
	int nSamples = SQRT_SAMPLES_NUM * SQRT_SAMPLES_NUM;
	this->xyz = sample.xyz;
	this->theta = sample.theta;
	this->phi = sample.phi;
	memcpy(this->shValues, sample.shValues, nSamples * sizeof(LFLOAT));
}

void Sampler::generateSamples()
{
	int numSamples = SQRT_SAMPLES_NUM * SQRT_SAMPLES_NUM;
	int numFunctions = BAND_NUM * BAND_NUM;
	SHEvalFunc SHEval[] = { SHEval3, SHEval3, SHEval3, SHEval3, SHEval4, SHEval5, SHEval6, SHEval7, SHEval8, SHEval9, SHEval10 };

	samples.resize(numSamples);

	//srand(unsigned(time(NULL)));

	unsigned index = 0;
	for (int i = 0; i < SQRT_SAMPLES_NUM; ++i)
	{
		for (int j = 0; j < SQRT_SAMPLES_NUM; ++j)
		{
			double a = (i + ((double)rand() / RAND_MAX)) / (double)SQRT_SAMPLES_NUM;
			double b = (j + ((double)rand() / RAND_MAX)) / (double)SQRT_SAMPLES_NUM;

			double theta = 2.0 * acos(sqrt(1.0 - a));
			double phi = 2.0 * M_PI * b;
			float x = float(sin(theta) * cos(phi));
			float y = float(sin(theta) * sin(phi));
			float z = float(cos(theta));
			samples[index].xyz = glm::vec3(x, y, z);
			samples[index].phi = phi;
			samples[index].theta = theta;


			SHEval[BAND_NUM](x, y, z, samples[index].shValues);

			/*for (int l = 0; l < BAND_NUM; ++l)
			{
				for (int m = -l; m <= l; ++m)
				{
					int index2 = l*(l + 1) + m;

					samples[index].shValues[index2] = SH(l, m, theta, phi);
				}
			}*/

			++index;
		}

	}
}

unsigned Sampler::size() const
{
	return this->samples.size();
}

SAMPLE &Sampler::operator[](unsigned int index)
{
	assert(index >= 0 && index < samples.size());
	return samples[index];
}