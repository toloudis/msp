//////////////////////////////////////////////////////////////////////////////
// Converted from EnvBackground.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// EnvBackground.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  EnvBackground.fx
**
**      render environment background on the current render target
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
// The SasGlobal effect description (gp : SasGlobal) is in
// EnvBackground.effect.json. Register layout shared with the material
// shaders: see Materials/Globals.hlsli.

#include "../Materials/Support.hlsli"

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

// b4: this shader's own parameters (defaults are in EnvBackground.effect.json)
cbuffer MaterialParams : register(b4)
{
	float4 camera_up;			// default 0
	float4 camera_dir;			// default 0
	float4 camera_left;			// default 0
	float plane_width;			// default 0
	float plane_height;			// default 0
	float plane_dist;			// default 0
	// full sized source image
	float4 envColor;			// default 0
	float envAngle;				// default 0
	float envFactor;			// default 0
	bool hasEnvTexture;			// default false
};
Texture2D envTexture : register(t4);

struct VS_OUTPUT
{
   float4 Pos: SV_POSITION;
   float2 img: TEXCOORD0;
};

VS_OUTPUT VSMain(float4 Pos: SV_POSITION, float2 UV : TEXCOORD0 )
{
   VS_OUTPUT Out;

   // Clean up inaccuracies
   Pos.xy = sign(Pos.xy);

   Out.Pos = float4(Pos.xy, 0, 1);
   Out.img.x = 0.5 * (1 + Pos.x);
   Out.img.y = 0.5 * (1 - Pos.y);

   return Out;
}

// Simple blur filter
float4 PS_Env(VS_OUTPUT v_in) : SV_TARGET 
{
	float imgX = v_in.img.x * 2.0f - 1.0f;
	float imgY = 1.0f - v_in.img.y * 2.0f;
	float3 dir = (plane_dist * camera_dir * 1.0f + 
				camera_left * imgX * plane_width * -1+ 
				camera_up * imgY * plane_height).xyz;
				
	dir = normalize(dir);
	
	float4 color = envColor * envFactor;
	if (hasEnvTexture)
	{
		color *= float4(SampleEnvDiffuse(dir, 
				envTexture, envAngle).rgb,1);
	}
	
	return color;
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

/***************************** eof ***/
