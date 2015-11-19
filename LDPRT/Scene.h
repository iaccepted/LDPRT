#ifndef _SCENE_H_
#define _SCENE_H_
#include "Global.h"
#include "Ray.h"
#include "Object.h"
#include "Sampler.h"
#include "BFGS.h"
#include "Directions.h"

class Scene
{
public:
	Scene() :numAllVertices(0), numAllIndices(0){}
	bool addModelFromFile(const char* path);
	bool generateCoeffs(Sampler &sampler);
	bool generateLobes(Sampler &sampler, Directions &dirs);
	//void bindBuffer();

	bool isRayBlocked(Ray& ray, unsigned *objIdx, unsigned *vertexIdx);
	std::vector<Object*> objects;

	GLuint VB;
	GLuint IB;
	unsigned long numAllIndices;

	unsigned long numAllVertices;

private:

	bool initCoeffs(Sampler &sampler);
	void generateDirectCoeffs(Sampler &sampler);
	void generateCoeffsDS(Sampler& sampler, int bounceTime);
};

#endif