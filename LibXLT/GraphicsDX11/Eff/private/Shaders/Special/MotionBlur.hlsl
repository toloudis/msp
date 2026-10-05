//////////////////////////////////////////////////////////////////////////////
// Converted from MotionBlur.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// MotionBlur.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////
//  MotionBlur.fx                              //
//                                             //
//	John Schwab                                //
//	Studio GPU                                 //
//	Copyright(C) 2009 - All Rights Reserved    //
/////////////////////////////////////////////////

struct PostProc_VSOut
{
    float4 pos   : SV_POSITION;
    float2 texUV : TEXCOORD0;
};

/////////////////////////////////////////////////
PostProc_VSOut MotionBlurVS(float4 Pos:SV_POSITION, float2 UV:TEXCOORD0 )
{
    PostProc_VSOut output = (PostProc_VSOut)0.0f;
	Pos.xy = sign(Pos.xy);    
    output.pos = float4( Pos.xy, 0.0f, 1.0f );    
	output.texUV = UV;
    return output;
}

/////////////////////////////////////////////////

// Explicit registers keep the binding layout identical for every entry point
// (and map directly onto a DX12 root signature / Vulkan descriptor set).

// full screen source images
Texture2D SceneTexture : register(t0);
Texture2D VelocityTexture : register(t1);

// sampler state is described in MotionBlur.effect.json
SamplerState g_Sampler : register(s0);

cbuffer MotionBlurParams : register(b0)
{
	int g_nSamples;
	float g_Scale;
};

//Performs a Line Integral Convolution on the scene using the pixel velocity
float4 PS_LIC(    float4 pos   : SV_POSITION,
		float2 texCoord: TEXCOORD0) : SV_TARGET
{
   float4 Color = (float4)0;
   float4 Vel = VelocityTexture.Sample( g_Sampler, texCoord ) * -g_Scale;	//flip the velocity vector
   Vel /= g_nSamples;
   for( int i = 0; i < g_nSamples; i++, texCoord += Vel.xy )
   {
      Color += SceneTexture.SampleLevel( g_Sampler, texCoord, 0 );
   }
   Color /= g_nSamples;

   return Color;
}


/////////////////////////////////////////////////
/////////////////////////////////////////////////
