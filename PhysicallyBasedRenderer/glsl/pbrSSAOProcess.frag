#version 430 core
in vec2 Texcoord;

uniform vec3 samples[64];
uniform mat4 viewMatrix;
uniform mat4 projectMatrix;
uniform sampler2D dposition;
uniform sampler2D dnormal;
uniform sampler2D ddepth;
uniform sampler2D randomNoise;

uniform float farPlane;
uniform float nearPlane;

const float radius = 10.0f;
const int sampleNum = 64;

void main(){
	// sample position.
	vec3 FragPos = texture(dposition, Texcoord).rgb;
	// sample normal.
	vec3 normal = texture(dnormal, Texcoord).rgb;
	// sample depth.
	float depth = texture(ddepth, Texcoord).r;
	
	// sample random vectors.
	vec2 depthTextureSize = textureSize(ddepth, 0);
	vec2 noiseTextureSize = textureSize(randomNoise, 0);
	vec2 noiseTexScale = vec2(depthTextureSize.x / noiseTextureSize.x, depthTextureSize.y / noiseTextureSize.y);
	vec3 randomVec = texture(randomNoise, Texcoord * noiseTexScale).rgb;
	
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBNMatrix = mat3(tangent, bitangent, normal);
	
	// calculate ambient occlusion.
	float occlusion = 0.0f;
	for(int i = 0;i < sampleNum;++i)
	{
		// change to world space.
		vec3 samplePoint = TBNMatrix * samples[i];
		samplePoint = FragPos + samplePoint * radius;

		// change to view space.
		samplePoint = vec3(viewMatrix * vec4(samplePoint, 1.0f));
		
		// change to ndc space & screen space.
		vec4 tmp = vec4(samplePoint, 1.0f);
		tmp = projectMatrix * tmp;
		tmp.xyz /= tmp.w;
		tmp.xyz = tmp.xyz * 0.5f + 0.5f;
		
		// get sample point's depth.
		float sampleDepth = texture(ddepth, tmp.xy).r;
		samplePoint.z /= -farPlane;
					
		// range check and accumulate.
		float rangeCheck = smoothstep(0.0, 1.0, radius / (abs(depth - sampleDepth) * farPlane));
		occlusion += (sampleDepth > samplePoint.z ? 0.0 : 1.0) * rangeCheck;
	}

	occlusion /= sampleNum;
	occlusion = 1.0f - occlusion;

	gl_FragDepth = occlusion;
	
}