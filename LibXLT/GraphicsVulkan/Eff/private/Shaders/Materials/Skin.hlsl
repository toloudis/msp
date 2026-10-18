//////////////////////////////////////////////////////////////////////////////
// Converted from Skin.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// Skin.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Skin.fx
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
// annotations of the material parameters are in Skin.effect.json.
// Register layout shared by all materials: see Globals.hlsli.

#include "Support.hlsli"
#include "Tessellate.hlsli"
#include "Skinning.hlsli"
#include "Lighting.hlsli"


// b4: this material's own parameters (defaults are in Skin.effect.json)
cbuffer MaterialParams : register(b4)
{
	bool hasDiffTex;	// default false
	float4 g_transColIn;	// : MaterialDiffuse, default (0.87,0.91,0.96,1)
	float4 g_transColOut;	// default (1,0.71,0.32,1)
	float4 g_transColBack;	// default (0.58,0.2,0.24,1)
	float g_transMultiplier;	// default 1
	bool hasTransTex;	// default false
	float g_transRampOff;	// default 1.5
	float4 g_specColor;	// default (0.45,0.65,1,1)
	float g_specPower;	// default 0.2
	float g_specGloss;	// default 15
	float g_specFresnel;	// default 3
	float g_fresnelPower;	// default 15
	float g_fresnelGloss;	// default 0
	bool hasSpecTex;	// default false
	bool hasSpecPowerTex;	// default false
	bool hasNormalTex;	// default false
	bool hasMicroTex;	// default false
	float g_microScale;	// default 50
	float g_transparency;	// : Opacity, default 1
	bool hasTransparencyTex;	// default false
	float g_reflectivity;	// : Reflectivity, default 1
	float fresnelPower;	// default 4
	float fresnelBias;	// default 0.2
	float reflBlur;	// default 0
	float g_reflMapAngle;	// default 0
	bool hasCubeMap;	// default false
	bool hasReflectFactorMap;	// default false
	// Skin.fx used g_viewIT without declaring it (no include declares it
	// either); declared here as in Default.fx.
	float4x4 g_viewIT;	// : ViewIT
};

Texture2D diffTex : register(t4);			// : DiffuseTexture
Texture2D transTex : register(t5);
Texture2D specTex : register(t6);
Texture2D specPowerTex : register(t7);
Texture2D normalTex : register(t8);
Texture2D microTex : register(t9);
Texture2D transparencyTex : register(t10);			// : OpacityTexture
Texture2D cubeTex : register(t11);
Texture2D reflectFactorTex : register(t12);
Texture2D glowMask : register(t13);			// : GlowMask

// g_bumpMapScale (: BumpMapScale, default 1 for this material) is the shared
// member of MaterialCommon (b3); projLightMap / projShadowMap and the
// projSampler / shadowMapSampler states are the shared ones in Lighting.hlsli,
// diffuseEnvMap / specularEnvMap the shared ones in Support.hlsli.
// The legacy sampler2D/sampler_state objects are gone: the helpers in
// Support.hlsli (Tex2DCombine, Tex2DReplace, ...) take the Texture2D and
// sample it with g_DefaultSampler, as in every converted material. The two
// samplers this file samples with directly are its own:
SamplerState MicroSampler : register(s10);		// linear, wrap
SamplerState glowSampler : register(s11);		// linear, wrap


//============================Input Structures============================
//application data passed to vertex shader
struct appdata 
{
    float3 Position	: POSITION;
    float3 Normal	: NORMAL;
    float2 UV		: TEXCOORD0;
    float3 T		: TANGENT;
    float3 B		: BINORMAL;
};

// Skin.fx called Tessellate(VS_INPUT_TESS, out appdata) and Skin(VS_INPUT_SKINNING,
// out appdata), which no include declares (Tessellate.h has no Tessellate()
// any more, and Skin() writes a STANDARD_VERTEX), so the .fx no longer built.
// These two material-local helpers restore what the calls meant: the linear
// (flat) superprim vertex of Tessellate.hlsli, and the skinned vertex (named
// SkinToAppdata rather than overloading Skin(), so fxc cannot find the two
// structurally identical vertex structs ambiguous).
void Tessellate(in VS_INPUT_TESS In, out appdata Out)
{
	Out.Position = GetLinearTriangleVertex( In ).xyz;
	Out.Normal = GetLinearTriangleNormal( In );
	Out.UV = GetLinearTriangleTexture( In );
	Out.T = GetLinearTriangleTangent( In );
	Out.B = GetLinearTriangleBitangent( In );
}
void SkinToAppdata(in VS_INPUT_SKINNING In, out appdata Out)
{
	STANDARD_VERTEX V;
	Skin( In, V );
	Out.Position = V.Position;
	Out.Normal = V.Normal;
	Out.UV = V.UV;
	Out.T = V.T;
	Out.B = V.B;
}

/* data passed from vertex shader to pixel shader */

struct vertexOutput {
    float4 HPosition	: POSITION;
    float4 TexCoord0	: TEXCOORD0;
    float2 UV			: TEXCOORD1;
    float4 diffCol	: COLOR0;
    float4 specCol	: COLOR1;
	float3 WorldEyeDir	: TEXCOORD2;
	float3 WorldTanMatrixX : TEXCOORD3;
	float3 WorldTanMatrixY : TEXCOORD4;
	float3 WorldTanMatrixZ : TEXCOORD5;
};

/*********** support functions ******/


/*********** vertex shader ******/
DOFvertexOutput DOFPrep_VS(appdata IN,
    uniform float4x4 WorldViewProj,
    uniform float4x4 WorldView)
{
    DOFvertexOutput OUT;
    // output position in proj space
	float4 Po = float4(IN.Position, 1.0f);
    OUT.HPosition = mul(WorldViewProj, Po);
    OUT.ViewSpacePos = mul(WorldView, Po);
    return OUT;
}
DOFvertexOutput DOFPrep_VS_Tess(VS_INPUT_TESS IN)
{
	appdata Vtx;
	Tessellate(IN, Vtx);
	return DOFPrep_VS(Vtx,g_wvp,g_wv);
}
DOFvertexOutput DOFPrep_VS_Skin(VS_INPUT_SKINNING IN)
{
	appdata Vtx;
	SkinToAppdata(IN, Vtx);
	return DOFPrep_VS(Vtx,g_wvp,g_wv);
}
DOFvertexOutput DOFPrep_VS_Default(appdata Vtx)
{
	return DOFPrep_VS(Vtx,g_wvp,g_wv);
}

TANGENT_VERTEX_OUTPUT singleLightVS(appdata IN,
    uniform float4x4 WorldViewProj,
    uniform float4x4 WorldIT,
    uniform float4x4 World,
    uniform float4x4 ViewIT,
    uniform float BumpMapScale,
    uniform float GlowSize
) {
    TANGENT_VERTEX_OUTPUT OUT;
    
	// decal and bump texture coords
    OUT.V.TexCoord0 = mul(g_uvTransform, float4(IN.UV,0,1)).xy;
    OUT.V.UV = IN.UV;
    
	float3 NewPos = IN.Position;

    // output position in proj space
    float4 Po = float4(NewPos + IN.Normal*GlowSize, 1.0);
    OUT.HPosition = TransformVertex(Po, IN.UV, WorldViewProj);
    
    // transform position to world space and get vector to light
    float3 Pw = mul(World, Po).xyz;
    OUT.V.WorldPos = Pw;

//	getTangentToWorldSpace(World,IN.T,IN.B,IN.Normal,BumpMapScale, 
//		OUT.WorldTanMatrixX,OUT.WorldTanMatrixY,OUT.WorldTanMatrixZ);
	OUT.V.WorldTan = TransformTangents( World, IN.T, IN.B, IN.Normal );

	OUT.ScreenPos = float3(0,0,0);//not used
	
    return OUT;
}
TANGENT_VERTEX_OUTPUT singleLightVS_Tess(VS_INPUT_TESS IN) 
{
	appdata Vtx;
	Tessellate(IN, Vtx);
	return singleLightVS( Vtx, g_wvp,g_worldIT,
					g_world, g_viewIT, g_bumpMapScale, g_glowSize );
}
TANGENT_VERTEX_OUTPUT singleLightVS_Skin(VS_INPUT_SKINNING IN) 
{
	appdata Vtx;
	SkinToAppdata(IN, Vtx);
	return singleLightVS( Vtx, g_wvp,g_worldIT,
					g_world, g_viewIT, g_bumpMapScale, g_glowSize );
}
TANGENT_VERTEX_OUTPUT singleLightVS_Default(appdata Vtx) 
{
	return singleLightVS( Vtx, g_wvp,g_worldIT,
					g_world, g_viewIT, g_bumpMapScale, g_glowSize );
}

//==============================Vertex shader================================
/*
// pass tangent-to-world matrix down to pix shader to convert 
// normals and do lighting into world space.
TANGENT_VERTEX_OUTPUT VertexShader(appdata IN, uniform float4 LightPosition) 
{
	TANGENT_VERTEX_OUTPUT OUT;

	OUT.WorldNormal 	= mul(IN.Normal, g_worldIT).xyz;
	OUT.WorldTangent 	= mul(IN.T, g_worldIT).xyz;
	OUT.WorldBinormal 	= mul(IN.B, g_worldIT).xyz;
	 
	float3 WorldSpacePos 	= mul(IN.Position, g_world);
	OUT.LightVec 		= LightPosition - WorldSpacePos;
	OUT.TexCoord.xy 	= IN.UV;
	OUT.EyeVec 		= g_eyePos.xyz - WorldSpacePos;
	OUT.Position 		= mul(IN.Position, g_wvp);
	return OUT;
	
}
*/

//=======================Translucency lighting model=======================
float4 TransPass(	float4 DotLN,
			float2 TexUV)
{
	float4 transSample = Tex2DReplace(hasTransTex, transTex, float4(TexUV,0,0), 0);
	
	static const float4 one = float4(1,1,1,1);	
	float4 Translucence 	= smoothstep(-g_transRampOff  * transSample,one,DotLN) 
					;//- smoothstep(one,one,DotLN); // or should it be step(one, DotLN);//???
	float4 Colourise		= lerp(g_transColBack, lerp(g_transColOut * g_transMultiplier,g_transColIn,DotLN),Translucence);
	
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
float4 SkinShader(float2 TexUV,  
	float3 WN, // normalize(world space normal)
	float3 EV, //= normalize(IN.WorldEyeDir);     //(world space)
	float3 LV, //= normalize(IN.LightVector.xyz); //(world space)
	uniform float3 LightColourDiff, uniform float3 LightColourSpec,
	float diffuseFactor)
{
	float4 DotLN		= dot(LV,WN);

	float4 Translucency	= TransPass(DotLN,TexUV);
    float4 a = Tex2DReplace(hasDiffTex, diffTex, float4(TexUV,0,0), 1);
//	float4 a 			= tex2D(diffTex,TexUV);
	float4 b 			= (Translucency);
//	float4 BaseLighting	= ((1 - a) * (a*b) + a * (1 - (1 - a) * (1 - b))) * b;
	float4 BaseLighting = (a * b)*(a + b + b - 2*a*b);

	float fresnelMask = FresnelMask(WN,EV);
	float FresnelStrength	= saturate(1 - fresnelMask) * g_fresnelPower + 1;
	float FresnelGlossiness	= fresnelMask * g_fresnelGloss + 1;
	float4 specTexSample = Tex2DReplace(hasSpecTex, specTex, float4(TexUV,0,0), float4(1,1,1,1));
	float specPowerTexSample = Tex2DReplace(hasSpecPowerTex, specPowerTex, float4(TexUV,0,0), 1).x;
	float4 Spec			= SpecularFrontOn(WN,EV,LV,
					g_specPower * specPowerTexSample * FresnelStrength,
					g_specGloss * FresnelGlossiness * max(0.1,specTexSample.a)) 
				* g_specColor * specTexSample * Translucency.x;
	
	float3 LightingOutput	= BaseLighting.rgb
					* LightColourDiff
					* diffuseFactor
					+ Spec.rgb * LightColourSpec.rgb;

	//alpha is handled by calling function					
	return float4(LightingOutput, 1);
} 

pixelOutput bumpReflectPS(vertexOutput IN,
		  uniform Texture2D DiffuseMap,
		  uniform Texture2D NormalMap,
		  uniform Texture2D SpecularMap,
		  uniform Texture2D CubeMap,
		  uniform Texture2D TransparencyMap, bool vFace : SV_ISFRONTFACE) 
{
	pixelOutput OUT; 

	float4 refl = float4(0,0,0,0);
	if (hasCubeMap)
	{
		//fetch bump normal
		float3 bumpNormal = Tex2DNormal(hasNormalTex, NormalMap, IN.TexCoord0);
		if (hasMicroTex)
			bumpNormal += expand(microTex.Sample(MicroSampler,IN.TexCoord0*g_microScale));
	    	
		float3 worldNormal;
		worldNormal.x = dot(bumpNormal, IN.WorldTanMatrixX);
		worldNormal.y = dot(bumpNormal, IN.WorldTanMatrixY);
		worldNormal.z = dot(bumpNormal, IN.WorldTanMatrixZ);
		if (g_bDoubleSided && vFace > 0)
			worldNormal = -worldNormal;
		refl = SampleEnvironment((IN.WorldEyeDir), worldNormal,CubeMap);
		refl = Tex2DCombine(hasReflectFactorMap, reflectFactorTex, IN.TexCoord0, refl);
		refl *= g_reflectivity;
	}

    float4 col = Tex2DCombine(hasDiffTex, diffTex, IN.TexCoord0, IN.diffCol);
    float4 spec = Tex2DCombine(hasSpecTex, specTex, IN.TexCoord0, IN.specCol);
		
    OUT.col.rgb = col.rgb + refl.rgb + spec;
    
	OUT.col.a = Tex2DCombine(hasTransparencyTex, TransparencyMap, IN.TexCoord0, g_transparency);

#ifdef PRE_MULT_ALPHA
	OUT.col.rgb *= OUT.col.a;
#endif

    return OUT;
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
    
	IncidentLight light;
	IlluminatePointLight(IN.V.WorldPos, i_Light, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);

	//fetch bump normal
	float3 worldNormal = Get2BumpNormal( IN.V, hasNormalTex, NormalMap, g_bumpMapScale, hasMicroTex, microTex, g_microScale, vFace );

	OUT.col = SkinShader(IN.V.TexCoord0.xy, normalize(worldNormal),
		-worldEyeDir, lightDir, light.Cld, light.Cls, 1);
    
	if (i_bDefaultPass)
	{
		OUT.col.rgb += envmap_approximation(g_diffuseFactor).rgb;	
	}
    
	OUT.col.a = Tex2DCombine(hasTransparencyTex, TransparencyMap, IN.V.TexCoord0, g_transparency);

#ifdef PRE_MULT_ALPHA
	OUT.col.rgb *= OUT.col.a;
#endif

    return OUT;
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
    
	IncidentLight light;
	IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, 
		nBlockerSamples, nShadowSamples, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
    
	//fetch bump normal
	float3 worldNormal = Get2BumpNormal( IN.V, hasNormalTex, NormalMap, g_bumpMapScale, hasMicroTex, microTex, g_microScale, vFace );

	OUT.col = SkinShader(IN.V.TexCoord0.xy, normalize(worldNormal),
		-worldEyeDir, lightDir, light.Cld, light.Cls, 1);

	OUT.col.a = Tex2DCombine(hasTransparencyTex, TransparencyMap, IN.V.TexCoord0, g_transparency);

#ifdef PRE_MULT_ALPHA
	OUT.col.rgb *= OUT.col.a;
#endif

    return OUT;
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
    
    float4 mask = g_bHasMask ? glowMask.Sample(glowSampler, IN.V.TexCoord0) : float4(1,1,1,1);
    if (g_bConstGlow)
    {
		OUT.col = mask;
		OUT.col.a = OUT.col.r;
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
		float3 worldNormal = Get2BumpNormal( IN.V, hasNormalTex, NormalMap, g_bumpMapScale, hasMicroTex, microTex, g_microScale, vFace );

		// zero out the diffuse factors here.
		OUT.col = SkinShader(IN.V.TexCoord0.xy, normalize(worldNormal),
			-worldEyeDir, lightDir, float3(0,0,0), light.Cls, 1);
		OUT.col.a = 1;
		OUT.col *= mask;
	}
	OUT.col.a = 1;
    return OUT;
}

pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 
    
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);

	float3 worldNormal = GetBumpNormal( IN.V, hasNormalTex, normalTex, g_bumpMapScale, vFace );

	float4 refl = 0;
	if (hasCubeMap)
	{    		
		refl = SampleEnvironmentLOD((-worldEyeDir), worldNormal, cubeTex , g_reflMapAngle*PI_DIV_180, reflBlur);
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
		
	float4 diff = Tex2DCombine(hasDiffTex, diffTex, IN.V.TexCoord0, g_transColIn*g_envDiffuseColor*g_diffuseFactor);
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvMap, g_diffuseEnvAngle).rgb,1);
	}
		
	OUT.col = float4(
		spec.rgb + refl.rgb + diff.rgb,
		g_transparency);		

	OUT.col.a = Tex2DCombine(hasTransparencyTex, transparencyTex, IN.V.TexCoord0, g_transparency);

#ifdef PRE_MULT_ALPHA
	OUT.col.rgb *= OUT.col.a;
#endif

    return OUT;
}


//================================TECHNIQUES================================

//////////////////////////////////////////////////////////////////////////////
// Entry points generated from the technique/pass compile statements.
// Each one binds the uniform arguments the .fx passed in its compile call.
//////////////////////////////////////////////////////////////////////////////

pixelOutput Default_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS(IN, diffTex, normalTex, specTex, cubeTex, g_lightInfo, microTex, transparencyTex, true, vFace);
}

pixelOutput SingleLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS(IN, diffTex, normalTex, specTex, cubeTex, g_lightInfo, microTex, transparencyTex, false, vFace);
}

pixelOutput ProjectedLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS(IN, diffTex, normalTex, specTex, cubeTex, g_lightInfo, g_projLight, microTex, projLightMap, projShadowMap, transparencyTex, 5, 5, vFace);
}

pixelOutput ProjectedLightSuperSample_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS(IN, diffTex, normalTex, specTex, cubeTex, g_lightInfo, g_projLight, microTex, projLightMap, projShadowMap, transparencyTex, 7, 7, vFace);
}

pixelOutput ProjectedLightSuperSample2_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS(IN, diffTex, normalTex, specTex, cubeTex, g_lightInfo, g_projLight, microTex, projLightMap, projShadowMap, transparencyTex, 9, 9, vFace);
}

pixelOutput ProjectedLightSuperSample3_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS(IN, diffTex, normalTex, specTex, cubeTex, g_lightInfo, g_projLight, microTex, projLightMap, projShadowMap, transparencyTex, 15, 15, vFace);
}

pixelOutput Glow_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return glowPS(IN, normalTex, specTex, microTex, g_lightInfo, g_projLight, projLightMap, projShadowMap, vFace);
}
