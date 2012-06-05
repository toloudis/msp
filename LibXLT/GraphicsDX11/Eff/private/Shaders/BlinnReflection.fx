/*****************************************************************************
**  BlinnReflection.fx
**
**      Reflective / refractive object surface 
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
int gp : SasGlobal
<
  int3 SasVersion = {1,0,0};
  string SasEffectAuthor			= "Daniel Toloudis";
  string SasEffectAuthoringSoftware = "MachStudio";
  string SasEffectCategory			= "/material";
  string SasEffectCompany			= "studio|gpu";
  string SasEffectDescription		= "Blinn Reflection";
  string SasEffectHelp				= "This is a blinn shader with oren nayar diffuse roughness and dynamic reflections.";    
  string SasEffectRevision			= "2";  
  // v2 swapped IOR and g_IOR as a bugfix.
>;

/*********** support data and functions ******/

#include "Support.h"
#include "Tessellate.h"
#include "Lighting.h"

// semantics will cause automatic binding to code-generated reflection map
bool g_isPlanar			: ReflectionMapIsPlanar = false;
bool g_hasCubeMap		: HasReflectionMap = false;
TextureCube g_cubeMap	: CubeReflectionMap;
Texture2D g_planarMap	: PlanarReflectionMap;

SamplerState g_cubeSampler
{
	FILTER = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;  // Allow 3D hardware to address cubemap's correctly
	AddressV = CLAMP;  // Allow 3D hardware to address cubemap's correctly
	AddressW = CLAMP;  // Allow 3D hardware to address cubemap's correctly
};

SamplerState planarSampler
{
	FILTER = MIN_MAG_MIP_LINEAR;
	AddressU = CLAMP;  
	AddressV = CLAMP;  
};

float4 g_diffuse : MaterialDiffuse
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Diffuse Color";
	string SasUiDescription = "diffuse color of surface";
	string UiCategory = "Diffuse";
	int UiIndex = 1;
>
= {0.5f, 0.5f, 0.5f, 1.0f};

bool hasDiffuseMap = false;
Texture2D diffuseMap		: DiffuseTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Diffuse Map";
	string SasUiDescription = "diffuse color";
	string UiCategory = "Diffuse";
	string ExistVar = "hasDiffuseMap";
	int UiIndex = 2;
>
;

float g_roughness
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Roughness";
	string SasUiDescription = "diffuse roughness";
	string UiCategory = "Diffuse";
	int UiIndex = 3;
>
= 0;

float4 g_emissive : MaterialEmissive
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Emissive Color";
	string SasUiDescription = "emissive color";
	string UiCategory = "Diffuse";
	int UiIndex = 4;
>
= {0.0f, 0.0f, 0.0f, 1.0f};

bool hasEmissiveMap = false;
Texture2D emissiveMap : EmissiveTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Emissive Map";
	string SasUiDescription = "emissive color";
	string UiCategory = "Diffuse";
	string ExistVar = "hasEmissiveMap";
	int UiIndex = 5;
>;

float g_emissiveIntensity 
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Intensity";
	string SasUiDescription = "Emissive Intensity";
	string UiCategory = "Diffuse";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 6;
> = 1.0f;

float4 g_specular : MaterialSpecular
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Specular Color";
	string SasUiDescription = "color of specular hilight";
	string UiCategory = "Specular";
	int UiIndex = 7;
>
= {1.0f, 1.0f, 1.0f, 1.0f};

bool hasSpecularMap = false;
Texture2D specularMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Specular Color Map";
	string SasUiDescription = "specular color";
	string UiCategory = "Specular";
	string ExistVar = "hasSpecularMap";
	int UiIndex = 8;
>
;

float g_specularPower
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Specular Power";
	string SasUiDescription = "Specular power controls intensity of highlight";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 9;
> = 1.0f;

float g_IOR
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Specular Roll Off";
	string SasUiDescription = "index of refraction";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 10;
>
= 1;

bool hasIORMap = false;
Texture2D iorMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Specular Roll Off Map";
	string SasUiDescription = "index of refraction texture";
	string UiCategory = "Specular";
	string ExistVar = "hasIORMap";
	int UiIndex = 11;
>
;

float g_shininess : MaterialPower
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Shininess";
	string SasUiDescription = "specular exponent controls size of highlight";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 12;
> = 0.5f;

bool hasGlossMap = false;
Texture2D glossMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Shininess Map";
	string SasUiDescription = "specular exponent multiplier";
	string UiCategory = "Specular";
	string ExistVar = "hasGlossMap";
	int UiIndex = 13;
>
;

float g_transparency : Opacity
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Opacity";
	string SasUiDescription = "opacity value, 0 transparent, 1 for opaque";
	string UiCategory = "Opacity";
	int UiIndex = 14;
>
= 1.0f;

bool hasTransparencyMap = false;
Texture2D transparencyMap	: OpacityTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Opacity Texture";
	string SasUiDescription = "opacity texture";
	string UiCategory = "Opacity";
	string ExistVar = "hasTransparencyMap";
	int UiIndex = 15;
>
;

float g_reflectivity : Reflectivity
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Environment Contribution";
	string SasUiDescription = "multiplier for reflection map";
	string UiCategory = "Reflection";
	int UiIndex = 16;
>
= 1.0f;


float fresnelPower
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Fresnel Power";
	string SasUiDescription = "fresnel exponent";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 5.0;
	int UiIndex = 17;
>
= 4.0;

float fresnelBias
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Fresnel Bias";
	string SasUiDescription = "fresnel bias";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 18;
>
= 0.2;

// mipmap LOD value for cubemap (refl and refr separate?)
float reflLOD
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Reflection Blur";
	string SasUiDescription = "reflection blurriness";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 19;
>
= 0;

float reflFactor 
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Reflection Factor";
	string SasUiDescription = "reflection contribution";
	string UiCategory = "Reflection";
	int UiIndex = 20;
>
= 1;

bool hasReflectFactorMap = false;
Texture2D reflectFactorMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Reflection Mask Map";
	string SasUiDescription = "reflection mask";
	string UiCategory = "Reflection";
	string ExistVar = "hasReflectFactorMap";
	int UiIndex = 21;
>
;

// IOR ratio (n1)/(n2) for refraction
float IOR
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Index of Refraction";
	string SasUiDescription = "index of refraction";
	string UiCategory = "Reflection";
	float SasUiMin = 0.3;
	float SasUiMax = 3.0;
	int UiIndex = 22;
> = 1.0;

float refrFactor
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Refraction Factor";
	string SasUiDescription = "refraction contribution";
	string UiCategory = "Reflection";
	int UiIndex = 23;
>
= 0;

float refrLOD
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Refraction Blur";
	string SasUiDescription = "refraction blurriness";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 24;
>
= 0;

bool hasSmearMap = false;
Texture2D reflectSmearMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Reflection Smear Map";
	string SasUiDescription = "reflection smear map";
	string UiCategory = "Reflection";
	string ExistVar = "hasSmearMap";
	int UiIndex = 25;
>
;

float smearMapScale
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Smear Scale";
	string SasUiDescription = "smear map scale";
	string UiCategory = "Reflection";
	float SasUiMin = 0;
	float SasUiMax = 20.0;
	float SasUiSteps = 20.0;
	int UiIndex = 26;
> = 0;

float g_specularBias = 0;

// textures

Texture2D glowMask		: GlowMask;

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

    float4 Po = float4( Vtx.Position + Vtx.Normal*g_glowSize, 1.0);
	return OUT;
}

/********* pixel shader ********/

SamplerState AnisoWrapSampler
{
	FILTER = ANISOTROPIC;
    AddressU = WRAP;
    AddressV = WRAP;
};

float4 GetReflection(float3 bumpNormal, float3 worldNormal, float3 ScreenPos, float3 WorldEyeDir, float2 TexCoord0, float bumpScale)
{
	float4 refl = float4(0,0,0,0);
	float4 refr = float4(0,0,0,0);
	float4 rr	= float4(0,0,0,0);
	float4 reflColorFactor=reflFactor,refrColorFactor=refrFactor;
	float fresnelRefl, fresnelRefr;

	float4 factor = Tex2DReplace(hasReflectFactorMap, reflectFactorMap, TexCoord0, float4(1,1,1,1));
	reflColorFactor *= factor;
	refrColorFactor *= factor;

	if (g_isPlanar)
	{
		float3 tc = float3(ScreenPos.x/ScreenPos.z, ScreenPos.y/ScreenPos.z, 1);		
		tc.y = 1.0 - tc.y;
		tc.xy += (bumpNormal.xy * (bumpScale * 0.1));

		fresnelRefl = fastFresnel(dot(WorldEyeDir, worldNormal), fresnelBias, fresnelPower);
		fresnelRefr = 1 - fresnelRefl;
	
//		refl = tex2Dbias(planarMap, float4(tc.xyz, reflLOD)) *  fresnelRefl * reflColorFactor;
		//refr = tex2Dbias(planarMap, float4(tc.xyz, refrLOD)) *  fresnelRefr * refrColorFactor;
		refl = g_planarMap.SampleBias( planarSampler, tc.xy, reflLOD ) * fresnelRefl * reflColorFactor;
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
//		refl = texCUBEbias(g_cubeMap, float4(reflVect, reflLOD)) *  fresnelRefl * reflColorFactor;
//		refr = texCUBEbias(g_cubeMap, float4(refrVect, refrLOD)) *  fresnelRefr * refrColorFactor;
		refl = g_cubeMap.SampleBias( g_cubeSampler, reflVect, reflLOD ) * fresnelRefl * reflColorFactor;
		refr = g_cubeMap.SampleBias( g_cubeSampler, refrVect, refrLOD ) * fresnelRefr * refrColorFactor;
	}
	rr = refl + refr;
	return rr;
}

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
	return (g_specularPower* specComp) * (lSpecColor * sSpecColor);
}

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN,
					uniform Texture2D DiffuseMap,
					uniform Texture2D NormalMap,
					uniform Texture2D SpecularMap,
					uniform Texture2D SpecPowerMap,
					uniform Texture2D IORMap,
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
	float sSpecPower = Tex2DCombine(hasGlossMap, SpecPowerMap, IN.V.TexCoord0, SpecPowerScale).r;
    	
	//fetch bump normal
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );

	// map 0..1 to 1..g_IOR 
	float ior = Tex2DCombine(hasIORMap, IORMap, IN.V.TexCoord0, g_IOR-1).r + 1;
	
	float3 diffuse = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor);
	if (g_roughness>0)
		diffuse *= OrenNayarDiffuse(lightDir, -worldEyeDir, worldNormal, g_roughness);
	float3 specular = BlinnSpecular(worldNormal, lightDir, -worldEyeDir, 
		light.Cls, sSpecColor, sSpecPower, ior, g_specularBias);

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
								uniform Texture2D SpecPowerMap,
								uniform Texture2D IORMap,
								uniform LightInfo i_Light,
								uniform float SpecPowerScale,
		  						uniform float reflectivity,
					  			uniform bool i_bDefaultPass,
								bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, DiffuseMap, NormalMap, SpecularMap, SpecPowerMap, IORMap, i_Light, SpecPowerScale, reflectivity, i_bDefaultPass, vFace );
}

pixelOutput singleLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
							    uniform Texture2D DiffuseMap,
								uniform Texture2D NormalMap,
								uniform Texture2D SpecularMap,
								uniform Texture2D SpecPowerMap,
								uniform Texture2D IORMap,
								uniform LightInfo i_Light,
								uniform float SpecPowerScale,
		  						uniform float reflectivity,
					  			uniform bool i_bDefaultPass,
							    bool vFace : SV_ISFRONTFACE )
{
	return singleLightPS( IN, DiffuseMap, NormalMap, SpecularMap, SpecPowerMap, IORMap, i_Light, SpecPowerScale, reflectivity, i_bDefaultPass, vFace );
}

pixelOutput projLightPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D DiffuseMap,
		uniform Texture2D NormalMap,
		uniform Texture2D SpecularMap,
		uniform Texture2D SpecPowerMap,
		uniform Texture2D IORMap,
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
	float sSpecPower = Tex2DCombine(hasGlossMap, SpecPowerMap, IN.V.TexCoord0, SpecPowerScale).r;

	//fetch bump normal
	float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );

	// map 0..1 to 1..g_IOR 
	float ior = Tex2DCombine(hasIORMap, IORMap, IN.V.TexCoord0, g_IOR-1).r + 1;

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
							uniform Texture2D NormalMap,
							uniform Texture2D SpecularMap,
							uniform Texture2D SpecPowerMap,
							uniform Texture2D IORMap,
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
	return projLightPS( IN, DiffuseMap, NormalMap, SpecularMap, SpecPowerMap, IORMap, i_Light, i_ProjLight, SpecPowerScale, reflectivity, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput projLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
							uniform Texture2D DiffuseMap,
							uniform Texture2D NormalMap,
							uniform Texture2D SpecularMap,
							uniform Texture2D SpecPowerMap,
							uniform Texture2D IORMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform float SpecPowerScale,
							uniform float reflectivity,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							uniform int nBlockerSamples, uniform int nShadowSamples,
							bool vFace : SV_ISFRONTFACE )
{
	return projLightPS( IN, DiffuseMap, NormalMap, SpecularMap, SpecPowerMap, IORMap, i_Light, i_ProjLight, SpecPowerScale, reflectivity, ProjTextureMap, ProjShadowMap, nBlockerSamples, nShadowSamples, vFace );
}

pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN,
		uniform Texture2D NormalMap,
		uniform Texture2D SpecularMap,
		uniform Texture2D SpecPowerMap,
		uniform Texture2D IORMap,
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
		float sSpecPower = Tex2DCombine(hasGlossMap, SpecPowerMap, IN.V.TexCoord0, SpecPowerScale).r;
	        
		//fetch bump normal
		float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, NormalMap, g_bumpMapScale, vFace );

		// map 0..1 to 1..g_IOR 
		float ior = Tex2DCombine(hasIORMap, IORMap, IN.V.TexCoord0, g_IOR-1).r + 1;

		OUT.col.rgb = BlinnSpecular(worldNormal, lightDir, -worldEyeDir, 
			light.Cls, sSpecColor, sSpecPower, ior, g_specularBias) * mask;
	}
    return OUT;
}

pixelOutput glowPS_Tess( TANGENT_VERTEX_OUTPUT IN,
						uniform Texture2D NormalMap,
						uniform Texture2D SpecularMap,
						uniform Texture2D SpecPowerMap,
						uniform Texture2D IORMap,
						uniform LightInfo i_Light,
						uniform ProjLightInfo i_ProjLight,
						uniform float SpecPowerScale,
						uniform Texture2D ProjTextureMap,
						uniform Texture2D ProjShadowMap,
						bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return glowPS( IN, NormalMap, SpecularMap, SpecPowerMap, IORMap, i_Light, i_ProjLight, SpecPowerScale, ProjTextureMap, ProjShadowMap, vFace );
}

pixelOutput glowPS_Default( TANGENT_VERTEX_OUTPUT IN, 
						   uniform Texture2D NormalMap,
							uniform Texture2D SpecularMap,
							uniform Texture2D SpecPowerMap,
							uniform Texture2D IORMap,
							uniform LightInfo i_Light,
							uniform ProjLightInfo i_ProjLight,
							uniform float SpecPowerScale,
							uniform Texture2D ProjTextureMap,
							uniform Texture2D ProjShadowMap,
							bool vFace : SV_ISFRONTFACE )
{
	return glowPS( IN, NormalMap, SpecularMap, SpecPowerMap, IORMap, i_Light, i_ProjLight, SpecPowerScale, ProjTextureMap, ProjShadowMap, vFace );
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
	
	float bumpScale = 1.0f;
    if (hasNormalMap)
		bumpScale = g_bumpMapScale;

	float4 rr = 0;
	if (g_hasCubeMap && g_bCubeMapEnabled)
	{
		float3 bumpNormal = Tex2DNormal(hasNormalMap, normalMap, IN.V.TexCoord0) * float3(g_bumpMapScale,g_bumpMapScale,1);
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


technique11 Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 singleLightPS_##PassName(diffuseMap, normalMap, specularMap,\
					glossMap, iorMap, g_lightInfo, g_shininess, g_reflectivity, true);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_DEFAULT(Default)
PASS_DEFAULT(Tess)
}

technique11 SingleLight
{
#define PASS_SINGLELIGHT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 singleLightPS_##PassName(diffuseMap, normalMap, specularMap,\
					glossMap, iorMap, g_lightInfo, g_shininess, g_reflectivity, false);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_SINGLELIGHT(Default)
PASS_SINGLELIGHT(Tess)
}

technique11 ProjectedLight
{
#define PASS_PROJECTEDLIGHT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffuseMap, normalMap, specularMap,\
					glossMap, iorMap, g_lightInfo, g_projLight, g_shininess, g_reflectivity,\
						projLightMap, projShadowMap, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHT(Default)
PASS_PROJECTEDLIGHT(Tess)
}
technique11 ProjectedLightSuperSample
{
#define PASS_PROJECTEDLIGHTSS(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffuseMap, normalMap, specularMap,\
					glossMap, iorMap, g_lightInfo, g_projLight, g_shininess, g_reflectivity,\
						projLightMap, projShadowMap, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHTSS(Default)
PASS_PROJECTEDLIGHTSS(Tess)
}
technique11 ProjectedLightSuperSample2
{
#define PASS_PROJECTEDLIGHTSS2(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffuseMap, normalMap, specularMap,\
					glossMap, iorMap, g_lightInfo, g_projLight, g_shininess, g_reflectivity,\
						projLightMap, projShadowMap, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHTSS2(Default)
PASS_PROJECTEDLIGHTSS2(Tess)
}
technique11 ProjectedLightSuperSample3
{
#define PASS_PROJECTEDLIGHTSS3(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 projLightPS_##PassName(diffuseMap, normalMap, specularMap,\
					glossMap, iorMap, g_lightInfo, g_projLight, g_shininess, g_reflectivity,\
						projLightMap, projShadowMap, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHTSS3(Default)
PASS_PROJECTEDLIGHTSS3(Tess)
}

technique11 Glow
{
#define PASS_GLOW(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 glowPS_##PassName(normalMap, specularMap, \
					glossMap, iorMap, g_lightInfo, g_projLight, g_shininess,\
					projLightMap, projShadowMap);\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_GLOW(Default)
PASS_GLOW(Tess)
}
technique11 DOFPrep
{
#define PASS_DOFPREP(PassName)	\
	pass P##PassName			\
	{						\
		SetVertexShader(CompileShader(vs_5_0, singleLightVS_##PassName()));\
		SetPixelShader(CompileShader(ps_5_0, DOFPrepTrans_PS( hasTransparencyMap, transparencyMap, g_transparency )));\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_DOFPREP(Default)
PASS_DOFPREP(Tess)
}
technique11 Matte
{
#define PASS_MATTE(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 simpleMattePS();\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_MATTE(Default)
PASS_MATTE(Tess)
}
technique11 Environment
{
#define PASS_ENVIRONMENT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 iblPS_##PassName();\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_ENVIRONMENT(Default)
PASS_ENVIRONMENT(Tess)
}
/***************************** eof ***/
