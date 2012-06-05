/*****************************************************************************
/* HEADER 
/****************************************************************************/
int gp : SasGlobal
<
  int3 SasVersion = {1,0,0};
  string SasEffectAuthor			= "Daniel Toloudis";
  string SasEffectAuthoringSoftware = "MachStudio";
  string SasEffectCategory			= "/material";
  string SasEffectCompany			= "studio|gpu";
  string SasEffectDescription		= "Phong w/Bump";
  string SasEffectHelp				= "This is a Phong shader.";    
  string SasEffectRevision			= "2";  
>;
struct perPixelVertexOutput {
    float4 HPosition	: POSITION;
    float4 TexCoord0	: TEXCOORD0;
    float4 UV			: TEXCOORD1;
    float3 WorldPos		: TEXCOORD2;
	float3 WorldTanMatrixX : TEXCOORD3;
	float3 WorldTanMatrixY : TEXCOORD4;
	float3 WorldTanMatrixZ : TEXCOORD5;
};
texture2D projLightMap	: ProjLightTexture;
texture2D projShadowMap	: ProjShadowMap;
texture2D glowMask		: GlowMask;
