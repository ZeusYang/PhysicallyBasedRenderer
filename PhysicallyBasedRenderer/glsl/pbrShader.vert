#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;
layout (location = 3) in vec3 color;
layout (location = 4) in vec3 tangent;
layout (location = 5) in vec3 bitangent;
layout (location = 6) in mat4 instanceMatrix;

out vec3 FragPos;
out vec2 Texcoord;
out mat3 TBNMatrix;

uniform bool instance;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectMatrix;
uniform mat4 normalMatrix;
uniform mat4 lightSpaceMatrix;

void main(){
	vec3 T = normalize(vec3(modelMatrix * vec4(tangent, 0.0f)));
	vec3 B = normalize(vec3(modelMatrix * vec4(bitangent, 0.0f)));
	vec3 N = normalize(vec3(modelMatrix * vec4(normal, 0.0f)));
	TBNMatrix = mat3(T, B, N);
	Texcoord = texcoord;
	if(!instance)
		FragPos = vec3(modelMatrix * vec4(position,1.0f));
	else 
		FragPos = vec3(modelMatrix * instanceMatrix * vec4(position,1.0f));
	gl_Position = projectMatrix * viewMatrix * vec4(FragPos,1.0f);
}