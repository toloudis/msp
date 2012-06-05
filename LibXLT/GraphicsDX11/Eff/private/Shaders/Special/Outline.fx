/*****************************************************************************
**  Outline.fx
**
**	John Schwab
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

int gp : SasGlobal
<
  int3 SasVersion = {1,0,0};
  string SasEffectAuthor			= "John Schwab";
  string SasEffectCategory			= "special/outline";
  string SasEffectCompany			= "studio|gpu";
  string SasEffectRevision			= "$Revision$";  
>;

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

// full sized source image
Texture2D colorTexture;
SamplerState g_Sampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

float g_depthScale;
float g_minAngle;
float g_maxAngle;
float g_thickness;
float4 g_outlineClr;
float2 g_ViewportDimensions;
float g_minWidth;
float g_maxWidth;

//------------------------------------------------------------------
// This function applies a Sobel filter to the alpha channel to detect edges in the image.
// The Sobel filter extracts the first order derivates of the image,
// that is, the slope. Where the slope is sharp there is an edge.
// These are the filter kernels:
//
//  SobelX       SobelY
//  1  0 -1      1  2  1
//  2  0 -2      0  0  0
//  1  0 -1     -1 -2 -1
//------------------------------------------------------------------

float SobelFilter( Texture2D map, float2 coord )
{
   float xOff = g_thickness / g_ViewportDimensions.x;
   float yOff = g_thickness / g_ViewportDimensions.y;

   // Sample neighbor pixels
   float s00 = map.Sample(g_Sampler, coord + float2(-xOff, -yOff)).r;
   float s01 = map.Sample(g_Sampler, coord + float2( 0,    -yOff)).r;
   float s02 = map.Sample(g_Sampler, coord + float2( xOff, -yOff)).r;
   float s10 = map.Sample(g_Sampler, coord + float2(-xOff,  0)).r;
   float s12 = map.Sample(g_Sampler, coord + float2( xOff,  0)).r;
   float s20 = map.Sample(g_Sampler, coord + float2(-xOff,  yOff)).r;
   float s21 = map.Sample(g_Sampler, coord + float2( 0,     yOff)).r;
   float s22 = map.Sample(g_Sampler, coord + float2( xOff,  yOff)).r;

   // Sobel filter in X direction
   float sobelX = s00 + 2 * s10 + s20 - s02 - 2 * s12 - s22;
   // Sobel filter in Y direction
   float sobelY = s00 + 2 * s01 + s02 - s20 - 2 * s21 - s22;

   // Find edge, skip sqrt() to improve performance ...
   float edgeSqr = ((sobelX * sobelX) + (sobelY * sobelY));

   // ... and scale by threshold
   return edgeSqr * g_depthScale;
}

float AngleFilter( Texture2D map, float2 coord )
{
	//determine view deviation using normal
   float dev = saturate(map.Sample(g_Sampler, coord ).z * 0.5 + 0.5f);
   float width = max(lerp( g_minWidth, g_maxWidth, dev ),0);

   float xOff = width / g_ViewportDimensions.x;
   float yOff = width / g_ViewportDimensions.y;
   
   float minAng = cos( radians( g_minAngle ));
   float maxAng = cos( radians( g_maxAngle ));
  
   // Sample neighbor pixels
   float3 n00 = map.Sample(g_Sampler, coord + float2(-xOff, -yOff)).xyz;
   float3 n02 = map.Sample(g_Sampler, coord + float2( xOff, -yOff)).xyz;
   float3 n20 = map.Sample(g_Sampler, coord + float2(-xOff,  yOff)).xyz;
   float3 n22 = map.Sample(g_Sampler, coord + float2( xOff,  yOff)).xyz;

   float sA = dot( n00, n22 );
   float sB = dot( n02, n20 );

   if( (sA < minAng) && (sA > maxAng) ||
	   (sB < minAng) && (sB > maxAng))
   {
	   return 1;
   }
   return 0;
}

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

// edge detection filter using depths
float4 PS_OutlineD(VS_OUTPUT v_in) : SV_TARGET 
{
	float4 OUT;

	OUT.rgb = g_outlineClr.xyz;
	OUT.a = saturate( SobelFilter( colorTexture, v_in.img ) );
	
	return OUT;
}

// edge detection filter using normals
float4 PS_OutlineN(VS_OUTPUT v_in) : SV_TARGET 
{
	float4 OUT;

	OUT.rgb = g_outlineClr.xyz;
	OUT.a = saturate( AngleFilter( colorTexture, v_in.img ) );
	
	return OUT;
}

technique11 Default
{
	pass p0 
	{
		VertexShader = compile vs_5_0 VSMain();		
		PixelShader = compile ps_5_0 PS_OutlineD();
	}
	pass p1
	{
		VertexShader = compile vs_5_0 VSMain();		
		PixelShader = compile ps_5_0 PS_OutlineN();
	}
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
/***************************** eof ***/
