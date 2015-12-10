/***************************************
*author: guohongzhi  zju
*date:2015.8.22
*func: load single object
****************************************/

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Global.h"
#include "Vertex.h"
#include "Ray.h"
#include "AABB.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Object
{
public:
	Object(){}
	void loadA(const char *path);
	void loadB(const char *path);
	bool doesRayHitObject(Ray& ray, unsigned *vertexIdx) const;

	void bindBuffer();
	void render();

	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	AABB aabb;

private:
	GLuint vao, vbo, ebo;
};

#endif 