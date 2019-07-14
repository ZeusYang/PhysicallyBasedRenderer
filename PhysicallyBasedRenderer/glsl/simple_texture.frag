#version 330 core
in vec2 Texcoord;
in vec3 Normal;
in vec3 Color;

uniform sampler2D albedo;

out vec4 color;

void main(){
	color = texture(albedo,Texcoord);
}