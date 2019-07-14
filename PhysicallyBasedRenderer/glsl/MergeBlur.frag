#version 330 core

in vec2 Texcoord;

out vec4 fragColor;

uniform sampler2D Color;
uniform sampler2D BrightColor;
uniform sampler2D DepthMap;

void main(){
	const float exposure = 1.0f;
	vec3 hdrColor = (texture(Color, Texcoord).rgb + texture(BrightColor, Texcoord).rgb);

	// tone mapping.
	hdrColor = vec3(1.0f) - exp(-hdrColor * exposure);
	
	// gamma correction.
	const float gamma = 2.2f;
	hdrColor = pow(hdrColor, vec3(1.0f / gamma));

    fragColor = vec4(hdrColor, 1.0f);
}