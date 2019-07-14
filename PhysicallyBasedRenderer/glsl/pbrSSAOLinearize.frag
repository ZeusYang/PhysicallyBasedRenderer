#version 430 core
in vec2 Texcoord;

uniform float nearPlane;
uniform float farPlane;
uniform sampler2D depthMap;

float linearizeDepth(float depth)
{
	float z = depth * 2.0f - 1.0f;
	return (2.0f * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main(){
	// sample depth.
	float depth = texture(depthMap, Texcoord).r;
	gl_FragDepth = linearizeDepth(depth)/farPlane;
}