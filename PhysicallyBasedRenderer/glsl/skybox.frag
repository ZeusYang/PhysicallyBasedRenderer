#version 330 core
in vec3 Texcoord;
in vec3 Normal;
in vec3 Color;

uniform samplerCube image;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

void main(){
	//fragColor = texture(image, Texcoord);
	fragColor = textureLod(image, Texcoord, 0.5);
	// glow map.
	float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 0.4f)
		brightColor = vec4(fragColor.rgb, 1.0f);
}