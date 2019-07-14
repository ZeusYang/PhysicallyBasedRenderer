#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectMatrix;
uniform float pointScale;
uniform float pointSize;

out vec3 Color;

void main(){
	Color = color;
	vec3 eyeSpacePos = (viewMatrix * modelMatrix * vec4(position.xyz, 1.0f)).xyz;
	gl_PointSize = max(-pointScale * pointSize / eyeSpacePos.z, 0.0f);
	gl_Position = projectMatrix * viewMatrix * modelMatrix * vec4(position.xyz, 1.0f);
}