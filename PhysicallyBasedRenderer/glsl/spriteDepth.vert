#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 lightSpaceMatrix;
uniform mat4 modelMatrix;
uniform float pointSize;

void main()
{
	vec3 eyeSpacePos = (lightSpaceMatrix * modelMatrix * vec4(position.xyz, 1.0f)).xyz;
	gl_PointSize = max(1.0f, 8.0f * pointSize / (1.0f - eyeSpacePos.z));
	gl_Position = lightSpaceMatrix * modelMatrix * vec4(position, 1.0f);
}