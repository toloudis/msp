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
int gp : SasGlobal
<
  int3 SasVersion = {1,0,0};
  string SasEffectAuthor			= "Daniel Toloudis";
  string SasEffectAuthoringSoftware = "MachStudio";
  string SasEffectCategory			= "/material";
  string SasEffectCompany			= "studio|gpu";
  string SasEffectDescription		= "SubSurfaceScatter";
  string SasEffectHelp				= "This is a skin-like shader with subsurface scattering.";    
  string SasEffectRevision			= "1";  
>;

#include "Support.h"
#include "Tessellate.h"
#include "Skinning.h"
#include "Lighting.h"


bool hasDiffTex = false;
texture2D diffTex : DiffuseTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Diffuse Map";
	string SasUiDescription = "diffuse skin map";
	string UiCategory = "Translucency";
	string ExistVar = "hasDiffTex";
	int UiIndex = 1;
>;

float4 g_transColIn : MaterialDiffuse
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Unscattered";
	string SasUiDescription = "unscattered color";
	string UiCategory = "Translucency";
	int UiIndex = 2;
>  = {0.87f, 0.91f, 0.96f, 1.0f};

float4 g_transColOut
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Melanin";
	string SasUiDescription = "melanin color";
	string UiCategory = "Translucency";
	int UiIndex = 3;
>  = {1.0f, 0.71f, 0.32f, 1.0f};

float4 g_transColBack
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Hemoglobin";
	string SasUiDescription = "hemoglobin color";
	string UiCategory = "Translucency";
	int UiIndex = 4;
>  = {0.58f, 0.2f, 0.24f, 1.0f};


float g_transMultiplier
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Translucent Power";
	string SasUiDescription = "translucent power";
	string UiCategory = "Translucency";
	float SasUiMin = 0.0;
	float SasUiMax = 20.0;
	float SasUiSteps = 2000.0;
	int UiIndex = 5;
> 	= 1.0;

bool hasTransTex = false;
texture2D transTex
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Translucency Map";
	string SasUiDescription = "Translucency Map";
	string UiCategory = "Translucency";
	string ExistVar = "hasTransTex";
	int UiIndex = 6;
>;

float g_transRampOff
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Translucent Ramp Off";
	string SasUiDescription = "translucent ramp off";
	string UiCategory = "Translucency";
	float SasUiMin = -5.0;
	float SasUiMax = 5.0;
	float SasUiSteps = 1000.0;
	int UiIndex = 7;
> 	= 1.5;

float4 g_specColor
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Specular Color";
	string SasUiDescription = "color of specular highlight";
	string UiCategory = "Specular";
	int UiIndex = 8;
>
= {0.45f, 0.65f, 1.0f, 1.0f};

float g_specPower
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Specular Factor";
	string SasUiDescription = "multiplier for specular contribution";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 9;
>
= 0.2;

// Specular Map alpha is used to modulate glossiness.
float g_specGloss
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Specular Exponent";
	string SasUiDescription = "power for specular contribution";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 10;
>
= 15.0;

float g_specFresnel
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Fresnel";
	string SasUiDescription = "fresnel strength";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 100.0;
	float SasUiSteps = 10000.0;
	int UiIndex = 11;
> 	= 3.0;

float g_fresnelPower
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Fresnel Power";
	string SasUiDescription = "fresnel power";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 100.0;
	float SasUiSteps = 10000.0;
	int UiIndex = 12;
> 	= 15.0;

float g_fresnelGloss
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Fresnel Gloss";
	string SasUiDescription = "fresnel gloss";
	string UiCategory = "Specular";
	float SasUiMin = 0.0;
	float SasUiMax = 100.0;
	float SasUiSteps = 10000.0;
	int UiIndex = 13;
> 	= 0.0;

// Specular Map alpha is used to modulate glossiness.
bool hasSpecTex = false;
texture2D specTex
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Specular Color Map";
	string SasUiDescription = "Specular Color Map (gloss in alpha)";
	string UiCategory = "Specular";
	string ExistVar = "hasSpecTex";
	int UiIndex = 14;
>;

bool hasSpecPowerTex = false;
texture2D specPowerTex
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Specular Power Map";
	string SasUiDescription = "Specular Power Map";
	string UiCategory = "Specular";
	string ExistVar = "hasSpecPowerTex";
	int UiIndex = 15;
>;

bool hasNormalTex = false;
texture2D normalTex
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Normal Map";
	string SasUiDescription = "normal map";
	string UiCategory = "Normal Map";
	string ExistVar = "hasNormalTex";
	int UiIndex = 16;
>;

float g_bumpMapScale : BumpMapScale
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Bump Scale";
	string SasUiDescription = "scaling of normal map extrusion";
	string UiCategory = "Normal Map";
	float SasUiMin = 0.0;
	float SasUiMax = 20.0;
	int UiIndex = 17;
>
= 1.0f;

bool hasMicroTex = false;
texture2D microTex
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Micro Normal Map";
	string SasUiDescription = "micro structure normal map";
	string UiCategory = "Normal Map";
	string ExistVar = "hasMicroTex";
	int UiIndex = 18;
>;

float g_microScale
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Micro Normal Scale";
	string SasUiDescription = "micro structure uv scale";
	string UiCategory = "Normal Map";
	float SasUiMin = -1000.0;
	float SasUiMax = 1000.0;
	float SasUiSteps = 2000.0;
	int UiIndex = 19;
> 	= 50;

float g_transparency : Opacity
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Opacity";
	string SasUiDescription = "Opacity value, 0 transparent, 1 for opaque";
	string UiCategory = "Opacity";
	int UiIndex = 20;
>
= 1.0f;

bool hasTransparencyTex = false;
texture2D transparencyTex : OpacityTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Opacity Map";
	string SasUiDescription = "surface opacity values";
	string UiCategory = "Opacity";
	string ExistVar = "hasTransparencyTex";
	int UiIndex = 21;
>;

float g_reflectivity : Reflectivity
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Environment Contribution";
	string SasUiDescription = "multiplier for reflection map";
	string UiCategory = "Reflection";
	int UiIndex = 22;
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
	int UiIndex = 23;
> = 4.0;

float fresnelBias
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Fresnel Bias";
	string SasUiDescription = "fresnel bias";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 24;
> = 0.2;

float reflBlur
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Reflection Blur";
	string SasUiDescription = "reflection blur";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 25;
>
= 0.0;

float g_reflMapAngle 
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Map Angle";
	string SasUiDescription = "Rotation of reflection around Y";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 360.0;
	int UiIndex = 26;
> = 0;


bool hasCubeMap = false;
texture2D cubeTex
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Reflection Map";
	string SasUiDescription = "static cube environment map";
	string UiCategory = "Reflection";
	string ExistVar = "hasCubeMap";
	int UiIndex = 27;
>;

bool hasReflectFactorMap = false;
texture2D reflectFactorTex
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Reflection Factor Map";
	string SasUiDescription = "reflection mask map";
	string UiCategory = "Reflection";
	string ExistVar = "hasReflectFactorMap";
	int UiIndex = 28;
>;


texture2D projLightMap	: ProjLightTexture;
texture2D projShadowMap	: ProjShadowMap;
texture2D glowMask		: GlowMask;


//============================Texture samplers==============================
sampler2D DiffSampler = sampler_state
{
	Texture	=	<diffTex>;
	MinFilter	=	Linear;
	MagFilter	=	Linear;
	MipFilter	=	Linear;
	AddressU	=	WRAP;
	AddressV	=	WRAP;
};


sampler2D SpecSampler = sampler_state
{
	Texture	=	<specTex>;
	MinFilter	=	Linear;
	MagFilter	=	Linear;
	MipFilter	=	Linear;
	AddressU	=	WRAP;
	AddressV	=	WRAP;
};

sampler2D SpecPowerSampler = sampler_state
{
	Texture	=	<specPowerTex>;
	MinFilter	=	Linear;
	MagFilter	=	Linear;
	MipFilter	=	Linear;
	AddressU	=	WRAP;
	AddressV	=	WRAP;
};

sampler2D TransSampler = sampler_state
{
	Texture	=	<transTex>;
	MinFilter	=	Linear;
	MagFilter	=	Linear;
	MipFilter	=	Linear;
	AddressU	=	WRAP;
	AddressV	=	WRAP;
};

sampler2D NormalSampler = sampler_state
{
	Texture	=	<normalTex>;
	MinFilter	=	Linear;
	MagFilter	=	Linear;
	MipFilter	=	Linear;
	AddressU	=	WRAP;
	AddressV	=	WRAP;
};

sampler2D MicroSampler = sampler_state
{
	Texture	=	<microTex>;	
	MinFilter	=	Linear;
	MagFilter	=	Linear;
	MipFilter	=	Linear;
	AddressU	=	WRAP;
	AddressV	=	WRAP;

};
sampler2D CubeMapSampler = sampler_state
{
	Texture	=	<cubeTex>;	
	MinFilter	=	Linear;
	MagFilter	=	Linear;
	MipFilter	=	Linear;
    AddressU = Wrap;
    AddressV = Clamp;
    AddressW = Clamp;
};
sampler2D ReflectFactorSampler = sampler_state
{
	Texture	=	<reflectFactorTex>;	
	MinFilter	=	Linear;
	MagFilter	=	Linear;
	MipFilter	=	Linear;
	AddressU	=	WRAP;
	AddressV	=	WRAP;

};

sampler projSampler = sampler_state
{
    Texture   = (projLightMap);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = Border;
    AddressV = Border;
    AddressW = Border;
};

sampler shadowMapSampler = sampler_state
{
    Texture   = (projShadowMap);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = Border;
    AddressV = Border;
    AddressW = Border;
};

sampler2D TransparencySampler = sampler_state
{
	Texture	=	(transparencyTex);
	MinFilter	=	Linear;
	MagFilter	=	Linear;
	MipFilter	=	Linear;
	AddressU	=	WRAP;
	AddressV	=	WRAP;
};



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
	Skin(IN, Vtx);
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
    OUT.V.TexCoord0 = mul(g_uvTransform, IN.UV);
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
	Skin(IN, Vtx);
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
	float4 transSample = Tex2DReplace(hasTransTex, TransSampler, float4(TexUV,0,0), 0);
	
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
    float4 a = Tex2DReplace(hasDiffTex, DiffSampler, float4(TexUV,0,0), 1);
//	float4 a 			= tex2D(DiffSampler,TexUV);
	float4 b 			= (Translucency);
//	float4 BaseLighting	= ((1 - a) * (a*b) + a * (1 - (1 - a) * (1 - b))) * b;
	float4 BaseLighting = (a * b)*(a + b + b - 2*a*b);

	float fresnelMask = FresnelMask(WN,EV);
	float FresnelStrength	= saturate(1 - fresnelMask) * g_fresnelPower + 1;
	float FresnelGlossiness	= fresnelMask * g_fresnelGloss + 1;
	float4 specTexSample = Tex2DReplace(hasSpecTex, SpecSampler, float4(TexUV,0,0), float4(1,1,1,1));
	float specPowerTexSample = Tex2DReplace(hasSpecPowerTex, SpecPowerSampler, float4(TexUV,0,0), 1).x;
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
		  uniform sampler2D DiffuseMap,
		  uniform sampler2D NormalMap,
		  uniform sampler2D SpecularMap,
		  uniform sampler2D CubeMap,
		  uniform sampler2D TransparencyMap, bool vFace : SV_ISFRONTFACE) 
{
	pixelOutput OUT; 

	float4 refl = float4(0,0,0,0);
	if (hasCubeMap)
	{
		//fetch bump normal
		float3 bumpNormal = Tex2DNormal(hasNormalTex, NormalMap, IN.TexCoord0);
		if (hasMicroTex)
			bumpNormal += expand(tex2D(MicroSampler,IN.TexCoord0*g_microScale));
	    	
		float3 worldNormal;
		worldNormal.x = dot(bumpNormal, IN.WorldTanMatrixX);
		worldNormal.y = dot(bumpNormal, IN.WorldTanMatrixY);
		worldNormal.z = dot(bumpNormal, IN.WorldTanMatrixZ);
		if (g_bDoubleSided && vFace > 0)
			worldNormal = -worldNormal;
		refl = SampleEnvironment((IN.WorldEyeDir), worldNormal,CubeMap);
		refl = Tex2DCombine(hasReflectFactorMap, ReflectFactorSampler, IN.TexCoord0, refl);
		refl *= g_reflectivity;
	}

    float4 col = Tex2DCombine(hasDiffTex, DiffSampler, IN.TexCoord0, IN.diffCol);
    float4 spec = Tex2DCombine(hasSpecTex, SpecSampler, IN.TexCoord0, IN.specCol);
		
    OUT.col.rgb = col.rgb + refl.rgb + spec;
    
	OUT.col.a = Tex2DCombine(hasTransparencyTex, TransparencyMap, IN.TexCoord0, g_transparency);

#ifdef PRE_MULT_ALPHA
	OUT.col.rgb *= OUT.col.a;
#endif

    return OUT;
}

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN,
					uniform sampler2D DiffuseMap,
					uniform sampler2D NormalMap,
					uniform sampler2D SpecularMap,
		  			uniform sampler2D CubeMap,
					uniform LightInfo i_Light,
					uniform sampler2D SpecPowerMap,
					uniform sampler2D TransparencyMap, 
					uniform bool i_bDefaultPass, 
					bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 
    
	IncidentLight light;
	IlluminatePointLight(IN.V.WorldPos, i_Light, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);

	//fetch bump normal
	float3 worldNormal = Get2BumpNormal( IN.V, hasNormalTex, NormalMap, g_bumpMapScale, hasMicroTex, MicroSampler, g_microScale, vFace );

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
		uniform sampler2D DiffuseMap,
		uniform sampler2D NormalMap,
		uniform sampler2D SpecularMap,
		uniform sampler2D CubeMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform sampler2D SpecPowerMap,
		uniform sampler2D ProjTextureMap,
		uniform sampler2D ProjShadowMap,
		uniform sampler2D TransparencyMap,
		uniform int nBlockerSamples, uniform int nShadowSamples, bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 
    
	IncidentLight light;
	IlluminateProjLight(IN.V.WorldPos, i_Light, i_ProjLight, ProjTextureMap, ProjShadowMap, 
		nBlockerSamples, nShadowSamples, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
    
	//fetch bump normal
	float3 worldNormal = Get2BumpNormal( IN.V, hasNormalTex, NormalMap, g_bumpMapScale, hasMicroTex, MicroSampler, g_microScale, vFace );

	OUT.col = SkinShader(IN.V.TexCoord0.xy, normalize(worldNormal),
		-worldEyeDir, lightDir, light.Cld, light.Cls, 1);

	OUT.col.a = Tex2DCombine(hasTransparencyTex, TransparencyMap, IN.V.TexCoord0, g_transparency);

#ifdef PRE_MULT_ALPHA
	OUT.col.rgb *= OUT.col.a;
#endif

    return OUT;
}

sampler2D glowSampler = sampler_state
{
	Texture = <glowMask>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
    AddressU = WRAP;
    AddressV = WRAP;
};
pixelOutput glowPS(TANGENT_VERTEX_OUTPUT IN,
		uniform sampler2D NormalMap,
		uniform sampler2D SpecularMap,
		uniform sampler2D SpecPowerMap,
		uniform LightInfo i_Light,
		uniform ProjLightInfo i_ProjLight,
		uniform sampler2D ProjTextureMap,
		uniform sampler2D ProjShadowMap, bool vFace : SV_ISFRONTFACE
) {
    pixelOutput OUT; 
    OUT.col = float4(0,0,0,0);
    
    float4 mask = g_bHasMask ? tex2D(glowSampler, IN.V.TexCoord0) : float4(1,1,1,1);
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
		float3 worldNormal = Get2BumpNormal( IN.V, hasNormalTex, NormalMap, g_bumpMapScale, hasMicroTex, MicroSampler, g_microScale, vFace );

		// zero out the diffuse factors here.
		OUT.col = SkinShader(IN.V.TexCoord0.xy, normalize(worldNormal),
			-worldEyeDir, lightDir, float3(0,0,0), light.Cls, 1);
		OUT.col.a = 1;
		OUT.col *= mask;
	}
	OUT.col.a = 1;
    return OUT;
}
texture2D diffuseEnvMap;
texture2D specularEnvMap;
sampler2D diffuseEnvSampler = sampler_state
{
    Texture   = <diffuseEnvMap>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
	AddressU = WRAP;  // Allow 3D hardware to address cubemap's correctly
	AddressV = CLAMP;  // Allow 3D hardware to address cubemap's correctly
};
sampler2D specularEnvSampler = sampler_state
{
    Texture   = <specularEnvMap>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
	AddressU = WRAP;  // Allow 3D hardware to address cubemap's correctly
	AddressV = CLAMP;  // Allow 3D hardware to address cubemap's correctly
};

pixelOutput iblPS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE) 
{
    pixelOutput OUT; 
    
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);

	float3 worldNormal = GetBumpNormal( IN.V, hasNormalTex, NormalSampler, g_bumpMapScale, vFace );

	float4 refl = 0;
	if (hasCubeMap)
	{    		
		refl = SampleEnvironmentLOD((-worldEyeDir), worldNormal, CubeMapSampler , g_reflMapAngle*PI_DIV_180, reflBlur);
		refl *= g_reflectivity;
		refl *= fastFresnel(dot(-worldEyeDir, worldNormal), fresnelBias, fresnelPower);
		refl = Tex2DCombine(hasReflectFactorMap, ReflectFactorSampler, IN.V.TexCoord0, refl);
	}

	float4 spec = g_reflectivity*g_envSpecularColor*g_specularFactor;
	if (g_bHasSpecularEnvMap) 
	{
		spec *= SampleEnvironment((-worldEyeDir), worldNormal, 
				specularEnvSampler, g_specularEnvAngle);
	}
		
	float4 diff = Tex2DCombine(hasDiffTex, DiffSampler, IN.V.TexCoord0, g_transColIn*g_envDiffuseColor*g_diffuseFactor);
	if (g_bHasDiffuseEnvMap)
	{
		diff *= float4(SampleEnvDiffuse(worldNormal, 
				diffuseEnvSampler, g_diffuseEnvAngle).rgb,1);
	}
		
	OUT.col = float4(
		spec.rgb + refl.rgb + diff.rgb,
		g_transparency);		

	OUT.col.a = Tex2DCombine(hasTransparencyTex, TransparencySampler, IN.V.TexCoord0, g_transparency);

#ifdef PRE_MULT_ALPHA
	OUT.col.rgb *= OUT.col.a;
#endif

    return OUT;
}


//================================TECHNIQUES================================


technique11 Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 singleLightPS(DiffSampler, NormalSampler, SpecSampler, CubeMapSampler,\
					g_lightInfo, MicroSampler, TransparencySampler, true );\
	}
PASS_DEFAULT(Default)
PASS_DEFAULT(Tess)
//PASS_DEFAULT(Skin)
}

technique11 SingleLight
{
#define PASS_SINGLELIGHT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 singleLightPS(DiffSampler, NormalSampler, SpecSampler, CubeMapSampler,\
					g_lightInfo, MicroSampler, TransparencySampler, false );\
	}
PASS_SINGLELIGHT(Default)
PASS_SINGLELIGHT(Tess)
//PASS_SINGLELIGHT(Skin)
}

technique11 ProjectedLight
{
#define PASS_PROJECTEDLIGHT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 projLightPS(DiffSampler, NormalSampler, SpecSampler, CubeMapSampler,\
					g_lightInfo, g_projLight, MicroSampler,\
						projSampler, shadowMapSampler, TransparencySampler, BLOCKER_SAMPLES_LOW, SHADOW_SAMPLES_LOW );\
	}
PASS_PROJECTEDLIGHT(Default)
PASS_PROJECTEDLIGHT(Tess)
//PASS_PROJECTEDLIGHT(Skin)
}
technique11 ProjectedLightSuperSample
{
#define PASS_PROJECTEDLIGHTSS(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 projLightPS(DiffSampler, NormalSampler, SpecSampler, CubeMapSampler,\
					g_lightInfo, g_projLight, MicroSampler,\
						projSampler, shadowMapSampler, TransparencySampler, BLOCKER_SAMPLES_MED, SHADOW_SAMPLES_MED );\
	}
PASS_PROJECTEDLIGHTSS(Default)
PASS_PROJECTEDLIGHTSS(Tess)
//PASS_PROJECTEDLIGHTSS(Skin)
}
technique11 ProjectedLightSuperSample2
{
#define PASS_PROJECTEDLIGHTSS2(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 projLightPS(DiffSampler, NormalSampler, SpecSampler, CubeMapSampler,\
					g_lightInfo, g_projLight, MicroSampler,\
						projSampler, shadowMapSampler, TransparencySampler, BLOCKER_SAMPLES_HIGH, SHADOW_SAMPLES_HIGH );\
	}
PASS_PROJECTEDLIGHTSS2(Default)
PASS_PROJECTEDLIGHTSS2(Tess)
//PASS_PROJECTEDLIGHTSS2(Skin)
}
technique11 ProjectedLightSuperSample3
{
#define PASS_PROJECTEDLIGHTSS3(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 projLightPS(DiffSampler, NormalSampler, SpecSampler, CubeMapSampler,\
					g_lightInfo, g_projLight, MicroSampler,\
						projSampler, shadowMapSampler, TransparencySampler, BLOCKER_SAMPLES_VERY_HIGH, SHADOW_SAMPLES_VERY_HIGH );\
	}
PASS_PROJECTEDLIGHTSS3(Default)
PASS_PROJECTEDLIGHTSS3(Tess)
//PASS_PROJECTEDLIGHTSS3(Skin)
}

technique11 Glow
{
#define PASS_GLOW(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 glowPS(NormalSampler, SpecSampler, MicroSampler,\
					g_lightInfo, g_projLight, \
					projSampler, shadowMapSampler);\
	}
PASS_GLOW(Default)
PASS_GLOW(Tess)
//PASS_GLOW(Skin)
}
technique11 DOFPrep
{
#define PASS_DOFPREP(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 DOFPrep_VS_##PassName();\
		PixelShader = compile ps_5_0 DOFPrep_PS();\
	}
PASS_DOFPREP(Default)
PASS_DOFPREP(Tess)
//PASS_DOFPREP(Skin)
}
technique11 Matte
{
#define PASS_MATTE(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 simpleMattePS();\
	}
PASS_MATTE(Default)
PASS_MATTE(Tess)
//PASS_MATTE(Skin)
}
technique11 Environment
{
#define PASS_ENVIRONMENT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 singleLightVS_##PassName();\
		PixelShader = compile ps_5_0 iblPS();\
	}
PASS_ENVIRONMENT(Default)
PASS_ENVIRONMENT(Tess)
//PASS_ENVIRONMENT(Skin)
}
