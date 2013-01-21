//This structure is how the data is formatted from the application
struct TANGENT_MATRIX
{
	float3 X : TEXCOORD3;	//right handed Tangent space basis,
	float3 Y : TEXCOORD4;	// can be non orthogonal,
	float3 Z : TEXCOORD5;	// and vectors are normalized
};
struct SHADE_POINT
{
	float4 HPosition		: SV_Position;
	float2 UV				: TEXCOORD0;	//original texture coordinates
	float2 TexCoord0		: TEXCOORD1;	//transformed texture coordinates
	float3 ViewPos			: TEXCOORD2;	//view space position
	TANGENT_MATRIX ViewTan;
};

// assume that unused cbuffers get compiled away.
cbuffer cbRenderer
{
	float4 g_EyePos = {0,0,-1,1};
};

cbuffer cbLight
{
	float4 g_LightPos;
	float4 g_LightColor;
};

struct IncidentLight
{
	float3 m_Color;
	float m_InShadow;
};

void IlluminatePointLight(out IncidentLight incidentLight)
{
	incidentLight.m_Color = g_LightColor.rgb;
	incidentLight.m_InShadow = 0;
}

