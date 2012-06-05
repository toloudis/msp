/*****************************************************************************
**  DOF.fx
**
**      Depth of Field shader
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

#define NUM_DOF_TAPS 64
// full scene image
Texture2D tSource;
SamplerState tSourceSampler
{
    AddressU = Clamp;
    AddressV = Clamp;
};

// blurred full scene image (downsampled and filtered)
Texture2D tSourceLow;

// poisson distributed positions in unit disc
/*
float2 poisson[NUM_DOF_TAPS] = {
  float2( 0.0,      0.0),
  float2( 0.527837,-0.085868),
  float2(-0.040088, 0.536087),
  float2(-0.670445,-0.179949),
  float2(-0.419418,-0.616039),
  float2( 0.440453,-0.639399),
  float2(-0.757088, 0.349334),
  float2( 0.574619, 0.685879)
};
*/
float2 poisson[82] = {
	float2(0,0),
	float2(-0.304967, -0.058754),
	float2(-0.043598, -0.452767),
	float2(-0.642527, 0.799442),
	float2(0.640218, 0.596450),
	float2(0.659291, -0.773008),
	float2(-0.178993, -0.872160),
	float2(0.276105, -0.820578),
	float2(0.580867, -0.117841),
	float2(-0.522690, -0.634022),
	float2(0.089258, 0.226401),
	float2(0.928508, 0.051883),
	float2(-0.868488, 0.350548),
	float2(0.941204, 0.957318),
	float2(-0.495066, 0.394927),
	float2(-0.125758, 0.984469),
	float2(-0.955218, -0.603685),
	float2(0.795363, -0.541541),
	float2(0.934481, 0.482255),
	float2(-0.755124, -0.059173),
	float2(0.311857, 0.783972),
	float2(-0.959199, -0.966749),
	float2(-0.065384, 0.641868),
	float2(0.923424, -0.935969),
	float2(0.040538, -0.118669),
	float2(-0.996426, 0.082208),
	float2(-0.613901, -0.982514),
	float2(0.236256, -0.397797),
	float2(0.609993, 0.962312),
	float2(-0.950827, 0.948278),
	float2(0.490153, 0.189333),
	float2(0.304875, 0.546167),
	float2(-0.263125, 0.281158),
	float2(-0.572705, 0.141030),
	float2(0.954722, -0.313642),
	float2(-0.346067, -0.330249),
	float2(-0.723442, -0.474052),
	float2(-0.383928, 0.651586),
	float2(0.286045, 0.018058),
	float2(-0.942373, 0.674203),
	float2(0.084795, -0.954489),
	float2(-0.948848, -0.187677),
	float2(0.535156, -0.407324),
	float2(0.659832, 0.351442),
	float2(-0.254076, -0.534739),
	float2(0.896457, 0.264414),
	float2(-0.396225, -0.983286),
	float2(-0.417622, 0.924533),
	float2(0.911242, 0.718191),
	float2(0.976459, -0.699264),
	float2(-0.116580, 0.108363),
	float2(0.319926, -0.220776),
	float2(-0.712503, 0.505410),
	float2(-0.774058, -0.812883),
	float2(0.090694, 0.829392),
	float2(0.330998, -0.608571),
	float2(-0.548603, -0.384875),
	float2(0.118297, 0.550209),
	float2(-0.919727, -0.400404),
	float2(0.024899, -0.646427),
	float2(-0.732525, 0.986321),
	float2(-0.060302, 0.361266),
	float2(0.178028, 0.986945),
	float2(0.527828, -0.931745),
	float2(-0.233099, 0.799864),
	float2(0.297159, 0.248822),
	float2(0.756461, -0.305501),
	float2(0.456517, 0.387692),
	float2(-0.533354, -0.140567),
	float2(0.739327, 0.111377),
	float2(-0.339785, -0.741848),
	float2(-0.755231, -0.263203),
	float2(0.489712, 0.714908),
	float2(-0.236110, 0.472629),
	float2(0.547581, -0.613419),
	float2(-0.643593, 0.332322),
	float2(0.736407, -0.057415),
	float2(0.064937, -0.287273),
	float2(-0.320082, 0.112393),
	float2(-0.992819, 0.524451),
	float2(-0.762316, 0.123410),
	float2(0.731807, -0.986733)
};

// 1/image resolution of full size image
float2 pixelSizeHigh; 
// 1/image resolution of downsampled image
float2 pixelSizeLow;

// maximum circle of confusion radius in pixels
float maxCoC = 5.0;

// scale factor for maximum CoC on smaller image
float radiusScale = 0.4; 

float4 PoissonDOFFilter(float2 texCoord /* screen space tex coord */)
{
	// uncomment to compare full-res and downasampled blurred texture
	//return texCoord.x > 0.5 ? tex2D(tSourceLowSampler, texCoord) : tex2D(tSourceSampler, texCoord);
	
	float4 cOut;
	float discRadius, discRadiusLow, centerDepth;
	
	// sample the image
	cOut = tSource.Sample(tSourceSampler, texCoord);
	// save the depth number
	centerDepth = cOut.a;
	
	float blurriness = abs((cOut.a * 2.0) - 1.0);
	// uncomment to render out the blurriness amounts:
	//return float4(centerDepth, centerDepth, centerDepth, 1);
	//return float4(blurriness,blurriness,blurriness,1);

	// convert depth into blur radius in pixels (0..maxCoC)
	discRadius = maxCoC * blurriness;
	// radius on smaller image will be smaller (in pixels!)
	discRadiusLow = discRadius * radiusScale;
	
	cOut = 0;
	for (int t = 0; t < NUM_DOF_TAPS; t++)
	{
		// tex coords at poisson sample points
		float2 coordLow = texCoord + (pixelSizeLow * poisson[t] * discRadiusLow);
		float2 coordHigh = texCoord + (pixelSizeHigh * poisson[t] * discRadius);
		
		// sample both textures
		float4 tapLow = tSourceLow.Sample(tSourceSampler, coordLow);
		float4 tapHigh = tSource.Sample(tSourceSampler, coordHigh);
		
		// interpolate between the 2 based on the depth value
		float tapBlur = abs((tapHigh.a * 2.0) - 1.0);
		float4 tap = lerp(tapHigh, tapLow, tapBlur);
		
		// ignore taps that are closer than the center tap and in focus
		tap.a = (tap.a >= centerDepth) ? 1.0 : abs((tap.a * 2.0) - 1.0);

		cOut.rgb += tap.rgb * tap.a;
		cOut.a += tap.a;
	}
	return (cOut / cOut.a);
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

float4 DOFPost_PS(VS_OUTPUT v_in)  : SV_TARGET
{
    return PoissonDOFFilter(v_in.img);
}
technique11 DOFPost
{
	pass p0 
	{		
		VertexShader = compile vs_5_0 VSMain();//NULL;
		PixelShader = compile ps_5_0 DOFPost_PS();
	}
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

/***************************** eof ***/
