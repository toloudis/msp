//////////////////////////////////////////////////////////////////////////////
// Converted from Particle.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// Particle.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Particle.fx
**
**      Decal texture with transparent texture on top, 
**	implementation for shader array
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

/*********** support data and functions ******/

#include "../Materials/Support.hlsli"
#include "../Materials/Lighting.hlsli"

// Register layout shared with the materials: see ../Materials/Globals.hlsli.
// b4: this shader's own parameters (defaults are in Particle.effect.json)
cbuffer MaterialParams : register(b4)
{
	float g_shininess;				// : MaterialPower, default 20
	float4 g_emissive;				// default (0,0,0,1)
	float4 g_diffuse;				// : MaterialDiffuse, default (1,1,1,1)
	float4 g_specular;				// : MaterialSpecular, default (1,1,1,1)
	float g_reflectivity;			// : Reflectivity, default 1
};

// textures
Texture2D diffuseMap : register(t4);
Texture2D glowMask : register(t5);				// : GlowMask

/************* DATA STRUCTS **************/

/* data from application vertex buffer */
struct appdata {
    float3 Position	: SV_POSITION;
    float3 Normal	: NORMAL;
    float4 Color	: COLOR;
    float2 UV		: TEXCOORD0;
};

/* data passed from vertex shader to pixel shader */
struct vertexOutput {
    float4 HPosition	: SV_POSITION;
    float2 TexCoord0	: TEXCOORD0;
    float4 diffCol	: COLOR0;
    float4 specCol	: COLOR1;
};

struct perPixelVertexOutput {
    float4 HPosition	: SV_POSITION;
    float2 TexCoord0	: TEXCOORD0;
    float3 WorldPos		: TEXCOORD1;
	float3 WorldNormal	: TEXCOORD2;
    float Alpha			: COLOR0;
};

/*********** support functions ******/

    
/*********** vertex shader ******/

perPixelVertexOutput singleLightVS(appdata IN,
    uniform float4x4 WorldViewProj,
    uniform float4x4 WorldIT,
    uniform float4x4 World,
    uniform float GlowSize
) {
    perPixelVertexOutput OUT;
    
    // output position in proj space
    float4 Po = float4(IN.Position.xyz + IN.Normal*GlowSize, 1.0);
    OUT.HPosition = mul(WorldViewProj, Po);
    
    // transform position to world space and get vector to light
    float3 Pw = mul(World, Po).xyz;
    OUT.WorldPos = Pw;

	OUT.WorldNormal = mul((float3x3)WorldIT, IN.Normal);
	
	// decal and bump texture coords
    OUT.TexCoord0 = IN.UV;

	// stuff per-particle alpha here:
	OUT.Alpha = IN.Color.a;
    
    return OUT;
}

/********* pixel shader ********/

SamplerState glowSampler : register(s10);

pixelOutput labPS(vertexOutput IN,
		  uniform Texture2D DiffuseMap) 
{
    // alpha blend refl layer and base color
    pixelOutput OUT;    
    float4 col = IN.diffCol;
    col *= DiffuseMap.Sample(g_DefaultSampler, IN.TexCoord0.xy);
	
    float4 spec = IN.specCol;
		
    OUT.col.rgb = col.rgb + spec.rgb;
    OUT.col.a = col.a;
    
    return OUT;
}

pixelOutput singleLightPS(perPixelVertexOutput IN,
					uniform Texture2D DiffuseMap,
					uniform LightInfo i_Light,
					uniform float SpecPowerScale)
{
    pixelOutput OUT; 
	
	IncidentLight light;
	IlluminatePointLight(IN.WorldPos, i_Light, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.WorldPos - g_eyePos.xyz);
    
	//fetch base and specular colors
	float4 sDiffColor = g_diffuse * DiffuseMap.Sample(g_DefaultSampler, IN.TexCoord0.xy);
	
	float sSpecPower = SpecPowerScale;
    	
	//fetch bump normal
	float3 worldNormal = normalize(IN.WorldNormal);
    	
	float3 diffuse = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor.rgb);
	float3 specular = PhongSpecular(worldNormal, lightDir, -worldEyeDir, 
		light.Cls, 0/*g_specular*/, sSpecPower);

    OUT.col.rgb = diffuse + specular;
    OUT.col.a = sDiffColor.a * i_Light.Diffuse.a;
	OUT.col.a *= IN.Alpha;

    return OUT;
}

pixelOutput projLightPS(perPixelVertexOutput IN,
		uniform Texture2D DiffuseMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform float SpecPowerScale,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap,
		uniform int nBlockerSamples, uniform int nShadowSamples)
{
    pixelOutput OUT; 
	
	// note: no shadow for now
	IncidentLight light;
	IlluminateProjLight(IN.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap,
		nBlockerSamples, nShadowSamples, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.WorldPos - g_eyePos.xyz);
    
	//fetch base and specular colors
	float4 sDiffColor = g_diffuse * DiffuseMap.Sample(g_DefaultSampler, IN.TexCoord0.xy);
	
	float sSpecPower = SpecPowerScale;
    	
	//fetch bump normal
	float3 worldNormal = normalize(IN.WorldNormal);
    	
	float3 diffuse = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor.rgb);
	float3 specular = PhongSpecular(worldNormal, lightDir, -worldEyeDir, 
		light.Cls, 0/*g_specular*/, sSpecPower);

    OUT.col.rgb = diffuse + specular;
    OUT.col.a = sDiffColor.a * i_Light.Diffuse.a;
	OUT.col.a *= IN.Alpha;

    return OUT;
}

pixelOutput glowPS(perPixelVertexOutput IN,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform float SpecPowerScale,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap
) {
    pixelOutput OUT; 
    OUT.col = float4(0,0,0,0);
	return (pixelOutput)0;
    
    float4 mask = g_bHasMask ? glowMask.Sample(glowSampler, IN.TexCoord0.xy) : float4(1,1,1,1);
    if (g_bConstGlow)
    {
		OUT.col = mask;
		OUT.col.a = OUT.col.r;
    }
    else //if (mask.r+mask.g+mask.b > 0)
    {
		IncidentLight light;
		if (g_bProjLt)
			IlluminateProjLight(IN.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, light);
		else
			IlluminatePointLight(IN.WorldPos, i_Light, light);
		
		float3 lightDir = normalize(light.L);
		float3 worldEyeDir = normalize(IN.WorldPos - g_eyePos.xyz);

		//fetch base and specular colors
		float sSpecPower = SpecPowerScale;
	        
		//fetch bump normal
		float3 worldNormal = normalize(IN.WorldNormal);
	    	
		float3 specular = PhongSpecular(worldNormal, lightDir, -worldEyeDir, 
			light.Cls, 1/*g_specular*/, sSpecPower);

		OUT.col.rgb = specular;
		OUT.col.a = 1;
		OUT.col *= mask;
	}

	OUT.col.a = 1;
    return OUT;
}

pixelOutput iblPS(perPixelVertexOutput IN) 
{
    pixelOutput OUT; 

	float3 worldEyeDir = normalize(IN.WorldPos - g_eyePos.xyz);
    	
	//fetch bump normal
	float3 bumpNormal = normalize(IN.WorldNormal);

	float4 spec = g_envSpecularColor*g_specularFactor;
	if (g_bHasSpecularEnvMap) 
	{
		spec *= SampleEnvironment((-worldEyeDir), bumpNormal, 
				specularEnvMap, g_specularEnvAngle);
	}
		
	float4 diff = Tex2DCombine(true, diffuseMap, IN.TexCoord0, g_diffuse*g_envDiffuseColor*g_diffuseFactor);
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(bumpNormal, 
				diffuseEnvMap, g_diffuseEnvAngle).rgb,1);
	}

	OUT.col = float4(
		g_emissive.rgb +
		spec.rgb*g_reflectivity + 
		diff.rgb,
		
		g_diffuse.a*diff.a*IN.Alpha);

    return OUT;
}

/*************/


/***************************** eof ***/

//////////////////////////////////////////////////////////////////////////////
// Entry points generated from the technique/pass compile statements.
// Each one binds the uniform arguments the .fx passed in its compile call.
//////////////////////////////////////////////////////////////////////////////

perPixelVertexOutput Default_p0_VS(appdata IN)
{
    return singleLightVS(IN, g_wvp, g_worldIT, g_world, 0);
}

pixelOutput Default_p0_PS(perPixelVertexOutput IN)
{
    return singleLightPS(IN, diffuseMap, g_lightInfo, g_shininess);
}

pixelOutput ProjectedLight_p0_PS(perPixelVertexOutput IN)
{
    return projLightPS(IN, diffuseMap, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, 5, 5);
}

pixelOutput ProjectedLightSuperSample_p0_PS(perPixelVertexOutput IN)
{
    return projLightPS(IN, diffuseMap, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, 7, 7);
}

pixelOutput ProjectedLightSuperSample2_p0_PS(perPixelVertexOutput IN)
{
    return projLightPS(IN, diffuseMap, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, 9, 9);
}

pixelOutput ProjectedLightSuperSample3_p0_PS(perPixelVertexOutput IN)
{
    return projLightPS(IN, diffuseMap, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap, 15, 15);
}

perPixelVertexOutput Glow_p0_VS(appdata IN)
{
    return singleLightVS(IN, g_wvp, g_worldIT, g_world, g_glowSize);
}

pixelOutput Glow_p0_PS(perPixelVertexOutput IN)
{
    return glowPS(IN, g_lightInfo, g_projLight, g_shininess, projLightMap, projShadowMap);
}
