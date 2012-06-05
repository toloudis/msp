/*****************************************************************************
**  CarPaint.fx
**
**      Car Paint
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
**  Version 0.9
\****************************************************************************/
int gp : SasGlobal
<
  int3 SasVersion = {1,0,0};
  string SasEffectAuthor			= "John Schwab";
  string SasEffectAuthoringSoftware = "MachStudioPro";
  string SasEffectCategory			= "/material";
  string SasEffectCompany			= "studio|gpu";
  string SasEffectDescription		= "Car Paint";
  string SasEffectHelp				= "Two Tone with flake and reflection layers, dual Blinn specular and fresnel terms.";
  string SasEffectRevision			= "3";
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
> = {1.0f, 1.0f, 1.0f, 1.0f};

float4 g_edgeColor
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Edge Color";
	string SasUiDescription = "color at the edge";
	string UiCategory = "Diffuse";
	int UiIndex = 2;
> = {1.0f, 1.0f, 1.0f, 1.0f};


float fresnelClrPower
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Color Fresnel Power";
	string SasUiDescription = "color fresnel exponent";
	string UiCategory = "Diffuse";
	float SasUiMin = 0.0;
	float SasUiMax = 5.0;
	int UiIndex = 3;
> = 4.0;

float fresnelClrBias
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Color Fresnel Bias";
	string SasUiDescription = "color fresnel bias";
	string UiCategory = "Diffuse";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 4;
> = 0.2;

float4 g_emissive : MaterialEmissive
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Emissive Color";
	string SasUiDescription = "emissive color";
	string UiCategory = "Diffuse";
	int UiIndex = 5;
> = {0.0f, 0.0f, 0.0f, 1.0f};

bool hasEmissiveMap = false;
Texture2D emissiveMap : EmissiveTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Emissive Map";
	string SasUiDescription = "emissive color";
	string UiCategory = "Diffuse";
	string ExistVar = "hasEmissiveMap";
	int UiIndex = 6;
>;

float g_emissiveIntensity 
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Intensity";
	string SasUiDescription = "Emissive Intensity";
	string UiCategory = "Diffuse";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 7;
> = 1.0f;

float4 g_specular : MaterialSpecular 
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Specular Color";
	string SasUiDescription = "color of specular hilight";
	string UiCategory = "Specular";
	int UiIndex = 8;
> = {1.0f, 1.0f, 1.0f, 1.0f};

float g_IOR
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Specular Roll Off";
	string SasUiDescription = "blinn specular index of refraction";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 9;
> = 0.5;

bool hasIORMap = false;
Texture2D iorMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Specular Roll Off Map";
	string SasUiDescription = "blinn specular index of refraction texture";
	string UiCategory = "Specular";
	string ExistVar = "hasIORMap";
	int UiIndex = 10;
>;

float g_shininess
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Shininess";
	string SasUiDescription = "Blinn specular exponent controls size of highlight";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 11;
> = 0.5f;

float4 g_specular2
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Specular Color 2";
	string SasUiDescription = "color of glossy specular hilight";
	string UiCategory = "Specular 2";
	float SasUiMin = 0.0;
	float SasUiMax = 100.0;
	int UiIndex = 12;
> = {0.0f, 0.0f, 0.0f, 1.0f};

float g_shininess2
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Secondary Shininess";
	string SasUiDescription = "Blinn specular exponent controls size of highlight";
	string UiCategory = "Specular 2";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 13;
> = 0.5f;

float g_IOR2
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Specular Roll Off 2";
	string SasUiDescription = "blinn specular index of refraction";
	string UiCategory = "Specular 2";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 14;
> = 0.5;

bool hasIORMap2 = false;
Texture2D iorMap2
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Specular Roll Off Map 2";
	string SasUiDescription = "blinn specular index of refraction texture";
	string UiCategory = "Specular 2";
	string ExistVar = "hasIORMap2";
	int UiIndex = 15;
>;

float g_transparency : Opacity
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Opacity";
	string SasUiDescription = "Opacity value, 0 transparent, 1 for opaque";
	string UiCategory = "Opacity";
	int UiIndex = 16;
> = 1.0f;

bool hasTransparencyMap = false;
Texture2D transparencyMap	: OpacityTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Opacity Texture";
	string SasUiDescription = "opacity texture";
	string UiCategory = "Opacity";
	string ExistVar = "hasTransparencyMap";
	int UiIndex = 17;
>
;

float g_reflectivity : Reflectivity
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Environment Contribution";
	string SasUiDescription = "multiplier for reflection map";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 18;
> = 1.0f;

float fresnelPower
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Fresnel Power";
	string SasUiDescription = "fresnel exponent";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 5.0;
	int UiIndex = 19;
> = 4.0;

float fresnelBias
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Fresnel Bias";
	string SasUiDescription = "fresnel bias";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 20;
> = 0.2;

// mipmap LOD value scaling for cubemap
float reflScale
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Reflection Blur";
	string SasUiDescription = "reflection blurriness scale from fresnel";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 21;
> = 0;

float g_reflectionEnvAngle
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Map Angle";
	string SasUiDescription = "Rotation of reflection around Y";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 360.0;
	int UiIndex = 22;
> = 0;

bool hasCubeReflMap = false;
Texture2D CubeReflMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Reflection Map";
	string SasUiDescription = "Cubic Reflection Map";
	string UiCategory = "Reflection";
	string ExistVar = "hasCubeReflMap";
	int UiIndex = 23;
>;

bool hasReflectFactorMap = false;
Texture2D reflectFactorMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Reflection Mask Map";
	string SasUiDescription = "reflection mask";
	string UiCategory = "Reflection";
	string ExistVar = "hasReflectFactorMap";
	int UiIndex = 24;
>
;

bool g_useDynamicCubeMap
<
	string SasUiControl = "CheckBox";
	string SasUiLabel = "Use AutoGen map";
	string SasUiDescription = "Enable the automaticaly generated reflection map instead of fixed.";
	string UiCategory = "Reflection";
	int UiIndex = 25;
> = false;

float4 flakeColor
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Color";
	string SasUiDescription = "color of the flakes.";
	string UiCategory = "Flakes";
	int UiIndex = 26;
> = {0.1f, 0.1f, 0.1f, 1.0f};

bool hasFlakeMap = false;
Texture2D FlakeMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Flake Map";
	string SasUiDescription = "speckle flake normal map";
	string UiCategory = "Flakes";
	string ExistVar = "hasFlakeMap";
	int UiIndex = 27;
>;

float flakeTile
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Tiles";
	string SasUiDescription = "Repeat amount for texture coordinates";
	string UiCategory = "Flakes";
	float SasUiMin = 0.0;
	float SasUiMax = 30.0;
	int UiIndex = 28;
> = 20;

float flakeTolerance
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Tolerance";
	string SasUiDescription = "Cutoff level for how flakey it looks.";
	string UiCategory = "Flakes";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 29;
> = 0;


float g_specularBias = 0;
float g_specularBias2 = 0;

Texture2D glowMask		: GlowMask;


/************* DATA STRUCTS **************/
/*********** support functions ******/
    
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

SamplerState AnisoWrapSampler
{
	FILTER = ANISOTROPIC;
    AddressU = WRAP;
    AddressV = WRAP;
};

SamplerState AnisoClampSampler
{
	FILTER = ANISOTROPIC;
    AddressU = WRAP;
    AddressV = CLAMP;
};


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

float3 PaintShaderSpecular(TANGENT_VERTEX_OUTPUT IN, float3 worldNormal, float3 lightDir, 
						   float3 worldEyeDir, IncidentLight light, float SpecPowerScale)
{
	float ior = Tex2DCombine(hasIORMap, iorMap, IN.V.TexCoord0, g_IOR-1).r + 1;
	float3 specular = BlinnSpecular(worldNormal, lightDir, -worldEyeDir, 
		              light.Cls, g_specular.rgb, SpecPowerScale, ior, g_specularBias);
	float ior2 = Tex2DCombine(hasIORMap2, iorMap2, IN.V.TexCoord0, g_IOR2-1).r + 1;
	float3 specular2 = BlinnSpecular(worldNormal, lightDir, -worldEyeDir, 
				       light.Cls, g_specular2.rgb, g_shininess2, ior2, g_specularBias2);
	return specular + specular2;
}

float3 doFlakes(TANGENT_VERTEX_OUTPUT IN, bool vFace, float3 worldEyeDir, float3 lightDir)
{
	float3 flakeNormal = Tex2DNormal(hasFlakeMap, FlakeMap, IN.V.TexCoord0 * flakeTile );
	float3 worldFlakeNormal = normalize(float3(dot(flakeNormal,IN.V.WorldTan.X),
		dot(flakeNormal,IN.V.WorldTan.Y),
		dot(flakeNormal,IN.V.WorldTan.Z)));
	if (g_bDoubleSided && vFace )
	{
		worldFlakeNormal = -worldFlakeNormal;
	}

	float3 sparkRefl = reflect( worldEyeDir, worldFlakeNormal );
	float val = dot( sparkRefl, lightDir );
	return (flakeColor.rgb * (val > flakeTolerance ? val : 0));	
}

pixelOutput PaintShaderPS( TANGENT_VERTEX_OUTPUT IN,
						   uniform IncidentLight light,
						   uniform float SpecPowerScale,
		  				   uniform float reflectivity,
						   bool UseReflection, 
						   uniform bool i_bDefaultPass, 
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

	OUT.col.rgb = PaintShaderSpecular( IN, worldNormal, lightDir, 
									   worldEyeDir, light, SpecPowerScale);

  //calculate car paint color
	float ndv = dot(-worldEyeDir,worldNormal);
	float Cfresnel = saturate(fastFresnel( ndv, fresnelClrBias, fresnelClrPower ));
	float3 Paint = lerp( g_diffuse.rgb, g_edgeColor.rgb, Cfresnel );

	float3 diffuse = PhongDiffuse(worldNormal, lightDir, light.Cld, Paint);
	OUT.col.rgb += diffuse;

	//calculate Speckle Color
	//fetch flake normal
	if( hasFlakeMap )
	{
		OUT.col.rgb += doFlakes(IN,vFace,worldEyeDir,lightDir);
	}

	if (i_bDefaultPass)
	{
		float3 emissive = Tex2DCombine(hasEmissiveMap, emissiveMap, IN.V.TexCoord0, g_emissive * g_emissiveIntensity).rgb;
		OUT.col.rgb += emissive + envmap_approximation(g_diffuseFactor).rgb;	
	}

	OUT.col.rgb *= OUT.col.a;	//premul alpha

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

	return PaintShaderPS( IN, light, SpecPowerScale, reflectivity, false, i_bDefaultPass, vFace );
}

pixelOutput singleLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
							   uniform LightInfo i_Light,
								uniform float SpecPowerScale,
		  						uniform float reflectivity,
							   uniform bool i_bDefaultPass, 
								bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, i_Light, SpecPowerScale, reflectivity, i_bDefaultPass, vFace );
}

pixelOutput singleLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
								  uniform LightInfo i_Light,
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

	return PaintShaderPS( IN, light, SpecPowerScale, reflectivity, false, false, vFace );
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

		//fetch base and specular colors
		float4 sSpecColor = g_specular;
		float sSpecPower = SpecPowerScale;	        

		//fetch bump normal
		float3 worldNormal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );
	    
		OUT.col.rgb = PaintShaderSpecular( IN, worldNormal, lightDir, 
										   worldEyeDir, light, SpecPowerScale) * mask;
		
		if( hasFlakeMap )
		{
			OUT.col.rgb += doFlakes(IN,vFace,worldEyeDir,lightDir);
		}
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
	if( g_useDynamicCubeMap )
	{
//		if( g_hasCubeMap ) refl = texCUBEbias( dynCubeMap, float4(reflVect, reflScale*fresnel)) * g_reflectivity;
		if( g_hasCubeMap && g_bCubeMapEnabled ) refl = g_cubeMap.SampleBias( g_cubeSampler, reflVect, reflScale*fresnel ) * g_reflectivity;
	}
	else
	{
		if( hasCubeReflMap && g_bCubeMapEnabled )
		{			
			float3 rotReflVect = rotateAboutY(reflVect, g_reflectionEnvAngle*PI_DIV_180);
//			refl = tex2Dlod( cubeMap, float4( CartesianToPolar( rotReflVect ), 0, reflScale*fresnel) ) * g_reflectivity;
			refl = CubeReflMap.SampleLevel( AnisoClampSampler, CartesianToPolar( rotReflVect ), reflScale*fresnel ) * g_reflectivity;
		}
	}

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

technique11 Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 singleLightPS_##PassName(g_lightInfo, \
					g_shininess, g_reflectivity, true);\
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
		PixelShader = compile ps_5_0 singleLightPS_##PassName(g_lightInfo, \
					g_shininess, g_reflectivity, false);\
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(g_lightInfo, g_projLight, \
					g_shininess, g_reflectivity,\
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(g_lightInfo, g_projLight, \
					g_shininess, g_reflectivity,\
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(g_lightInfo, g_projLight, \
					g_shininess, g_reflectivity,\
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
		PixelShader = compile ps_5_0 projLightPS_##PassName(g_lightInfo, g_projLight, \
					g_shininess, g_reflectivity,\
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
		PixelShader = compile ps_5_0 glowPS_##PassName(g_lightInfo, g_projLight, g_shininess,\
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
