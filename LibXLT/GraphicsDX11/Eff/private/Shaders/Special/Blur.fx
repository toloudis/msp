/*****************************************************************************
**  Blur.fx
**
**      Separable Gaussian Blur shader
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

// full sized source image
Texture2D sceneTexture : SCENE_TEXTURE;
SamplerState sceneSampler
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

// down-sampled image of the source
Texture2D downsampledTexture;
SamplerState downsampledSampler
{
//    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

SamplerState BlurSampler
{
    Filter = ANISOTROPIC;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = BORDER;
    MipLODBias = 0;
    MaxAnisotropy = 16;
    ComparisonFunc = NEVER;
    BorderColor = float4(0, 0, 0, 0);
    MinLOD = 0;
    MaxLOD = 0;
};

// texture that will store the intermediate results of the blur
Texture2D horizontalBlurTexture;

// (wid, ht, 1/width, 1/ht) of source texture ( = pixel size)
float4 srcSizeInfo;

// (wid, ht, 1/width, 1/ht) of downsampled texture
float4 downsampledSizeInfo;

int VSMDepth = 1;

//#ifndef SEPERABLE_BLUR_KERNEL_SIZE
#define SEPERABLE_BLUR_KERNEL_SIZE 7
//#endif

static const int BLUR_KERNEL_BEGIN = SEPERABLE_BLUR_KERNEL_SIZE / -2; 
static const int BLUR_KERNEL_END = SEPERABLE_BLUR_KERNEL_SIZE / 2 + 1;
static const float FLOAT_BLUR_KERNEL_SIZE = (float)SEPERABLE_BLUR_KERNEL_SIZE;

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

// simple pass-thru pixel shader
float4 DownSamplePS(VS_OUTPUT px) : SV_TARGET
{
	//return tex2D(sceneSampler, texCoord);
	
	float2 texCoordSample;
	float4 cOut;

	// it would be more efficient if the texture coordinates
	// were computed in the vertex shader and passed down
	texCoordSample = px.img + float2(-srcSizeInfo.z,  srcSizeInfo.w);
	cOut = sceneTexture.Sample(sceneSampler, texCoordSample);

	texCoordSample = px.img + float2( srcSizeInfo.z,  srcSizeInfo.w);
	cOut += sceneTexture.Sample(sceneSampler, texCoordSample);

	texCoordSample = px.img + float2( srcSizeInfo.z, -srcSizeInfo.w);
	cOut += sceneTexture.Sample(sceneSampler, texCoordSample);

	texCoordSample = px.img + float2(-srcSizeInfo.z, -srcSizeInfo.w);
	cOut += sceneTexture.Sample(sceneSampler, texCoordSample);
	
	return cOut*0.25;	    
}

// pixel kernels for Gaussian Blur
static const int g_KernelSize = 3;
float2 HPixelOffsets[g_KernelSize] = {{-2,0}, {0,0}, {2,0}};
float2 VPixelOffsets[g_KernelSize] = {{0,-2}, {0,0}, {0,2}};
static const float BlurWeights[g_KernelSize] = {0.25, 0.5, 0.25};

// Separable Gaussian Blur Shader
float4 Blur(VS_OUTPUT px,
	uniform float2 pixelOffsets[g_KernelSize],
	uniform Texture2D passSampler,
	uniform float oneOverSourceWidth): SV_TARGET
{
	float4 color = 0;
	for (int i = 0; i < g_KernelSize; i++)
	{
		color += passSampler.Sample(downsampledSampler, px.img + pixelOffsets[i].xy*oneOverSourceWidth) * BlurWeights[i];
	}
	return color;
}

//------------------------------------------------------------------------------
// Logarithmic filtering
//------------------------------------------------------------------------------

float log_conv ( float x0, float X, float y0, float Y )
{
    return (X + log(x0 + (y0 * exp(Y - X))));
}

//--------------------------------------------------------------------------------------
// Pixel shader that performs bump mapping on the final vertex
//--------------------------------------------------------------------------------------
float4 PSBlurX(VS_OUTPUT px, uniform Texture2D passSampler) : SV_Target
{	
    float4 dep=0;
    [unroll]for ( int x = BLUR_KERNEL_BEGIN; x < BLUR_KERNEL_END; ++x ) {
        dep += passSampler.Sample( BlurSampler,  px.img, int2( x,0 ) );
    }
    dep /= FLOAT_BLUR_KERNEL_SIZE;
    return dep;
}

//--------------------------------------------------------------------------------------
// Pixel shader that performs bump mapping on the final vertex
//--------------------------------------------------------------------------------------
float4 PSBlurY(VS_OUTPUT px, uniform Texture2D passSampler) : SV_Target
{
    float4 dep=0;
    [unroll]for ( int y = BLUR_KERNEL_BEGIN; y < BLUR_KERNEL_END; ++y ) {
        dep += passSampler.Sample( BlurSampler,  px.img, int2( 0,y ) );
    }
    dep /= FLOAT_BLUR_KERNEL_SIZE;
    return dep;  
}

//--------------------------------------------------------------------------------------
// Pixel shader that performs horizontal sum scan
//--------------------------------------------------------------------------------------
float4 PSHScan(VS_OUTPUT px, uniform Texture2D passSampler) : SV_Target
{	
	float4 summed = 0;
	
	summed = passSampler.Load(float3(px.Pos.xy, 0));
	int3 offset = int3(pow(2, VSMDepth - 1), 0, 0);
	if ((px.Pos.x - offset.x) >= 0)
		summed += passSampler.Load(float3(px.Pos.xy, 0) - offset);
    
    return summed;
}

//--------------------------------------------------------------------------------------
// Pixel shader that performs vertical sum scan
//--------------------------------------------------------------------------------------
float4 PSVScan(VS_OUTPUT px, uniform Texture2D passSampler) : SV_Target
{
	float4 summed = 0;
	
	summed = passSampler.Load(float3(px.Pos.xy, 0));
	int3 offset = int3(0, pow(2, VSMDepth - 1), 0);
	if ((px.Pos.y - offset.y) >= 0)
		summed += passSampler.Load(float3(px.Pos.xy, 0) - offset);
    
    return summed;
}

technique11 SimpleGaussianBlur
{
	// render target is downsampledTexture
	pass downSamplePass 
	{		
		VertexShader = compile vs_5_0 VSMain();
		PixelShader = compile ps_5_0 DownSamplePS();
	}
	
	// render target is horizontalBlurTexture
	pass horizontalBlurPass 
	{		
		VertexShader = compile vs_5_0 VSMain();
		PixelShader = compile ps_5_0 Blur(HPixelOffsets,
			downsampledTexture, downsampledSizeInfo.z);
	}
	
	// last pass: render target is result rendertarget of choice!
	pass verticalBlurPass 
	{		
		VertexShader = compile vs_5_0 VSMain();
		PixelShader = compile ps_5_0 Blur(VPixelOffsets,
			horizontalBlurTexture, downsampledSizeInfo.w);
	}
}

technique11 SimpleBlur
{
	// render target is horizontalBlurTexture
	pass BlurXPass 
	{		
		VertexShader = compile vs_5_0 VSMain();
		PixelShader = compile ps_5_0 PSBlurX(downsampledTexture);
	}
	
	// last pass: render target is result rendertarget of choice!
	pass BlurYPass 
	{		
		VertexShader = compile vs_5_0 VSMain();
		PixelShader = compile ps_5_0 PSBlurY(horizontalBlurTexture);
	}
}

technique11 SummedParallelScan
{
	// render target is horizontalBlurTexture
	pass HorizontalScan
	{		
		VertexShader = compile vs_5_0 VSMain();
		PixelShader = compile ps_5_0 PSHScan(sceneTexture);
	}
	
	// last pass: render target is result rendertarget of choice!
	pass VerticalScan
	{		
		VertexShader = compile vs_5_0 VSMain();
		PixelShader = compile ps_5_0 PSVScan(sceneTexture);
	}
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

/***************************** eof ***/
