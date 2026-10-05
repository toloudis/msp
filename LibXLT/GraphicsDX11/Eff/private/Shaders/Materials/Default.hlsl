//////////////////////////////////////////////////////////////////////////////
// Converted from Default.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// Default.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Default.fx
**
**      Single directional light with shadows off (no single light passes).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\\****************************************************************************/

// The annotations/defaults of the material parameters are in
// Default.effect.json. Register layout shared by all materials: see
// Globals.hlsli. Default.fx included only Globals.h and Tessellate.h (not
// Support.h or Lighting.h), so this file does the same with the converted
// includes. Its transforms, g_targetRes, g_AlphaTestRef, g_bDoubleSided,
// g_lightInfo and g_lightArray are members of the shared cbuffers now
// (b0-b3); the private LightInfo struct Default.fx declared is replaced by
// the shared one in Globals.hlsli, which has the same member offsets for the
// members used here (Pos, Diffuse, Specular).

#include "Globals.hlsli"

/*********** support data and functions ******/
// Same name and register as the shared sampler in Support.hlsli (s0); its
// state (linear, wrap) is described in Default.effect.json.
SamplerState g_DefaultSampler : register(s0);

// Output pixel values
struct pixelOutput {
  float4 col : SV_Target;
};

// b4: this material's own parameters (defaults are in Default.effect.json)
cbuffer MaterialParams : register(b4)
{
	float4x4 g_viewIT;				// : ViewIT
	// material properties
	float g_shininess;				// : MaterialPower, default 1
	float4 g_ambient;				// : MaterialAmbient, default (0.5,0.5,0.5,1)
	float4 g_diffuse;				// : MaterialDiffuse, default (1,1,1,1)
	float4 g_specular;				// : MaterialSpecular, default (1,1,1,1)
	float g_uScale;					// : UScale, default 1
	float g_vScale;					// : VScale, default 1
	// textures
	bool hasDiffuseMap;				// default false
};

Texture2D diffuseMap : register(t4);

/************* DATA STRUCTS **************/

/* data from application vertex buffer */
#include "Tessellate.hlsli"

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


/***************************** eof ***/

//////////////////////////////////////////////////////////////////////////////
// Entry points generated from the technique/pass compile statements.
// Each one binds the uniform arguments the .fx passed in its compile call.
//////////////////////////////////////////////////////////////////////////////

pixelOutput SingleLight_PDefault_PS(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
    return singleLightPS(IN, g_lightInfo, g_shininess, vFace);
}
