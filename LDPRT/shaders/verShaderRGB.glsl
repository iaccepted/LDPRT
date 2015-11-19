#version 430 core

in vec3 position;
in vec3 color;


uniform mat4 MVP;
out vec3 fcolor;

vec3 lightDir = vec3(0.0f, 0.0f, 1.0f);
vec3 lightColor = vec3(0.6, 0.6, 0.6);


void main()
{
	gl_Position = MVP * vec4(position, 1.0);
	
	
	fcolor = color;
}