//--------------------------------------------------------------------------------------
// File: FinalPass.hlsl
//
// The PSs for doing tone-mapping based on the input luminance, used in CS path of 
// HDRToneMappingCS11 sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
Texture2D<float4> myTex : register( t0 );
SamplerState mySampler : register (s0);

cbuffer cbPS : register( b0 )
{
    float4    g_param;   
};

struct QuadVS_Input
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct QuadVS_Output
{
    float4 Pos : SV_POSITION;              
    float2 Tex : TEXCOORD0;
};

QuadVS_Output QuadVS( QuadVS_Input Input )
{
    QuadVS_Output Output;
    Output.Pos = Input.Pos;
    Output.Tex = Input.Tex;
    return Output;
}

float4 PSFinalPass( QuadVS_Output Input ) : SV_TARGET
{
	Input.Tex.y = 1 - Input.Tex.y;
	Input.Tex.x = 1 - Input.Tex.x;
    return myTex.Sample( mySampler, Input.Tex );
}
