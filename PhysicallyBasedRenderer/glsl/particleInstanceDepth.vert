#version 330 core
layout (location = 0) in vec3 position;
layout (location = 4) in vec4 particle;

uniform mat4 lightSpaceMatrix;
uniform mat4 modelMatrix;

void main()
{
	vec3 FragPos = vec3(modelMatrix * vec4(position,1.0f));
 	gl_Position = lightSpaceMatrix * modelMatrix * vec4(FragPos + particle.xyz, 1.0f);
}