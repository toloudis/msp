/*****************************************************************************
/* UI
/****************************************************************************/
float4 g_diffuse : MaterialDiffuse 
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Diffuse Color";
	string SasUiDescription = "diffuse color of surface";
	string UiCategory = "Diffuse";
	int UiIndex = 1;
> = {1.0f, 1.0f, 1.0f, 1.0f};
bool hasDiffuseMap = false;
texture2D diffuseMap : DiffuseTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Diffuse Map";
	string SasUiDescription = "diffuse color";
	string UiCategory = "Diffuse";
	string ExistVar = "hasDiffuseMap";
	int UiIndex = 2;
>;
float4 g_emissive : MaterialEmissive
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Emissive Color";
	string SasUiDescription = "emitted color";
	string UiCategory = "Diffuse";
	int UiIndex = 3;
> = {0.0f, 0.0f, 0.0f, 1.0f};

float4 g_specular : MaterialSpecular 
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Specular Color";
	string SasUiDescription = "color of specular hilight";
	string UiCategory = "Specular";
	int UiIndex = 4;
> = {1.0f, 1.0f, 1.0f, 1.0f};
bool hasSpecularMap = false;
texture2D specularMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Specular Color Map";
	string SasUiDescription = "specular color";
	string UiCategory = "Specular";
	string ExistVar = "hasSpecularMap";
	int UiIndex = 5;
>;
bool hasGlossMap = false;
texture2D glossMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Specular Power Map";
	string SasUiDescription = "specular exponent multiplier";
	string UiCategory = "Specular";
	string ExistVar = "hasGlossMap";
	int UiIndex = 6;
>;
float g_shininess : MaterialPower
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Shininess";
	string SasUiDescription = "specular exponent controls size of highlight";
	string UiCategory = "Specular";
	float SasUiMin = 15.0;
	float SasUiMax = 1000.0;
	int UiIndex = 7;
> = 20.0f;
bool hasNormalMap = false;
texture2D normalMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Normal Map";
	string SasUiDescription = "normal map";
	string UiCategory = "Normal Map";
	string ExistVar = "hasNormalMap";
	int UiIndex = 8;
>;
float g_bumpMapScale : BumpMapScale
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Bump Scale";
	string SasUiDescription = "scaling of normal map extrusion";
	string UiCategory = "Normal Map";
	float SasUiMin = 0.0;
	float SasUiMax = 20.0;
	int UiIndex = 9;
> = 1.0f;
float g_transparency : Opacity
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Opacity";
	string SasUiDescription = "Opacity value, 0 transparent, 1 for opaque";
	string UiCategory = "Opacity";
	int UiIndex = 10;
> = 1.0f; 

bool hasTransparencyMap = false;
texture2D transparencyMap		: OpacityTexture
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Opacity Texture";
	string SasUiDescription = "Opacity texture";
	string UiCategory = "Opacity";
	string ExistVar = "hasTransparencyMap";
	int UiIndex = 11;
>;
float g_reflectivity : Reflectivity
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Environment Contribution";
	string SasUiDescription = "multiplier for reflection map";
	string UiCategory = "Reflection";
	int UiIndex = 12;
> = 1.0f;
float fresnelPower
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Fresnel Power";
	string SasUiDescription = "fresnel exponent";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 5.0;
	int UiIndex = 13;
> = 4.0;
float fresnelBias
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Fresnel Bias";
	string SasUiDescription = "fresnel bias";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 14;
> = 0.2;
float reflBlur
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Reflection Blur";
	string SasUiDescription = "reflection blur";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 15;
> = 0.0;
float g_reflMapAngle 
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Map Angle";
	string SasUiDescription = "Rotation of reflection around Y";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 360.0;
	int UiIndex = 16;
> = 0;
bool hasCubeMap = false;
texture2D cubeMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Reflection Map";
	string SasUiDescription = "reflection map";
	string UiCategory = "Reflection";
	string ExistVar = "hasCubeMap";
	int UiIndex = 17;
>;
bool hasReflectFactorMap = false;
texture2D reflectFactorMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Reflection Mask Map";
	string SasUiDescription = "reflection mask";
	string UiCategory = "Reflection";
	string ExistVar = "hasReflectFactorMap";
	int UiIndex = 18;
>;

/*****************************************************************************
/* Samplers
/****************************************************************************/
sampler2D diffuseSampler = sampler_state
{
	Texture = <diffuseMap>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
    AddressU = WRAP;
    AddressV = WRAP;
};
sampler2D cubeSampler = sampler_state 
{
	Texture = <cubeMap>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Clamp;
    AddressW = Clamp;
};
sampler2D reflectFactorSampler = sampler_state 
{
	Texture = <reflectFactorMap>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
    AddressU = WRAP;
    AddressV = WRAP;
};
sampler2D specularSampler = sampler_state 
{
	Texture = <specularMap>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
    AddressU = WRAP;
    AddressV = WRAP;
};
sampler2D glossSampler = sampler_state 
{
	Texture = <glossMap>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
    AddressU = WRAP;
    AddressV = WRAP;
};
sampler2D transparencySampler = sampler_state 
{
	Texture = <transparencyMap>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
    AddressU = WRAP;
    AddressV = WRAP;
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
sampler2D glowSampler = sampler_state
{
	Texture = <glowMask>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};
sampler2D normalSampler = sampler_state 
{
	Texture = <normalMap>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
    AddressU = WRAP;
    AddressV = WRAP;
};

/*****************************************************************************
/* CustomShader()
/****************************************************************************/
float4 CustomShader(perPixelVertexOutput IN, IncidentLight light, float vFace)
{

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.WorldPos - g_eyePos.xyz);
    
	//fetch base and specular colors
	float4 sDiffColor = Tex2DCombine(hasDiffuseMap, diffuseSampler, IN.TexCoord0, g_diffuse);
	float4 sSpecColor = Tex2DCombine(hasSpecularMap, specularSampler, IN.TexCoord0, g_specular);
	float sSpecPower = Tex2DCombine(hasGlossMap, glossSampler, IN.TexCoord0, g_shininess).r;

	//fetch bump normal
	float3 bumpNormal = Tex2DNormal(hasNormalMap, normalSampler, IN.TexCoord0);
	float3 worldNormal = normalize(float3(dot(bumpNormal,IN.WorldTanMatrixX),
		dot(bumpNormal,IN.WorldTanMatrixY),
		dot(bumpNormal,IN.WorldTanMatrixZ)));
	if (g_bDoubleSided && vFace > 0)
	{
		worldNormal = -worldNormal;
	}

	float3 diffuse = PhongDiffuse(worldNormal, lightDir, light.Cld, sDiffColor);
	float3 specular = PhongSpecular(worldNormal, lightDir, -worldEyeDir, 
		light.Cls, sSpecColor, sSpecPower);

	return float4( 0, 1, 0, 1);
	
    return float4( (diffuse + specular) , Tex2DCombine(hasTransparencyMap, transparencySampler, IN.TexCoord0, g_transparency).x );

}
