in TANGENT_VERTEX_OUTPUT OUT;

out vec4 colorOut;

uniform vec4 color;
uniform bool hasTex;
uniform sampler2D tex;

void main( ) 
{
	vec4 c = color;
	if (hasTex) {
		c *= texture2D(tex, OUT.V.UV);
	}
	colorOut = c;
}

