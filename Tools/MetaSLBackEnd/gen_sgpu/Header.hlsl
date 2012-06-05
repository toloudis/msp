
/*****************************************************************************
/* HEADER 
/****************************************************************************/

/*********** MetaSL structures ******/

//
// The state structure is used internally within the fragment shader to
// commonly used values.
//
struct State
{
	float4 texture_coordinate[4];
	float3 position;
	float3 origin;
	float3 geometry_normal;
	float3 normal;
	float dot_nd;
	float3 direction;
	float ray_length;
	float3x3 tangent_space[1];
	float3 texture_tangent[1];
	float3 texture_binormal[1];
};

//
// The light iterator structure holds the return values resulting from
// evaluating a light.
//
struct Light_iterator 
{
    float3 msl_point;
    float4 contribution;
    float4 raw_contribution;
    float  dot_nl;
    float3 direction;
    float  distance;
    float4 shadow;
    int    count;
};

/*********** Support functions to help conversion ******/

float4 msl_tex2D(texture2D msl_texture, float2 texture_uv)
{
	return msl_texture.Sample(g_DefaultSampler,texture_uv);
}
float4 msl_tex1D(texture1D msl_texture, float texture_u)
{
	return msl_texture.Sample(g_DefaultSampler,texture_u);
}
float4 msl_tex3D(texture3D msl_texture, float3 texture_uvw)
{
	return msl_texture.Sample(g_DefaultSampler,texture_uvw);
}

float4 __color_ctor(float4 v)
{
	return v;
}
float4 __color_ctor(float v)
{
	return v.xxxx;
}

/*********** Shader Constants ******/

float4 g_emissive : MaterialEmissive
<
	string SasUiControl = "ColorPicker";
	string SasUiLabel = "Emissive Color";
	string SasUiDescription = "emissive color";
	string UiCategory = "Diffuse";
	int UiIndex = 4;
>
= {0.0f, 0.0f, 0.0f, 1.0f};

float g_emissiveIntensity 
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Intensity";
	string SasUiDescription = "Emissive Intensity";
	string UiCategory = "Diffuse";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 5;
> = 1.0f;

float g_transparency : Opacity
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Opacity";
	string SasUiDescription = "opacity value, 0 transparent, 1 for opaque";
	string UiCategory = "Opacity";
	int UiIndex = 13;
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
	int UiIndex = 14;
>
;

float g_reflectivity : Reflectivity
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Environment Contribution";
	string SasUiDescription = "multiplier for reflection map";
	string UiCategory = "Reflection";
	int UiIndex = 15;
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
	int UiIndex = 16;
> = 4.0;

float fresnelBias
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Fresnel Bias";
	string SasUiDescription = "fresnel bias";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 1.0;
	int UiIndex = 17;
> = 0.2;

float reflBlur
<
	string SasUiControl = "Slider";
	string SasUiLabel = "Reflection Blur";
	string SasUiDescription = "reflection blur";
	string UiCategory = "Reflection";
	float SasUiMin = 0.0;
	float SasUiMax = 10.0;
	int UiIndex = 18;
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
	int UiIndex = 19;
> = 0;

bool hasCubeMap = false;
Texture2D cubeMap
<
	string SasUiControl = "FilePicker";
	string SasUiLabel = "Reflection Map";
	string SasUiDescription = "reflection map";
	string UiCategory = "Reflection";
	string ExistVar = "hasCubeMap";
	int UiIndex = 20;
>
;

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

Texture2D glowMask		: GlowMask;

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

/********* MetaSL Generated Code ********/
