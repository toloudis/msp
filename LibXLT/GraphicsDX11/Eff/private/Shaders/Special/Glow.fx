/*****************************************************************************
**  Glow.fx
**
**      Smear a specular texture on top of the current render target
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
int gp : SasGlobal
<
  int3 SasVersion = {1,0,0};
  string SasEffectAuthor			= "Daniel Toloudis";
  string SasEffectAuthoringSoftware = "Visual Studio .NET 2003";
  string SasEffectCategory			= "special/glow";
  string SasEffectCompany			= "studio|gpu";
  string SasEffectDescription		= "Blend a glow texture on top of the current render target.";
  string SasEffectHelp				= "Good luck.";    
  string SasEffectRevision			= "$Revision$";  
>;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

// full sized source image
Texture2D glowTexture;
SamplerState glowSampler
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

float glowAmount
<
	string SasUiControl = "Slider";
	string SasUiDescription = "how much should glow contribute";
	string SasUiLabel = "Glow Amount";
	float SasUiMin = 0;
	float SasUiMax = 20;
	float SasUiSteps = 200;
>
= 1.0;

float4 glowScale
<
	string SasUiControl = "Direction";
	string SasUiDescription = "glow filter size in pixels";
	string SasUiLabel = "Blur Filter Size(X,Y)(pixels)";
>
= float4(1,1,0,1);

float glowSize
<
	string SasUiControl = "Slider";
	string SasUiDescription = "how big is the glow";
	string SasUiLabel = "Glow Extrusion";
	float SasUiMin = 0;
	float SasUiMax = 2;
	float SasUiSteps = 200;
>
= 0.0;

bool bConstantGlow
<
	string SasUiControl = "Checkbox";
	string SasUiDescription = "use a constant glow effect, rather than specular";
	string SasUiLabel = "Ignore Specular";
>
= false;

// (wid, ht, 1/width, 1/ht) of source texture ( = pixel size)
float4 srcSizeInfo;

static const int nSamples = 27;
static const float2 offsets[nSamples] = {
/*   -0.326212, -0.405805,
   -0.840144, -0.073580,
   -0.695914,  0.457137,
   -0.203345,  0.620716,
    0.962340, -0.194983,
    0.473434, -0.480026,
    0.519456,  0.767022,
    0.185461, -0.893124,
    0.507431,  0.064425,
    0.896420,  0.412458,
   -0.321940, -0.932615,
   -0.791559, -0.597705,


/*	0.527837,-0.085868,
	-0.040088, 0.536087,
	-0.670445,-0.179949,
	-0.419418,-0.616039,
	0.440453,-0.639399,
	-0.757088, 0.349334,
	0.574619, 0.685879,
*/

0.0460524,      0.641499,
0.0521561,      0.218879,
0.0549638,      0.480483,
0.108768,       0.0267647,
0.164373,       0.762139,
0.219947,       0.911191,
0.227912,       0.433424,
0.269051,       0.612201,
0.272256,       0.103,
0.335795,       0.802973,
0.349925,       0.271218,
0.381695,       0.490646,
0.431867,       0.0554827,
0.483474,       0.892178,
0.484664,       0.619617,
0.58385,        0.352611,
0.595111,       0.0863063,
0.640156,       0.767327,
0.662221,       0.546709,
0.732414,       0.191626,
0.76574,        0.923612,
0.804621,       0.398541,
0.825587,       0.761406,
0.841578,       0.593219,
0.89642,        0.1742,
0.934843,       0.89758,
0.956175,       0.347667,

};

struct VS_OUTPUT
{
   float4 Pos: SV_POSITION;
   float2 img: TEXCOORD0;
};

VS_OUTPUT VSMain(float4 Pos: SV_POSITION, float2 UV : TEXCOORD0 )
{
   VS_OUTPUT Out;

   // Clean up inaccuracies
   Pos.xy = sign(Pos.xy);

   Out.Pos = float4(Pos.xy, 0, 1);
   Out.img.x = 0.5 * (1 + Pos.x);
   Out.img.y = 0.5 * (1 - Pos.y);

   return Out;
}

// Simple blur filter
float4 PS_Glow(VS_OUTPUT v_in) : SV_TARGET 
{
	float2 texCoordSample;
	float2 texCoord = v_in.img;
	float4 sum = glowTexture.Sample(glowSampler, texCoord);
	float sampleWeight = 1;
	float cumWeight = 1;
	float xscale = glowScale.x * srcSizeInfo.z;
	float yscale = glowScale.y * srcSizeInfo.w;
	// sample each quadrant. use half the samples for now
	for (int i = 0; i < nSamples; i+=2)
	{
		texCoordSample.x = texCoord.x + (xscale * offsets[i].x);
		texCoordSample.y = texCoord.y + (yscale * offsets[i].y);
		//sampleWeight = 1 - sqrt(offsets[i].x*offsets[i].x+offsets[i].y*offsets[i].y);
		sum += glowTexture.Sample(glowSampler, texCoordSample) * sampleWeight;
		cumWeight += sampleWeight;

		texCoordSample.x = texCoord.x + (xscale * -offsets[i].x);
		texCoordSample.y = texCoord.y + (yscale * offsets[i].y);
		//sampleWeight = 1 - sqrt(offsets[i].x*offsets[i].x+offsets[i].y*offsets[i].y);
		sum += glowTexture.Sample(glowSampler, texCoordSample) * sampleWeight;
		cumWeight += sampleWeight;

		texCoordSample.x = texCoord.x + (xscale * offsets[i].x);
		texCoordSample.y = texCoord.y + (yscale * -offsets[i].y);
		//sampleWeight = 1 - sqrt(offsets[i].x*offsets[i].x+offsets[i].y*offsets[i].y);
		sum += glowTexture.Sample(glowSampler, texCoordSample) * sampleWeight;
		cumWeight += sampleWeight;

		texCoordSample.x = texCoord.x + (xscale * -offsets[i].x);
		texCoordSample.y = texCoord.y + (yscale * -offsets[i].y);
		//sampleWeight = 1 - sqrt(offsets[i].x*offsets[i].x+offsets[i].y*offsets[i].y);
		sum += glowTexture.Sample(glowSampler, texCoordSample) * sampleWeight;
		cumWeight += sampleWeight;
	}
	return glowAmount * sum / cumWeight;
}

technique11 Default
{
	pass blurPass 
	{		
//		AlphaBlendEnable = true;
//		BlendOp = ADD;
//		SrcBlend = srcalpha;
//		DestBlend = one;
		VertexShader = compile vs_5_0 VSMain();
		PixelShader = compile ps_5_0 PS_Glow();
	}
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

/***************************** eof ***/
