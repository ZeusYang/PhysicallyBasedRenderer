#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec3 color;

out vec2 Texcoord;

void main(){
	Texcoord = texcoord;
	gl_Position = vec4(position,1.0f);
}