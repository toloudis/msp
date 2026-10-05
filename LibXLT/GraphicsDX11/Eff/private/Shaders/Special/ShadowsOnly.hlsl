//////////////////////////////////////////////////////////////////////////////
// Converted from ShadowsOnly.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// ShadowsOnly.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  ShadowsOnly.fx
**
**      Lighting only, no surface shading
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

/*********** support data and functions ******/

// Register layout shared with the material shaders: see Materials/Globals.hlsli.
#include "../Materials/Support.hlsli"
#include "../Materials/Tessellate.hlsli"
#include "../Materials/Lighting.hlsli"

static const float3 LUMINANCE_VECTOR  = float3(0.265068,  0.67023428, 0.06409157);

// b4: this shader's own parameters (defaults are in ShadowsOnly.effect.json)
cbuffer MaterialParams : register(b4)
{
	bool g_UseCosine;			// default true
	float g_transparency;		// : Opacity, default 1
	bool hasTransparencyMap;	// default false
};
Texture2D transparencyMap : register(t4);		// : OpacityTexture

/************* DATA STRUCTS **************/

struct VertexOutput
{
    float4 HPosition	: SV_POSITION;
    float2 TexCoord0	: TEXCOORD0;
    float4 diffCol		: COLOR0;
    float4 specCol		: COLOR1;
};

struct VertexOutputTess
{
    float2 TexCoord0	: TEXCOORD0;
    float4 diffCol		: COLOR0;
    float4 specCol		: COLOR1;
    float3 Position		: TEXCOORD3;
    float3 Normal		: TEXCOORD4;
};

//--------------------------------------------------------------------------------------
// Vertex Shaders
//--------------------------------------------------------------------------------------

TANGENT_VERTEX TangentVS( STANDARD_VERTEX In )
{
    TANGENT_VERTEX OUT;

    OUT.TexCoord0 = mul(g_uvTransform, float4(In.UV,0,1)).xy;
    
	float3 newPos = In.Position;

    // output position in proj space
    float4 Po = float4(newPos + In.Normal*g_glowSize, 1.0);
    
    // transform position to world space and get vector to light
    OUT.WorldPos = mul( g_world, Po).xyz;

	OUT.WorldTan = TransformTangents( g_world, In.T, In.B, In.Normal );

	// decal and bump texture coords
    OUT.UV = In.UV;

    return OUT;
}

TANGENT_VERTEX_OUTPUT singleLightVS_Default( STANDARD_VERTEX Vtx, out float ClipDist : SV_ClipDistance0 ) 
{
	TANGENT_VERTEX_OUTPUT OUT;
	OUT.V = TangentVS( Vtx );

    float4 Po = float4( Vtx.Position + Vtx.Normal*g_glowSize, 1.0);
    OUT.HPosition = TransformVertex( Po, Vtx.UV, g_wvp );
	OUT.ScreenPos = float3(0,0,0);//not used

	ClipDist = ClipWorldPos( OUT.V.WorldPos );

	return OUT;
}

TANGENT_TESS_OUTPUT singleLightVS_Tess( STANDARD_VERTEX Vtx ) 
{
	TANGENT_TESS_OUTPUT OUT;
	OUT.V = TangentVS( Vtx );

	return OUT;
}

//--------------------------------------------------------------------------------------
// Pixel Shaders
//--------------------------------------------------------------------------------------

pixelOutput labPS( VertexOutput IN,
		  uniform Texture2D TransparencyMap,
		  uniform float transparency) 
{
    // alpha blend refl layer and base color
    pixelOutput OUT; 
    OUT.col = 1;

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, TransparencyMap, IN.TexCoord0, transparency).x;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

    return OUT;
}

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN,
					uniform LightInfo i_Light,
					uniform float transparency,
					uniform Texture2D TransparencyMap, bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, TransparencyMap, IN.V.TexCoord0, transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	IncidentLight light;
	IlluminatePointLight(IN.V.WorldPos, i_Light, light);
	if(dot(GetNormal(IN.V, vFace), normalize(light.L)) <= 0)
		light.Cld = float3(0.0f, 0.0f, 0.0f);

	float cosine = 1;
	//if (g_UseCosine)
	//{
		//fetch bump normal
		float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );

		cosine = saturate(dot(normalize(light.L), worldNormal));
		// Using bump normal to truncate backfacing normal
		if(dot(normalize(light.L), worldNormal) <= 0)
		{
			light.Cld = float3(0.0f, 0.0f, 0.0f); // default shadow color to black
			cosine = 1.0f;
		}
	//}
	
	OUT.col.rgb = light.Cld * cosine;
	OUT.col.rgb = 1 - OUT.col.rgb; // reverse the shdw color from white to black

    return OUT;
}

pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform float transparency,
		uniform Texture2D TransparencyMap,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap,
		uniform int nBlockerSamples, uniform int nShadowSamples, bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 
    
	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, TransparencyMap, IN.V.TexCoord0, transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	IncidentLight light;
	IlluminateProjLightShadowsOnly(IN.V.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, 
		nBlockerSamples, nShadowSamples, light);
	if(dot(GetNormal(IN.V, vFace), normalize(light.L)) <= 0)
		light.Cld = float3(0.0f, 0.0f, 0.0f);	// default shadow color to black
		
	float cosine = 1;
	//if (g_UseCosine)
	//{
		//fetch bump normal
		float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );

		cosine = saturate(dot(normalize(light.L), worldNormal));
		// Using bump normal to truncate backfacing normal
		if(dot(normalize(light.L), worldNormal) <= 0)
		{
			light.Cld = float3(0.0f, 0.0f, 0.0f); // default shadow color to black
			cosine = 1.0f;
		}
	//}
	
	OUT.col.rgb = light.Cld * cosine;
	OUT.col.rgb = 1 - OUT.col.rgb; // reverse the shdw color from white to black

    return OUT;
}

pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN)
{
    pixelOutput OUT; 
    OUT.col = float4(0,0,0,1);
    return OUT;
}

pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN) 
{
    pixelOutput OUT; 
    OUT.col.rgb = float3(0,0,0);

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).x;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

    return OUT;
}

//--------------------------------------------------------------------------------------
// TECHNIQUES
//--------------------------------------------------------------------------------------


/***************************** eof ***/

//////////////////////////////////////////////////////////////////////////////
// Entry points generated from the technique/pass compile statements.
// Each one binds the uniform arguments the .fx passed in its compile call.
//////////////////////////////////////////////////////////////////////////////

pixelOutput Default_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS(IN, g_lightInfo, g_transparency, transparencyMap, vFace);
}

pixelOutput ProjectedLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS(IN, g_lightInfo, g_projLight, g_transparency, transparencyMap, projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, vFace);
}

pixelOutput ProjectedLightSuperSample_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS(IN, g_lightInfo, g_projLight, g_transparency, transparencyMap, projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED, vFace);
}

pixelOutput ProjectedLightSuperSample2_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS(IN, g_lightInfo, g_projLight, g_transparency, transparencyMap, projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample3_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS(IN, g_lightInfo, g_projLight, g_transparency, transparencyMap, projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH, vFace);
}

pixelOutput DOFPrep_PDefault_PS(TANGENT_VERTEX_OUTPUT IN)
{
    return DOFPrepTrans_PS(IN, hasTransparencyMap, transparencyMap, g_transparency);
}
