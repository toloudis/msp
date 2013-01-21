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
	vec4 g_EyePos;// = vec4(0,0,-1,1);
	bool g_HasDiffuseTexture;// = false;
};

uniform sampler2D g_DiffuseTexture;
//SamplerState diffSampler
//{
//	Filter = MIN_MAG_LINEAR_MIP_LINEAR;
//	AddressU = Wrap;
//	AddressV = Wrap;
//};
float saturate(float x) {return clamp(x,0,1);}

out vec4 fragColor;
void main()
{
	vec3 e = normalize(g_EyePos.xyz - ViewPos);
	vec3 c = g_Color.rgb * saturate(dot(e, normalize(TAN_Z))); 
	if (g_HasDiffuseTexture)
		c *= texture(g_DiffuseTexture, UV).rgb;
	fragColor = vec4(c, 1);
}
