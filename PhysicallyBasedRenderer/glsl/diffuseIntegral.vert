#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec3 color;

out vec3 worldPos;

uniform mat4 viewMatrix;
uniform mat4 projectMatrix;

void main(){
	worldPos = position;
	gl_Position = projectMatrix * viewMatrix * vec4(position,1.0f);
}