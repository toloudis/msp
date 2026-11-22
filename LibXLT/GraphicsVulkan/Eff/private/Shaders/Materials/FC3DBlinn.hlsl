//////////////////////////////////////////////////////////////////////////////
// Converted from FC3DBlinn.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// FC3DBlinn.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Blinn.fx
**
**      Blinn (Torrance-Sparrow) shader
**
**	Studio GPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
// The SasGlobal effect description (gp : SasGlobal) and the SAS UI
// annotations of the material parameters are in FC3DBlinn.effect.json.
// Register layout shared by all materials: see Globals.hlsli.

/*********** support data and functions ******/

#define FC3D 1
#include "Support.hlsli"
#include "Tessellate.hlsli"
#include "Lighting.hlsli"

// b4: this material's own parameters (defaults are in FC3DBlinn.effect.json)
cbuffer MaterialParams : register(b4)
{
	float4 g_diffuse;				// : MaterialDiffuse, default (1,1,1,1)
	bool hasDiffuseMap;				// default false
	float g_roughness;				// default 0
	float4 g_emissive;				// : MaterialEmissive, default (0,0,0,1)
	float g_emissiveIntensity;		// default 1
	float4 g_specular;				// : MaterialSpecular, default (1,1,1,1)
	bool hasSpecularMap;			// default false
	float g_specularPower;			// default 1
	float g_IOR;					// default 1
	float g_shininess;				// : MaterialPower, default 0.5
	float g_reflectivity;			// : Reflectivity, default 1
	float fresnelPower;				// default 4
	float fresnelBias;				// default 0.2
	float reflBlur;					// default 0
	float g_reflMapAngle;			// default 0
	bool hasCubeMap;				// default false
	float g_specularBias;			// default 0
};

Texture2D diffuseMap : register(t4);			// : DiffuseTexture
Texture2D specularMap : register(t5);
Texture2D cubeMap : register(t6);

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
    float4 Po = float4(NewPos, 1.0);
    
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

    float4 Po = float4( Vtx.Position, 1.0);
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

float OrenNayarDiffuse(float3 L, float3 I, float3 N, float roughness)
{
	float sigmasq = roughness*roughness;
	float A = 1.0 - 0.5 * sigmasq/(sigmasq+0.33);
	float B = 0.45 * sigmasq/(sigmasq+0.09);
	
	float IdotN = dot(I,N);
	float LdotN = dot(L,N);
	float theta_r = acos(IdotN);
	float theta_i = acos(LdotN);
	float Bfactor = max(0, cos(theta_i-theta_r));
	if (Bfactor > 0)
	{
		float alpha = max(theta_i, theta_r);
		float beta = min(theta_i, theta_r);
		Bfactor *= sin(alpha) * tan(beta);
	}
	return A + B * Bfactor;
}

float3 BlinnSpecular(float3 normal, float3 lightDir, float3 eyeDir, 
	float3 lSpecColor, float3 sSpecColor, float eccentricity, float IOR,
	float specularBias)
{
	// eyeDir is passed as dir FROM shade point TO eye
	// lightDir is passed as dir FROM shade point TO light

	float3 H = normalize(lightDir + eyeDir);
	// NdotH = cos a
	float NdotH = dot(normal, H);
	float NdotE = dot(normal, eyeDir);
	float EdotH = dot(eyeDir, H);
	float NdotL = dot(normal, lightDir);
	
	float Gb = 2 * NdotH * NdotE / EdotH;
	float Gc = 2 * NdotH * NdotL / EdotH; 
	float G = min(Gb, Gc);
	G = min(1, G);
	
	// eccentricity varies from 0 to 1. 
	// D3 = [ c^2 / (1 + cos^2(a)(c^2 - 1)) ]^2
	float c2 = eccentricity*eccentricity;
	float D3 = ( c2 / ( 1 + (NdotH*NdotH*(c2-1)) ) );
	float D = D3*D3;
	
	// eta = ni/nt. assume air-material is the interface
	float F = fresnel(NdotE, 1/(IOR+0.0001)); 

	float specComp = D * G * F / NdotE;
	specComp = max(specComp, 0);
	return (specComp * g_specularPower) * (lSpecColor * sSpecColor);
}

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN,
					uniform Texture2D DiffuseMap,
					uniform Texture2D NormalMap,
					uniform Texture2D SpecularMap,
		  			uniform Texture2D CubeMap,
					uniform LightInfo i_Light,
					uniform float SpecPowerScale,
					bool i_bDefaultPass,
		  			bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = 1;

	IncidentLight light;
	IlluminatePointLight(IN.V.WorldPos, i_Light, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
    
	//fetch base and specular colors
	float3 sDiffColor = Tex2DCombine(hasDiffuseMap, DiffuseMap, IN.V.TexCoord0, g_diffuse).rgb;
	float3 sSpecColor = Tex2DCombine(hasSpecularMap, SpecularMap, IN.V.TexCoord0, g_specular).rgb;
	float sSpecPower = SpecPowerScale;
    	
	//fetch bump normal
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
	
	// map 0..1 to 1..g_IOR 
	float ior = g_IOR;
	
	float3 diffuse = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor);
	if (g_roughness>0)
		diffuse *= OrenNayarDiffuse(lightDir, -worldEyeDir, worldNormal, g_roughness);
	float3 specular = BlinnSpecular(worldNormal, lightDir, -worldEyeDir, 
		light.Cls, sSpecColor, sSpecPower, ior, g_specularBias);

    OUT.col.rgb = diffuse + specular;

	if (i_bDefaultPass)
	{
		OUT.col.rgb += (g_emissive.rgb * g_emissiveIntensity) + envmap_approximation(g_diffuseFactor).rgb;	
	}
	
	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput singleLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
								uniform Texture2D DiffuseMap,
								uniform Texture2D normalMap,
								uniform Texture2D SpecularMap,
  								uniform Texture2D CubeMap,
								uniform LightInfo i_Light,
								uniform float SpecPowerScale,
								uniform bool i_bDefaultPass,
								bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, DiffuseMap, normalMap, SpecularMap, CubeMap, i_Light, SpecPowerScale, i_bDefaultPass, vFace );
}

pixelOutput singleLightPS_Default( TANGENT_VERTEX_OUTPUT IN, 
								uniform Texture2D DiffuseMap,
								uniform Texture2D normalMap,
								uniform Texture2D SpecularMap,
  								uniform Texture2D CubeMap,
								uniform LightInfo i_Light,
								uniform float SpecPowerScale,
								uniform bool i_bDefaultPass,
								bool vFace : SV_ISFRONTFACE )
{
	return singleLightPS( IN, DiffuseMap, normalMap, SpecularMap, CubeMap, i_Light, SpecPowerScale, i_bDefaultPass, vFace );
}

pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D DiffuseMap,
		uniform Texture2D NormalMap,
		uniform Texture2D SpecularMap,
		uniform Texture2D CubeMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform float SpecPowerScale,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap,
		uniform int nBlockerSamples, uniform int nShadowSamples, bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = 1;

	IncidentLight light;
	IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, 
		nBlockerSamples, nShadowSamples, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
    
	//fetch base and specular colors
	float3 sDiffColor = Tex2DCombine(hasDiffuseMap, DiffuseMap, IN.V.TexCoord0, g_diffuse).rgb;
	float3 sSpecColor = Tex2DCombine(hasSpecularMap, SpecularMap, IN.V.TexCoord0, g_specular).rgb;
	float sSpecPower = SpecPowerScale;
    	
	//fetch bump normal
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );
	
	// map 0..1 to 1..g_IOR 
	float ior = g_IOR;

	float3 diffuse = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor);
	if (g_roughness>0)
		diffuse *= OrenNayarDiffuse(lightDir, -worldEyeDir, worldNormal, g_roughness);
	float3 specular = BlinnSpecular(worldNormal, lightDir, -worldEyeDir, 
		light.Cls, sSpecColor, sSpecPower, ior, g_specularBias);

    OUT.col.rgb = diffuse + specular;

	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput projLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							uniform Texture2D DiffuseMap,
							uniform Texture2D normalMap,
							uniform Texture2D SpecularMap,
							uniform Texture2D CubeMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform float SpecPowerScale,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform int nBlockerSamples, uniform int nShadowSamples,
							bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return projLightPS( IN, DiffuseMap, normalMap, SpecularMap, CubeMap, i_Light, i_ProjLight, SpecPowerScale, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput projLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
							uniform Texture2D DiffuseMap,
							uniform Texture2D normalMap,
							uniform Texture2D SpecularMap,
							uniform Texture2D CubeMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform float SpecPowerScale,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform int nBlockerSamples, uniform int nShadowSamples,
							bool vFace : SV_ISFRONTFACE )
{
	return projLightPS( IN, DiffuseMap, normalMap, SpecularMap, CubeMap, i_Light, i_ProjLight, SpecPowerScale, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D NormalMap,
		uniform Texture2D SpecularMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform float SpecPowerScale,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap, bool vFace : SV_ISFRONTFACE
) {
    pixelOutput OUT; 
    OUT.col = float4(0,0,0,0);
    return OUT;
}

pixelOutput glowPS_Tess( TANGENT_VERTEX_OUTPUT IN,
						uniform Texture2D normalMap,
						uniform Texture2D SpecularMap,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform float SpecPowerScale,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap,
						bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return glowPS( IN, normalMap, SpecularMap, i_Light, i_ProjLight, SpecPowerScale, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput glowPS_Default( TANGENT_VERTEX_OUTPUT IN,
						uniform Texture2D normalMap,
						uniform Texture2D SpecularMap,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform float SpecPowerScale,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap,
						bool vFace : SV_ISFRONTFACE )
{
	return glowPS( IN, normalMap, SpecularMap, i_Light, i_ProjLight, SpecPowerScale, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN,
		  uniform Texture2D NormalMap, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = 1;
    
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );

	float4 refl = 0;
	if (hasCubeMap){		
		refl = SampleEnvironmentLOD((-worldEyeDir), worldNormal, cubeMap, g_reflMapAngle*PI_DIV_180, reflBlur);
		refl *= g_reflectivity;
		refl *= fastFresnel(dot(-worldEyeDir, worldNormal), fresnelBias, fresnelPower);
	}

	float4 diff = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.V.TexCoord0, g_diffuse*g_envDiffuseColor*g_diffuseFactor);
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvMap, g_diffuseEnvAngle).rgb,1);
	}
		
	OUT.col.rgb = g_IsolateReflection ? refl.rgb : g_emissive.rgb * g_emissiveIntensity + refl.rgb + diff.rgb;

	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput iblPS_Tess( TANGENT_VERTEX_OUTPUT IN, uniform Texture2D normalMap, bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return iblPS( IN, normalMap, vFace );
}

pixelOutput iblPS_Default( TANGENT_VERTEX_OUTPUT IN, uniform Texture2D normalMap, bool vFace : SV_ISFRONTFACE )
{
	return iblPS( IN, normalMap, vFace );
}

/*************/


/***************************** eof ***/

//////////////////////////////////////////////////////////////////////////////
// Entry points generated from the technique/pass compile statements.
// Each one binds the uniform arguments the .fx passed in its compile call.
//////////////////////////////////////////////////////////////////////////////

pixelOutput Default_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Default(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_shininess, true, vFace);
}

pixelOutput Default_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Tess(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_shininess, true, vFace);
}

pixelOutput SingleLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Default(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_shininess, false, vFace);
}

pixelOutput SingleLight_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Tess(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_shininess, false, vFace);
}

pixelOutput ProjectedLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, vFace);
}

pixelOutput ProjectedLight_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, vFace);
}

pixelOutput ProjectedLightSuperSample_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED, vFace);
}

pixelOutput ProjectedLightSuperSample_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED, vFace);
}

pixelOutput ProjectedLightSuperSample2_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample2_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample3_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample3_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffuseMap, normalMap, specularMap, cubeMap, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH, vFace);
}

pixelOutput Glow_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return glowPS_Default(IN, normalMap, specularMap, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, vFace);
}

pixelOutput Glow_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return glowPS_Tess(IN, normalMap, specularMap, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, vFace);
}

pixelOutput DOFPrep_PDefault_PS(TANGENT_VERTEX_OUTPUT IN)
{
    return DOFPrepTrans_PS(IN, false, normalMap, 1);	// the .fx passed NULL: the texture is unused when bHasTMap is false
}

pixelOutput Environment_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return iblPS_Default(IN, normalMap, vFace);
}

pixelOutput Environment_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return iblPS_Tess(IN, normalMap, vFace);
}
