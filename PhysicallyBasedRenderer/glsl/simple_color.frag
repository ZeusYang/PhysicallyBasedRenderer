#version 330 core
in vec2 Texcoord;
in vec3 Normal;
in vec3 Color;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

void main(){
	fragColor = vec4(Color,1.0f);
	float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 0.7f)
		brightColor = vec4(fragColor.rgb, 1.0f);
}