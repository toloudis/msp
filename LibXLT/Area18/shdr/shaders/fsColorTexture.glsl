#version 420

out vec4 colorOut;

uniform vec4 color;
uniform bool hasTex;
uniform sampler tex;

void main( ) 
{
	vec4 c = color;
	if (hasTex) {
		c *= texture2d(tex, uv);
	}
	colorOut = c;
}

