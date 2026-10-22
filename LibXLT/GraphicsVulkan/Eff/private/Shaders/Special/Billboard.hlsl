//////////////////////////////////////////////////////////////////////////////
// Converted from Billboard.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// Billboard.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Billboard.fx
**
**      billboard: env only, no lights.
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

/*********** support data and functions ******/

#include "../Materials/Support.hlsli"

// Register layout shared with the materials: see ../Materials/Globals.hlsli.
// b4: this shader's own parameters (defaults are in Billboard.effect.json)
cbuffer MaterialParams : register(b4)
{
	float4 g_diffuse;				// : MaterialDiffuse, default (1,1,1,1)

	bool hasDiffuseMap;				// default false

	// chroma key
	bool bCKActive;					// default false
	float4 CKColor;					// default (0,0,0,0)
	float CKTolerance;				// default 0.1
	bool bCKRemoveSpill;			// default false
	int CKSpillType;				// default 0
	float CKSpillBias;				// default 0
	int CKEdgeBlur;					// default 0
	float g_brightness;				// default 1
	int texWidth;					// default 1
	int texHeight;					// default 1
};

Texture2D diffuseMap : register(t4);			// : DiffuseTexture
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
	OUT.WorldNormal = mul((float3x3)WorldIT, IN.Normal);
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

float3 RGB_to_YCrCb(float3 c)
{
	return float3(0.299f * c.r + 0.587f * c.g + 0.114f * c.b, 
					-0.168738f * c.r - 0.331264f * c.g + 0.5f * c.b,
					0.5f * c.r - 0.418688f * c.g - 0.081312f * c.b);
}


void ChromaKey(inout float4 color, float2 UV)
{
	if (!bCKActive)
		return;
		
//bool bCKActive = false;
//float4 CKColor = float4(0,0,0,0);
//float CKTolerance = 0.1f;
//bool bCKRemoveSpill = false;
//int CKSpillType = 0;
//float CKSpillBias = 0.0f;
//int CKEdgeBlur = 0;
		
	float alpha = 1.0f, spill = 0.0f;
	bool spillG = 1 - CKSpillType, spillB = CKSpillType;
	// YCrCb range
	float3 cKey = RGB_to_YCrCb(CKColor.rgb);
	
	const int r = min(abs(CKEdgeBlur), 7);
	float sum = 0;
	
	[unroll(15)]for (int ix = -r; ix <= r; ix++)
	{
		[unroll(15)]for(int iy = -r; iy <= r; iy++)
		{
			float2 offsetUV = UV + float2((float)ix / texWidth, (float)iy / texHeight);
			float4 c = g_diffuse*g_envDiffuseColor*g_diffuseFactor;
			if (hasDiffuseMap)
				c *= diffuseMap.Sample(g_DefaultSampler, offsetUV);
			c.rgb = RGB_to_YCrCb(c.rgb);
			
			if ( sqrt((c.g - cKey.g) * (c.g - cKey.g) + (c.b - cKey.b) * (c.b - cKey.b) ) <= CKTolerance)
			{
				alpha = 0.0f;
			}
			else
			{
				alpha = 1.0f;
			}
			
			sum += alpha;
		}
	}
	
	color.a = sum / float((r * 2 + 1) * (r * 2 + 1));
	if (bCKRemoveSpill)
	{
		spill = max(0.0f, (color.g * spillG + color.b * spillB) - 
						(color.r * CKSpillBias + (1.0f - CKSpillBias) * color.b * spillG + (1.0f - CKSpillBias) * color.g * spillB));
	}
	color.g = color.g - spill * spillG;
	color.b = color.b - spill * spillB;
	
	return;
}

pixelOutput labPS( VertexOutput IN) 
{
    // alpha blend refl layer and base color
    pixelOutput OUT; 
    OUT.col = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.TexCoord0, g_diffuse);
    
    ChromaKey(OUT.col, IN.TexCoord0);

	// alpha test
	if( g_AlphaTestRef >= OUT.col.a ) discard;

    return OUT;
}

pixelOutput iblPS( VertexOutput IN, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.TexCoord0, g_diffuse).a;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

//	OUT.col = Tex2DCombine(hasDiffuseMap, diffuseSampler, IN.TexCoord0, g_diffuse);
//	return OUT;

	//fetch bump normal
	float3 worldEyeDir = normalize(IN.WorldPos - g_eyePos.xyz);
	float3 worldNormal = normalize(IN.WorldNormal);
	if (g_bDoubleSided && vFace > 0)
	{
		worldNormal = -worldNormal;
	}

	float4 diff = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.TexCoord0, g_diffuse*g_envDiffuseColor*g_diffuseFactor);	
	diff *= float4(g_brightness,g_brightness,g_brightness,1);
	
	ChromaKey(diff, IN.TexCoord0);
	
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvMap, g_diffuseEnvAngle).rgb,1);
	}
		
	OUT.col.rgb = g_IsolateReflection ? 0 : diff.rgb * diff.a;
	OUT.col.a = diff.a;

    return OUT;
}

float4 PSBlur
    (
    in float4 vPos : SV_POSITION,
    in float2 vScreenPosition : TEXCOORD0
    ) : SV_TARGET
{
    
    return float4(1,0,0,1);	
}

/*************/


/***************************** eof ***/
