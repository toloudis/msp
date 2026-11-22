//////////////////////////////////////////////////////////////////////////////
// Converted from Cartoon.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// Cartoon.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Cartoon.fx
**
**      
**  John Schwab
**	Extra Large Technoloy
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
// The SasGlobal effect description (gp : SasGlobal) and the SAS UI
// annotations of the material parameters are in Cartoon.effect.json.
// Register layout shared by all materials: see Globals.hlsli.

/*********** support data and functions ******/

#include "Support.hlsli"
#include "Lighting.hlsli"
#include "Tessellate.hlsli"

// b4: this material's own parameters (defaults are in Cartoon.effect.json)
cbuffer MaterialParams : register(b4)
{
	float4 g_ambient;				// : MaterialAmbient, default (0.5,0.5,0.5,1)
	float4 g_midtone;				// : MaterialMidtone, default (0.5,0.5,0.5,1)
	float4 g_diffuse;				// : MaterialDiffuse, default (1,1,1,1)
	// float g_colorTransition0 (Ambient to Midtone, default 0.1) is disabled in Cartoon.fx
	float g_colorTransition1;		// default 0.5
	float g_smoothness;				// default 0.5
	float g_transparency;			// : Opacity, default 1
	bool hasTransparencyMap;		// default false
	bool g_hasSpecular;				// default false
	float4 g_specular;				// default (1,1,1,1)
	float g_specularPower;			// default 1
	float g_colorTransition2;		// default 0.9
	bool hasDiffuseMap;				// default false
	// bool hasGradientMap / texture1D gradientMap are disabled in Cartoon.fx
	bool hasSpecularMap;			// default false
	bool hasGlossMap;				// default false
};

Texture2D transparencyMap : register(t4);	// : OpacityTexture
Texture2D diffuseMap : register(t5);		// : DiffuseTexture
Texture2D specularMap : register(t6);
Texture2D glossMap : register(t7);
Texture2D glowMask : register(t8);			// : GlowMask

SamplerState AnisoWrapSampler : register(s10);

/*********** vertex shader ******/

TANGENT_VERTEX TangentVS( STANDARD_VERTEX In )
{
    TANGENT_VERTEX OUT;

	// decal and bump texture coords
    OUT.TexCoord0 = mul(g_uvTransform, float4(In.UV,0,1)).xy;
    OUT.UV = In.UV;
    
	float3 NewPos = In.Position;
    
    // output position in proj space
    float4 Po = float4(NewPos + In.Normal*g_glowSize, 1.0);
    
    // transform position to world space and get vector to light
    float3 Pw = mul( g_world, Po).xyz;
    OUT.WorldPos = Pw;

	OUT.WorldTan = TransformTangents( g_world, In.T, In.B, In.Normal );

    return OUT;
}

TANGENT_VERTEX_OUTPUT singleLightVS_Default( STANDARD_VERTEX Vtx, out float ClipDist : SV_ClipDistance0 ) 
{
	TANGENT_VERTEX_OUTPUT OUT;
	OUT.V = TangentVS( Vtx );

    float4 Po = float4( Vtx.Position + Vtx.Normal*g_glowSize, 1.0);
    OUT.HPosition = TransformVertex(Po, Vtx.UV, g_wvp );
	OUT.ScreenPos = float3(0,0,0);

	ClipDist = ClipWorldPos( OUT.V.WorldPos );

	return OUT;
}

TANGENT_TESS_OUTPUT singleLightVS_Tess( STANDARD_VERTEX Vtx ) 
{
	TANGENT_TESS_OUTPUT OUT;
	OUT.V = TangentVS( Vtx );

	return OUT;
}

/********* pixel shader ********/
float4 ToonShader(
	uniform float2 texCoord,
	uniform float3 WorldPos,
	uniform float3 WorldNormal,
	uniform IncidentLight Light
	)
{
   float4 color = float4(0,0,0,0);
   
	float3 lightDir = normalize(Light.L);
	float3 worldEyeDir = normalize(WorldPos - g_eyePos.xyz);
	float3 worldNormal = normalize(WorldNormal);

   float4 diffColor = float4(Light.Cld,1);
   if( hasDiffuseMap )
   {
      diffColor *= diffuseMap.Sample( AnisoWrapSampler, texCoord );
   }
   
   float dif = saturate(dot( lightDir, worldNormal ));
   
   float spec = 0;
   if( g_hasSpecular )
   {
  	  float spec = saturate(dot(worldNormal,normalize(lightDir - worldEyeDir)));
	  float3 specClr = g_specular.rgb * g_specularPower;
	  float specTrans = g_colorTransition2;
	  if( hasSpecularMap )
	  {
		  specClr = specularMap.Sample( AnisoWrapSampler, texCoord ).rgb;
	  }
	  if( hasGlossMap )
	  {
		  specTrans = glossMap.Sample( AnisoWrapSampler, texCoord ).r;
	  }
      float interp = saturate( 1 - ((specTrans - spec) / g_smoothness) );
	  color.rgb = specClr * Light.Cls * interp;
   }      
   
   float trans0 = g_colorTransition1 * 0.25f;
   if( dif > g_colorTransition1 )
   {
      float interp = saturate(((dif - g_colorTransition1) / ((1 - g_colorTransition1) * g_smoothness )));
      diffColor *= lerp( g_midtone, g_diffuse, interp );
   }
   else if( dif > trans0 )
   {
      float interp = saturate(((dif - trans0) / ((g_colorTransition1 - trans0) * g_smoothness)));
      diffColor *= lerp( g_ambient, g_midtone, interp );
   }
   else
   {
	   diffColor *= g_ambient;
   }

   color += diffColor;
   
	return color;
}

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN,
					uniform Texture2D DiffuseMap,
					uniform LightInfo i_Light,
					uniform bool i_bDefaultPass,
					bool vFace : SV_ISFRONTFACE )
{
    pixelOutput OUT;

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	IncidentLight light;
	IlluminatePointLight(IN.V.WorldPos, i_Light, light);
	
	float3 bumpNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );

	float3 diffuse = ToonShader( IN.V.TexCoord0, IN.V.WorldPos, bumpNormal/*IN.V.WorldTan.Z*/, light ).rgb;

    OUT.col.rgb = diffuse;
    
	if (i_bDefaultPass)
	{
		OUT.col.rgb += (g_ambient.rgb) + envmap_approximation(g_diffuseFactor).rgb;	
	}
    
	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput singleLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							   uniform Texture2D DiffuseMap,
								uniform LightInfo i_Light,
								uniform bool i_bDefaultPass,
								bool vFace : SV_ISFRONTFACE  )
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, DiffuseMap, i_Light, i_bDefaultPass, vFace );
}

pixelOutput singleLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
								  uniform Texture2D DiffuseMap,
								uniform LightInfo i_Light,
								uniform bool i_bDefaultPass,
								bool vFace : SV_ISFRONTFACE  )
{
	return singleLightPS( IN, DiffuseMap, i_Light, i_bDefaultPass, vFace );
}

pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D DiffuseMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap,
		uniform int nBlockerSamples, uniform int nShadowSamples,
		bool vFace : SV_ISFRONTFACE  )
{
    pixelOutput OUT;

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	IncidentLight light;
	IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, 
		nBlockerSamples, nShadowSamples, light);

	float3 bumpNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );

	float3 diffuse = ToonShader( IN.V.TexCoord0, IN.V.WorldPos, bumpNormal, light ).rgb;

    OUT.col.rgb = diffuse;
	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput projLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							 uniform Texture2D DiffuseMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform int nBlockerSamples, uniform int nShadowSamples,
							bool vFace : SV_ISFRONTFACE   )
{
	TangentDisplace( IN.V );
	return projLightPS( IN, DiffuseMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput projLightPS_Default( TANGENT_VERTEX_OUTPUT IN, 
								uniform Texture2D DiffuseMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform int nBlockerSamples, uniform int nShadowSamples,
							bool vFace : SV_ISFRONTFACE   )
{
	return projLightPS( IN, DiffuseMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D DiffuseMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap, bool vFace : SV_ISFRONTFACE )
{
    pixelOutput OUT; 
    OUT.col = float4(0,0,0,0);
    
	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

    float3 mask = g_bHasMask ? glowMask.Sample( AnisoWrapSampler, IN.V.TexCoord0).rgb : float3(1,1,1);
    if (g_bConstGlow)
    {
		OUT.col.rgb = mask;
    }
    else //if (mask.r+mask.g+mask.b > 0)
    {
		IncidentLight light;
		if (g_bProjLt)
			IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, light);
		else
			IlluminatePointLight(IN.V.WorldPos, i_Light, light);
		
		float3 lightDir = normalize(light.L);
		float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);

		// this material is all diffuse so lets just glow the diffuse lighting.    
		float3 sDiffColor = Tex2DCombine(hasDiffuseMap, DiffuseMap, IN.V.TexCoord0, g_diffuse).rgb;

		float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace ); //GetNormal( IN.V, vFace );
	        
		OUT.col.rgb = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor) * mask;
	}
    return OUT;
}

pixelOutput glowPS_Tess( TANGENT_VERTEX_OUTPUT IN,
						uniform Texture2D DiffuseMap,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap,
						bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return glowPS( IN, DiffuseMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput glowPS_Default( TANGENT_VERTEX_OUTPUT IN,
						   uniform Texture2D DiffuseMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
						   bool vFace : SV_ISFRONTFACE )
{
	return glowPS( IN, DiffuseMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace ); //GetNormal( IN.V, vFace );
		
	float4 diff = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.V.TexCoord0, g_diffuse*g_envDiffuseColor*g_diffuseFactor);
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvMap, g_diffuseEnvAngle).rgb,1);
	}

	OUT.col.rgb = g_IsolateReflection ? 0 : diff.rgb;

	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput iblPS_Tess( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return iblPS( IN, vFace );
}

pixelOutput iblPS_Default( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE )
{
	return iblPS( IN, vFace );
}

/*************/


/***************************** eof ***/

//////////////////////////////////////////////////////////////////////////////
// Entry points generated from the technique/pass compile statements.
// Each one binds the uniform arguments the .fx passed in its compile call.
//////////////////////////////////////////////////////////////////////////////

pixelOutput Default_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Default(IN, diffuseMap, g_lightInfo, true, vFace);
}

pixelOutput Default_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Tess(IN, diffuseMap, g_lightInfo, true, vFace);
}

pixelOutput SingleLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Default(IN, diffuseMap, g_lightInfo, false, vFace);
}

pixelOutput SingleLight_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Tess(IN, diffuseMap, g_lightInfo, false, vFace);
}

pixelOutput ProjectedLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffuseMap, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, vFace);
}

pixelOutput ProjectedLight_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffuseMap, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, vFace);
}

pixelOutput ProjectedLightSuperSample_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffuseMap, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED, vFace);
}

pixelOutput ProjectedLightSuperSample_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffuseMap, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED, vFace);
}

pixelOutput ProjectedLightSuperSample2_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffuseMap, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample2_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffuseMap, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample3_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffuseMap, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample3_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffuseMap, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH, vFace);
}

pixelOutput Glow_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return glowPS_Default(IN, diffuseMap, g_lightInfo, g_projLight, projLightMap, projShadowMap, vFace);
}

pixelOutput Glow_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return glowPS_Tess(IN, diffuseMap, g_lightInfo, g_projLight, projLightMap, projShadowMap, vFace);
}

pixelOutput DOFPrep_PDefault_PS(TANGENT_VERTEX_OUTPUT IN)
{
    return DOFPrepTrans_PS(IN, hasTransparencyMap, transparencyMap, g_transparency);
}
