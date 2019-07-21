#version 330 core
in vec3 worldPos;
out vec4 fragColor;

uniform sampler2D hdrMap;

// (1/(pi/2), 1/(pi))
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	// to [0,1].
	uv *= invAtan;
	uv += vec2(0.5f);
	return uv;
}

void main()
{
	// map 3d texcoord to 2d texcoord.
	vec2 uv = sampleSphericalMap(normalize(worldPos));
	// sample hdr map.
	vec3 sampler = texture(hdrMap, uv).rgb;
	// save to one face of cubemap.
	fragColor = vec4(sampler, 1.0f);
}