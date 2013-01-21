#version 410
//This structure is how the data is formatted from the application

in vec4 HPosition;
in vec2 UV;	//original texture coordinates
in vec2 TexCoord0;	//transformed texture coordinates
in vec3 ViewPos;	//view space position
in vec3 TAN_X;	//right handed Tangent space basis,
in vec3 TAN_Y;	// can be non orthogonal,
in vec3 TAN_Z;	// and vectors are normalized

uniform cb0
{
	vec4 g_Color;// = vec4(1,1,1,1);
};

out vec4 fragColor;
void main()
{
	fragColor = g_Color;
}

