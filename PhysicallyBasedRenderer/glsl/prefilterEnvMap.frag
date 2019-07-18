#version 330 core
in vec3 worldPos;
out vec4 fragColor;

uniform float roughness;
uniform samplerCube environmentMap;

const float PI = 3.14159265359;

float radicalInverseVdc(uint bits);
vec2 hammersley(uint i, uint N);
vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness);

void main()
{
	vec3 N = normalize(worldPos);
	vec3 V = N;

	const uint sampleCount = 1024u;
	float totalWeight = 0.0f;
	vec3 prefilteredColor = vec3(0.0f);
	for(uint i = 0u;i < sampleCount;++ i)
	{
		vec2 Xi = hammersley(i, sampleCount);
		vec3 H = importanceSampleGGX(Xi, N, roughness);
		vec3 L = normalize(2.0 * dot(V, H) * H - V);
		
		float NdotL = max(dot(N, L), 0.0);
		if(NdotL > 0.0f);
		{
			prefilteredColor += texture(environmentMap, L).rgb * NdotL;
			totalWeight += NdotL;
		}
	}
	prefilteredColor = prefilteredColor / totalWeight;
	
	fragColor = vec4(prefilteredColor, 1.0f);
}

float radicalInverseVdc(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 hammersley(uint i, uint N)
{
	return vec2(float(i) / float(N), radicalInverseVdc(i));
}

vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
	float a = roughness * roughness;
	
	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;

	vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);

	vec3 sampleVec = H.x * tangent + H.y * bitangent + H.z * N;
	return normalize(sampleVec);
}