#version 430 core

uniform sampler2D image;
uniform vec3 baseColor;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;
uniform float pointSize;
uniform mat4 projectMatrix;

in vec3 eyeSpacePos;

void main(){
	fragColor = vec4(baseColor, 1.0f);
	
	vec3 normal;
	normal.xy = gl_PointCoord.xy * vec2(2.0, -2.0) + vec2(-1.0,1.0);
	float mag = dot(normal.xy, normal.xy);
	if(mag > 1.0) discard;
	normal.z = sqrt(1.0 - mag);

    vec3 ambient = dirLight.ambient * fragColor.xyz;
    // diffuse
    vec3 lightDir = dirLight.direction;
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * dirLight.diffuse * fragColor.xyz;

	fragColor.xyz = ambient + diffuse;
	// gamma correction.
	const float gamma = 2.2f;
	fragColor.rgb = pow(fragColor.rgb,vec3(1.0f/gamma));

	// glow map.
	float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	//if(brightness > 0.3f)
		brightColor = vec4(fragColor.rgb * brightness, 1.0f);

	vec4 pixelEyePos = vec4(eyeSpacePos + vec3(normal.x, normal.y, -normal.z) * pointSize, 1.0f);
	vec4 pixelClipPos = projectMatrix * pixelEyePos;
	gl_FragDepth = pixelClipPos.z / pixelClipPos.w;
}