#include "Light.h"

Light::Light()
{
	unsigned nFuncs = BAND_NUM * BAND_NUM;
	for (unsigned i = 0; i < 3; ++i)
	{
		coeffs[i] = new double[nFuncs];
		rotatedCoeffs[i] = new double[nFuncs];
	}
}

Light::~Light()
{
	unsigned nFuncs = BAND_NUM * BAND_NUM;

	for (unsigned i = 0; i < 3; ++i)
	{
		if (coeffs[i] != NULL)
		{
			delete[] coeffs[i];
			coeffs[i] = NULL;
		}
		
		if (rotatedCoeffs[i] != NULL)
		{
			delete[] rotatedCoeffs[i];
			rotatedCoeffs[i] = NULL;
		}
	}
}

double Light::lightIntensity(double theta, double phi)
{
	return theta < (M_PI / 6) ? 1 : 0;
}

void Light::directLight(const Sampler &sampler)
{
	unsigned nFuncs = BAND_NUM * BAND_NUM;
	unsigned nSamples = sampler.size();
	const std::vector<SAMPLE> &samples = sampler.samples;

	for (unsigned i = 0; i < nFuncs; ++i)
	{
		coeffs[0][i] = 0.0f;
		coeffs[1][i] = 0.0f;
		coeffs[2][i] = 0.0f;

		for (unsigned j = 0; j < nSamples; ++j)
		{
			coeffs[0][i] += lightIntensity(samples[j].theta, samples[j].phi) * samples[j].shValues[i];
			coeffs[1][i] += lightIntensity(samples[j].theta, samples[j].phi) * samples[j].shValues[i];
			coeffs[2][i] += lightIntensity(samples[j].theta, samples[j].phi) * samples[j].shValues[i];
		}

		coeffs[0][i] *= (4 * M_PI / nSamples);
		coeffs[1][i] *= (4 * M_PI / nSamples);
		coeffs[2][i] *= (4 * M_PI / nSamples);
	}

	cout << (4 * M_PI / nSamples) << endl;
}

void LightProbeAccess(vec3* color, Image* image, vec3& direction)
{
	float d = sqrt(direction.x*direction.x + direction.y*direction.y);
	float r = (d == 0) ? 0.0f : (1.0f / M_PI / 2.0f) * acos(direction.z) / d;
	float tex_coord[2];
	tex_coord[0] = 0.5f + direction.x * r;
	tex_coord[1] = 0.5f + direction.y * r;
	int pixel_coord[2];
	pixel_coord[0] = int(tex_coord[0] * image->width);
	pixel_coord[1] = int(tex_coord[1] * image->height);
	int pixel_index = pixel_coord[1] * image->width + pixel_coord[0];
	color->r = image->pixel[0][pixel_index];
	color->g = image->pixel[1][pixel_index];
	color->b = image->pixel[2][pixel_index];
}

void Light::lightFromImage(const char* imagePath, Sampler& sampler, float lightIntensity)
{
	Image img;
	img.loadFromFile(imagePath);

	int nFuncs = BAND_NUM * BAND_NUM;

	for (int i = 0; i < nFuncs; i++) {
		coeffs[0][i] = 0.0f;
		coeffs[1][i] = 0.0f;
		coeffs[2][i] = 0.0f;
	}


	for (unsigned i = 0; i < sampler.size(); ++i) {
		vec3& direction = sampler.samples[i].xyz;
		for (int j = 0; j < nFuncs; ++j) {
			vec3 color;
			LightProbeAccess(&color, &img, direction);
			float shFunction = sampler.samples[i].shValues[j];
			coeffs[0][j] += (color.r * shFunction);
			coeffs[1][j] += (color.g * shFunction);
			coeffs[2][j] += (color.b * shFunction);
		}
	}
	float weight = 4.0f * M_PI * lightIntensity;
	float scale = weight / sampler.size();
	for (int i = 0; i < nFuncs; i++) {
		coeffs[0][i] *= scale;
		coeffs[1][i] *= scale;
		coeffs[2][i] *= scale;
	}
}

void Light::rotateLightCoeffs(const double theta, const double phi)
{
	rotateSHCoefficients(BAND_NUM, coeffs[0], rotatedCoeffs[0], theta, phi);
	rotateSHCoefficients(BAND_NUM, coeffs[1], rotatedCoeffs[1], theta, phi);
	rotateSHCoefficients(BAND_NUM, coeffs[2], rotatedCoeffs[2], theta, phi);
}



bool Image::loadFromFile(const char* filename)
{
	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(filename, 0);
	FIBITMAP* env = FreeImage_Load(fifmt, filename);
	if (!env) {
		printf("[Error] load image %s failed.\n", filename);
		return false;
	}
	FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(env);

	//env = FreeImage_ConvertTo24Bits(env);
	//env = FreeImage_ConvertToFloat(env);
	FIRGBF* pixels = (FIRGBF*)FreeImage_GetBits(env);
	height = FreeImage_GetWidth(env);
	width = FreeImage_GetHeight(env);

	pixel[0] = new float[height * width];
	pixel[1] = new float[height * width];
	pixel[2] = new float[height * width];

	float factor = 0.6f;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int idx = i*width + j;
			pixel[0][idx] = pixels[idx].red * factor;
			pixel[1][idx] = pixels[idx].green * factor;
			pixel[2][idx] = pixels[idx].blue * factor;
		}
	}
	return true;
}