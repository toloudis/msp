//////////////////////////////////////////////////////////////////////////////
// Converted from PhongReflection.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// PhongReflection.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  PhongReflection.fx
**
**      Reflective / refractive object surface 
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
// The SasGlobal effect description (gp : SasGlobal) and the SAS UI
// annotations of the material parameters are in PhongReflection.effect.json.
// Register layout shared by all materials: see Globals.hlsli.

/*********** support data and functions ******/

#include "Support.hlsli"
#include "Tessellate.hlsli"
#include "Lighting.hlsli"

// b4: this material's own parameters (defaults are in PhongReflection.effect.json)
cbuffer MaterialParams : register(b4)
{
	// semantics will cause automatic binding to code-generated reflection map
	bool g_isPlanar;				// : ReflectionMapIsPlanar, default false
	bool g_hasCubeMap;				// : HasReflectionMap, default false
	float4 g_diffuse;				// : MaterialDiffuse, default (0.5,0.5,0.5,1)
	bool hasDiffuseMap;				// default false
	float4 g_emissive;				// : MaterialEmissive, default (0,0,0,1)
	bool hasEmissiveMap;			// default false
	float g_emissiveIntensity;		// default 1
	float4 g_specular;				// : MaterialSpecular, default (1,1,1,1)
	bool hasSpecularMap;			// default false
	float g_specularPower;			// default 1
	float g_shininess;				// : MaterialPower, default 20
	float g_transparency;			// : Opacity, default 1
	bool hasTransparencyMap;		// default false
	float g_reflectivity;			// : Reflectivity, default 1
	float fresnelPower;				// default 4
	float fresnelBias;				// default 0.2
	// mipmap LOD value for cubemap (refl and refr separate?)
	float reflLOD;					// default 0
	float reflFactor;				// default 1
	bool hasReflectFactorMap;		// default false
	// IOR ratio (n1)/(n2) for refraction
	float IOR;						// default 1
	float refrFactor;				// default 0
	float refrLOD;					// default 0
	bool hasSmearMap;				// default false
	float smearMapScale;			// default 0
};

Texture2D diffuseMap : register(t4);			// : DiffuseTexture
Texture2D emissiveMap : register(t5);			// : EmissiveTexture
Texture2D specularMap : register(t6);
Texture2D transparencyMap : register(t7);		// : OpacityTexture
Texture2D reflectFactorMap : register(t8);
Texture2D reflectSmearMap : register(t9);
Texture2D glowMask : register(t10);				// : GlowMask

// renderer-generated reflection maps (bound by semantic), in the t20+ range
// Globals.hlsli reserves for reflection maps
TextureCube g_cubeMap : register(t20);			// : CubeReflectionMap
Texture2D g_planarMap : register(t21);			// : PlanarReflectionMap

SamplerState g_cubeSampler : register(s10);
SamplerState planarSampler : register(s11);
SamplerState AnisoWrapSampler : register(s12);

/*********** support functions ******/

void myRefract(
	in float3 incom, 
	in float3 normal, 
	in float index_external, 
	in float index_internal, 
	out float3 o_reflection, 
	out float3 o_refraction, 
	out float o_reflectance, 
	out float o_transmittance
	) 
{
	float eta = index_external/index_internal; 
	
	o_reflection = reflect(incom, normal);
	o_refraction = refract(incom, normal, eta);
	
	o_reflectance = fastFresnel(dot(-incom, normal), fresnelBias, fresnelPower);
	o_transmittance = 1 - o_reflectance;
/*	
	// theta1 is angle btw normal and incident ray
	float cos_theta1 = dot(incom, normal); 
	// theta2 is angle btw refracted ray and -normal
	float cos_theta2 = sqrt(1.0 - ((eta * eta) * ( 1.0 - (cos_theta1 * cos_theta1)))); 
	o_reflection = incom - 2.0 * cos_theta1 * normal; 
	o_refraction = (eta * incom) + (cos_theta2 - eta * cos_theta1) * normal;
		
	float fresnel_rs = (index_external * cos_theta1 - index_internal * cos_theta2 ) / 
		(index_external * cos_theta1 + index_internal * cos_theta2); 
	float fresnel_rp = (index_external * cos_theta2 - index_internal * cos_theta1 ) / 
		(index_external * cos_theta2 + index_internal * cos_theta1); 
	o_reflectance = ((fresnel_rs * fresnel_rs) + (fresnel_rp * fresnel_rp)) * 0.5; 
	o_transmittance = (1.0 - o_reflectance); 
*/
}


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
    OUT.HPosition = TransformVertex(Po, Vtx.UV, g_wvp);
    float4 Ph = OUT.HPosition;
	OUT.ScreenPos.x = 0.5 * (Ph.w + Ph.x);
	OUT.ScreenPos.y = 0.5 * (Ph.w + Ph.y);
	OUT.ScreenPos.z = Ph.w;

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

float4 GetReflection(float3 bumpNormal, float3 worldNormal, float3 ScreenPos, float3 WorldEyeDir, float2 TexCoord0, float bumpScale)
{
	float4 refl = float4(0,0,0,0);
	float4 refr = float4(0,0,0,0);
	float4 rr	= float4(0,0,0,0);
	float4 reflColorFactor=reflFactor,refrColorFactor=refrFactor;
	float fresnelRefl, fresnelRefr;

	float4 factor = Tex2DReplace(hasReflectFactorMap, reflectFactorMap, TexCoord0, 1);
	reflColorFactor *= factor;
	refrColorFactor *= factor;

	if (g_isPlanar)
	{
		float3 tc = float3(ScreenPos.x/ScreenPos.z, ScreenPos.y/ScreenPos.z, 1);		
		tc.y = 1.0 - tc.y;
		tc.xy += (bumpNormal.xy * (bumpScale  * 0.1));

		fresnelRefl = fastFresnel(dot(WorldEyeDir, worldNormal), fresnelBias, fresnelPower);
		fresnelRefr = 1 - fresnelRefl;
	
		refl = g_planarMap.SampleBias( planarSampler, tc.xy, reflLOD) *  fresnelRefl * reflColorFactor;
		//refr = tex2Dbias(planarMap, float4(tc.xyz, refrLOD)) *  fresnelRefr * refrColorFactor;
	}
	else
	{
		float3 reflVect;
		float3 refrVect;
		myRefract(
			-WorldEyeDir, 
			worldNormal, 
			1.0, 
			IOR, 
			reflVect, 
			refrVect, 
			fresnelRefl, 
			fresnelRefr
		);
		refl = g_cubeMap.SampleBias( g_cubeSampler, reflVect, reflLOD) *  fresnelRefl * reflColorFactor;
		refr = g_cubeMap.SampleBias( g_cubeSampler, refrVect, refrLOD) *  fresnelRefr * refrColorFactor;
	}
	rr = refl + refr;
	return rr;
}

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN,
					uniform Texture2D DiffuseMap,
					uniform Texture2D NormalMap,
					uniform Texture2D SpecularMap,
					uniform LightInfo i_Light,
					uniform float SpecPowerScale,
		  			uniform float reflectivity, 
		  			uniform bool i_bDefaultPass,
		  			bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

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

	float3 diffuse = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor);
	float3 specular = PhongSpecular(worldNormal, lightDir, -worldEyeDir, 
		light.Cls, sSpecColor, sSpecPower) * g_specularPower;

    OUT.col.rgb = diffuse + specular;
  
	if (i_bDefaultPass)
	{
		float3 emissive = Tex2DCombine(hasEmissiveMap, emissiveMap, IN.V.TexCoord0, g_emissive * g_emissiveIntensity).rgb;
		OUT.col.rgb += emissive + envmap_approximation(g_diffuseFactor).rgb;	
	}

	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput singleLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							   uniform Texture2D DiffuseMap,
								uniform Texture2D NormalMap,
								uniform Texture2D SpecularMap,
								uniform LightInfo i_Light,
								uniform float SpecPowerScale,
		  						uniform float reflectivity, 
					  			uniform bool i_bDefaultPass,
							   bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, DiffuseMap, NormalMap, SpecularMap, i_Light, SpecPowerScale, reflectivity, i_bDefaultPass, vFace );
}

pixelOutput singleLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
								  uniform Texture2D DiffuseMap,
								uniform Texture2D NormalMap,
								uniform Texture2D SpecularMap,
								uniform LightInfo i_Light,
								uniform float SpecPowerScale,
		  						uniform float reflectivity, 
					  			uniform bool i_bDefaultPass,
								  bool vFace : SV_ISFRONTFACE )
{
	return singleLightPS( IN, DiffuseMap, NormalMap, SpecularMap, i_Light, SpecPowerScale, reflectivity, i_bDefaultPass, vFace );
}

pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D DiffuseMap,
		uniform Texture2D NormalMap,
		uniform Texture2D SpecularMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform float SpecPowerScale,
		uniform float reflectivity,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap,
		uniform int nBlockerSamples, uniform int nShadowSamples, bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;

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

	float3 diffuse = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor);
	float3 specular = PhongSpecular(worldNormal, lightDir, -worldEyeDir, 
		light.Cls, sSpecColor, sSpecPower) * g_specularPower;

    OUT.col.rgb = diffuse + specular;
    
	OUT.col.rgb *= OUT.col.a;	//premul alpha

    return OUT;
}

pixelOutput projLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							 uniform Texture2D DiffuseMap,
							uniform Texture2D NormalMap,
							uniform Texture2D SpecularMap,
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
	return projLightPS( IN, DiffuseMap, NormalMap, SpecularMap, i_Light, i_ProjLight, SpecPowerScale, reflectivity, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput projLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
								uniform Texture2D DiffuseMap,
								uniform Texture2D NormalMap,
								uniform Texture2D SpecularMap,
								uniform LightInfo i_Light,
								uniform ProjLightInfo i_ProjLight,
								uniform float SpecPowerScale,
								uniform float reflectivity,
								uniform Texture2D ProjTextureMap,
								uniform Texture2D ProjShadowMap,
								uniform int nBlockerSamples, uniform int nShadowSamples, 
								bool vFace : SV_ISFRONTFACE )
{
	return projLightPS( IN, DiffuseMap, NormalMap, SpecularMap, i_Light, i_ProjLight, SpecPowerScale, reflectivity, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
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

		//fetch base and specular colors
		float3 sSpecColor = Tex2DCombine(hasSpecularMap, SpecularMap, IN.V.TexCoord0, g_specular).rgb;
		float sSpecPower = SpecPowerScale;
	        
		//fetch bump normal
		float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );

		OUT.col.rgb = PhongSpecular(worldNormal, lightDir, -worldEyeDir, 
			light.Cls, sSpecColor, sSpecPower) * g_specularPower * mask;
	}
    return OUT;
}

pixelOutput glowPS_Tess( TANGENT_VERTEX_OUTPUT IN,
						uniform Texture2D NormalMap,
						uniform Texture2D SpecularMap,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform float SpecPowerScale,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap, 
						bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return glowPS( IN, NormalMap, SpecularMap, i_Light, i_ProjLight, SpecPowerScale, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput glowPS_Default( TANGENT_VERTEX_OUTPUT IN,
						   uniform Texture2D NormalMap,
						uniform Texture2D SpecularMap,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform float SpecPowerScale,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap, 
						   bool vFace : SV_ISFRONTFACE )
{
	return glowPS( IN, NormalMap, SpecularMap, i_Light, i_ProjLight, SpecPowerScale, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 

	//early alpha test
	OUT.col.a = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.V.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= OUT.col.a ) discard;
    
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);

	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );
	
	float bumpScale = 1.0f;
    if (hasNormalMap)
		bumpScale = g_bumpMapScale;

	float4 rr = 0;
	if (g_hasCubeMap && g_bCubeMapEnabled)
	{
		float3 bumpNormal = Tex2DNormal(hasNormalMap, normalMap, IN.V.TexCoord0) * float3( g_bumpMapScale, g_bumpMapScale, 1 );
		if (hasSmearMap)
		{
			bumpNormal = Tex2DNormal(hasSmearMap, reflectSmearMap, IN.V.TexCoord0);// * float3( smearMapScale, smearMapScale, 1 );
			worldNormal = GetBumpNormal(IN.V, hasSmearMap, reflectSmearMap, smearMapScale, vFace);
			bumpScale = smearMapScale;
		}
		
		rr = GetReflection(bumpNormal, worldNormal, IN.ScreenPos, -worldEyeDir, IN.V.TexCoord0, bumpScale);
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
	OUT.col.rgb = g_IsolateReflection ? rr.rgb : emissive + spec.rgb + rr.rgb + diff.rgb;
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
    return singleLightPS_Default(IN, diffuseMap, normalMap, specularMap, g_lightInfo, g_shininess, g_reflectivity, true, vFace);
}

pixelOutput Default_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Tess(IN, diffuseMap, normalMap, specularMap, g_lightInfo, g_shininess, g_reflectivity, true, vFace);
}

pixelOutput SingleLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Default(IN, diffuseMap, normalMap, specularMap, g_lightInfo, g_shininess, g_reflectivity, false, vFace);
}

pixelOutput SingleLight_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Tess(IN, diffuseMap, normalMap, specularMap, g_lightInfo, g_shininess, g_reflectivity, false, vFace);
}

pixelOutput ProjectedLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffuseMap, normalMap, specularMap, g_lightInfo, g_projLight, g_shininess, g_reflectivity, projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, vFace);
}

pixelOutput ProjectedLight_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffuseMap, normalMap, specularMap, g_lightInfo, g_projLight, g_shininess, g_reflectivity, projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, vFace);
}

pixelOutput ProjectedLightSuperSample_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffuseMap, normalMap, specularMap, g_lightInfo, g_projLight, g_shininess, g_reflectivity, projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED, vFace);
}

pixelOutput ProjectedLightSuperSample_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffuseMap, normalMap, specularMap, g_lightInfo, g_projLight, g_shininess, g_reflectivity, projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED, vFace);
}

pixelOutput ProjectedLightSuperSample2_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffuseMap, normalMap, specularMap, g_lightInfo, g_projLight, g_shininess, g_reflectivity, projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample2_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffuseMap, normalMap, specularMap, g_lightInfo, g_projLight, g_shininess, g_reflectivity, projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample3_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Default(IN, diffuseMap, normalMap, specularMap, g_lightInfo, g_projLight, g_shininess, g_reflectivity, projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample3_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, diffuseMap, normalMap, specularMap, g_lightInfo, g_projLight, g_shininess, g_reflectivity, projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH, vFace);
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
    return DOFPrepTrans_PS(IN, hasTransparencyMap, transparencyMap, g_transparency);
}
