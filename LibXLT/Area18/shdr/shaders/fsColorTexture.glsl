#version 420

in TANGENT_VERTEX_OUTPUT OUT;

out vec4 colorOut;

uniform vec4 color;
uniform bool hasTex;
uniform sampler tex;

void main( ) 
{
	vec4 c = color;
	if (hasTex) {
		c *= texture2d(tex, OUT.V.UV);
	}
	colorOut = c;
}

