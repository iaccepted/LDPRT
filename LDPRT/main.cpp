#include <iostream>
#include <Windows.h>
#include "Scene.h"
#include "Sampler.h"
#include "Renderer.h"
#include "Light.h"
#include "Global.h"
#include "Directions.h"

using namespace std;


int main()
{
	cout << "BAND_NUM: " << BAND_NUM << endl;
	cout << "LOBE_NUM: " << LOBE_NUM << endl;
	cout << "DIR_NUM: " << SQRT_DIR_NUM * SQRT_DIR_NUM  << endl;
	cout << "SAMPLE_NUM: " << SQRT_SAMPLES_NUM * SQRT_SAMPLES_NUM << endl;

	//sampling
	Sampler sampler;
	sampler.generateSamples();

	//create a set of direction
	Directions directions;
	directions.generateDir(SQRT_DIR_NUM);

	Light light;
	light.directLight(sampler);
	//light.lightFromImage("./Environments/galileo_probe.pfm", sampler);

	Scene scene;
	//scene.addModelFromFile("./models/cube.obj");
	scene.addModelFromFile("./models/001.obj");
	

	Scene deformedScene;
	deformedScene.addModelFromFile("./models/002.obj");

	//print_n(deformedScene);
	scene.generateCoeffsAndLobes(sampler, directions);
	deformedScene.generateDeformedLobes(&scene);

	Renderer renderer(600, 600);
	renderer.compileShaderFromFile("./shaders/verShader.glsl", "./shaders/fragShader.glsl");
	renderer.setUniform();
	//renderer.renderSceneWithLight(scene, light);
	renderer.renderSceneWithLight(deformedScene, light);

	return 0;
}