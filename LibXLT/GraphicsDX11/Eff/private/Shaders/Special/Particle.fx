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

#include "..\Support.h"
#include "..\Lighting.h"

float g_shininess : MaterialPower = 20.0f;
float4 g_emissive = {0.0f, 0.0f, 0.0f, 1.0f};
float4 g_diffuse : MaterialDiffuse = {1.0f, 1.0f, 1.0f, 1.0f};
float4 g_specular : MaterialSpecular = {1.0f, 1.0f, 1.0f, 1.0f};
float g_reflectivity : Reflectivity = 1.0f;

// textures
texture2D diffuseMap;
texture2D glowMask		: GlowMask;

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

SamplerState glowSampler
{
    Filter = ANISOTROPIC;
	MaxAnisotropy = 16;
};

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

technique11 Default
{
	pass p0 
	{		
		VertexShader = compile vs_5_0 singleLightVS(g_wvp,g_worldIT,
					g_world, 0);
		PixelShader = compile ps_5_0 singleLightPS(diffuseMap,
					g_lightInfo, g_shininess);
	}
}

technique11 SingleLight
{
	pass p0 
	{		
		VertexShader = compile vs_5_0 singleLightVS(g_wvp,g_worldIT,
					g_world, 0);
		PixelShader = compile ps_5_0 singleLightPS(diffuseMap,
					g_lightInfo, g_shininess);
	}
}

technique11 ProjectedLight
{
	pass p0 
	{		
		VertexShader = compile vs_5_0 singleLightVS(g_wvp,g_worldIT,
					g_world, 0);
		PixelShader = compile ps_5_0 projLightPS(diffuseMap,
					g_lightInfo, g_projLight, g_shininess,
						projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW);
	}
}
technique11 ProjectedLightSuperSample
{
	pass p0
	{
		VertexShader = compile vs_5_0 singleLightVS(g_wvp,g_worldIT,
					g_world, 0);
		PixelShader = compile ps_5_0 projLightPS(diffuseMap, 
					g_lightInfo, g_projLight, g_shininess,
						projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED);
	}
}
technique11 ProjectedLightSuperSample2
{
	pass p0
	{
		VertexShader = compile vs_5_0 singleLightVS(g_wvp,g_worldIT,
					g_world, 0);
		PixelShader = compile ps_5_0 projLightPS(diffuseMap, 
					g_lightInfo, g_projLight, g_shininess,
						projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH);
	}
}

technique11 ProjectedLightSuperSample3
{
	pass p0
	{
		VertexShader = compile vs_5_0 singleLightVS(g_wvp,g_worldIT,
					g_world, 0);
		PixelShader = compile ps_5_0 projLightPS(diffuseMap, 
					g_lightInfo, g_projLight, g_shininess,
						projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH);
	}
}

technique11 Glow
{
	pass p0
	{
		VertexShader = compile vs_5_0 singleLightVS(g_wvp,g_worldIT,
					g_world, g_glowSize);
		PixelShader = compile ps_5_0 glowPS(g_lightInfo, g_projLight, g_shininess,
					projLightMap, projShadowMap);

	}
}
technique11 DOFPrep
{
	pass p0
	{
		VertexShader = compile vs_5_0 DOFPrepVS_Default();
		PixelShader = compile ps_5_0 DOFPrep_PS();
	}

}
technique11 Matte
{
	pass p0 
	{		
		VertexShader = compile vs_5_0 singleLightVS(g_wvp,g_worldIT,
					g_world, 0);
		PixelShader = compile ps_5_0 simpleMattePS();
	}
}
technique11 Environment
{
	pass p0 
	{		
		VertexShader = compile vs_5_0 singleLightVS(g_wvp,g_worldIT,
					g_world, 0);
		PixelShader = compile ps_5_0 iblPS();
	}
}
/***************************** eof ***/
