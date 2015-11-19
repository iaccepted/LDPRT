#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "Global.h"

class Vertex
{
public:
	glm::vec3 position;
	glm::vec3 normal;

	//Color of vertex including lighting
	glm::vec3 litColor;

	//Diffuse material color
	glm::vec3 diffuseMaterial;

	//SH coefficients for transfer function
	LFLOAT *unshadowedCoeffs;
	LFLOAT *shadowedCoeffs[4];

	LFLOAT *lobes[LOBE_NUM];

	bool *isBlocked;	// true if the ray in this direction is blocked;
	glm::vec2 *blockIdx;		// 0 store the obj index and 1 store the triangle index

	Vertex() : litColor(1.f, 1.f, 1.f), unshadowedCoeffs(NULL), diffuseMaterial(1.f, 1.f, 1.f), isBlocked(NULL)
	{
		for (int i = 0; i < 4; i++)
		{
			shadowedCoeffs[i] = NULL;
		}

		for (int i = 0; i < LOBE_NUM; ++i)
		{
			lobes[i] = NULL;
		}
	}

	Vertex(float px, float py, float pz, float nx, float ny, float nz)
		: position(px, py, pz), normal(nx, ny, nz), litColor(1.f, 1.f, 1.f), unshadowedCoeffs(NULL),
		diffuseMaterial(1.f, 1.f, 1.f), isBlocked(NULL), blockIdx(NULL)
	{
		normal = glm::normalize(normal);
		for (int i = 0; i < 4; i++)
			shadowedCoeffs[i] = NULL;

		for (int i = 0; i < LOBE_NUM; ++i)
		{
			lobes[i] = NULL;
		}
	}

	~Vertex() {
		delete[] unshadowedCoeffs;
		unshadowedCoeffs = NULL;

		for (int i = 0; i < 4; i++) {
			delete[] shadowedCoeffs[i];
			shadowedCoeffs[i] = NULL;
		}

		for (int i = 0; i < LOBE_NUM; ++i)
		{
			if (lobes[i] != NULL)
			{
				delete[] lobes[i];
				lobes[i] = NULL;
			}
			
		}
		delete[] isBlocked;
		delete[] blockIdx;

		isBlocked = NULL;
		blockIdx = NULL;
	}
};

#endif