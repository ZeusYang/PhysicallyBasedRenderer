#version 330 core
in vec3 worldPos;
out vec4 fragColor;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{
	vec3 normal = normalize(worldPos);
	vec3 irradiance = vec3(0.0f);
	
	// tangent space.
	vec3 up = vec3(0.0f, 1.0f, 0.0f);
	vec3 right = cross(up, normal);
	up = cross(normal, right);
	
	float sampleDelta = 0.025;
	float nSamples = 0.0f;
	for(float phi = 0.0f; phi < 2.0 * PI;phi += sampleDelta)
	{
		for(float theta = 0.0f; theta < 0.5 * PI;theta += sampleDelta)
		{
			vec3 sampleDir = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// tangent space to world space.
			sampleDir = sampleDir.x * right + sampleDir.y * up + sampleDir.z * normal;
			
			irradiance += texture(environmentMap, sampleDir).rgb * cos(theta) * sin(theta);
			++ nSamples;
		}
	}
	irradiance = PI * irradiance * (1.0f / nSamples);
	fragColor = vec4(irradiance, 1.0f);
}