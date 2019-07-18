#version 330 core
in vec3 worldPos;
out vec4 fragColor;

uniform sampler2D hdrMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += vec2(0.5f);
	return uv;
}

void main()
{
	vec2 uv = sampleSphericalMap(normalize(worldPos));
	vec3 sampler = texture(hdrMap, uv).rgb;
	fragColor = vec4(sampler, 1.0f);
	//fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}