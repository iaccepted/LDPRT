#ifndef _GLOBAL_H_
#define _GLOBAL_H_


#include <iostream>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <vector>
#include <time.h>
#include <float.h>
#include <GL/glew.h>
#include <algorithm>

using std::cout;
using std::endl;
using std::cerr;
using std::string;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;

#ifndef LOBE_NUM
#define LOBE_NUM 4
#endif

#ifndef BAND_NUM
#define BAND_NUM 3
#endif

#ifndef SQRT_SAMPLES_NUM
#define SQRT_SAMPLES_NUM 50
#endif

#ifndef SQRT_DIR_NUM
#define SQRT_DIR_NUM 10
#endif

#ifndef BOUNCE_NUM
#define BOUNCE_NUM 3
#endif

#define INDEX(l, m) (l * (l + 1) + m)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef EPSILON
#define EPSILON 0.01f
#endif

//#define DEBUG_RGB
#define DEBUG_LOBE
#define DEBUG_ORIGINAL 0

typedef float LFLOAT;

#endif