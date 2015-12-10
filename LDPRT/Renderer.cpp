#include "Renderer.h"


LIGHTING_TYPE Renderer::lightType = LIGHTING_TYPE_SH_UNSHADOWED;
SHOW_TYPE Renderer::showType = SHOW_TYPE_ORIGINAL;
double Renderer::theta = 101.7f;
double Renderer::phi = 90.7f;
//double PRTRenderer::theta = 0.0f;
//double PRTRenderer::phi = 0.0f;
double Renderer::preMouseX = 0.0f;
double Renderer::preMouseY = 0.0f;
bool Renderer::isNeedRotate = false;
bool Renderer::isLeftButtonPress = false;

Renderer::Renderer(int _width, int _height) : width(_width), height(_height)
{
	initGLFW();
	initGL();
}

void Renderer::initGL()
{
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (err != GLEW_OK)
	{
		cerr << "Failed to init glew : " << glewGetErrorString(err) << endl;
		exit(-1);
	}

	glViewport(0, 0, width, height);
	/* Enable smooth shading */
	glShadeModel(GL_SMOOTH);
	/* Set the background */
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	/* Depth buffer setup */
	glClearDepth(1.0f);
	/* Enables Depth Testing */
	glEnable(GL_DEPTH_TEST);
	/* The Type Of Depth Test To Do */
	glDepthFunc(GL_LEQUAL);
	///* Really Nice Perspective Calculations */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	modelMatrix = mat4(1.0f);
	//为渲染人脸新加的
	modelMatrix = glm::scale(modelMatrix, vec3(0.2f, 0.2f, 0.2f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), vec3(0.0, 1.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(25.0f), vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::translate(modelMatrix, vec3(0.0f, -360.0f, 0.0f));
	//modelMatrix = glm::translate(modelMatrix, vec3(0.0f, 0.0f, -200.0f));
	

	//正常的
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), vec3(1.0, 0.0, 0.0));
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(-50.0f), vec3(0.0, 1.0, 0.0));

	//modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), vec3(0.0, 1.0, 0.0));

	viewMatrix = glm::lookAt(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, -10.0f), vec3(0.0f, 1.0f, 0.0f));
}

void Renderer::initGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(width, height, "LDPRT", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
}


void Renderer::renderSceneWithLight(Scene &scene, Light &light)
{
	light.rotateLightCoeffs(theta, phi);
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		if (isNeedRotate && isLeftButtonPress)
		{
			cout << theta << "  " << phi << endl;
			light.rotateLightCoeffs(theta, phi);
			isNeedRotate = false;
		}


		this->precomputeColor(scene, light);

		unsigned nObjects = scene.objects.size();

		GLuint vao, vbo, ebo;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (unsigned i = 0; i < nObjects; ++i)
		{
			Object *curObject = scene.objects[i];

			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ebo);

			glBindVertexArray(vao);

			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* curObject->vertices.size(), &curObject->vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* curObject->indices.size(), &curObject->indices[0], GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, position));
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, litColor));
			glEnableVertexAttribArray(1);

			glDrawElements(GL_TRIANGLES, scene.numAllIndices, GL_UNSIGNED_INT, 0);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &vbo);
			glDeleteBuffers(1, &ebo);
		}
		glfwSwapBuffers(window);
	}
	glfwTerminate();
}



void Renderer::compileShaderFromFile(const char *verFileName, const char *fragFileName)
{
	bool ret = prog.compileShaderFromFile(verFileName, GLSLShader::VERTEX);
	if (!ret)
	{
		cerr << "Compile Vertex Shader Error: " << prog.log() << endl;
		exit(-1);
	}
	ret = prog.compileShaderFromFile(fragFileName, GLSLShader::FRAGMENT);
	if (!ret)
	{
		cerr << "Compile Fragment Shader Error: " << prog.log() << endl;
		exit(-1);
	}

	prog.bindAttribLocation(0, "position");
	prog.bindAttribLocation(1, "color");

	ret = prog.link();
	if (!ret)
	{
		cerr << "Link Error: " << prog.log() << endl;
		exit(-1);
	}

	ret = prog.validate();
	if (!ret)
	{
		cerr << "Validate Error: " << prog.log() << endl;
		exit(-1);
	}

	prog.use();
}

void Renderer::setUniform()
{
	glfwGetFramebufferSize(window, &width, &height);
	mat4 projection = glm::perspective(45.0f, float(width) / height, 0.1f, 1000.0f);
	mat4 mv = viewMatrix * modelMatrix;

	prog.setUniform("MVP", projection * mv);
}

void Renderer::precomputeColor(Scene &scene, Light &light)
{
	unsigned nObjects = scene.objects.size();
	unsigned nFuncs = BAND_NUM * BAND_NUM;

	LFLOAT rmsr = 0.0, rmsg = 0.0, rmsb = 0.0;

	SHEvalFunc SHEval[] = { SHEval3, SHEval3, SHEval3, SHEval3, SHEval4, SHEval5, SHEval6, SHEval7, SHEval8, SHEval9, SHEval10 };

	LFLOAT *tApproximateCoeffs = new LFLOAT[nFuncs];
	LFLOAT *tShValues = new LFLOAT[nFuncs];

	if (lightType == LIGHTING_TYPE_SH_UNSHADOWED)
	{
		for (unsigned objIdx = 0; objIdx < nObjects; ++objIdx)
		{
			Object *curObject = scene.objects[objIdx];

			unsigned nVertices = curObject->vertices.size();

			for (unsigned verIdx = 0; verIdx < nVertices; ++verIdx)
			{
				Vertex &curVertex = curObject->vertices[verIdx];

				//memset(tApproximateCoeffs, 0, nFuncs * sizeof(LFLOAT));
				////fit SH coeffs
				//for (unsigned lobeIdx = 0; lobeIdx < LOBE_NUM; ++lobeIdx)
				//{
				//	LFLOAT *lobe = curVertex.lobes[lobeIdx];
				//	SHEval[BAND_NUM](lobe[0], lobe[1], lobe[2], tShValues);
				//	for (unsigned l = 0; l < nFuncs; ++l)
				//	{
				//		tApproximateCoeffs[l] += curVertex.lobes[lobeIdx][l + 3] * tShValues[l];
				//	}
				//}
				
				vec3 litColor = vec3(0.0f);
				for (unsigned l = 0; l < nFuncs; ++l)
				{
					litColor.r += light.rotatedCoeffs[0][l] * curVertex.unshadowedCoeffs[l];
					litColor.g += light.rotatedCoeffs[1][l] * curVertex.unshadowedCoeffs[l];
					litColor.b += light.rotatedCoeffs[2][l] * curVertex.unshadowedCoeffs[l];
				}

				curVertex.litColor = litColor;
			}
		}
	}
	else if (lightType == LIGHTING_TYPE_SH_SHADOWED)
	{
		for (unsigned objIdx = 0; objIdx < nObjects; ++objIdx)
		{
			Object *curObject = scene.objects[objIdx];

			unsigned nVertices = curObject->vertices.size();

			for (unsigned verIdx = 0; verIdx < nVertices; ++verIdx)
			{
				Vertex &curVertex = curObject->vertices[verIdx];

				memset(tApproximateCoeffs, 0, nFuncs * sizeof(LFLOAT));
				//fit SH coeffs
				for (unsigned lobeIdx = 0; lobeIdx < LOBE_NUM; ++lobeIdx)
				{
					LFLOAT *lobe = curVertex.lobes[lobeIdx];
					SHEval[BAND_NUM](lobe[0], lobe[1], lobe[2], tShValues);

					for (unsigned l = 0; l < nFuncs; ++l)
					{
						tApproximateCoeffs[l] += curVertex.lobes[lobeIdx][l + 3] * tShValues[l];
					}
				}

				vec3 litColor = vec3(0.0f, 0.0f, 0.0f);
				LFLOAT or = 0.0, og = 0.0, ob = 0.0;

				for (unsigned l = 0; l < nFuncs; ++l)
				{
					or += light.rotatedCoeffs[0][l] * curVertex.shadowedCoeffs[0][l];
					og += light.rotatedCoeffs[1][l] * curVertex.shadowedCoeffs[0][l];
					ob += light.rotatedCoeffs[2][l] * curVertex.shadowedCoeffs[0][l];
				}
				for (unsigned l = 0; l < nFuncs; ++l)
				{
					litColor.r += light.rotatedCoeffs[0][l] * tApproximateCoeffs[l];
					litColor.g += light.rotatedCoeffs[1][l] * tApproximateCoeffs[l];
					litColor.b += light.rotatedCoeffs[2][l] * tApproximateCoeffs[l];
				}

				rmsr += (litColor.r - or) * (litColor.r - or);
				rmsg += (litColor.g - og) * (litColor.g - og);
				rmsb += (litColor.b - ob) * (litColor.b - ob);


				if (showType == SHOW_TYPE_ORIGINAL)
				{
					litColor = vec3(or, og, ob);
				}
				

				curVertex.litColor = litColor;				
			}
		}
		static bool mark = true;
		if (mark)
		{
			rmsr = sqrt(rmsr / scene.numAllVertices);
			rmsg = sqrt(rmsg / scene.numAllVertices);
			rmsb = sqrt(rmsg / scene.numAllVertices);

			cout << endl;
			cout << "RMS_R: " << rmsr << endl;
			cout << "RMS_G: " << rmsg << endl;
			cout << "RMS_B: " << rmsb << endl;
			mark = false;
		}
		
	}
	else
	{
		int bounceTime = lightType - 1;
		for (unsigned objIdx = 0; objIdx < nObjects; ++objIdx)
		{
			Object *curObject = scene.objects[objIdx];

			unsigned nVertices = curObject->vertices.size();

			for (unsigned verIdx = 0; verIdx < nVertices; ++verIdx)
			{
				Vertex &curVertex = curObject->vertices[verIdx];

				vec3 litColor = vec3(0.0f, 0.0f, 0.0f);

				unsigned nFuncs = BAND_NUM * BAND_NUM;
				for (unsigned l = 0; l < nFuncs; ++l)
				{
					litColor.r += light.rotatedCoeffs[0][l] * curVertex.shadowedCoeffs[bounceTime][l];
					litColor.g += light.rotatedCoeffs[1][l] * curVertex.shadowedCoeffs[bounceTime][l];
					litColor.b += light.rotatedCoeffs[2][l] * curVertex.shadowedCoeffs[bounceTime][l];
				}

				curVertex.litColor = litColor;
			}
		}
	}
}


void Renderer::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_1:
		lightType = LIGHTING_TYPE_SH_UNSHADOWED;
		break;
	case GLFW_KEY_2:
		lightType = LIGHTING_TYPE_SH_SHADOWED;
		break;
	case GLFW_KEY_3:
		lightType = LIGHTING_TYPE_SH_SHADOWED_BOUNCE_1;
		break;
	case GLFW_KEY_4:
		lightType = LIGHTING_TYPE_SH_SHADOWED_BOUNCE_2;
		break;
	case GLFW_KEY_5:
		lightType = LIGHTING_TYPE_SH_SHADOWED_BOUNCE_3;
		break;
	case GLFW_KEY_O:
		showType = SHOW_TYPE_ORIGINAL;
		break;
	case GLFW_KEY_F:
		showType = SHOW_TYPE_FIT;
		break;
	}
}

void Renderer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button != GLFW_MOUSE_BUTTON_LEFT)return;
	if (action == GLFW_PRESS) {
		isLeftButtonPress = true;
	}
	else {
		isLeftButtonPress = false;
	}
}

void Renderer::cursorPositionCallback(GLFWwindow* window, double x, double y)
{
	/*theta += ((y - preMouseY) * 0.1f);
	if (theta < 0.0f)theta = 0.0f;
	if (theta > 180.0f)theta = 180.0f;
	phi += ((x - preMouseX) * 0.1);

	preMouseX = x;
	preMouseY = y;

	isNeedRotate = true;*/
}
Renderer::~Renderer()
{

}