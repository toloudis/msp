//////////////////////////////////////////////////////////////////////////////
// Converted from Phong_wBump.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// Phong_wBump.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  PhongBump.fx
**
**      phong with normal map
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
// The SasGlobal effect description (gp : SasGlobal) and the SAS UI
// annotations of the material parameters are in Phong_wBump.effect.json.
// Register layout shared by all materials: see Globals.hlsli.

/*********** support data and functions ******/

#include "Support.hlsli"
#include "Tessellate.hlsli"
#include "Lighting.hlsli"

// b4: this material's own parameters (defaults are in Phong_wBump.effect.json)
cbuffer MaterialParams : register(b4)
{
	float4 g_diffuse;				// : MaterialDiffuse, default (1,1,1,1)
	bool hasDiffuseMap;				// default false
	float4 g_emissive;				// : MaterialEmissive, default (0,0,0,1)
	bool hasEmissiveMap;			// default false
	float g_emissiveIntensity;		// default 1
	float4 g_specular;				// : MaterialSpecular, default (1,1,1,1)
	bool hasSpecularMap;			// default false
	bool hasGlossMap;				// default false
	float g_shininess;				// : MaterialPower, default 20
	float g_transparency;			// : Opacity, default 1
	bool hasTransparencyMap;		// default false
	float g_reflectivity;			// : Reflectivity, default 1
	float fresnelPower;				// default 4
	float fresnelBias;				// default 0.2
	float reflBlur;					// default 0
	float g_reflMapAngle;			// default 0
	bool hasCubeMap;				// default false
	bool hasReflectFactorMap;		// default false
};

Texture2D diffuseMap : register(t4);			// : DiffuseTexture
Texture2D emissiveMap : register(t5);			// : EmissiveTexture
Texture2D specularMap : register(t6);
Texture2D glossMap : register(t7);
Texture2D transparencyMap : register(t8);		// : OpacityTexture
Texture2D cubeMap : register(t9);				// lat-long reflection map
Texture2D reflectFactorMap : register(t10);
Texture2D glowMask : register(t11);				// : GlowMask

/*********** vertex shader ******/

TANGENT_VERTEX TangentVS( STANDARD_VERTEX In )
{
    TANGENT_VERTEX OUT;

    OUT.TexCoord0 = mul(g_uvTransform, float4(In.UV,0,1)).xy;
    
	float3 newPos = In.Position;
    
    // output position in proj space
    float4 Po = float4(newPos + In.Normal*g_glowSize, 1.0);
    
    // transform position to world space and get vector to light
    float3 Pw = mul(g_world, Po).xyz;
    OUT.WorldPos = Pw;

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

/********* pixel shader ********/

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN,
					uniform Texture2D DiffuseMap,
					uniform Texture2D NormalMap,
					uniform Texture2D SpecularMap,
		  			uniform Texture2D CubeMap,
					uniform LightInfo i_Light,
					uniform Texture2D SpecPowerMap,
					uniform float SpecPowerScale,
		  			uniform float reflectivity,
					uniform Texture2D TransparencyMap,
					uniform float transparency, 
					uniform bool i_bDefaultPass, 
					bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, TransparencyMap, IN.V.TexCoord0, transparency).x;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	IncidentLight light;
	IlluminatePointLight(IN.V.WorldPos, i_Light, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
    
	//fetch base and specular colors
	float4 sDiffColor = Tex2DCombine(hasDiffuseMap, DiffuseMap, IN.V.TexCoord0, g_diffuse);
	float4 sSpecColor = Tex2DCombine(hasSpecularMap, SpecularMap, IN.V.TexCoord0, g_specular);
	float sSpecPower = Tex2DCombine(hasGlossMap, SpecPowerMap, IN.V.TexCoord0, SpecPowerScale).r;
    	
	//fetch bump normal
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );

	float3 diffuse = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor.xyz);
	float3 specular = PhongSpecular(worldNormal, lightDir, -worldEyeDir, 
		light.Cls, sSpecColor.xyz, sSpecPower);

    OUT.col.rgb = diffuse + specular;

	if (i_bDefaultPass)
	{
		float3 emissive = Tex2DCombine(hasEmissiveMap, emissiveMap, IN.V.TexCoord0, g_emissive * g_emissiveIntensity).rgb;
		OUT.col.rgb += emissive + envmap_approximation(g_diffuseFactor).rgb;	
	}

	OUT.col.rgb *= OUT.col.a;
    return OUT;
}

pixelOutput singleLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							   uniform Texture2D DiffuseMap,
								uniform Texture2D NormalMap,
								uniform Texture2D SpecularMap,
		  						uniform Texture2D CubeMap,
								uniform LightInfo i_Light,
								uniform Texture2D SpecPowerMap,
								uniform float SpecPowerScale,
		  						uniform float reflectivity,
								uniform Texture2D TransparencyMap,
								uniform float transparency, 
								uniform bool i_bDefaultPass, 
							   bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, DiffuseMap, NormalMap, SpecularMap, CubeMap, i_Light, SpecPowerMap, SpecPowerScale, 
		reflectivity, TransparencyMap, transparency, i_bDefaultPass, vFace );
}

pixelOutput singleLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
								  uniform Texture2D DiffuseMap,
								uniform Texture2D NormalMap,
								uniform Texture2D SpecularMap,
		  						uniform Texture2D CubeMap,
								uniform LightInfo i_Light,
								uniform Texture2D SpecPowerMap,
								uniform float SpecPowerScale,
		  						uniform float reflectivity,
								uniform Texture2D TransparencyMap,
								uniform float transparency, 
								uniform bool i_bDefaultPass, 
								  bool vFace : SV_ISFRONTFACE )
{
	return singleLightPS( IN, DiffuseMap, NormalMap, SpecularMap, CubeMap, i_Light, SpecPowerMap, SpecPowerScale, 
		reflectivity, TransparencyMap, transparency, i_bDefaultPass, vFace );
}

pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D DiffuseMap,
		uniform Texture2D NormalMap,
		uniform Texture2D SpecularMap,
		uniform Texture2D CubeMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D SpecPowerMap,
		uniform float SpecPowerScale,
		uniform float reflectivity,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap,
		uniform Texture2D TransparencyMap,
		uniform float transparency,
		uniform int nBlockerSamples, uniform int nShadowSamples, bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, TransparencyMap, IN.V.TexCoord0, transparency).x;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	IncidentLight light;
	IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, 
		nBlockerSamples, nShadowSamples, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
    
	//fetch base and specular colors
	float4 sDiffColor = Tex2DCombine(hasDiffuseMap, DiffuseMap, IN.V.TexCoord0, g_diffuse);
	float4 sSpecColor = Tex2DCombine(hasSpecularMap, SpecularMap, IN.V.TexCoord0, g_specular);
	float sSpecPower = Tex2DCombine(hasGlossMap, SpecPowerMap, IN.V.TexCoord0, SpecPowerScale).r;

	//fetch bump normal
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );

	float3 diffuse = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor.xyz);
	float3 specular = PhongSpecular(worldNormal, lightDir, -worldEyeDir, 
		light.Cls, sSpecColor.xyz, sSpecPower);

    OUT.col.rgb = diffuse + specular;

	OUT.col.rgb *= OUT.col.a;
    return OUT;
}

pixelOutput projLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							 uniform Texture2D DiffuseMap,
							uniform Texture2D NormalMap,
							uniform Texture2D SpecularMap,
							uniform Texture2D CubeMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform Texture2D SpecPowerMap,
							uniform float SpecPowerScale,
							uniform float reflectivity,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform Texture2D TransparencyMap,
							uniform float transparency,
							uniform int nBlockerSamples, uniform int nShadowSamples, 
							 bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return projLightPS( IN, DiffuseMap, NormalMap, SpecularMap, CubeMap, i_Light, i_ProjLight, SpecPowerMap, SpecPowerScale, reflectivity, ProjTextureMap, ProjShadowMap, TransparencyMap, transparency, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput projLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
								uniform Texture2D DiffuseMap,
								uniform Texture2D NormalMap,
								uniform Texture2D SpecularMap,
								uniform Texture2D CubeMap,
								uniform LightInfo i_Light,
								uniform ProjLightInfo i_ProjLight,
								uniform Texture2D SpecPowerMap,
								uniform float SpecPowerScale,
								uniform float reflectivity,
								uniform Texture2D ProjTextureMap,
								uniform Texture2D ProjShadowMap,
								uniform Texture2D TransparencyMap,
								uniform float transparency,
								uniform int nBlockerSamples, uniform int nShadowSamples, 
								bool vFace : SV_ISFRONTFACE )
{
	return projLightPS( IN, DiffuseMap, NormalMap, SpecularMap, CubeMap, i_Light, i_ProjLight, SpecPowerMap, SpecPowerScale, reflectivity, ProjTextureMap, ProjShadowMap, TransparencyMap, transparency, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D NormalMap,
		uniform Texture2D SpecularMap,
		uniform Texture2D SpecPowerMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform float SpecPowerScale,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap, bool vFace : SV_ISFRONTFACE
) {
    pixelOutput OUT; 
    OUT.col = float4(0,0,0,0);
    
	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

    float3 mask = g_bHasMask ? glowMask.Sample(g_DefaultSampler, IN.V.TexCoord0.xy).rgb : float3(1,1,1);
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
    
		//fetch base and specular colors
		float4 sSpecColor = Tex2DCombine(hasSpecularMap, SpecularMap, IN.V.TexCoord0, g_specular);
		float sSpecPower = Tex2DCombine(hasGlossMap, SpecPowerMap, IN.V.TexCoord0, SpecPowerScale).r;
	        
		//fetch bump normal
		float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
	        
		OUT.col.rgb = PhongSpecular(worldNormal, lightDir, -worldEyeDir, 
			light.Cls, sSpecColor.xyz, sSpecPower) * mask;
	}

    return OUT;
}

pixelOutput glowPS_Tess( TANGENT_VERTEX_OUTPUT IN,
						uniform Texture2D NormalMap,
						uniform Texture2D SpecularMap,
						uniform Texture2D SpecPowerMap,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform float SpecPowerScale,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap, 
						bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return glowPS( IN, NormalMap, SpecularMap, SpecPowerMap, i_Light, i_ProjLight, SpecPowerScale, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput glowPS_Default( TANGENT_VERTEX_OUTPUT IN,
						   uniform Texture2D NormalMap,
							uniform Texture2D SpecularMap,
							uniform Texture2D SpecPowerMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform float SpecPowerScale,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap, 
						   bool vFace : SV_ISFRONTFACE )
{
	return glowPS( IN, NormalMap, SpecularMap, SpecPowerMap, i_Light, i_ProjLight, SpecPowerScale, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN,
		  uniform Texture2D NormalMap, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).x;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );

	float4 refl = 0;
	if (hasCubeMap && g_bCubeMapEnabled)
	{    		
		refl = SampleEnvironmentLOD((-worldEyeDir), worldNormal, cubeMap , g_reflMapAngle*PI_DIV_180 , reflBlur );
		refl *= g_reflectivity;
		refl *= fastFresnel(dot(-worldEyeDir, worldNormal), fresnelBias, fresnelPower);
		refl = Tex2DCombine(hasReflectFactorMap, reflectFactorMap, IN.V.TexCoord0, refl);
	}

	float4 spec = g_reflectivity*g_envSpecularColor*g_specularFactor;
	if (g_bHasSpecularEnvMap) 
	{
		spec *= SampleEnvironment((-worldEyeDir), worldNormal, 
				specularEnvMap, g_specularEnvAngle);
	}
		
	float4 diff = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.V.TexCoord0, g_diffuse*g_envDiffuseColor*g_diffuseFactor);
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvMap, g_diffuseEnvAngle).rgb,1);
	}
		
	float3 emissive = Tex2DCombine(hasEmissiveMap, emissiveMap, IN.V.TexCoord0, g_emissive * g_emissiveIntensity).rgb;
	OUT.col.rgb = g_IsolateReflection ? refl.rgb : emissive + spec.rgb + refl.rgb + diff.rgb;
	OUT.col.rgb *= OUT.col.a;

    return OUT;
}

pixelOutput iblPS_Tess( TANGENT_VERTEX_OUTPUT IN, uniform Texture2D NormalMap, bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return iblPS( IN, NormalMap, vFace );
}
pixelOutput iblPS_Default( TANGENT_VERTEX_OUTPUT IN, uniform Texture2D NormalMap, bool vFace : SV_ISFRONTFACE )
{
	return iblPS( IN, NormalMap, vFace );
}

/*************/


/***************************** eof ***/

//////////////////////////////////////////////////////////////////////////////
// Entry points generated from the technique/pass compile statements.
// Each one binds the uniform arguments the .fx passed in its compile call.
//////////////////////////////////////////////////////////////////////////////

pixelOutput Default_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Default(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, glossMap, g_shininess, g_reflectivity, transparencyMap, g_transparency, true, vFace);
}

pixelOutput Default_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Tess(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, glossMap, g_shininess, g_reflectivity, transparencyMap, g_transparency, true, vFace);
}

pixelOutput SingleLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Default(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, glossMap, g_shininess, g_reflectivity, transparencyMap, g_transparency, false, vFace);
}

pixelOutput SingleLight_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Tess(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, glossMap, g_shininess, g_reflectivity, transparencyMap, g_transparency, false, vFace);
}

pixelOutput ProjectedLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_projLight, glossMap, g_shininess, g_reflectivity, projLightMap, projShadowMap, transparencyMap, g_transparency, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, vFace);
}

pixelOutput ProjectedLight_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_projLight, glossMap, g_shininess, g_reflectivity, projLightMap, projShadowMap, transparencyMap, g_transparency, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, vFace);
}

pixelOutput ProjectedLightSuperSample_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_projLight, glossMap, g_shininess, g_reflectivity, projLightMap, projShadowMap, transparencyMap, g_transparency, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED, vFace);
}

pixelOutput ProjectedLightSuperSample_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_projLight, glossMap, g_shininess, g_reflectivity, projLightMap, projShadowMap, transparencyMap, g_transparency, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED, vFace);
}

pixelOutput ProjectedLightSuperSample2_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_projLight, glossMap, g_shininess, g_reflectivity, projLightMap, projShadowMap, transparencyMap, g_transparency, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample2_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_projLight, glossMap, g_shininess, g_reflectivity, projLightMap, projShadowMap, transparencyMap, g_transparency, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample3_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_projLight, glossMap, g_shininess, g_reflectivity, projLightMap, projShadowMap, transparencyMap, g_transparency, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample3_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_projLight, glossMap, g_shininess, g_reflectivity, projLightMap, projShadowMap, transparencyMap, g_transparency, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH, vFace);
}

pixelOutput Glow_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return glowPS_Default(IN, normalMap, specularMap, glossMap, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, vFace);
}

pixelOutput Glow_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return glowPS_Tess(IN, normalMap, specularMap, glossMap, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, vFace);
}

pixelOutput DOFPrep_PDefault_PS(TANGENT_VERTEX_OUTPUT IN)
{
    return DOFPrepTrans_PS(IN, hasTransparencyMap, transparencyMap, g_transparency);
}

pixelOutput Environment_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return iblPS_Default(IN, normalMap, vFace);
}

pixelOutput Environment_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return iblPS_Tess(IN, normalMap, vFace);
}
