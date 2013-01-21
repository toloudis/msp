//--------------------------------------------------------------------------------------
// File: DumpToTexture.hlsl
//
// The PS for converting CS output buffer to a texture, used in CS path of 
// HDRToneMappingCS11 sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
Texture2D buffer : register( t0 );

struct QuadVS_Output
{
    float4 Pos : SV_POSITION;              
    float2 Tex : TEXCOORD0;
};
SamplerState g_DefaultSampler
{
//    Filter = MIN_MAG_LINEAR_MIP_POINT;
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PSDump( QuadVS_Output Input ) : SV_TARGET
{
    return buffer.Sample(g_DefaultSampler, Input.Tex.xy);
    //return buffer.Load(int3(Input.Pos.xy, 0));
}
