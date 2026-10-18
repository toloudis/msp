//////////////////////////////////////////////////////////////////////////////
// Converted from SubSurfaceScatter.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// SubSurfaceScatter.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  SubSurfaceScatter.fx
**
Specular Map alpha is used for glossiness.

Features:
	- Diffuse
	- Soft Light blending algorithm
	- Normal map lighting model
	- Translucency kludge (based off melanin and hemoglobin tones)
	- Correct fresnel ramped specular

Known nasties:
	- Controls still quite tweaky
	- No depth ramp, it's a constant 
	(eg, no differentiation between a thin ear and a thick skull)
	
Referances & thanks:
	Ben Cloward - used your normal lighting method too :)
	Nvidia samples
	Direct X sdk
	Shader X2
	Polycount
	Sumea
	CGtalk
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
// The SasGlobal effect description (gp : SasGlobal) and the SAS UI
// annotations of the material parameters are in SubSurfaceScatter.effect.json.
// Register layout shared by all materials: see Globals.hlsli.

#include "Support.hlsli"
#include "Tessellate.hlsli"
#include "Lighting.hlsli"


// b4: this material's own parameters (defaults are in SubSurfaceScatter.effect.json)
cbuffer MaterialParams : register(b4)
{
	bool hasDiffTex;				// default false
	float4 g_transColIn;			// : MaterialDiffuse, default (0.87,0.91,0.96,1)
	bool hasTransMapIn;				// default false
	float4 g_transColOut;			// default (1,0.71,0.32,1)
	bool hasTransMapOut;			// default false
	float4 g_transColBack;			// default (0.58,0.2,0.24,1)
	bool hasTransMapBack;			// default false
	float g_transMultiplier;		// default 1
	bool hasTransTex;				// default false
	float g_transRampOff;			// default 1
	float4 g_specColor;				// default (0.45,0.65,1,1)
	float g_specPower;				// default 0.2
	float g_specGloss;				// Specular Map alpha is used to modulate glossiness. default 15
	float g_specFresnel;			// default 3
	float g_fresnelPower;			// default 15
	float g_fresnelGloss;			// default 0
	bool hasSpecTex;				// default false (Specular Map alpha is used to modulate glossiness)
	bool hasSpecPowerTex;			// default false
	bool hasMicroTex;				// default false
	float g_microScale;				// default 50
	float g_transparency;			// : Opacity, default 1
	bool hasTransparencyTex;		// default false
	float g_reflectivity;			// : Reflectivity, default 1
	float fresnelPower;				// default 4
	float fresnelBias;				// default 0.2
	float reflBlur;					// default 0
	float g_reflMapAngle;			// default 0
	bool hasCubeMap;				// default false
	bool hasReflectFactorMap;		// default false
};

Texture2D diffTex : register(t4);			// : DiffuseTexture
Texture2D transMapIn : register(t5);
Texture2D transMapOut : register(t6);
Texture2D transMapBack : register(t7);
Texture2D transTex : register(t8);
Texture2D specTex : register(t9);
Texture2D specPowerTex : register(t10);
Texture2D microTex : register(t11);
Texture2D transparencyTex : register(t12);	// : OpacityTexture
Texture2D cubeTex : register(t13);
Texture2D reflectFactorTex : register(t14);
Texture2D glowMask : register(t15);			// : GlowMask

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
    float3 Pw = mul(g_world, Po).xyz;
    OUT.WorldPos = Pw;

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


//=======================Translucency lighting model=======================
float4 TransPass(	float4 DotLN,
			float2 TexUV)
{
	float4 transSample = Tex2DReplace(hasTransTex, transTex, TexUV, 0);
	
	static const float4 one = float4(1,1,1,1);	
	float4 Translucence 	= smoothstep(-g_transRampOff  * transSample,one,abs(DotLN)) 
					;//- smoothstep(one,one,DotLN); // or should it be step(one, DotLN);//???
	
	float4 transColMapIn = Tex2DCombine(hasTransMapIn, transMapIn, TexUV, g_transColIn);
	float4 transColMapOut = Tex2DCombine(hasTransMapOut, transMapOut, TexUV, g_transColOut);
	float4 transColMapBack = Tex2DCombine(hasTransMapBack, transMapBack, TexUV, g_transColBack);
	//float4 Colourise		= lerp(g_transColBack, lerp(g_transColOut * g_transMultiplier,g_transColIn,DotLN),Translucence);
	float4 Colourise		= lerp(transColMapBack, lerp(transColMapOut * g_transMultiplier,transColMapIn,DotLN),Translucence);
	
	return (Colourise * Translucence);
}

//================Specular component 1 (front on)=======================
float4 SpecularFrontOn(	float3 Normals,
					float3 EyeVec,
					float3 LightVec,
					float Power,
					float Gloss)
{
	float3 SpecReflect = (2 * dot(Normals,LightVec) * Normals - LightVec);
	// when dot prod is less than 0, spec contrib is 0.
	// correct for low gloss with epsilon
	float4 Specular = pow(saturate(dot(SpecReflect, EyeVec)), max(0.001f, Gloss)) * Power;
	return Specular;
}


float FresnelMask(	float3 Normals,
				float3 EyeVec)
{
	float Fresnel = dot(EyeVec,Normals) * g_specFresnel;
	return saturate(Fresnel);
}
//================================Pixel shader - Complete================================

float3 SkinShader(float2 TexUV,  
	float3 WN, // normalize(world space normal)
	float3 EV, //= normalize(IN.WorldEyeDir);     //(world space)
	float3 LV, //= normalize(IN.LightVector.xyz); //(world space)
	uniform float3 LightColourDiff, uniform float3 LightColourSpec,
	float diffuseFactor)
{
	float4 DotLN		= dot(LV,WN);

	float4 Translucency	= TransPass(DotLN,TexUV);
    float4 a = Tex2DReplace(hasDiffTex, diffTex, TexUV, 1);
//	float4 a 			= tex2D(DiffMap,TexUV);
	float4 b 			= (Translucency);
//	float4 BaseLighting	= ((1 - a) * (a*b) + a * (1 - (1 - a) * (1 - b))) * b;
	float4 BaseLighting = (a * b)*(a + b + b - 2*a*b);

	float fresnelMask = FresnelMask(WN,EV);
	float FresnelStrength	= saturate(1 - fresnelMask) * g_fresnelPower + 1;
	float FresnelGlossiness	= fresnelMask * g_fresnelGloss + 1;
	float4 specTexSample = Tex2DReplace(hasSpecTex, specTex, TexUV, 1);
	float specPowerTexSample = Tex2DReplace(hasSpecPowerTex, specPowerTex, TexUV, 1).x;
	float4 Spec			= SpecularFrontOn(WN,EV,LV,
					g_specPower * specPowerTexSample * FresnelStrength,
					g_specGloss * FresnelGlossiness * max(0.1,specTexSample.a)) 
				* g_specColor * specTexSample * Translucency.x;
	
	float3 LightingOutput	= BaseLighting.rgb
					* LightColourDiff
					* diffuseFactor
					+ Spec.rgb * LightColourSpec.rgb;

	return LightingOutput;
} 

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN,
					uniform Texture2D DiffuseMap,
					uniform Texture2D NormalMap,
					uniform Texture2D SpecularMap,
		  			uniform Texture2D CubeMap,
					uniform LightInfo i_Light,
					uniform Texture2D SpecPowerMap,
					uniform Texture2D TransparencyMap, 
					uniform bool i_bDefaultPass,
					bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyTex, TransparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	IncidentLight light;
	IlluminatePointLight(IN.V.WorldPos, i_Light, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);

	//fetch bump normal
	float3 worldNormal = Get2BumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, hasMicroTex, microTex, g_microScale, vFace );

	OUT.col.rgb = SkinShader(IN.V.TexCoord0.xy, normalize(worldNormal),
		-worldEyeDir, lightDir, light.Cld, light.Cls, 1);
    
	if (i_bDefaultPass)
	{
		OUT.col.rgb += envmap_approximation(g_diffuseFactor).rgb;	
	}

	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput singleLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							   uniform Texture2D DiffuseMap,
								uniform Texture2D NormalMap,
								uniform Texture2D SpecularMap,
		  						uniform Texture2D CubeMap,
								uniform LightInfo i_Light,
								uniform Texture2D SpecPowerMap,
								uniform Texture2D TransparencyMap, 
								uniform bool i_bDefaultPass,
							   bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, DiffuseMap, NormalMap, SpecularMap, CubeMap, i_Light, SpecPowerMap, TransparencyMap, i_bDefaultPass, vFace );
}

pixelOutput singleLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
								  uniform Texture2D DiffuseMap,
								uniform Texture2D NormalMap,
								uniform Texture2D SpecularMap,
		  						uniform Texture2D CubeMap,
								uniform LightInfo i_Light,
								uniform Texture2D SpecPowerMap,
								uniform Texture2D TransparencyMap, 
								uniform bool i_bDefaultPass,
								  bool vFace : SV_ISFRONTFACE )
{
	return singleLightPS( IN, DiffuseMap, NormalMap, SpecularMap, CubeMap, i_Light, SpecPowerMap, TransparencyMap, i_bDefaultPass, vFace );
}

pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D DiffuseMap,
		uniform Texture2D NormalMap,
		uniform Texture2D SpecularMap,
		uniform Texture2D CubeMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D SpecPowerMap,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap,
		uniform Texture2D TransparencyMap,
		uniform int nBlockerSamples, uniform int nShadowSamples, bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyTex, TransparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

	IncidentLight light;
	IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, 
		nBlockerSamples, nShadowSamples, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
    
	//fetch bump normal
	float3 worldNormal = Get2BumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, hasMicroTex, microTex, g_microScale, vFace );

	OUT.col.rgb = SkinShader(IN.V.TexCoord0.xy, normalize(worldNormal),
		-worldEyeDir, lightDir, light.Cld, light.Cls, 1);

	OUT.col.rgb *= OUT.col.a;	//premul alpha

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
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform Texture2D TransparencyMap,
							uniform int nBlockerSamples, uniform int nShadowSamples, 
							 bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return projLightPS( IN, DiffuseMap, NormalMap, SpecularMap, CubeMap, i_Light, i_ProjLight, SpecPowerMap, ProjTextureMap, ProjShadowMap, TransparencyMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput projLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
								uniform Texture2D DiffuseMap,
								uniform Texture2D NormalMap,
								uniform Texture2D SpecularMap,
								uniform Texture2D CubeMap,
								uniform LightInfo i_Light,
								uniform ProjLightInfo i_ProjLight,
								uniform Texture2D SpecPowerMap,
								uniform Texture2D ProjTextureMap,
								uniform Texture2D ProjShadowMap,
								uniform Texture2D TransparencyMap,
								uniform int nBlockerSamples, uniform int nShadowSamples, 
								bool vFace : SV_ISFRONTFACE )
{
	return projLightPS( IN, DiffuseMap, NormalMap, SpecularMap, CubeMap, i_Light, i_ProjLight, SpecPowerMap, ProjTextureMap, ProjShadowMap, TransparencyMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D NormalMap,
		uniform Texture2D SpecularMap,
		uniform Texture2D SpecPowerMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap, bool vFace : SV_ISFRONTFACE
) {
    pixelOutput OUT; 
    OUT.col = float4(0,0,0,0);

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyTex, transparencyTex, IN.V.TexCoord0, g_transparency).r;
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
		float3 worldNormal = Get2BumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, hasMicroTex, microTex, g_microScale, vFace );

		// zero out the diffuse factors here.
		OUT.col.rgb = SkinShader(IN.V.TexCoord0.xy, normalize(worldNormal),
			-worldEyeDir, lightDir, float3(0,0,0), light.Cls, 1) * mask;
	}
    return OUT;
}

pixelOutput glowPS_Tess( TANGENT_VERTEX_OUTPUT IN,
						uniform Texture2D NormalMap,
						uniform Texture2D SpecularMap,
						uniform Texture2D SpecPowerMap,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap, 
						bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return glowPS( IN, NormalMap, SpecularMap, SpecPowerMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput glowPS_Default( TANGENT_VERTEX_OUTPUT IN,
						   uniform Texture2D NormalMap,
							uniform Texture2D SpecularMap,
							uniform Texture2D SpecPowerMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap, 
						   bool vFace : SV_ISFRONTFACE )
{
	return glowPS( IN, NormalMap, SpecularMap, SpecPowerMap, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, vFace );
}


pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyTex, transparencyTex, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;
    
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);

	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );

	float4 refl = 0;
	if (hasCubeMap && g_bCubeMapEnabled)
	{    		
		refl = SampleEnvironmentLOD((-worldEyeDir), worldNormal, cubeTex, g_reflMapAngle*PI_DIV_180, reflBlur);
		refl *= g_reflectivity;
		refl *= fastFresnel(dot(-worldEyeDir, worldNormal), fresnelBias, fresnelPower);
		refl = Tex2DCombine(hasReflectFactorMap, reflectFactorTex, IN.V.TexCoord0, refl);
	}

	float4 spec = g_reflectivity*g_envSpecularColor*g_specularFactor;
	if (g_bHasSpecularEnvMap) 
	{
		spec *= SampleEnvironment((-worldEyeDir), worldNormal, 
				specularEnvMap, g_specularEnvAngle);
	}
	
	float4 transColMapIn = 	Tex2DCombine(hasTransMapIn, transMapIn, IN.V.TexCoord0.xy, g_transColIn);
	float3 diff = Tex2DCombine(hasDiffTex, diffTex, IN.V.TexCoord0, transColMapIn*g_envDiffuseColor*g_diffuseFactor).rgb;
	if (g_bHasDiffuseEnvMap)
	{
		diff *= SampleEnvDiffuse(worldNormal, diffuseEnvMap, g_diffuseEnvAngle).rgb;
	}
		
	OUT.col.rgb = g_IsolateReflection ? refl.rgb : spec.rgb + refl.rgb + diff.rgb;
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


//================================TECHNIQUES================================

//////////////////////////////////////////////////////////////////////////////
// Entry points generated from the technique/pass compile statements.
// Each one binds the uniform arguments the .fx passed in its compile call.
//////////////////////////////////////////////////////////////////////////////

pixelOutput Default_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Default(IN, diffTex, normalMap, specTex, cubeTex, g_lightInfo, microTex, transparencyTex, true, vFace);
}

pixelOutput Default_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Tess(IN, diffTex, normalMap, specTex, cubeTex, g_lightInfo, microTex, transparencyTex, true, vFace);
}

pixelOutput SingleLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Default(IN, diffTex, normalMap, specTex, cubeTex, g_lightInfo, microTex, transparencyTex, false, vFace);
}

pixelOutput SingleLight_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Tess(IN, diffTex, normalMap, specTex, cubeTex, g_lightInfo, microTex, transparencyTex, false, vFace);
}

pixelOutput ProjectedLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffTex, normalMap, specTex, cubeTex, g_lightInfo, g_projLight, microTex, projLightMap, projShadowMap, transparencyTex, 5, 5, vFace);
}

pixelOutput ProjectedLight_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffTex, normalMap, specTex, cubeTex, g_lightInfo, g_projLight, microTex, projLightMap, projShadowMap, transparencyTex, 5, 5, vFace);
}

pixelOutput ProjectedLightSuperSample_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffTex, normalMap, specTex, cubeTex, g_lightInfo, g_projLight, microTex, projLightMap, projShadowMap, transparencyTex, 7, 7, vFace);
}

pixelOutput ProjectedLightSuperSample_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffTex, normalMap, specTex, cubeTex, g_lightInfo, g_projLight, microTex, projLightMap, projShadowMap, transparencyTex, 7, 7, vFace);
}

pixelOutput ProjectedLightSuperSample2_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffTex, normalMap, specTex, cubeTex, g_lightInfo, g_projLight, microTex, projLightMap, projShadowMap, transparencyTex, 9, 9, vFace);
}

pixelOutput ProjectedLightSuperSample2_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffTex, normalMap, specTex, cubeTex, g_lightInfo, g_projLight, microTex, projLightMap, projShadowMap, transparencyTex, 9, 9, vFace);
}

pixelOutput ProjectedLightSuperSample3_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffTex, normalMap, specTex, cubeTex, g_lightInfo, g_projLight, microTex, projLightMap, projShadowMap, transparencyTex, 15, 15, vFace);
}

pixelOutput ProjectedLightSuperSample3_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffTex, normalMap, specTex, cubeTex, g_lightInfo, g_projLight, microTex, projLightMap, projShadowMap, transparencyTex, 15, 15, vFace);
}

pixelOutput Glow_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return glowPS_Default(IN, normalMap, specTex, microTex, g_lightInfo, g_projLight, projLightMap, projShadowMap, vFace);
}

pixelOutput Glow_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return glowPS_Tess(IN, normalMap, specTex, microTex, g_lightInfo, g_projLight, projLightMap, projShadowMap, vFace);
}

pixelOutput DOFPrep_PDefault_PS(TANGENT_VERTEX_OUTPUT IN)
{
    return DOFPrepTrans_PS(IN, hasTransparencyTex, transparencyTex, g_transparency);
}
