/*****************************************************************************
**  Default.fx
**
**      Single directional light with shadows off (no single light passes).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\\****************************************************************************/

#include "Globals.h"

/*********** support data and functions ******/
SamplerState g_DefaultSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

// transforms/viewing
float4x4 g_worldIT : WorldIT;
float4x4 g_wvp : WorldViewProjection;
float4x4 g_wv : WorldView;
float4x4 g_world : World;
float4x4 g_viewIT : ViewIT;
float4x4 g_vp : ViewProjection;
float4 g_eyePos : CameraPos;
// xres, yres, 1/xres, 1/yres
float4 g_targetRes; 

float g_AlphaTestRef = 0.0f;

// Output pixel values
struct pixelOutput {
  float4 col : SV_Target;
};

// material properties
float g_shininess : MaterialPower = 1.0f;
float4 g_ambient : MaterialAmbient = {0.5f, 0.5f, 0.5f, 1.0f};
float4 g_diffuse : MaterialDiffuse = {1.0f, 1.0f, 1.0f, 1.0f};
float4 g_specular : MaterialSpecular = {1.0f, 1.0f, 1.0f, 1.0f};
float g_uScale : UScale = 1.0f;
float g_vScale : VScale = 1.0f;
bool g_bDoubleSided : DoubleSided = false;

struct LightInfo
{
	float4 Pos;
	float4 Diffuse;
	float4 Specular;
	float3 Falloff;
	float4 ConeInfo;	/* x,y,z are normalized direction, w is cos(ConeAngle) */
};
LightInfo g_lightArray[8] : LightArray;
LightInfo g_lightInfo : LightInfo;

// textures
bool hasDiffuseMap = false;
Texture2D diffuseMap;

/************* DATA STRUCTS **************/

/* data from application vertex buffer */
#include "Tessellate.h"

/*********** support functions ******/

/*********** vertex shader ******/

TANGENT_VERTEX TangentVS( STANDARD_VERTEX In )
{
    TANGENT_VERTEX OUT;

	// decal and bump texture coords
	OUT.UV = In.UV;
    OUT.TexCoord0 = In.UV * float2(g_uScale, g_vScale);
    
	float4 Po = float4(In.Position, 1.0f);
    
    // transform position to world space and get vector to light
    float3 Pw = mul(g_world, Po).xyz;
    OUT.WorldPos = Pw;

	OUT.WorldTan.X = mul( (float3x3)g_world, In.T);
	OUT.WorldTan.Y = mul( (float3x3)g_world, In.B);
	OUT.WorldTan.Z = mul( (float3x3)g_world, In.Normal);
	
    return OUT;
}

TANGENT_VERTEX_OUTPUT VS_Default( STANDARD_VERTEX Vtx, out float ClipDist : SV_ClipDistance0 ) 
{
	TANGENT_VERTEX_OUTPUT OUT;
	OUT.V = TangentVS( Vtx );

	float4 Po = float4(Vtx.Position, 1.0f);
    OUT.HPosition = mul( g_wvp, Po);
	OUT.ScreenPos = float3(0,0,0);//not used

	ClipDist = ClipWorldPos( OUT.V.WorldPos );

	return OUT;
}

TANGENT_TESS_OUTPUT VS_Tess( STANDARD_VERTEX Vtx ) 
{
	TANGENT_TESS_OUTPUT OUT;
	OUT.V = TangentVS( Vtx );

	return OUT;
}

/********* pixel shader ********/

struct IncidentLight
{
	// direction, unnormalized
	float3 L;
	// color
	float3 Cld;
	float3 Cls;
};

void IlluminateDirectionalLight(in float3 Ps, in LightInfo light, out IncidentLight OUT)
{
	// assume light.pos.w = 0 and the light.pos.xyz is a directional vector.
	OUT.L = light.Pos.xyz;
	OUT.Cld = light.Diffuse.rgb;
	OUT.Cls = light.Specular.rgb;
}

TANGENT_VERTEX_OUTPUT VS_DefaultTEST(STANDARD_VERTEX Vtx, out float ClipDist : SV_ClipDistance0) 
{
    TANGENT_VERTEX_OUTPUT OUT;

	float4 Po = float4(Vtx.Position, 1.0f);
    OUT.HPosition = mul(g_wvp, Po);
	OUT.V.UV = Vtx.UV;
    OUT.V.TexCoord0 = Vtx.UV * float2(g_uScale, g_vScale);
    float3 Pw = mul(g_world, Po).xyz;
    OUT.V.WorldPos = Pw;
	OUT.V.WorldTan.X = mul((float3x3)g_worldIT, Vtx.T);
	OUT.V.WorldTan.Y = mul((float3x3)g_worldIT, Vtx.B);
	OUT.V.WorldTan.Z = mul((float3x3)g_worldIT, Vtx.Normal);
	OUT.ScreenPos = float3(0,0,0);//not used

	ClipDist = ClipWorldPos( Pw );

    return OUT;
}
pixelOutput defaultPSTEST(TANGENT_VERTEX_OUTPUT IN,
					bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 
    OUT.col = float4(1,1,1,1);
    return OUT;
}

pixelOutput defaultPS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 
    
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
	IncidentLight light;
	light.L = -worldEyeDir;
	light.Cld = 0.85;//light.Diffuse.rgb;
	light.Cls = 0.85;//light.Specular.rgb;
	
	float3 worldNormal = GetNormal( IN.V, vFace );

	//fetch base and specular colors
	float4 sDiffColor = g_diffuse;
	if (hasDiffuseMap)
		sDiffColor *= diffuseMap.Sample(g_DefaultSampler, IN.V.TexCoord0.xy);
	
	float3 diffuse = saturate(dot(worldNormal,light.L)) * light.Cld * sDiffColor.rgb;

    OUT.col.rgb = diffuse;

	//early alpha test
    OUT.col.a = sDiffColor.a;
	if( g_AlphaTestRef >= OUT.col.a ) discard;
    
    return OUT;
}

pixelOutput defaultPS_Tess( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return defaultPS( IN, vFace );
}

pixelOutput defaultPS_Default( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE )
{
	return defaultPS( IN, vFace );
}

pixelOutput ambientPS( TANGENT_VERTEX_OUTPUT IN )
{
    pixelOutput OUT; 
    
	//fetch base and specular colors
	OUT.col = g_ambient;
	if (hasDiffuseMap)
		OUT.col *= diffuseMap.Sample(g_DefaultSampler, IN.V.TexCoord0.xy);

	//early alpha test
	if( g_AlphaTestRef >= OUT.col.a ) discard;

    return OUT;
}

pixelOutput singleLightPS(TANGENT_VERTEX_OUTPUT IN,
					uniform LightInfo i_Light,
					uniform float SpecPowerScale, bool vFace : SV_ISFRONTFACE)
{
    pixelOutput OUT; 

   
	IncidentLight light;
	IlluminateDirectionalLight(IN.V.WorldPos, i_Light, light);

	float3 lightDir = normalize(light.L);
	float3 worldEyeDir = normalize(IN.V.WorldPos - g_eyePos.xyz);
	
	float3 worldNormal = GetNormal( IN.V, vFace );

	//fetch base and specular colors
	float4 sDiffColor = float4(1,1,1,1);
	if (hasDiffuseMap)
		sDiffColor = diffuseMap.Sample(g_DefaultSampler, IN.V.TexCoord0.xy);
	
	float4 sSpecColor = float4(1,1,1,1);
	float sSpecPower = SpecPowerScale;
    	
	float3 diffuse = saturate(dot(worldNormal,lightDir)) * light.Cld * sDiffColor.rgb;
//	float3 specular = pow (saturate(dot(worldNormal,normalize(lightDir - worldEyeDir))), 
//		max(0.001, sSpecPower)) * light.Cls;
	float3 specular = 0;

    OUT.col.rgb = diffuse + specular;

	//alpha test
    OUT.col.a = sDiffColor.a * i_Light.Diffuse.a;
	if( g_AlphaTestRef >= OUT.col.a ) discard;
    
    return OUT;
}

pixelOutput singleLightPS_Tess( TANGENT_VERTEX_OUTPUT IN,
								uniform LightInfo i_Light,
								uniform float SpecPowerScale, bool vFace : SV_ISFRONTFACE )
{
	TangentDisplace( IN.V );
	return singleLightPS( IN, i_Light, SpecPowerScale, vFace );
}

pixelOutput singleLightPS_Default( TANGENT_VERTEX_OUTPUT IN,
								uniform LightInfo i_Light,
								uniform float SpecPowerScale, bool vFace : SV_ISFRONTFACE )
{
	return singleLightPS( IN, i_Light, SpecPowerScale, vFace );
}

/*************/

technique11 Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		SetVertexShader(CompileShader(vs_5_0, VS_##PassName()));\
		SetPixelShader(CompileShader(ps_5_0, defaultPS()));\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_DEFAULT(Default)
PASS_DEFAULT(Tess)
}
technique11 Ambient
{
#define PASS_AMBIENT(PassName)	\
	pass P##PassName			\
	{						\
		SetVertexShader(CompileShader(vs_5_0, VS_##PassName()));\
		SetPixelShader(CompileShader(ps_5_0, ambientPS()));\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_AMBIENT(Default)
PASS_AMBIENT(Tess)
}
technique11 Environment
{
#define PASS_ENVIRONMENT(PassName)	\
	pass P##PassName			\
	{						\
		SetVertexShader(CompileShader(vs_5_0, VS_##PassName()));\
		SetPixelShader(CompileShader(ps_5_0, ambientPS()));\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_ENVIRONMENT(Default)
PASS_ENVIRONMENT(Tess)
}
technique11 SingleLight
{
// assumes light is directional!!!!!
#define PASS_SINGLELIGHT(PassName)	\
	pass P##PassName			\
	{						\
		SetVertexShader(CompileShader(vs_5_0, VS_##PassName()));\
		SetPixelShader(CompileShader(ps_5_0, singleLightPS(g_lightInfo, g_shininess)));\
		TANGENT_HULL_AND_DOMAIN_##PassName\
	}
PASS_SINGLELIGHT(Default)
PASS_SINGLELIGHT(Tess)
}
/***************************** eof ***/
