/*****************************************************************************
**  DepthRender.fx
**
**      Render depth in pixel shader. Skips pixels where the texture's alpha is below a threshold.
**
**	Gigawatt Studios
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

/*********** support data and functions ******/

#include "..\Support.h"

float g_FogDepthStart;	// dist from camera
float g_FogDepthRange;	// camera space distance
float g_FogDensity = 1;
float4 g_FogColor;
float3 g_FogOrientation; // orientation vector 

// transform to get view space position.
float2 g_InvFocalLen;
// transform to get world space position.
float4x4 g_CameraToWorldSpace;

float g_FogAltitudeStart = 100; // for world space height of fog.
float g_FogAltitudeRange = 100; // for world space height of fog.
float g_FogAltitudeDensity = 1;
float g_FogThinning = 0.1; 

Texture2D tLinDepth; // floats (R32F)
SamplerState samNearest
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};
Texture2D tColors; 

/************* DATA STRUCTS **************/
struct PostProc_VSOut
{
    float4 pos   : SV_POSITION;
    float2 texUV : TEXCOORD0;
};

// Vertex shader that generates a full screen quad with texcoords
PostProc_VSOut FullScreenQuadVS(float4 Pos:SV_POSITION, float2 UV:TEXCOORD0 )
{
    PostProc_VSOut output = (PostProc_VSOut)0.0f;

    // -1..1, -1..1
    output.pos = Pos;
	// 0..1
	output.texUV.xy = UV;
   
    return output;
}

//----------------------------------------------------------------------------------
float3 uv_to_eye(float2 uv, float eye_z)
{
// this represents the inversion of the view matrix.
	// put 0..1 uv into -1..1 screenspace
    uv = (uv * float2(2.0, -2.0) - float2(1.0, -1.0));
    // now use (z*tan(fovx), z*tan(fovx)/aspect) to get point back in view space.
    return float3(uv * g_InvFocalLen * eye_z, eye_z);
}

//----------------------------------------------------------------------------------
float3 fetch_eye_pos(float2 uv)
{
	float z = tLinDepth.SampleLevel(samNearest, uv, 0).x;
    return uv_to_eye(uv, z);
}

float4 LinearFogPS(float4 pos   : SV_POSITION,
	in float2 INtexUV : TEXCOORD0) : SV_TARGET
{
	float3 viewPos = fetch_eye_pos(INtexUV);
	// note mul order. matrix is not transposed in source code file.
	float3 worldPos = mul(float4(viewPos,1), g_CameraToWorldSpace).xyz;
		
	float4 pixel = tColors.SampleLevel(samNearest, INtexUV, 0);
	
//	float fogLerpParam = saturate((viewPos.z-g_FogDepthStart) / g_FogDepthRange);
	
	float depthPercent = (viewPos.z-g_FogDepthStart) / g_FogDepthRange;
	float heightPercent = 1 - ((dot(worldPos, g_FogOrientation)-g_FogAltitudeStart) / g_FogAltitudeRange);
	//float fogLerpParam = saturate(depthPercent * heightPercent);
	float fogLerpParam = saturate(depthPercent);
	fogLerpParam *= saturate(heightPercent);

	float4 result = lerp(pixel,g_FogColor,fogLerpParam);
	result.a = pixel.a;
	return result;
}
float4 ExponentialFogPS(float4 pos   : SV_POSITION,
	in float2 INtexUV : TEXCOORD0) : SV_TARGET
{
	float3 viewPos = fetch_eye_pos(INtexUV);
	// note mul order. matrix is not transposed in source code file.
	float3 worldPos = mul(float4(viewPos,1), g_CameraToWorldSpace).xyz;
	
	float4 pixel = tColors.SampleLevel(samNearest, INtexUV, 0);
	
	float relativeZ = saturate((g_FogDepthStart+g_FogDepthRange-viewPos.z)/g_FogDepthRange);
	float relativeY = 1 - saturate((g_FogAltitudeStart+g_FogAltitudeRange-dot(worldPos, g_FogOrientation))/g_FogAltitudeRange);
	float fogLerpParam = exp(-g_FogDensity*relativeZ);
	fogLerpParam *= exp(-g_FogAltitudeDensity*relativeY);
	float4 result = lerp(pixel,g_FogColor,saturate(fogLerpParam));
	result.a = pixel.a;
	return result;
}
float4 ExponentialSquaredFogPS(float4 pos   : SV_POSITION,
	in float2 INtexUV : TEXCOORD0) : SV_TARGET
{
	float3 viewPos = fetch_eye_pos(INtexUV);
	// note mul order. matrix is not transposed in source code file.
	float3 worldPos = mul(float4(viewPos,1), g_CameraToWorldSpace).xyz;
	
	float4 pixel = tColors.SampleLevel(samNearest, INtexUV, 0);
	
	float relativeZ = saturate((g_FogDepthStart+g_FogDepthRange-viewPos.z)/g_FogDepthRange);
	float relativeY = 1 - saturate((g_FogAltitudeStart+g_FogAltitudeRange-dot(worldPos, g_FogOrientation))/g_FogAltitudeRange);
	float fogLerpParam = exp(-g_FogDensity*g_FogDensity*relativeZ*relativeZ);
	fogLerpParam *= exp(-g_FogAltitudeDensity*g_FogAltitudeDensity*relativeY*relativeY);
	float4 result = lerp(pixel,g_FogColor,saturate(fogLerpParam));
	result.a = pixel.a;
	return result;
}

//----------------------------------------------------------------------------------
technique11 LinearFog
{
    pass p0
    {
        VertexShader	= compile vs_5_0 FullScreenQuadVS();
        PixelShader		= compile ps_5_0 LinearFogPS();
    }
}
technique11 ExponentialFog
{
    pass p0
    {
        VertexShader	= compile vs_5_0 FullScreenQuadVS();
        PixelShader		= compile ps_5_0 ExponentialFogPS();
    }
}
technique11 ExponentialSquaredFog
{
    pass p0
    {
        VertexShader	= compile vs_5_0 FullScreenQuadVS();
        PixelShader		= compile ps_5_0 ExponentialSquaredFogPS();
    }
}
/***************************** eof ***/
