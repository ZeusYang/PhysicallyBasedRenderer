#version 430 core

in vec3 Color;

layout(location = 0) out vec3 dposition;
layout(location = 1) out vec3 dnormal;
layout(location = 2) out vec3 dalbedo;
layout(location = 3) out vec3 droughness;

void main(){	
	vec3 normal;
	normal.xy = gl_PointCoord.xy * vec2(2.0, -2.0) + vec2(-1.0,1.0);
	float mag = dot(normal.xy, normal.xy);
	if(mag > 1.0) discard;
	
	dposition.rgb = vec3(0.0f);
	dalbedo.rgb = Color;
	dnormal.rgb = vec3(0.0f);
	droughness.r = 0.0f;
	droughness.g = 1.0f;
	droughness.b = gl_FragCoord.z;
}