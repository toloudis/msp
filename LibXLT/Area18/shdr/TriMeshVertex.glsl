//This structure is how the data is formatted from the application
struct STANDARD_VERTEX
{
	float3 Position	: SV_POSITION;
	float3 Normal	: NORMAL;
	float2 UV		: TEXCOORD0;
	float3 T		: TANGENT;
	float3 B		: BINORMAL;
};

struct TANGENT_MATRIX
{
	float3 X : TEXCOORD3;	//right handed Tangent space basis,
	float3 Y : TEXCOORD4;	// can be non orthogonal,
	float3 Z : TEXCOORD5;	// and vectors are normalized
};

struct VERTEX_OUTPUT
{
	float4 HPosition		: SV_Position;
	float2 UV				: TEXCOORD0;	//original texture coordinates
	float2 TexCoord0		: TEXCOORD1;	//transformed texture coordinates
	float3 ViewPos			: TEXCOORD2;	//camera space position
	TANGENT_MATRIX ViewTan;
};

cbuffer cb0
{
	float4x4 g_uvTransform = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
	float4x4 g_wv : WorldView;
	float4x4 g_wvp : WorldViewProjection;
};

TANGENT_MATRIX TransformTangents( uniform float4x4 mat,
								 in float3 Tangent,
								 in float3 Binormal,
								 in float3 Normal )
{
	TANGENT_MATRIX tmat;
	tmat.X = mul( (float3x3)mat, Tangent);
	tmat.Y = mul( (float3x3)mat, Binormal);
	tmat.Z = mul( (float3x3)mat, Normal);
	return tmat;
}

VERTEX_OUTPUT VS_Default( STANDARD_VERTEX Vtx ) 
{
	VERTEX_OUTPUT OUT;
	//OUT.V = TangentVS( Vtx );

	// decal and bump texture coords
    OUT.TexCoord0 = mul(g_uvTransform, float4(Vtx.UV,0,1)).xy;
    OUT.UV = Vtx.UV;

    float4 Po = float4( Vtx.Position, 1.0);
    OUT.HPosition = mul(g_wvp, Po);
    OUT.ViewPos = mul( g_wv, Po).xyz;

	OUT.ViewTan = TransformTangents( g_wv, Vtx.T, Vtx.B, Vtx.Normal );

	return OUT;
}

