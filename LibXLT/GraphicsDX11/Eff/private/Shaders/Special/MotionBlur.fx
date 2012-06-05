/////////////////////////////////////////////////
//  MotionBlur.fx                              //
//                                             //
//	John Schwab                                //
//	Studio GPU                                 //
//	Copyright(C) 2009 - All Rights Reserved    //
/////////////////////////////////////////////////

int gp : SasGlobal
<
  int3 SasVersion = {1,0,0};
  string SasEffectAuthor			= "John Schwab";
  string SasEffectCategory			= "special/MotionBlur";
  string SasEffectCompany			= "studio|gpu";
  string SasEffectRevision			= "$Revision$";  
>;

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

// full screen source images

Texture2D SceneTexture;
Texture2D VelocityTexture;
SamplerState g_Sampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

int g_nSamples;
float g_Scale;

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

technique11 Default
{
	pass SceneBlur
	{
		VertexShader = compile vs_5_0 MotionBlurVS();
		PixelShader =  compile ps_5_0 PS_LIC();
	}
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
