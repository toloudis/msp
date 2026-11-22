//--------------------------------------------------------------------------------------
// File: MapNormalsToScreen.hlsl
//
// The PS for doing converting normals to rgba8 pixel colors
// 
//--------------------------------------------------------------------------------------
Texture2D<float4> myTex : register( t0 );
SamplerState mySampler : register (s0);

struct QuadVS_Output
{
    float4 Pos : SV_POSITION;              
    float2 Tex : TEXCOORD0;
};

float4 main( QuadVS_Output Input ) : SV_TARGET
{
    float4 nor = myTex.Sample( mySampler, Input.Tex );
    return float4( 0.5 * (nor.xyz + 1), 1);
}
