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

float Os : Opacity = 1.0f; 
float4 Csd : MaterialDiffuse = {1.0f, 1.0f, 1.0f, 1.0f};
float4 Css : MaterialSpecular = {1.0f, 1.0f, 1.0f, 1.0f};
float g_bumpMapScale : BumpMapScale	= 1.0f;

// Normal
bool hasNormalMap = false;		// default, ibl
texture2D normalMap : NormalMap;
sampler2D normalSampler = sampler_state  // all PS's except dofprep & matte
{
	Texture = <normalMap>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
    AddressU = WRAP;
    AddressV = WRAP;
};

// Transparency
bool hasTransparencyMap = false;	// default, ibl
texture2D transparencyMap	: OpacityTexture;
sampler2D transparencySampler = sampler_state // default, ibl
{
	Texture = <transparencyMap>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
    AddressU = WRAP;
    AddressV = WRAP;
};

// Glow
texture2D glowMask		: GlowMask;
sampler2D glowSampler = sampler_state // glow
{
	Texture = <glowMask>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};
