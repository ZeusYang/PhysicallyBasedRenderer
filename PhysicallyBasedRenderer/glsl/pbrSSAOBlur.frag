#version 430 core
in vec2 Texcoord;

uniform sampler2D ddepth;

void main(){
	// sample random vectors.
	vec2 texelSize = 1.0 / vec2(textureSize(ddepth, 0));
	float result = 0.0f;
	for(int x = -2;x <= 2;++x)
	{
		for(int y = -2;y <= 2;++y)
		{
			vec2 offset = vec2(float(x), float(y)) *texelSize;
			result += texture(ddepth, Texcoord + offset).r;
		}
	}
	gl_FragDepth = result/25.0f;
}