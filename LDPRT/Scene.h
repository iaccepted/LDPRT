#ifndef _SCENE_H_
#define _SCENE_H_
#include "Global.h"
#include "Ray.h"
#include "Object.h"
#include "Sampler.h"
#include "BFGS.h"
#include "Directions.h"
#include <fstream>

class Scene
{
public:
	Scene() :numAllVertices(0), numAllIndices(0){}
	bool addModelFromFile(const char* path);
	bool generateCoeffsAndLobes(Sampler &sampler, Directions &dirs);
	
	bool isRayBlocked(Ray& ray, unsigned *objIdx, unsigned *vertexIdx);
	std::vector<Object*> objects;

	unsigned long numAllIndices;
	unsigned long numAllVertices;

private:

	bool allocMemories(Sampler &sampler);
	bool readCoeffsAndLobesFromFile(const char *path = "./coeffs.dat");
	bool writeCoeffsAndLobesToFile(const char *path = "./coeffs.dat");
	bool generateCoeffs(Sampler &sampler);
	bool generateLobes(Sampler &sampler, Directions &dirs);
	void generateDirectCoeffs(Sampler &sampler);
	void generateCoeffsDS(Sampler& sampler, int bounceTime);
};

#endif