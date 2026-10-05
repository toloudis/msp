//////////////////////////////////////////////////////////////////////////////
// Converted from ReflectiveShadowMap.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// ReflectiveShadowMap.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  ReflectiveShadowMap.fx
**
**      Generating reflective shadow map in one pass using geometry shader
**		Output texture array stores info in the order of: 
**			depth, world space position, normal, reflected radiant flux
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

/*********** support data and functions ******/

#include "../Materials/Support.hlsli"
#include "../Materials/Tessellate.hlsli"
#include "../Materials/Lighting.hlsli"


// Register layout shared with the materials: see ../Materials/Globals.hlsli.
// b4: this shader's own parameters (defaults are in ReflectiveShadowMap.effect.json)
cbuffer MaterialParams : register(b4)
{
	bool hasTransparencyMap;		// default false
	float alphaThreshold;			// default 0.1
	float g_transparency;			// default 1

	float4 g_emissiveColor;			// default (0,0,0,0)
	float g_emissiveIntensity;		// default 1
	float4 g_diffuseColor;			// default (0,0,0,0)
	bool hasDiffuseMap;				// default false

	// larger bias will make shadows MORE transparent
	bool g_bUseDither;				// default false
	float g_DitherAlphaBias;		// default 0.85
};

Texture2D transparencyMap : register(t4);
Texture2D g_diffuseMap : register(t5);

/************* DATA STRUCTS **************/


///* data passed from vertex shader to geometry shader */
//struct VertexOutput
//{
	//float4 HPosition	: SV_POSITION;
    //float2 TexCoord		: TEXCOORD0;
    //float4 posInfo		: TEXCOORD1;	// world space position info
    //float3 Normal		: TEXCOORD2;	// world space normal vector
    //float3 LightDir		: TEXCOORD3;	// object to light vec, used to calculate radiant flux
//};
//
///* data passed from vertex shader to hull and domain shader for tessellation */
//struct VertexOutputTess
//{
    //float2 TexCoord		: TEXCOORD0;
    //float4 posInfo		: TEXCOORD1;	// world space position info
	//float3 Normal		: TEXCOORD2;	// world space normal vector
	//float3 LightDir		: TEXCOORD3;	// object to light vec, used to calculate radiant flux
//};

struct RSMPixelOutput
{
	float4 DepthAndPos	: SV_Target0;
	float4 WNormal		: SV_Target1;
	float4 Flux			: SV_Target2;
};

/*********** vertex shader ******/

TANGENT_VERTEX_OUTPUT VS_Default( STANDARD_VERTEX In, out float ClipDist : SV_ClipDistance0 )
{
    //VertexOutput OUT;
    //
    //OUT.TexCoord = mul(g_uvTransform, float4(IN.UV,0,1)).xy;
    //
    //// Output vertex position
	//float4 Po = float4(IN.Position, 1.0f);
    //float4 Pp = mul(g_wvp, Po);
    //float4 Pw = mul( g_world, Po );
    //
    //float4 No = float4(IN.Normal, 0.0f);
    //float4 Nw = mul(g_world, No);
    //
    //// Camera-object vector
    //float3 V = normalize( g_eyePos.xyz - Pw.xyz);
    //if (g_bDoubleSided)	Nw.xyz = faceforward(Nw.xyz, -V, Nw.xyz);
	//Nw.xyz = normalize( Nw.xyz );
    //
    //OUT.HPosition = Pp;
    //OUT.posInfo = Pw;
    //OUT.Normal = Nw;
    //OUT.LightDir = V;
//
	//ClipDist = ClipWorldPos( Pw.xyz );
	//return OUT;
	
	TANGENT_VERTEX_OUTPUT Out;

	float4 Pos = float4(In.Position, 1.0f);
	Out.V.WorldPos = mul(g_world, Pos ).xyz;

	Out.V.UV = In.UV;
	Out.V.TexCoord0 = mul(g_uvTransform, float4(In.UV,0,1)).xy;
	Out.V.WorldTan = TransformTangents( g_world, In.T, In.B, In.Normal );

	Out.ScreenPos = mul( g_wv, Pos ).xyz;
    //Out.HPosition = TransformVertex( Pos, In.UV, g_wvp);
    Out.HPosition =  mul(g_wvp, Pos);

	ClipDist = ClipWorldPos( Out.V.WorldPos );

    return Out;
}

TANGENT_TESS_OUTPUT VS_Tess( STANDARD_VERTEX In )
{
    //VertexOutputTess OUT;
    //
    //OUT.TexCoord = mul(g_uvTransform, float4(IN.UV,0,1)).xy;
    //
    //// Output vertex position
	//float4 Po = float4(IN.Position, 1.0f);
    //float4 Pp = mul(g_wvp, Po);
    //float4 Pw = mul( g_world, Po );
    //
     //// Camera-object vector
    //float3 V = normalize( g_eyePos.xyz - Pw.xyz);
    //float3 worldNormal = mul( (float3x3)g_world, IN.Normal );
    //if (g_bDoubleSided)	worldNormal = faceforward(worldNormal, -V, worldNormal);
	//normalize( worldNormal );
    //
    //OUT.posInfo = Pw;
	//OUT.Normal = worldNormal;
	//OUT.LightDir = V;
    //
    //return OUT;
    
    TANGENT_TESS_OUTPUT Out;

	float4 Pos = float4(In.Position, 1.0f);

    Out.V.WorldPos = mul(g_world, Pos).xyz;

	Out.V.UV = In.UV;
	Out.V.TexCoord0 = mul(g_uvTransform, float4(In.UV,0,1)).xy;
	Out.V.WorldTan = TransformTangents( g_world, In.T, In.B, In.Normal );

    return Out;
}

/********* pixel shader ********/


RSMPixelOutput labPS( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE) 
{
    RSMPixelOutput OUT; 
    
	float alpha = g_transparency;
	if( hasTransparencyMap )
	{
		alpha *= transparencyMap.Sample( g_DefaultSampler, IN.V.TexCoord0.xy ).r;
	}

	float threshold = alphaThreshold;
	// dither shadowmap based on alpha value:
	if (g_bUseDither)
	{
		threshold += g_DitherAlphaBias;
	}
	
	// low alphas will have more pixels skipped.
	// clip skips pixel if value is negative
	clip(alpha-threshold); 

	// put light space projected depth in shadow map color buffer.
	//OUT.col = IN.outInfo;	
	
	// when TRUE pcss is working, prob will need to take heed of the W divide.
	float3 lightDir = normalize( g_eyePos.xyz - IN.V.WorldPos.xyz);
	//fetch bump normal
	float3 norm = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );
	
	OUT.DepthAndPos = float4(IN.V.WorldPos.xyz + norm * 0.0f, IN.HPosition.z);	
	OUT.WNormal = float4(norm, 1.0f);
	
	float3 diff = Tex2DCombine(hasDiffuseMap, g_diffuseMap, IN.V.TexCoord0, g_diffuseColor).rgb;
	float4 objreflectivity = g_emissiveColor * g_emissiveIntensity + float4(diff, 1.0f);
	//OUT.Flux = max(0, dot(lightDir, norm)) * objreflectivity / 3.1415926f;
	OUT.Flux = max(0, objreflectivity);

    return OUT;
}

RSMPixelOutput PS_Default(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
	return labPS(IN, vFace);
}

RSMPixelOutput PS_Tess(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE)
{
	TangentDisplace( IN.V );
	return labPS(IN, vFace);
}

////--------------------------------------------------------------------------------------
//// Hull shader
////--------------------------------------------------------------------------------------
//
//[domain("tri")]
//[partitioning("fractional_odd")]
//[outputtopology("triangle_cw")]
//[outputcontrolpoints(3)]
//[patchconstantfunc("Constants_HS")]
//[maxtessfactor(11.0)]
//VertexOutputTess Depth_HS( InputPatch<VertexOutputTess, 3> inputPatch, uint uCPID : SV_OutputControlPointID )
//{
	//VertexOutputTess    output = (VertexOutputTess)0;
//
	//// Copy inputs to outputs
	//output.TexCoord = inputPatch[uCPID].TexCoord;
    //output.posInfo = inputPatch[uCPID].posInfo;
    //output.Normal = inputPatch[uCPID].Normal;
    //output.LightDir = inputPatch[uCPID].LightDir;
//
	//return output;
//}
//
////--------------------------------------------------------------------------------------
//// Domain Shader
////--------------------------------------------------------------------------------------
//
//[domain("tri")]
//VertexOutput Depth_DS( HS_CONSTANT_DATA_OUTPUT input, float3 BarycentricCoordinates : SV_DomainLocation, 
						 //const OutputPatch<VertexOutputTess, 3> TrianglePatch, out float ClipDist : SV_ClipDistance0 )
//{
	//VertexOutput output = (VertexOutput)0;
//
	//// Interpolate world space position with barycentric coordinates
//
	//float3 vWorldPos = BarycentricCoordinates.x * TrianglePatch[0].posInfo.xyz + 
		//BarycentricCoordinates.y * TrianglePatch[1].posInfo.xyz + 
		//BarycentricCoordinates.z * TrianglePatch[2].posInfo.xyz;
//
	//float2 TexCoord = BarycentricCoordinates.x * TrianglePatch[0].TexCoord + 
		//BarycentricCoordinates.y * TrianglePatch[1].TexCoord + 
		//BarycentricCoordinates.z * TrianglePatch[2].TexCoord;
//
	//float4 vPosInfo  = BarycentricCoordinates.x * TrianglePatch[0].posInfo + 
		//BarycentricCoordinates.y * TrianglePatch[1].posInfo + 
		//BarycentricCoordinates.z * TrianglePatch[2].posInfo;
//
	//float3 vNormal  = BarycentricCoordinates.x * TrianglePatch[0].Normal + 
		//BarycentricCoordinates.y * TrianglePatch[1].Normal + 
		//BarycentricCoordinates.z * TrianglePatch[2].Normal;
	//
	//float3 vLightDir  = BarycentricCoordinates.x * TrianglePatch[0].LightDir + 
		//BarycentricCoordinates.y * TrianglePatch[1].LightDir + 
		//BarycentricCoordinates.z * TrianglePatch[2].LightDir;
//
	//output.TexCoord = TexCoord;
    //output.posInfo = vPosInfo;
	//output.Normal = vNormal;
	//output.LightDir = vLightDir;
//
	////need normal to displace
//#ifdef APPLY_DISPLACEMENT
	//vWorldPos = DisplaceVertex( vWorldPos, vNormal, TexCoord );
//#endif
//
	//// Transform world position with viewprojection matrix
	//output.HPosition = mul( g_vp, float4( vWorldPos, 1.0 ) );
//
	//ClipDist = ClipWorldPos( vWorldPos );
//
	//return output;
//}
//
//#define DEPTH_HULL_AND_DOMAIN_Default
//
//#define DEPTH_HULL_AND_DOMAIN_Tess\
	//SetHullShader(CompileShader(hs_5_0, Depth_HS()));\
	//SetDomainShader(CompileShader(ds_5_0, Depth_DS()));

//-----------------------------------------------------------------------------
// TECHNIQUES
//-----------------------------------------------------------------------------


/***************************** eof ***/
