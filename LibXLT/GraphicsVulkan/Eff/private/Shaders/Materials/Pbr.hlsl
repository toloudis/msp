/*****************************************************************************
**  Pbr.hlsl
**
**      glTF 2.0 metallic-roughness material (Vulkan only: it has no DX11
**      original, and DX11 draws Pbr.fx materials as Blinn).
**
**      BRDF: GGX distribution, height-correlated Smith-GGX visibility,
**      Schlick Fresnel with F0 = mix(0.04, baseColor, metallic), Lambert
**      diffuse on (1 - F) * baseColor * (1 - metallic). The effect has
**      Blinn's techniques, passes and entry-point names, so every shdw pass
**      that draws Blinn draws Pbr; lights come one per pass.
**
**      Textures are sampled as Blinn samples them (no sRGB conversion), so
**      PBR and Blinn materials agree on texture color. glTF channel layout:
**      metallicRoughnessMap B = metallic, G = roughness; occlusionMap R.
**
**	Extra Large Technology
\****************************************************************************/
// The SasGlobal effect description (gp : SasGlobal) and the SAS UI
// annotations of the material parameters are in Pbr.effect.json; the
// parameter names are effPbrData's (Graphics/Eff/effPbrData.hpp).
// Register layout shared by all materials: see Globals.hlsli.

/*********** support data and functions ******/

#include "Support.hlsli"
#include "Tessellate.hlsli"
#include "Lighting.hlsli"

// b4: this material's own parameters (defaults are in Pbr.effect.json)
cbuffer MaterialParams : register(b4)
{
	float4 g_baseColor;					// : MaterialDiffuse, default (1, 1, 1, 1)
	bool hasBaseColorMap;				// default false
	float g_metallic;					// default 1
	float g_roughness;					// default 1
	bool hasMetallicRoughnessMap;		// default false
	bool hasOcclusionMap;				// default false
	float g_occlusionStrength;			// default 1
	float4 g_emissive;					// : MaterialEmissive, default (0, 0, 0, 1)
	bool hasEmissiveMap;				// default false
	float g_emissiveStrength;			// default 1
	int g_alphaMode;					// : AlphaMode, 0 OPAQUE, 1 MASK, 2 BLEND
	float g_alphaCutoff;				// : AlphaCutoff, default 0.5
	bool hasCubeMap;					// default false
	float g_reflMapAngle;				// default 0
};

Texture2D baseColorMap : register(t4);			// : DiffuseTexture
Texture2D metallicRoughnessMap : register(t5);
Texture2D occlusionMap : register(t6);
Texture2D emissiveMap : register(t7);			// : EmissiveTexture
Texture2D cubeMap : register(t8);
Texture2D glowMask : register(t12);				// : GlowMask

SamplerState AnisoWrapSampler : register(s10);

static const int c_AlphaMask = 1;
static const int c_AlphaBlend = 2;
// Roughness is clamped here, so GGX never divides by zero
static const float c_MinRoughness = 0.045;
// Environment blur level at roughness 1 (SampleLevel clamps to the last mip)
static const float c_EnvMaxLod = 6.0;

/*********** vertex shader (Blinn's) ******/

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

/********* surface and BRDF ********/

// The surface's alpha: OPAQUE 1; MASK discards below the cutoff, else 1;
// BLEND the base color's alpha
float PbrAlpha(float baseAlpha)
{
	if (g_alphaMode == c_AlphaMask)
	{
		if (baseAlpha < g_alphaCutoff)
			discard;
		return 1;
	}
	return (g_alphaMode == c_AlphaBlend) ? baseAlpha : 1;
}

struct PbrSurface
{
	float3 diffuseColor;	// baseColor * (1 - metallic)
	float3 F0;				// reflectance at normal incidence
	float roughness;		// perceptual, clamped to [c_MinRoughness, 1]
	float alpha;
	float3 N;				// world normal (bumped)
	float3 V;				// from the shade point to the eye
	float2 uv;
};

// The surface at a pixel; discards where the alpha mode or the renderer's
// alpha test (g_AlphaTestRef, as Blinn's) says so
PbrSurface GetPbrSurface(TANGENT_VERTEX_OUTPUT IN, bool vFace)
{
	PbrSurface s;
	s.uv = IN.V.TexCoord0;

	float4 baseColor = Tex2DCombine(hasBaseColorMap, baseColorMap, s.uv, g_baseColor);
	s.alpha = PbrAlpha(baseColor.a);
	if( g_AlphaTestRef >= s.alpha ) discard;

	float4 mr = Tex2DCombine(hasMetallicRoughnessMap, metallicRoughnessMap, s.uv, float4(1, g_roughness, g_metallic, 1));
	float metallic = saturate(mr.b);
	s.roughness = clamp(mr.g, c_MinRoughness, 1.0);

	s.diffuseColor = baseColor.rgb * (1 - metallic);
	s.F0 = lerp(float3(0.04, 0.04, 0.04), baseColor.rgb, metallic);
	s.N = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );
	s.V = normalize(g_eyePos.xyz - IN.V.WorldPos);
	return s;
}

float3 PbrEmissive(float2 uv)
{
	return Tex2DCombine(hasEmissiveMap, emissiveMap, uv, g_emissive * g_emissiveStrength).rgb;
}

float D_GGX(float NdotH, float a)
{
	float a2 = a*a;
	float d = NdotH*NdotH*(a2 - 1) + 1;
	return a2 / (PI*d*d);
}

float V_SmithGGX(float NdotL, float NdotV, float a)
{
	float a2 = a*a;
	float gv = NdotL*sqrt(NdotV*NdotV*(1 - a2) + a2);
	float gl = NdotV*sqrt(NdotL*NdotL*(1 - a2) + a2);
	return 0.5 / max(gv + gl, 1e-5);
}

float3 F_Schlick(float3 F0, float VdotH)
{
	return F0 + (1 - F0) * pow(1 - VdotH, 5);
}

// The light's terms at the surface: NdotL, the Fresnel term and the
// specular BRDF (times PI, see PbrDirect)
void PbrLightTerms(PbrSurface s, IncidentLight light, out float NdotL, out float3 F, out float3 specular)
{
	float3 L = normalize(light.L);
	// L + V is zero when the light is exactly behind the surface, seen
	// from the front: no NaN, NdotH and VdotH are 0 then
	float3 h = L + s.V;
	float3 H = h * rsqrt(max(dot(h, h), 1e-8));
	NdotL = saturate(dot(s.N, L));
	float NdotV = max(dot(s.N, s.V), 1e-4);
	float NdotH = saturate(dot(s.N, H));
	float VdotH = saturate(dot(s.V, H));
	float a = s.roughness * s.roughness;
	F = F_Schlick(s.F0, VdotH);
	specular = PI * D_GGX(NdotH, a) * V_SmithGGX(NdotL, NdotV, a) * F;
}

// One light. MachStudio's light convention (as PhongDiffuse): a white
// Lambert surface lit head-on by colour c shows c, so the reference BRDF
// is multiplied by PI; the light's diffuse colour (Cld) lights the
// diffuse term and its specular colour (Cls) the specular term, as in
// Blinn.
float3 PbrDirect(PbrSurface s, IncidentLight light)
{
	float NdotL;
	float3 F, specular;
	PbrLightTerms(s, light, NdotL, F, specular);
	return NdotL * (light.Cld * (1 - F) * s.diffuseColor + light.Cls * specular);
}

// The specular term alone (Glow)
float3 PbrSpecularOnly(PbrSurface s, IncidentLight light)
{
	float NdotL;
	float3 F, specular;
	PbrLightTerms(s, light, NdotL, F, specular);
	return NdotL * light.Cls * specular;
}

// Split-sum environment specular weight, analytic (Karis' mobile
// approximation of the environment BRDF; no lookup texture)
float3 EnvBRDFApprox(float3 F0, float roughness, float NdotV)
{
	const float4 c0 = float4(-1, -0.0275, -0.572, 0.022);
	const float4 c1 = float4(1, 0.0425, 1.04, -0.04);
	float4 r = roughness * c0 + c1;
	float a004 = min(r.x * r.x, exp2(-9.28 * NdotV)) * r.x + r.y;
	float2 AB = float2(-1.04, 1.04) * a004 + r.zw;
	return F0 * AB.x + AB.y;
}

/********* pixel shaders ********/

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN, uniform LightInfo i_Light, uniform bool i_bDefaultPass, bool vFace)
{
    pixelOutput OUT;

	PbrSurface s = GetPbrSurface(IN, vFace);

	IncidentLight light;
	IlluminatePointLight(IN.V.WorldPos, i_Light, light);

	OUT.col.rgb = PbrDirect(s, light);
	if (i_bDefaultPass)
		OUT.col.rgb += PbrEmissive(s.uv) + envmap_approximation(g_diffuseFactor).rgb;

	OUT.col.a = s.alpha;
	OUT.col.rgb *= OUT.col.a;	//premul alpha
    return OUT;
}

pixelOutput singleLightPS_Tess(TANGENT_VERTEX_OUTPUT IN, uniform LightInfo i_Light, uniform bool i_bDefaultPass, bool vFace)
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, i_Light, i_bDefaultPass, vFace );
}

pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap,
		uniform int nBlockerSamples, uniform int nShadowSamples, bool vFace)
{
    pixelOutput OUT;

	PbrSurface s = GetPbrSurface(IN, vFace);

	IncidentLight light;
	IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap,
		nBlockerSamples, nShadowSamples, light);

	OUT.col.rgb = PbrDirect(s, light);

	OUT.col.a = s.alpha;
	OUT.col.rgb *= OUT.col.a;	//premul alpha
    return OUT;
}

pixelOutput projLightPS_Tess(TANGENT_VERTEX_OUTPUT IN,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap,
		uniform int nBlockerSamples, uniform int nShadowSamples, bool vFace)
{
	TangentDisplace( IN.V );
	return projLightPS( IN, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap, bool vFace)
{
    pixelOutput OUT;
    OUT.col = float4(0,0,0,0);

	PbrSurface s = GetPbrSurface(IN, vFace);
	OUT.col.a = s.alpha;

    float3 mask = g_bHasMask ? glowMask.Sample( AnisoWrapSampler, s.uv).rgb : float3(1,1,1);
    if (g_bConstGlow)
    {
		OUT.col.rgb = mask;
    }
    else
    {
		IncidentLight light;
		if (g_bProjLt)
			IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, light);
		else
			IlluminatePointLight(IN.V.WorldPos, i_Light, light);

		OUT.col.rgb = PbrSpecularOnly(s, light) * mask;
	}
    return OUT;
}

pixelOutput glowPS_Tess(TANGENT_VERTEX_OUTPUT IN,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform Texture2D ProjTextureMap,
		uniform Texture2D ProjShadowMap, bool vFace)
{
	TangentDisplace( IN.V );
	return glowPS( IN, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, vFace );
}

// Image-based light: the environment's diffuse and specular, and the
// material's reflection map, weighted by the split-sum Fresnel term;
// roughness picks the blur level; occlusion darkens the environment only
pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN, bool vFace)
{
    pixelOutput OUT;

	PbrSurface s = GetPbrSurface(IN, vFace);

	float NdotV = max(dot(s.N, s.V), 1e-4);
	float3 w = EnvBRDFApprox(s.F0, s.roughness, NdotV);
	float lod = s.roughness * c_EnvMaxLod;

	float ao = 1;
	if (hasOcclusionMap)
		ao = lerp(1, occlusionMap.Sample(g_DefaultSampler, s.uv).r, g_occlusionStrength);

	float3 diffuse = s.diffuseColor * g_envDiffuseColor.rgb * g_diffuseFactor;
	if (g_bHasDiffuseEnvMap)
		diffuse *= SampleEnvDiffuse(s.N, diffuseEnvMap, g_diffuseEnvAngle).rgb;

	float3 specular = g_envSpecularColor.rgb * g_specularFactor * w;
	if (g_bHasSpecularEnvMap)
		specular *= SampleEnvironmentLOD(s.V, s.N, specularEnvMap, g_specularEnvAngle, lod).rgb;

	float3 reflection = float3(0, 0, 0);
	if (hasCubeMap && g_bCubeMapEnabled)
		reflection = SampleEnvironmentLOD(s.V, s.N, cubeMap, g_reflMapAngle*PI_DIV_180, lod).rgb * w;

	OUT.col.rgb = g_IsolateReflection ? reflection : PbrEmissive(s.uv) + ao * (diffuse + specular + reflection);

	OUT.col.a = s.alpha;
	OUT.col.rgb *= OUT.col.a;	//premul alpha
    return OUT;
}

pixelOutput iblPS_Tess(TANGENT_VERTEX_OUTPUT IN, bool vFace)
{
	TangentDisplace( IN.V );
	return iblPS( IN, vFace );
}

/*************/

//////////////////////////////////////////////////////////////////////////////
// Entry points, named as Blinn's (Pbr.effect.json's techniques)
//////////////////////////////////////////////////////////////////////////////

pixelOutput Default_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS(IN, g_lightInfo, true, vFace);
}

pixelOutput Default_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Tess(IN, g_lightInfo, true, vFace);
}

pixelOutput SingleLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS(IN, g_lightInfo, false, vFace);
}

pixelOutput SingleLight_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS_Tess(IN, g_lightInfo, false, vFace);
}

pixelOutput ProjectedLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS(IN, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, vFace);
}

pixelOutput ProjectedLight_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW, vFace);
}

pixelOutput ProjectedLightSuperSample_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS(IN, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED, vFace);
}

pixelOutput ProjectedLightSuperSample_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED, vFace);
}

pixelOutput ProjectedLightSuperSample2_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS(IN, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample2_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample3_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS(IN, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH, vFace);
}

pixelOutput ProjectedLightSuperSample3_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return projLightPS_Tess(IN, g_lightInfo, g_projLight, projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH, vFace);
}

pixelOutput Glow_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return glowPS(IN, g_lightInfo, g_projLight, projLightMap, projShadowMap, vFace);
}

pixelOutput Glow_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return glowPS_Tess(IN, g_lightInfo, g_projLight, projLightMap, projShadowMap, vFace);
}

pixelOutput DOFPrep_PDefault_PS(TANGENT_VERTEX_OUTPUT IN)
{
	// MASK cuts out here too
	PbrAlpha(Tex2DCombine(hasBaseColorMap, baseColorMap, IN.V.TexCoord0, g_baseColor).a);
    return DOFPrepTrans_PS(IN, false, baseColorMap, 1);
}

pixelOutput Environment_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return iblPS(IN, vFace);
}

pixelOutput Environment_PTess_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return iblPS_Tess(IN, vFace);
}
