//////////////////////////////////////////////////////////////////////////////
// Converted from Anisotropic.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// Anisotropic.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Anisotropic.fx
**
**      Car Paint
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
**  Version 0.1
\****************************************************************************/
// The SasGlobal effect description (gp : SasGlobal) and the SAS UI
// annotations of the material parameters are in Anisotropic.effect.json.
// Register layout shared by all materials: see Globals.hlsli.

/*********** support data and functions ******/

#include "Support.hlsli"
#include "Tessellate.hlsli"
#include "Lighting.hlsli"

// b4: this material's own parameters (defaults are in Anisotropic.effect.json)
cbuffer MaterialParams : register(b4)
{
	bool g_isPlanar;					// : ReflectionMapIsPlanar, default false
	bool g_hasCubeMap;					// : HasReflectionMap, default false
	float4 g_diffuse;					// : MaterialDiffuse, default (1, 1, 1, 1)
	bool hasDiffuseMap;					// default false
	float4 g_emissive;					// : MaterialEmissive, default (0, 0, 0, 1)
	bool hasEmissiveMap;				// default false
	float g_emissiveIntensity;			// default 1
	float4 g_specular;					// : MaterialSpecular, default (1, 1, 1, 1)
	bool hasSpecularMap;				// default false
	float SpecularColorPower;			// default 1
	bool hasSpecularColorPowerMap;		// default false
	float g_shininess;					// default 1
	bool hasShininessMap;				// default false
	float g_UPower;						// default 1
	float g_VPower;						// default 0
	bool hasAnisotropyMap;				// default false
	float g_transparency;				// : Opacity, default 1
	bool hasTransparencyMap;			// default false
	float g_reflectivity;				// : Reflectivity, default 1
	float fresnelPower;					// default 4
	float fresnelBias;					// default 0.2
	// mipmap LOD value scaling for cubemap
	float reflScale;					// default 0
	float g_reflectionEnvAngle;			// default 0
	bool hasCubeReflMap;				// default false
	bool hasReflectFactorMap;			// default false
	float g_specularBias;				// default 0
	float g_specularBias2;				// default 0
};

Texture2D diffuseMap : register(t4);	// : DiffuseTexture
Texture2D emissiveMap : register(t5);	// : EmissiveTexture
Texture2D specularMap : register(t6);
Texture2D specularColorPowerMap : register(t7);
Texture2D shininessMap : register(t8);
Texture2D anisotropyMap : register(t9);
Texture2D transparencyMap : register(t10);	// : OpacityTexture
Texture2D CubeReflMap : register(t11);
Texture2D reflectFactorMap : register(t12);
Texture2D glowMask : register(t13);	// : GlowMask

// semantics will cause automatic binding to code-generated reflection map
// (renderer-provided, so in the t20+ range of Globals.hlsli)
TextureCube g_cubeMap : register(t20);	// : CubeReflectionMap
Texture2D g_planarMap : register(t21);	// : PlanarReflectionMap

SamplerState g_reflCubeSampler : register(s10);
SamplerState planarSampler : register(s11);
SamplerState AnisoWrapSampler : register(s12);
SamplerState AnisoClampSampler : register(s13);

    
/*********** vertex shader ******/

TANGENT_VERTEX TangentVS( STANDARD_VERTEX In )
{
    TANGENT_VERTEX OUT;

	// decal and bump texture coords
    OUT.UV = In.UV;

    OUT.TexCoord0 = mul(g_uvTransform, float4(In.UV,0,1)).xy;
    
	float3 newPos = In.Position;
    
    // output position in proj space
    float4 Po = float4(newPos + In.Normal*g_glowSize, 1.0);
    
    // transform position to world space and get vector to light
    OUT.WorldPos = mul( g_world, Po).xyz;

	OUT.WorldTan = TransformTangents( g_world, In.T, In.B, In.Normal );

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




//Jim Blinn Model for Specular Reflection
//http://www.siggraph.org/education/materials/HyperGraph/illumin/specular_highlights/blinn_model_for_specular_reflect_1.htm

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
	return specComp * (lSpecColor * sSpecColor);
}

float WardAnisotropicSpecular(float3 normal, float3 lightDir, float3 eyeDir, float3 Tangent, float3 Binormal, float Ax, float Ay )
{
	//inputs
	//V = surface to eye unit vector
	//L = surface to light unit vector
	//H = half angle
	//N = Surface normal unit vector
	//X = Tangent
	//Y = Binormal
	//Ax = x anisotropy
	//Ay = y anisotropy
	
	float3 V = eyeDir;
	float3 L = lightDir;
	float3 H = normalize(V + L);
	float3 N = normal;
	float3 X = Tangent;
	float3 Y = Binormal;
	
	//Ay = clamp(Ay, 0, 90) * PI_DIV_180;
	//float wx = max(Ax * sin(Ay), AThres);
	//float wy = max(Ax * cos(Ay), AThres);
	
	// Set Ax, Ay make sure wx, wy they are not going to zero
	Ay = clamp(Ay, 0.1, 89.9) * PI_DIV_180;
	Ax = clamp(Ax, 0, 1.0);	
	
	float wx = 0.5 - 0.5 * Ax * (cos(Ay) - sin(Ay));
	float wy = 0.5 + 0.5 * Ax * (cos(Ay) - sin(Ay));

	//precalc
	float NdotL = dot(N, L);
	float NdotH = dot(N, H);
	float NdotV = dot(N, V);
	float HdotX = dot(H, X) / wx;
	float HdotY = dot(H, Y) / wy;
	
	if( NdotL < 0 ) return 0;
	if( NdotV < 0 ) return 0;

	//specular distribution term
	float power = -2 * ( (HdotX*HdotX + HdotY*HdotY) / (1 + NdotH) );
	float coef = rsqrt( NdotL * NdotV );
	float refl = NdotL / (4 * wx * wy);
	float term = coef * refl * exp( power );

	return term;
}

float3 AnisotropicShaderSpecular( TANGENT_VERTEX_OUTPUT IN, float3 worldNormal, float3 lightDir, 
						   float3 worldEyeDir, IncidentLight light, float SpecularPowerScale )
{
	float3 worldTan = normalize( IN.V.WorldTan.X );
	float3 worldBinorm = -normalize( IN.V.WorldTan.Y );
	
	float3 specularCombine = Tex2DCombine(hasSpecularMap, specularMap, IN.V.TexCoord0, g_specular).rgb;
	float specularPowerCombine = Tex2DCombine(hasSpecularColorPowerMap, specularColorPowerMap, IN.V.TexCoord0, SpecularColorPower).r;
	float shininessCombine = Tex2DCombine(hasShininessMap, shininessMap, IN.V.TexCoord0, SpecularPowerScale).r;
	float anisotropy = WardAnisotropicSpecular( worldNormal, lightDir, -worldEyeDir, worldTan, worldBinorm, g_UPower, g_VPower );
	float anisotropyCombine = saturate(Tex2DCombine(hasAnisotropyMap, anisotropyMap, IN.V.TexCoord0, anisotropy).r);

	float3 specular = specularPowerCombine * light.Cls * specularCombine * pow(anisotropyCombine, max(shininessCombine, 0.001)) ;
	
	
	return specular;
}

pixelOutput AnisotropicShaderPS( TANGENT_VERTEX_OUTPUT IN,
						   uniform IncidentLight light,
						   uniform float SpecPowerScale,
		  				   uniform float reflectivity,
						   bool UseReflection, 
							bool i_bDefaultPass,
						   float vFace)
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
	
	//fetch bump normal
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );

	OUT.col.rgb = AnisotropicShaderSpecular( IN, worldNormal, lightDir, worldEyeDir, light, SpecPowerScale );

  //calculate car paint color
	//float ndv = dot(-worldEyeDir,worldNormal);
	//float Cfresnel = saturate(fastFresnel( ndv, fresnelClrBias, fresnelClrPower ));
	float3 diff = Tex2DCombine(hasDiffuseMap, diffuseMap, IN.V.TexCoord0, g_diffuse).rgb;
	float3 Paint = diff;
	//float3 Paint = diff, g_edgeColor.rgb, Cfresnel );

	float3 diffuse = PhongDiffuse(worldNormal, lightDir, light.Cld, Paint);
	if (i_bDefaultPass)
	{
		float3 emissive = Tex2DCombine(hasEmissiveMap, emissiveMap, IN.V.TexCoord0, (g_emissive * g_emissiveIntensity)).rgb;
		diffuse +=  emissive + envmap_approximation(g_diffuseFactor).rgb;	
	}
	OUT.col.rgb += diffuse * OUT.col.a; //premul alpha


	return OUT;
}

pixelOutput singleLightPS( TANGENT_VERTEX_OUTPUT IN,
					uniform LightInfo i_Light,
					uniform float SpecPowerScale,
		  			uniform float reflectivity, 
					uniform bool i_bDefaultPass,
		  			bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 
    
	IncidentLight light;
	IlluminatePointLight(IN.V.WorldPos, i_Light, light);

	return AnisotropicShaderPS( IN, light, SpecPowerScale, reflectivity, false, i_bDefaultPass, vFace );
}

pixelOutput singleLightPS_Tess( TANGENT_VERTEX_OUTPUT IN, uniform LightInfo i_Light,
					uniform float SpecPowerScale,
		  			uniform float reflectivity,
					uniform bool i_bDefaultPass,
					bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, i_Light, SpecPowerScale, reflectivity, i_bDefaultPass, vFace );
}

pixelOutput singleLightPS_Default( TANGENT_VERTEX_OUTPUT IN,uniform LightInfo i_Light,
					uniform float SpecPowerScale,
		  			uniform float reflectivity,
					uniform bool i_bDefaultPass,
					bool vFace : SV_ISFRONTFACE )
{
	return singleLightPS( IN, i_Light, SpecPowerScale, reflectivity, i_bDefaultPass, vFace );
}

pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform float SpecPowerScale,
		uniform float reflectivity,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap,
		uniform int nBlockerSamples, uniform int nShadowSamples, bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 

	IncidentLight light;
	IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, 
		nBlockerSamples, nShadowSamples, light);

	return AnisotropicShaderPS( IN, light, SpecPowerScale, reflectivity, false, false, vFace );
}

pixelOutput projLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							 uniform LightInfo i_Light,
 							uniform ProjLightInfo i_ProjLight,
							uniform float SpecPowerScale,
							uniform float reflectivity,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform int nBlockerSamples, uniform int nShadowSamples,
							bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return projLightPS( IN, i_Light, i_ProjLight, SpecPowerScale, reflectivity, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput projLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform float SpecPowerScale,
							uniform float reflectivity,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform int nBlockerSamples, uniform int nShadowSamples,
							bool vFace : SV_ISFRONTFACE )
{
	return projLightPS( IN, i_Light, i_ProjLight, SpecPowerScale, reflectivity, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN,
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

		//fetch bump normal
		float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );

		OUT.col.rgb = AnisotropicShaderSpecular( IN, worldNormal, lightDir, 
										   worldEyeDir, light, SpecPowerScale ) * mask;
	}	
    return OUT;
}

pixelOutput glowPS_Tess( TANGENT_VERTEX_OUTPUT IN,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform float SpecPowerScale,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap,
						bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return glowPS( IN, i_Light, i_ProjLight, SpecPowerScale, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput glowPS_Default( TANGENT_VERTEX_OUTPUT IN,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform float SpecPowerScale,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap,
						bool vFace : SV_ISFRONTFACE )
{
	return glowPS( IN, i_Light, i_ProjLight, SpecPowerScale, ProjTextureMap, ProjShadowMap, vFace );
}


pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
	//fetch bump normal
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );

	float4 spec = g_envSpecularColor*g_specularFactor;
	if (g_bHasSpecularEnvMap) 
	{
		spec *= SampleEnvironment((-worldEyeDir), worldNormal, 
				specularEnvMap, g_specularEnvAngle);
	}
		
	float4 diff = g_diffuse*g_envDiffuseColor*g_diffuseFactor;
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvMap, g_diffuseEnvAngle).rgb,1);
	}

	float ndv = dot(-worldEyeDir,worldNormal);
	float fresnel = saturate(fastFresnel( ndv, fresnelBias, fresnelPower ));
	float3 reflVect = reflect( worldEyeDir, worldNormal );
	float4 refl = float4(0,0,0,0);
	//if( g_useDynamicCubeMap )
	//{
		//if( g_hasCubeMap ) refl = texCUBEbias( dynCubeMap, float4(reflVect, reflScale*fresnel)) * g_reflectivity;
	//}
	//else
	//{
		if( hasCubeReflMap && g_bCubeMapEnabled )
		{			
			float3 rotReflVect = rotateAboutY(reflVect, g_reflectionEnvAngle*PI_DIV_180);
			refl = CubeReflMap.SampleLevel( AnisoClampSampler, CartesianToPolar( rotReflVect ), reflScale*fresnel ) * g_reflectivity;
		}
	//}

	refl = Tex2DCombine(hasReflectFactorMap, reflectFactorMap, IN.V.TexCoord0, refl);
	refl *= fresnel;

	float3 emissive = Tex2DCombine(hasEmissiveMap, emissiveMap, IN.V.TexCoord0, g_emissive * g_emissiveIntensity).rgb;
	OUT.col.rgb = g_IsolateReflection ? refl.rgb : emissive + spec.rgb*g_reflectivity + diff.rgb + refl.rgb;
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
    return singleLightPS_Default(IN, g_lightInfo, g_shininess, g_reflectivity, true, vFace);
}

pixelOutput Default_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Tess(IN, g_lightInfo, g_shininess, g_reflectivity, true, vFace);
}

pixelOutput SingleLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Default(IN, g_lightInfo, g_shininess, g_reflectivity, false, vFace);
}

pixelOutput SingleLight_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Tess(IN, g_lightInfo, g_shininess, g_reflectivity, false, vFace);
}

pixelOutput ProjectedLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, g_lightInfo, g_projLight, g_shininess, g_reflectivity, projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, vFace);
}

pixelOutput ProjectedLight_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, g_lightInfo, g_projLight, g_shininess, g_reflectivity, projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, vFace);
}

pixelOutput ProjectedLightSuperSample_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, g_lightInfo, g_projLight, g_shininess, g_reflectivity, projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED, vFace);
}

pixelOutput ProjectedLightSuperSample_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, g_lightInfo, g_projLight, g_shininess, g_reflectivity, projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED, vFace);
}

pixelOutput ProjectedLightSuperSample2_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, g_lightInfo, g_projLight, g_shininess, g_reflectivity, projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample2_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, g_lightInfo, g_projLight, g_shininess, g_reflectivity, projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample3_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, g_lightInfo, g_projLight, g_shininess, g_reflectivity, projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample3_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, g_lightInfo, g_projLight, g_shininess, g_reflectivity, projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH, vFace);
}

pixelOutput Glow_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return glowPS_Default(IN, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, vFace);
}

pixelOutput Glow_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return glowPS_Tess(IN, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, vFace);
}

pixelOutput DOFPrep_PDefault_PS(TANGENT_VERTEX_OUTPUT IN)
{
    return DOFPrepTrans_PS(IN, hasTransparencyMap, transparencyMap, g_transparency);
}
