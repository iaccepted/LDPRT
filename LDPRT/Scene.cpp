#include "Scene.h"

bool Scene::addModelFromFile(const char* path)
{
	Object* obj = new Object();
	obj->load(path);
	unsigned index = objects.size() + 1;

	cout << "Model " << index << ": vertices = " << obj->vertices.size() << ", triangles = " << obj->indices.size() / 3 << endl;
	numAllVertices += obj->vertices.size();
	numAllIndices += obj->indices.size();
	objects.push_back(obj);
	return true;
}

bool Scene::generateCoeffs(Sampler &sampler)
{
	if (!initCoeffs(sampler))return false;
	this->generateDirectCoeffs(sampler);

	//bounce
	for (int i = 1; i <= BOUNCE_NUM; ++i)
	{
		this->generateCoeffsDS(sampler, i);
	}
	return true;
}

bool Scene::initCoeffs(Sampler &sampler)
{
	int nFuncs = BAND_NUM * BAND_NUM;

	unsigned nObjs = objects.size();
	for (unsigned objIdx = 0; objIdx < nObjs; ++objIdx)
	{
		Object *curObject = objects[objIdx];
		unsigned nVertices = curObject->vertices.size();

		for (unsigned verIdx = 0; verIdx < nVertices; ++verIdx)
		{
			Vertex &curVertex = curObject->vertices[verIdx];
			curVertex.blockIdx = new vec2[sampler.size()];
			curVertex.isBlocked = new bool[sampler.size()];

			curVertex.unshadowedCoeffs = new LFLOAT[nFuncs];

			for (unsigned i = 0; i < 4; ++i)
			{
				curVertex.shadowedCoeffs[i] = new LFLOAT[nFuncs];
			}

			if (NULL == curVertex.unshadowedCoeffs || NULL == curVertex.shadowedCoeffs[3])
			{
				cerr << "Failed to allocate memory for coefficients." << endl;
				return false;
			}

			for (int i = 0; i < nFuncs; ++i)
			{
				curVertex.unshadowedCoeffs[i] = 0.0f;

				for (int j = 0; j < 4; ++j)
				{
					curVertex.shadowedCoeffs[j][i] = 0.0f;
				}
			}
		}
	}
	return true;
}

void Scene::generateDirectCoeffs(Sampler &sampler)
{
	const unsigned nFuncs = BAND_NUM * BAND_NUM;
	const unsigned nSamples = sampler.size();

	cout << "Compute unshadowed and shadowed SH." << endl;
	unsigned interval = 0, curNumVertices = 0;

	unsigned nObjs = objects.size();
	for (unsigned objIdx = 0; objIdx < nObjs; ++objIdx)
	{
		Object *curObject = objects[objIdx];
		unsigned nVertices = curObject->vertices.size();

		for (unsigned verIdx = 0; verIdx < nVertices; ++verIdx, ++curNumVertices)
		{

			if (curNumVertices == interval)
			{
				cout << curNumVertices * 100 / numAllVertices << "% ";
				interval += numAllVertices / 10;
			}
			Vertex &curVertex = objects[objIdx]->vertices[verIdx];

			std::vector<SAMPLE> &samples = sampler.samples;
			for (unsigned sampleIdx = 0; sampleIdx < nSamples; ++sampleIdx)
			{
				double cosine = glm::dot(samples[sampleIdx].xyz, curVertex.normal);

				if (cosine > 0.0f)
				{
					Ray ray(curVertex.position + 2 * EPSILON * curVertex.normal,
						samples[sampleIdx].xyz);

					unsigned hitObjectIdx, hitTriangleIdx;
					bool blocked = isRayBlocked(ray, &hitObjectIdx, &hitTriangleIdx);
					curVertex.isBlocked[sampleIdx] = blocked;
					if (blocked)
					{
						curVertex.blockIdx[sampleIdx] = vec2(hitObjectIdx, hitTriangleIdx);
					}

					for (unsigned l = 0; l < nFuncs; ++l)
					{
						double contribution = cosine * samples[sampleIdx].shValues[l];
						curVertex.unshadowedCoeffs[l] += contribution;

						if (!blocked)
						{
							curVertex.shadowedCoeffs[0][l] += contribution;
						}
					}
				}
			}

			//rescale the coefficients
			double scale = 4 * M_PI / nSamples;
			for (unsigned l = 0; l < nFuncs; ++l)
			{
				curVertex.unshadowedCoeffs[l] *= scale;
				curVertex.shadowedCoeffs[0][l] *= scale;
			}
		}
	}
}

void Scene::generateCoeffsDS(Sampler& sampler, int bounceTime)
{
	assert(bounceTime >= 1 && bounceTime <= 3);
	const unsigned nFuncs = BAND_NUM * BAND_NUM;
	const unsigned nSamples = sampler.size();

	unsigned nObjects = objects.size();

	cout << endl;
	cout << "Compute shadowed bounce " << bounceTime << " SH." << endl;
	unsigned interval = 0, curNumVertices = 0;

	for (unsigned objIdx = 0; objIdx < nObjects; objIdx++)
	{
		const unsigned nVertices = objects[objIdx]->vertices.size();

		for (unsigned int verIdx = 0; verIdx < nVertices; ++verIdx)
		{
			Vertex& curVertex = objects[objIdx]->vertices[verIdx];
			for (unsigned int j = 0; j < nFuncs; j++) {
				curVertex.shadowedCoeffs[bounceTime][j] = curVertex.shadowedCoeffs[bounceTime - 1][j];
			}
		}

		for (unsigned verIdx = 0; verIdx < nVertices; ++verIdx, ++curNumVertices)
		{
			if (curNumVertices == interval)
			{
				cout << curNumVertices * 100 / numAllVertices << "% ";
				interval += numAllVertices / 10;
			}
			Vertex& curVertex = objects[objIdx]->vertices[verIdx];

			for (unsigned sampleIdx = 0; sampleIdx < nSamples; ++sampleIdx)
			{
				if (curVertex.isBlocked[sampleIdx]) {
					int objIndex = curVertex.blockIdx[sampleIdx][0];
					int triangleIndex = curVertex.blockIdx[sampleIdx][1];

					if (triangleIndex == 0)continue;

					int vidx0 = objects[objIndex]->indices[triangleIndex * 3];
					int vidx1 = objects[objIndex]->indices[triangleIndex * 3 + 1];
					int vidx2 = objects[objIndex]->indices[triangleIndex * 3 + 2];
					Vertex& v0 = objects[objIndex]->vertices[vidx0];
					Vertex& v1 = objects[objIndex]->vertices[vidx1];
					Vertex& v2 = objects[objIndex]->vertices[vidx2];
					float fScale = 0.00001f;
					float cosineTerm0 = -glm::dot(sampler[sampleIdx].xyz, v0.normal);
					float cosineTerm1 = -glm::dot(sampler[sampleIdx].xyz, v1.normal);
					float cosineTerm2 = -glm::dot(sampler[sampleIdx].xyz, v2.normal);
					for (unsigned k = 0; k < nFuncs; k++) 
					{
						v0.shadowedCoeffs[bounceTime][k] += fScale * curVertex.shadowedCoeffs[bounceTime - 1][k] * cosineTerm0;
						v1.shadowedCoeffs[bounceTime][k] += fScale * curVertex.shadowedCoeffs[bounceTime - 1][k] * cosineTerm1;
						v2.shadowedCoeffs[bounceTime][k] += fScale * curVertex.shadowedCoeffs[bounceTime - 1][k] * cosineTerm2;
					}
				}
			}
		}
	}
}

bool Scene::isRayBlocked(Ray& ray, unsigned *objIdx, unsigned *triangleIdx)
{
	unsigned nObjs = objects.size();
	for (unsigned i = 0; i < nObjs; ++i) {
		bool ret = objects[i]->doesRayHitObject(ray, triangleIdx);
		if (ret)
		{
			if (objIdx) *objIdx = i;
			return true;
		}
	}
	return false;
}

bool Scene::generateLobes(Sampler &sampler, Directions &dirs)
{
	int nSamples = sampler.size();
	int nFuncs = BAND_NUM * BAND_NUM;
	int nDirs = dirs.size();

	cout << endl;
	cout << "Compute lobes." << endl;
	unsigned interval = 0, curNumVertices = 0;

	LFLOAT *tCoeffs = new LFLOAT[nFuncs];
	LFLOAT *tApproximateCoeffs = new LFLOAT[nFuncs];

	SHEvalFunc SHEval[] = { SHEval3, SHEval3, SHEval3, SHEval3, SHEval4, SHEval5, SHEval6, SHEval7, SHEval8, SHEval9, SHEval10 };

	std::vector<SAMPLE> &samples = sampler.samples;
	for (int i = 0; i < nSamples; ++i)
	{
		BFGS::samples[i] = samples[i];
	}

	int nObjs = this->objects.size();
	for (int objIdx = 0; objIdx < nObjs; ++objIdx)
	{
		Object *curObj = this->objects[objIdx];
		int nVertices = curObj->vertices.size();

		for (int vertexIdx = 0; vertexIdx < nVertices; ++vertexIdx, ++curNumVertices)
		{

			if (curNumVertices == interval)
			{
				cout << curNumVertices * 100 / numAllVertices << "% ";
				interval += numAllVertices / 10;
			}
			Vertex &curVertex = curObj->vertices[vertexIdx];
			//this
			memcpy(tCoeffs, curVertex.shadowedCoeffs[0], nFuncs * sizeof(LFLOAT));

			for (int lobeIdx = 0; lobeIdx < LOBE_NUM; ++lobeIdx)
			{
				curVertex.lobes[lobeIdx] = new LFLOAT[nFuncs + 3];

				memcpy(BFGS::tlm, tCoeffs, nFuncs * sizeof(LFLOAT));

				lbfgsfloatval_t minFx = FLT_MAX;
				for (int dirIdx = 0; dirIdx < nDirs; ++dirIdx)
				{
					DIR &dir = dirs.directions[dirIdx];

					BFGS::init();

					BFGS::x[0] = dir.xyz.x;
					BFGS::x[1] = dir.xyz.y;
					BFGS::x[2] = dir.xyz.z;

					int index = 3;

					for (int l = 0; l < BAND_NUM; ++l)
					{
						int m;
						double numerator = 0;
						double denominator = 0;

						for (m = -l; m <= l; ++m)
						{
							//this
							numerator += curVertex.shadowedCoeffs[0][INDEX(l, m)] * dir.shValues[INDEX(l, m)];
						}
						denominator = (2.0 * l + 1) / (4 * M_PI);
						for (int j = 0; j < 2 * l + 1; ++j)
						{
							BFGS::x[index + j] = numerator / denominator;
						}
						index += 2 * l + 1;
					}

					lbfgsfloatval_t fx;
					BFGS::bfgs(&fx);

					if (fx < minFx)
					{
						LFLOAT x = BFGS::x[0], y = BFGS::x[1], z = BFGS::x[2];
						LFLOAT distance = sqrt(x * x + y * y + z * z);

						BFGS::x[0] = x / distance;
						BFGS::x[1] = y / distance;
						BFGS::x[2] = z / distance;

						minFx = fx;
						for (int i = 0; i < nFuncs + 3; ++i)
						{
							curVertex.lobes[lobeIdx][i] = BFGS::x[i];
						}
					}
				}//dir end

				LFLOAT x = curVertex.lobes[lobeIdx][0], y = curVertex.lobes[lobeIdx][1], z = curVertex.lobes[lobeIdx][2];
				SHEval[BAND_NUM](x, y, z, tApproximateCoeffs);

				for (int i = 0; i < nFuncs; ++i)
				{
					tApproximateCoeffs[i] *= curVertex.lobes[lobeIdx][i + 3];
					tCoeffs[i] -= tApproximateCoeffs[i];
				}
			}//lobe end
		}//vertex end
	}//obj end

	delete[] tCoeffs;
	delete[] tApproximateCoeffs;
	return true;
}