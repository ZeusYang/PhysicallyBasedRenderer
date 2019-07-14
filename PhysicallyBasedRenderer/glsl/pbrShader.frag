#version 330 core
in vec3 FragPos;
in vec2 Texcoord;
in mat3 TBNMatrix;

uniform float nearPlane;
uniform float farPlane;
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D roughMap;
uniform sampler2D metallicMap;
uniform sampler2D depthMap;

layout(location = 0) out vec3 dposition;
layout(location = 1) out vec3 dnormal;
layout(location = 2) out vec3 dalbedo;
layout(location = 3) out vec3 droughness;

void main(){
	// sample albedo.
	vec3 albedo = texture(albedoMap, Texcoord).rgb;
	// sample normal.
	vec3 normal = normalize(2.0f * texture(normalMap, Texcoord).rgb - vec3(1.0f));
	normal = TBNMatrix * normal;
	// sample roughness.
	float roughness = texture(roughMap, Texcoord).r;
	// sample metallic.
	float metallic = texture(metallicMap, Texcoord).r;
	dposition = FragPos;
	dnormal = normal;
	dalbedo = albedo;
	droughness = vec3(roughness, metallic, gl_FragCoord.z);
}