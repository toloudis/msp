/*****************************************************************************
**  ShadowsOnly.fx
**
**      Lighting only, no surface shading
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

/*********** support data and functions ******/

#include "..\Support.h"
#include "..\Tessellate.h"
#include "..\Lighting.h"

static const float3 LUMINANCE_VECTOR  = float3(0.265068,  0.67023428, 0.06409157);

bool g_UseCosine = true;

float g_transparency : Opacity = 1.0f;
bool hasTransparencyMap = false;
texture2D transparencyMap		: OpacityTexture;

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

technique11 Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 singleLightPS(\
					g_lightInfo, g_transparency, transparencyMap);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_DEFAULT(Default)
PASS_DEFAULT(Tess)
}

technique11 SingleLight
{
#define PASS_SINGLELIGHT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 singleLightPS(\
					g_lightInfo, g_transparency, transparencyMap);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_SINGLELIGHT(Default)
PASS_SINGLELIGHT(Tess)
}

technique11 ProjectedLight
{
#define PASS_PROJECTEDLIGHT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 projLightPS(\
					g_lightInfo, g_projLight, g_transparency,\
						transparencyMap, projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHT(Default)
PASS_PROJECTEDLIGHT(Tess)
}
technique11 ProjectedLightSuperSample
{
#define PASS_PROJECTEDLIGHTSS(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 projLightPS(\
					g_lightInfo, g_projLight, g_transparency,\
						transparencyMap, projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHTSS(Default)
PASS_PROJECTEDLIGHTSS(Tess)
}
technique11 ProjectedLightSuperSample2
{
#define PASS_PROJECTEDLIGHTSS2(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 projLightPS(\
					g_lightInfo, g_projLight, g_transparency,\
						transparencyMap, projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHTSS2(Default)
PASS_PROJECTEDLIGHTSS2(Tess)
}
technique11 ProjectedLightSuperSample3
{
#define PASS_PROJECTEDLIGHTSS3(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 projLightPS(\
					g_lightInfo, g_projLight, g_transparency,\
						transparencyMap, projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHTSS3(Default)
PASS_PROJECTEDLIGHTSS3(Tess)
}

technique11 Glow
{
#define PASS_GLOW(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 glowPS();\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_GLOW(Default)
PASS_GLOW(Tess)
}
technique11 DOFPrep
{
#define PASS_DOFPREP(PassName)	\
	pass P##PassName			\
	{						\
		SetVertexShader(CompileShader(vs_5_0, singleLightVS_##PassName()));\
		SetPixelShader(CompileShader(ps_5_0, DOFPrepTrans_PS( hasTransparencyMap, transparencyMap, g_transparency )));\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_DOFPREP(Default)
PASS_DOFPREP(Tess)
}
technique11 Matte
{
#define PASS_MATTE(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 simpleMattePS();\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_MATTE(Default)
PASS_MATTE(Tess)
}
technique11 Environment
{
#define PASS_ENVIRONMENT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 iblPS();\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_ENVIRONMENT(Default)
PASS_ENVIRONMENT(Tess)
}
/***************************** eof ***/
