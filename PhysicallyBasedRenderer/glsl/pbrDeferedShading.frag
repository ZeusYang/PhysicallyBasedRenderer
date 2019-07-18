#version 330 core
in vec2 Texcoord;

struct DirLight 
{
    vec3 direction;
    vec3 radiance;
};

struct PointLight
{
	float radius;
	vec3 position;
	vec3 radiance;
};

uniform vec3 cameraPos;

// lighting.
#define MAX_POINT_LIGHT 128
uniform int pointLightNum;
uniform DirLight dirLight;
uniform PointLight pointLight[MAX_POINT_LIGHT];
uniform float lightAttenuationCoff;
uniform mat4 lightSpaceMatrix;

// pbr material texture.
uniform sampler2D dposition;
uniform sampler2D dnormal;
uniform sampler2D dalbedo;
uniform sampler2D droughness;
uniform sampler2D ddepth;
uniform sampler2D shadowDepth;
uniform samplerCube irradianceMap;
uniform samplerCube prefilteredMap;
uniform sampler2D brdfLutMap;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

// brdf auxiliary functions.
float NormalDistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
float shadowCalculation(vec4 fragPosLightSpace, float bias);

const float PI = 3.14159265359;

void main()
{
	// sample position.
	vec3 FragPos = texture(dposition, Texcoord).rgb;
	// sample albedo.
	vec3 albedo = texture(dalbedo, Texcoord).rgb;
	// sample normal.
	vec3 normal = texture(dnormal, Texcoord).rgb;
	// sample roughness.
	float roughness = texture(droughness, Texcoord).r;
	// sample metallic.
	float metallic = texture(droughness, Texcoord).g;
	// sample depth.
	float depth = texture(droughness, Texcoord).b;
	// sample ambient occlusion.
	float ao = texture(ddepth, Texcoord).r;

	// emssive
	if(normal.x == 0.0f && normal.y == 0.0f && normal.z == 0.0f)
	{
		fragColor.rgb = albedo;
		// glow map.
		float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
		brightColor = vec4(fragColor.rgb, 1.0f);

		// write depth to buffer for forwarding shading.
		gl_FragDepth = depth;
		return;
	}

	// some directions.
	vec3 viewDir = normalize(cameraPos - FragPos);
	// index of refracted.
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	// *******************************directional light************************************
	vec3 lightDir = dirLight.direction;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	// fresnel factor.
	vec3 fresnel = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0f), F0);
	// normal distribution factor.
	float distribution = NormalDistributionGGX(normal, halfwayDir, roughness);
	// geometry facror.
	float geometryFactor = GeometrySmith(normal, viewDir, lightDir, roughness);
	// brdf function.
	vec3 brdf = distribution * fresnel * geometryFactor / 
				(4.0f * max(dot(viewDir, normal), 0.0f) * max(dot(lightDir, normal), 0.0f) + 0.0001f);
	vec3 kSpecular = fresnel;
	vec3 kDiffuse = vec3(1.0f) - kSpecular;
	kDiffuse *= (1.0f - metallic);
	// rendering equation.
	fragColor.rgb = (kDiffuse * albedo / PI + brdf) * dirLight.radiance * max(dot(normal, lightDir), 0.0f);
	// ************************************************************************************

	// *************************************point lights***********************************
	vec3 pointLightRadiance = vec3(0.0f);
	for(int i = 0;i < pointLightNum;++ i)
	{
		vec3 lightDir = normalize(pointLight[i].position - FragPos);
		vec3 halfwayDir = normalize(viewDir + lightDir);
		float distance = length(pointLight[i].position - FragPos);
		if(distance > pointLight[i].radius) 	
			continue;
		float attenuation = 1.0f / (lightAttenuationCoff * distance * distance + 0.00001);
		vec3 radiance = pointLight[i].radiance * attenuation;
		
		vec3 fresnel = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0f), F0);
		// normal distribution factor.
		float distribution = NormalDistributionGGX(normal, halfwayDir, roughness);
		// geometry facror.
		float geometryFactor = GeometrySmith(normal, viewDir, lightDir, roughness);
		// brdf function.
		vec3 brdf = distribution * fresnel * geometryFactor / 
					(4.0f * max(dot(viewDir, normal), 0.0f) * max(dot(lightDir, normal), 0.0f) + 0.0001f);
		vec3 kSpecular = fresnel;
		vec3 kDiffuse = vec3(1.0f) - kSpecular;
		kDiffuse *= (1.0f - metallic);
		// rendering equation.
		pointLightRadiance += (kDiffuse * albedo / PI + brdf) * radiance * max(dot(normal, lightDir), 0.0f);
	}
	// ************************************************************************************

	// shadow
	float shadow = 1.0f;
	vec4 FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0f);
	shadow = 1.0f - shadowCalculation(FragPosLightSpace, 0.0f);

	// ambient lighting.
	vec3 ambientS = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0f), F0, roughness);
	vec3 ambientD = vec3(1.0f) - ambientS;
	vec3 irradiance = texture(irradianceMap, normal).rgb;

	vec3 R = reflect(-viewDir, normal);
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = texture(prefilteredMap, R, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 envBrdf = texture(brdfLutMap, vec2(max(dot(normal, viewDir), 0.0f), roughness)).rg;
	vec3 envSpecular = prefilteredColor * (/*ambientS **/ envBrdf.x + envBrdf.y);	
	
	vec3 ambient = (albedo * irradiance * ambientD + envSpecular) * ao;

	fragColor.xyz = ambient + fragColor.xyz * shadow + pointLightRadiance;
	
	// glow map.
	float brightness = dot(fragColor.rgb / (fragColor.rgb + vec3(1.0f)), vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 0.55f)
		brightColor = vec4(fragColor.rgb / (fragColor.rgb + vec3(1.0f)), 1.0f);

	// write depth to buffer for forwarding shading.
	gl_FragDepth = depth;
}

float NormalDistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float aSquared = a * a;
	float NdotH = max(dot(N, H), 0.0f);
	float NdotHSquared = NdotH * NdotH;
	float nom = aSquared;
	float denom = (NdotHSquared * (aSquared - 1.0f) + 1.0f);
	denom = PI * denom * denom;
	return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0f);
	float k = (r * r) / 8.0f;
	
	float nom = NdotV;
	float denom = NdotV * (1.0f - k) + k;
	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0f);
	float NdotL = max(dot(N, L), 0.0f);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	return ggx2 * ggx1;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0f - F0) * pow(1.0 - cosTheta, 5.0f);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow (1.0 - cosTheta, 5.0f);
}

float shadowCalculation(vec4 fragPosLightSpace, float bias)
{
	// perspective division.
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0)
        return 0.0f;

	// pcf.
	float shadowFactor = 0.0f;
	float currentDepth = projCoords.z;
	vec2 texelSize = 1.0 / textureSize(shadowDepth, 0);
	for(int x = -1; x <= 1; ++x)
	{
    		for(int y = -1; y <= 1; ++y)
    		{
        		float pcfDepth = texture(shadowDepth, projCoords.xy + vec2(x, y) * texelSize).r; 
        		shadowFactor += ((currentDepth - bias) > pcfDepth) ? 1.0 : 0.0;        
    		}
	}
	shadowFactor /= 9.0;
	return shadowFactor;
}