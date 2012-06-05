/*****************************************************************************
**  EnvBackground.fx
**
**      render environment background on the current render target
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
int gp : SasGlobal
<
  int3 SasVersion = {1,0,0};
  string SasEffectAuthor			= "Riva Chang";
  string SasEffectAuthoringSoftware = "Visual Studio .NET 2003";
  string SasEffectCategory			= "special/envbackground";
  string SasEffectCompany			= "studio|gpu";
  string SasEffectDescription		= "render environment background on the current render target.";
  string SasEffectHelp				= "Good luck.";    
  string SasEffectRevision			= "$Revision$";  
>;

#include "..\Support.h"

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

float4 camera_up = 0;
float4 camera_dir = 0;
float4 camera_left = 0;
float plane_width = 0;
float plane_height = 0;
float plane_dist = 0;
// full sized source image
float4 envColor = 0;
float envAngle = 0;
float envFactor = 0;
bool hasEnvTexture = false;
Texture2D envTexture;

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

technique11 Default
{
	pass p0
	{		
		VertexShader = compile vs_5_0 VSMain();
		PixelShader = compile ps_5_0 PS_Env();
	}
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

/***************************** eof ***/
