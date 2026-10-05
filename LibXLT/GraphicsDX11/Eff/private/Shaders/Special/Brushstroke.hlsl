//////////////////////////////////////////////////////////////////////////////
// Converted from Brushstroke.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// Brushstroke.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Brushstroke.fx
**
**      brushstroke: paint brush shader
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

/*********** support data and functions ******/

// Annotations of the parameters are in Brushstroke.effect.json.
// Register layout shared with the material shaders: see Materials/Globals.hlsli.

#include "../Materials/Support.hlsli"

// b4: this shader's own parameters (defaults are in Brushstroke.effect.json)
cbuffer MaterialParams : register(b4)
{
	float4 g_diffuse;			// : MaterialDiffuse, default (1,1,1,1)
	bool hasDiffuseMap;			// default false
};

Texture2D diffuseMap : register(t4);		// : DiffuseTexture

/************* DATA STRUCTS **************/

struct VertexOutput
{
    float4 HPosition	: SV_POSITION;
    float2 TexCoord0	: TEXCOORD0;
    float2 UV			: TEXCOORD1;
    float3 WorldPos		: TEXCOORD2;
   	float3 WorldNormal	: TEXCOORD3;
};


/*********** support functions ******/
    
/*********** vertex shader ******/

VertexOutput singleLightVS(STANDARD_VERTEX IN,
    uniform float4x4 WorldViewProj,
    uniform float4x4 WorldIT,
    uniform float4x4 World,
    uniform float GlowSize )
{
    VertexOutput OUT;

	// decal and bump texture coords
    OUT.TexCoord0 = mul(g_uvTransform, float4(IN.UV,0,1)).xy;
    OUT.UV = IN.UV;
    
	float3 NewPos = IN.Position;

    // output position in proj space
    float4 Po = float4(NewPos + IN.Normal*GlowSize, 1.0);
    OUT.HPosition = TransformVertex(Po, IN.UV, WorldViewProj);
    
    // transform position to world space and get vector to light
    float3 Pw = mul(World, Po).xyz;
    OUT.WorldPos = Pw;
	OUT.WorldNormal = mul((float3x3)WorldIT, IN.Normal).xyz;

    return OUT;
}

VertexOutput singleLightVS_Default(STANDARD_VERTEX Vtx, out float ClipDist : SV_ClipDistance0 ) 
{
	VertexOutput Out;
	Out = singleLightVS( Vtx, g_wvp,g_worldIT,
					g_world, g_glowSize );

	ClipDist = ClipWorldPos( Out.WorldPos );

	return Out;
}

/********* pixel shader ********/

SamplerState diffuseSampler : register(s10);

pixelOutput labPS( VertexOutput IN,
		  uniform Texture2D DiffuseMap) 
{
    // alpha blend refl layer and base color
    pixelOutput OUT; 
    float4 col = Tex2DCombine(hasDiffuseMap, DiffuseMap, IN.TexCoord0, g_diffuse);
	
    OUT.col.rgb = col.rgb;
    //OUT.col.rgb *= GetAO(IN.UV);

	OUT.col.a = col.a;
	
	float radius_sq = length(IN.UV.xy - float2(0.5,0.5));
	radius_sq *= radius_sq;
	//OUT.col.rgb = g_diffuse * exp(-radius_sq);
	//OUT.col.rgb *= GetAO(IN.UV);
	//OUT.col.a = exp(-radius_sq);
    OUT.col.a = col.a * exp(-radius_sq);
    return OUT;
}

pixelOutput iblPS( VertexOutput IN, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 
//	OUT.col = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.TexCoord0, g_diffuse);
//	return OUT;

	//fetch bump normal
	float3 worldEyeDir = normalize(IN.WorldPos - g_eyePos.xyz);
	float3 worldNormal = normalize(IN.WorldNormal);
	if (g_bDoubleSided && vFace > 0)
	{
		worldNormal = -worldNormal;
	}

	float4 diff = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.TexCoord0, g_diffuse*g_envDiffuseColor*g_diffuseFactor);
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvMap, g_diffuseEnvAngle).rgb,1);
	}
	
	OUT.col = float4(
		diff.rgb,
		diff.a );		

    //OUT.col.rgb *= GetAO(IN.UV);
	//OUT.col.a = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.TexCoord0, g_diffuse).a;
	
	
	//float radius_sq = (IN.UV.xy - float2(0.5,0.5));
	//radius_sq *= radius_sq;
	//OUT.col.rgb = g_diffuse * exp(-radius_sq);
	//OUT.col.rgb *= GetAO(IN.UV);
	//OUT.col.a = exp(-radius_sq);
	OUT.col.a = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.TexCoord0, g_diffuse).a;
	
    return OUT;
}

/*************/


/***************************** eof ***/

//////////////////////////////////////////////////////////////////////////////
// Entry points generated from the technique/pass compile statements.
// Each one binds the uniform arguments the .fx passed in its compile call.
//////////////////////////////////////////////////////////////////////////////

pixelOutput Default_PDefault_PS(VertexOutput IN)
{
    return labPS(IN, diffuseMap);
}
