/*****************************************************************************
**  Lighting.h
**
**      Support functions for .fx shaders
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

//define this to disable PCSS (shaders load 10x faster)
//#define SIMPLE_SHADOWS
//#define PCSS_SAT

/*********** support data ******/
struct LightInfo
{
	float4 Pos;
	float4 Diffuse;
	float4 Specular;
	float4 Falloff;
	float4 ConeInfo;	/* x,y,z are normalized direction, w is cos(ConeAngle) */
	float FalloffStart;
};
struct ProjLightInfo
{
	float4 Pos;
	float4x4 Matrix;
	float LightSize;
	float PCSSAdjust;
	float Scale;
	float ShadowIntensity;
	float4 ShadowColor;
	float2 NearFar;
	float MapSize;
	float InnerAngle;
	float OuterAngle;
	float Aspect;
};
struct IncidentLight
{
	// direction, unnormalized
	float3 L;
	// color
	float3 Cld;
	float3 Cls;
};

//int g_lightArrayNum : LightArrayNum = 0;
//LightInfo g_lightArray[8] : LightArray = 
//{
//	(LightInfo)0,(LightInfo)0,(LightInfo)0,(LightInfo)0,
//	(LightInfo)0,(LightInfo)0,(LightInfo)0,(LightInfo)0
//};
LightInfo g_lightInfo : LightInfo;
ProjLightInfo g_projLight : ProjLightInfo;

bool g_bProjLt = false;
bool g_bHasProjMap : HasProjectedTexture = false;
bool g_bHasShadowMap : HasShadowMap = false;

#ifdef SIMPLE_SHADOWS
#define BLOCKER_SAMPLES_LOW 2
#define SHADOW_SAMPLES_LOW 2

#define BLOCKER_SAMPLES_MED 2
#define SHADOW_SAMPLES_MED 2

#define BLOCKER_SAMPLES_HIGH 2
#define SHADOW_SAMPLES_HIGH 2

#define BLOCKER_SAMPLES_VERY_HIGH 2
#define SHADOW_SAMPLES_VERY_HIGH 2

#else//SIMPLE_SHADOWS

#define BLOCKER_SAMPLES_LOW 5
#define SHADOW_SAMPLES_LOW 5

#define BLOCKER_SAMPLES_MED 7
#define SHADOW_SAMPLES_MED 7

#define BLOCKER_SAMPLES_HIGH 9
#define SHADOW_SAMPLES_HIGH 9

#define BLOCKER_SAMPLES_VERY_HIGH 15
#define SHADOW_SAMPLES_VERY_HIGH 15

#endif//SIMPLE_SHADOWS

Texture2D projLightMap	: ProjLightTexture;
SamplerState projSampler
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = Border;
	AddressV = Border;
	AddressW = Border;
	BorderColor = float4(0,0,0,0);
};

Texture2D projShadowMap	: ProjShadowMap;
SamplerState shadowMapSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Border;
	AddressV = Border;
	AddressW = Border;
	BorderColor = float4(1,1,1,1);
};

Texture2D g_Poisson;
SamplerState poissionSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

/*********** support functions ******/
//--------------------------------------------------------------------
//	attenuation() - calculate a factor to modify light intensity based on distance
//--------------------------------------------------------------------// 
float attenuation(float3 Pw,		// Position of vertex in world coords
				  LightInfo light)
{
	float atten = 1;
	if (light.Pos.w > 0.5) // only point lights (w == 1)
	{
		float d = distance(Pw, light.Pos.xyz);
		if (d > light.FalloffStart)
		{
			atten = 1 / (light.Falloff.x + 
						light.Falloff.y * (d - light.FalloffStart) + 
						light.Falloff.z * (d - light.FalloffStart) * (d - light.FalloffStart) + 
						light.Falloff.w * (d - light.FalloffStart) * (d - light.FalloffStart) * (d - light.FalloffStart));
		}
		else
		{
			atten = 1 / (light.Falloff.x);
		}
	}
	return atten;
}

//--------------------------------------------------------------------
//	diffuse_contrib()
//--------------------------------------------------------------------
void diffuse_contrib(LightInfo lightInfo,
						float3 Pw,			// Position of vertex in world coords
						float3 Nn,			// Normalize vertex normal
						float3 V,			// Eye position, world - vertex position
						float shininess,	// Specular power

						out float4 diffContrib,
						out float4 specContrib)
{
	float atten = attenuation(Pw, lightInfo);
    float3 Ln = normalize(lightInfo.Pos.xyz - mul(Pw, lightInfo.Pos.w));

	float ldn = (dot(Ln,Nn));
    float diffComp = saturate(ldn) * atten;

	float3 H = normalize(Ln + V);
	float hdn = saturate(dot(Nn, H));
	float specComp = pow(hdn, max(0.001f, shininess)) * atten;

	// Only approximated Projected lights have
	// the cone angle set, all others use 0 for w
	// (Note: w==0 could also occur when the angle of the
	// projected light is 180 degrees)
	if (lightInfo.ConeInfo.w > 0)
	{
		// Cone angle computation
		// ConeInfo.xyz is negated direction of spot light,
		// ConeInfo.w is cos(0.5 * angle of light)
		// If dot of light direction and vector to light is less
		// than cone angle, remove the light's contribution
		float cos_cone = (dot(Ln, lightInfo.ConeInfo.xyz));
		if (cos_cone < lightInfo.ConeInfo.w)
		{
			diffComp = 0.0;
			specComp = 0.0;
		}
	}

    diffContrib = ((diffComp) * (lightInfo.Diffuse));
    //diffContrib.w = 1.0;
    diffContrib.w = 0.0;

	//diffContrib = float4(0,0,0,1);
	//diffContrib = CheckNan4(diffContrib);


	//if (diffComp <= 0) specComp = 0;
	specContrib = ((specComp) * (lightInfo.Specular));
    specContrib.w = 0.0;

	//specContrib = float4(0,0,0, 0);
	//specContrib = CheckNan4(specContrib);

    //return float4(diffComp,diffComp,diffComp,1);
}

//--------------------------------------------------------------------
//	GetPoissonVal() - PCSS helper function
//--------------------------------------------------------------------
float2 GetPoissonVal(int i)
{
	return g_Poisson.Load(int3(i,0,0)).xy;
}

float linstep(float min, float max, float v)
{
	return clamp((v - min)/(max - min), 0, 1);
}

SamplerState shdwSampler
{
	FILTER = MIN_MAG_MIP_LINEAR;
    AddressU = Border;
    AddressV = Border;
	BorderColor = float4(1, 1, 1, 1);
};

SamplerState SATSampler
{
	FILTER = MIN_MAG_MIP_LINEAR;
	//FILTER = ANISOTROPIC;
	MaxAnisotropy = 16;
    AddressU = Border;
    AddressV = Border;
	BorderColor = float4(0, 0, 0, 0);
};

//SamplerState shdwSamplerPoint
//{
//	FILTER = MIN_MAG_MIP_POINT;
//    AddressU = Border;
//    AddressV = Border;
//	BorderColor = float4(1, 1, 1, 1);
//};

//SamplerComparisonState shdwSamplerPointCmp
//{
//	FILTER = COMPARISON_MIN_MAG_MIP_POINT;
//    AddressU = Border;
//    AddressV = Border;
//	ComparisonFunc = LESS_EQUAL;
//	BorderColor = float4(1, 1, 1, 1);
//};

SamplerState CubeSampler
{
	FILTER = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
	AddressW = Wrap;
};

/*********** shadow mapping **********/
// simple averaging with grid of samples about point:
// fTexelSize is half filter width (samples are spread from 
// -fTexelSize to +fTexelSize around ProjTexCoord)
// numSamples must be > 1! odd numbers of samples will 
// sample the original point, while even numbers will miss it.
//--------------------------------------------------------------------
//	SampleShadowMap() - deprecated
//--------------------------------------------------------------------
float4 SampleShadowMap(Texture2D ProjShadowMap, float4 ProjTexCoord, 
					   uniform int samples, float fTexelSize = 1.0f/24.0f)
{
	// divide filter width by number of samples to use
	float fShadowTerm = 0.0f;	
	float fCurShadowTerm;

	// iterate through search region and add up depth values
	for (int i=0; i<samples*samples; i++) 
	{
		float2 offset = GetPoissonVal(i) * (fTexelSize * ProjTexCoord.w);
		fCurShadowTerm = ProjShadowMap.Sample( shdwSampler, (ProjTexCoord.xy + offset) / ProjTexCoord.w ).x;
		fShadowTerm += fCurShadowTerm;
	}
	fShadowTerm /= (samples*samples);

	fShadowTerm = lerp(1-g_projLight.ShadowIntensity, 1, fShadowTerm);

	return fShadowTerm;
}

//--------------------------------------------------------------------
//	Variance Shadow Map
//--------------------------------------------------------------------
float ChebyshevUpperBound(float2 Moments, float zReceiver)
{
	float p = (zReceiver <= Moments.x);
	float variance = ( Moments.y ) - ( Moments.x * Moments.x );
	//variance = max(variance + 0.00001f, 0.0);
	variance       = min(1.0f, max( 0.0f, variance + 0.00001f ));

	float d        = zReceiver - Moments.x; 
	float p_max    = variance / ( variance + d*d );

	// To combat light-bleeding, experiment with raising p_max to some power
	// (Try values from 0.1 to 100.0, if you like.)
	return pow( max(p, p_max), 1.0f );
	//return linstep(0.2f, 1, max(p, p_max)); // Quick light bleeding reduction
}
float g_DistributeFactor = 256;
float2 RecombinePrecision(float4 Value)  
{  
	float FactorInv = 1 / g_DistributeFactor;  
	return (Value.zw * FactorInv + Value.xy);  
}

float CalculateVarianceShadow(uniform Texture2D ProjShadowMap, float4 ProjTexCoord)
{
	float fPercentLit = 0.0f;
	float2 uv = ProjTexCoord.xy/ProjTexCoord.w;
	float zReceiver = ProjTexCoord.z;
	float2 mapDepth = 0;

	/*float2 vShadowTexCoordDDX = 
		ddx(ProjTexCoord.xy);
    float2 vShadowTexCoordDDY = 
		ddy(ProjTexCoord.xy);*/

	mapDepth += ProjShadowMap.SampleLevel(shdwSampler, uv, 0).xy;
	/*mapDepth += (ProjShadowMap.SampleLevel(SATSampler, uv, 0, int2(0,0))).xy;
	mapDepth -= (ProjShadowMap.SampleLevel(SATSampler, uv, 0, int2(-1,0))).xy;
	mapDepth -= (ProjShadowMap.SampleLevel(SATSampler, uv, 0, int2(0,-1))).xy;
	mapDepth += (ProjShadowMap.SampleLevel(SATSampler, uv, 0, int2(-1,-1))).xy;*/

	/*if (zReceiver <= mapDepth.x)
		fPercentLit = 1.0f;
	else*/
	fPercentLit = ChebyshevUpperBound(mapDepth+0.5, zReceiver);

	float shadowed = lerp(1-g_projLight.ShadowIntensity, 1, fPercentLit);
	return shadowed;
}

//--------------------------------------------------------------------
//	PenumbraSize() - PCSS helper function
//--------------------------------------------------------------------
float PenumbraSize(float zReceiver, float zBlocker) //Parallel plane estimation
{
	return (zReceiver - zBlocker) / zBlocker;
}

//--------------------------------------------------------------------
//	FindBlocker() - PCSS helper function
//--------------------------------------------------------------------
void FindBlocker(out float avgBlockerDepth,
				 out float numBlockers,
				 float2 uv, float zReceiver,
				 uniform Texture2D ShadowMap, 
				 int nBlockerSamples, float lightSizeUV)
{
	// This uses similar triangles to compute what area of the shadow map we should search
	float searchWidth = lightSizeUV * (zReceiver - g_projLight.NearFar.x) / zReceiver;
	float blockerSum = 0;
	numBlockers = 0;
	float2 offset;
	float shadowMapDepth;
	for( int i = 0; i < nBlockerSamples; ++i )
	{
		for( int j = 0; j < nBlockerSamples; ++j )
		{
			offset = GetPoissonVal(i*nBlockerSamples+j) * searchWidth;

			shadowMapDepth = ShadowMap.SampleLevel(shdwSampler, uv + offset, 0).x;
			//shadowMapDepth = ShadowMap.GatherRed( shdwSampler, uv + offset, int2( 0, 0 ) ).x;
			/*float4 d4 = ShadowMap.GatherRed( shdwSampler, uv + offset, int2( 0, 0 ) );
            float4 b4  = ( zReceiver.xxxx <= d4 ) ? (0.0).xxxx : (1.0).xxxx; 

			blockerSum += dot( d4, b4 );
			numBlockers += dot( b4, (1.0).xxxx );*/
	
			if ( shadowMapDepth < zReceiver ) {
				blockerSum += shadowMapDepth;
				numBlockers++;
			}
		}
	}

	avgBlockerDepth = blockerSum / numBlockers;
}

//--------------------------------------------------------------------
//	FindBlockerSAT() - PCSS helper function
//--------------------------------------------------------------------
void FindBlocker_SAT(out float avgBlockerDepth,
				 out float numBlockers,
				 float2 uv, float zReceiver,
				 uniform Texture2D ShadowMap, 
				 int nBlockerSamples, float lightSizeUV)
{
	// This uses similar triangles to compute what area of the shadow map we should search
	float searchWidth = lightSizeUV * (zReceiver - g_projLight.NearFar.x) / zReceiver;
	float blockerSum = 0;
	numBlockers = 0;
	float2 offset;
	float shadowMapDepth;
	for( int i = 0; i < nBlockerSamples; ++i )
	{
		for( int j = 0; j < nBlockerSamples; ++j )
		{
			offset = GetPoissonVal(i*nBlockerSamples+j) * searchWidth;

			//shadowMapDepth = ShadowMap.SampleLevel(shdwSampler, uv + offset, 0).x;
			float Summed;

			Summed = (ShadowMap.SampleLevel(SATSampler, uv+offset, 0, int2(0, 0))).x;
			Summed -= (ShadowMap.SampleLevel(SATSampler, uv+offset, 0, int2(-1, 0))).x;
			Summed -= (ShadowMap.SampleLevel(SATSampler, uv+offset, 0, int2(0, -1))).x;
			Summed += (ShadowMap.SampleLevel(SATSampler, uv+offset, 0, int2(-1, -1))).x;
			/*int2 poffset = (uv+offset) * g_projLight.MapSize;
			int3 puv = int3(poffset, 0);
			Summed = ShadowMap.Load(puv, int2(0, 0)).x;
			Summed -= ShadowMap.Load(puv, int2(-1, 0)).x;
			Summed -= ShadowMap.Load(puv, int2(0, -1)).x;
			Summed += ShadowMap.Load(puv, int2(-1, -1)).x;*/

			shadowMapDepth = Summed + 0.5f;
			//shadowMapDepth = ShadowMap.GatherRed( shdwSampler, uv + offset, int2( 0, 0 ) ).x;
			/*float4 d4 = ShadowMap.GatherRed( shdwSampler, uv + offset, int2( 0, 0 ) );
            float4 b4  = ( zReceiver.xxxx <= d4 ) ? (0.0).xxxx : (1.0).xxxx; 

			blockerSum += dot( d4, b4 );
			numBlockers += dot( b4, (1.0).xxxx );*/
	
			if ( shadowMapDepth < zReceiver ) {
				blockerSum += shadowMapDepth;
				numBlockers++;
			}
		}
	}

	avgBlockerDepth = blockerSum / numBlockers;
}

//--------------------------------------------------------------------
//	PCF_Filter() - PCSS helper function
//--------------------------------------------------------------------
float PCF_Filter_SAT( float2 uv, float zReceiver, float filterRadiusUV,
				  uniform Texture2D ShadowMap, int nShadowSamples)
{
	float shadowed = 1.0f;
	float2 Summed;
	int numSample = filterRadiusUV * g_projLight.MapSize * 2;
	float2 nuv = uv + filterRadiusUV.xx;
	if (numSample < 1)
	{
		Summed = (ShadowMap.SampleLevel(SATSampler, uv, 0, int2(0,0))).xy;
		Summed -= (ShadowMap.SampleLevel(SATSampler, uv, 0, int2(-1,0))).xy;
		Summed -= (ShadowMap.SampleLevel(SATSampler, uv, 0, int2(0,-1))).xy;
		Summed += (ShadowMap.SampleLevel(SATSampler, uv, 0, int2(-1,-1))).xy;

		/*int2 poffset = (uv) * g_projLight.MapSize;
		int3 puv = int3(poffset, 0);
		Summed = ShadowMap.Load(puv, int2(0, 0)).xy;
		Summed -= ShadowMap.Load(puv, int2(-1, 0)).xy;
		Summed -= ShadowMap.Load(puv, int2(0, -1)).xy;
		Summed += ShadowMap.Load(puv, int2(-1, -1)).xy;*/
		numSample = 1;
	}
	else
	{
		Summed = (ShadowMap.SampleLevel(SATSampler, nuv, 0)).xy;
		Summed -= (ShadowMap.SampleLevel(SATSampler, nuv - float2(0, filterRadiusUV) * 2, 0)).xy;
		Summed -= (ShadowMap.SampleLevel(SATSampler, nuv - float2(filterRadiusUV, 0) * 2, 0)).xy;
		Summed += (ShadowMap.SampleLevel(SATSampler, nuv - float2(filterRadiusUV, filterRadiusUV) * 2, 0)).xy;

		/*int2 poffset = (uv) * g_projLight.MapSize;
		int3 puv = int3(poffset, 0);
		Summed = ShadowMap.Load(puv).xy;
		Summed -= ShadowMap.Load(puv - int3(0, numSample, 0)).xy;
		Summed -= ShadowMap.Load(puv - int3(numSample, 0, 0)).xy;
		Summed += ShadowMap.Load(puv - int3(numSample, numSample, 0)).xy;*/
	}

	Summed = (Summed + 0.5f) / (numSample * numSample);
	shadowed = ChebyshevUpperBound(Summed.xy, zReceiver);

	return shadowed;
}

//--------------------------------------------------------------------
//	PCF_Filter() - PCSS helper function
//--------------------------------------------------------------------
float PCF_Filter( float2 uv, float zReceiver, float filterRadiusUV,
				  uniform Texture2D ShadowMap, int nShadowSamples)
{
	float sum = 0.0f;
	/*float sum2 = 0.0f;
	int numSamples = 0;*/
	float2 offset;
	float shadowMapDepth;
	for( int i = 0; i < nShadowSamples; ++i )
	{	
		for( int j = 0; j < nShadowSamples; ++j )
		{
			offset = GetPoissonVal(i*nShadowSamples+j) * filterRadiusUV;
			shadowMapDepth = ShadowMap.SampleLevel(shdwSampler, uv + offset, 0).x;
			//shadowMapDepth = ShadowMap.GatherRed( shdwSampler, uv + offset, int2( 0, 0 ) ).x;

			//float4 d4 = ShadowMap.GatherCmpRed( shdwSamplerPointCmp, uv + offset, int2( 0, 0 ) );
			//float4 b4  = ( zReceiver.xxxx <= d4 ) ? (1.0).xxxx : (0.0).xxxx;

			sum += (zReceiver <= shadowMapDepth) ? 1 : 0;
			//sum += dot(d4, (1.0).xxxx);
		}
	}

	return sum / (nShadowSamples*nShadowSamples);
}

//--------------------------------------------------------------------
//	PCSS() - percentage-closer soft shadows
//--------------------------------------------------------------------
float PCSS( uniform Texture2D ProjShadowMap, float4 ProjTexCoord, 
			uniform int nBlockerSamples, uniform int nShadowSamples)
{
	float2 uv = ProjTexCoord.xy/ProjTexCoord.w;
#ifdef PCSS_SAT
	float zReceiver = ProjTexCoord.z; // Assumed to be eye-space z in this code
#else
	float zReceiver = ProjTexCoord.z/ProjTexCoord.w; // Assumed to be eye-space z in this code
#endif
	
	float lightSizeUV = g_projLight.LightSize / g_projLight.Scale;

	// STEP 1: blocker search
	float avgBlockerDepth = 0;
	float numBlockers = 0;
#ifdef PCSS_SAT
	FindBlocker_SAT( avgBlockerDepth, numBlockers, uv, zReceiver, ProjShadowMap, nBlockerSamples, lightSizeUV );
#else
	FindBlocker( avgBlockerDepth, numBlockers, uv, zReceiver, ProjShadowMap, nBlockerSamples, lightSizeUV );
#endif

	// There are no occluders so early out (this saves filtering)
	if( numBlockers < 1)		
		return 1.0f;

	// STEP 2: penumbra size
	float shadowed = 0.0f;
	float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth);
	float filterRadiusUV = penumbraRatio * lightSizeUV * g_projLight.NearFar.x / (zReceiver);
	float interpolated = lerp(g_projLight.LightSize,filterRadiusUV,g_projLight.PCSSAdjust);
	
	// STEP 3: filtering
#ifdef PCSS_SAT
	shadowed = PCF_Filter_SAT( uv, zReceiver, interpolated, ProjShadowMap, nShadowSamples );
#else
	shadowed = PCF_Filter( uv, zReceiver, interpolated, ProjShadowMap, nShadowSamples );
#endif

	// STEP 4: shadow intensity
	shadowed = lerp(1-g_projLight.ShadowIntensity, 1, shadowed);
	
	return shadowed;
}

// Illuminate* computes how much light arrives at point Ps 
//--------------------------------------------------------------------
//	IlluminatePointLight() - simple point light
//--------------------------------------------------------------------
void IlluminatePointLight(in float3 Ps, in LightInfo light, out IncidentLight OUT)
{
	// w = 1 for point light, w = 0 for directional.
	OUT.L = light.Pos.xyz - (Ps * light.Pos.w);

	float atten = attenuation(Ps, light);

	OUT.Cld = atten * light.Diffuse.rgb;
	OUT.Cls = atten * light.Specular.rgb;
}

float EllipticalPenumbra(float i_InnerAngle, float i_OuterAngle, float i_Aspect, float2 i_UV)
{
	// this is based on ellipses, see ARman section 14.3.1.
	float a = 0.5 * tan(i_InnerAngle*0.5)/tan(i_OuterAngle*0.5);
	float A = 0.5;
	float b = 0.5 * tan(i_Aspect*i_InnerAngle*0.5)/tan(i_Aspect*i_OuterAngle*0.5);
	float B = 0.5;
	float2 xy = abs(i_UV-0.5);
	float q = a*b/sqrt(b*b*xy.x*xy.x + a*a*xy.y*xy.y);
	float r = A*B/sqrt(B*B*xy.x*xy.x + A*A*xy.y*xy.y);
	return 1-smoothstep(q,r+0.001f,1);
//			float3 lvec = -normalize(OUT.L);
//			float3 ldir = -(light.ConeInfo.xyz);
//			return (dot(lvec, ldir) - cos(projLight.OuterAngle*0.5)) / (cos(projLight.InnerAngle*0.5) - cos(projLight.OuterAngle*0.5));
}

//--------------------------------------------------------------------
//	IlluminateProjLight() - projected light, no shadow
//--------------------------------------------------------------------
void IlluminateProjLight(in float3 Ps, in LightInfo light, in ProjLightInfo projLight, 
						 in Texture2D ProjTextureMap, out IncidentLight OUT)
{
	OUT.L = projLight.Pos.xyz - Ps;
	if (light.Pos.w == 0) // directional projected
		OUT.L = light.ConeInfo.xyz;

	OUT.Cld = OUT.Cls = float3(0,0,0);
	float4 projTexCoord = mul(projLight.Matrix, float4(Ps,1));
	if (projTexCoord.z >= 0 && projTexCoord.z < projLight.NearFar.y)
	{
		float4 tex_col = 1;
		if (g_bHasProjMap)
		{
			tex_col = ProjTextureMap.SampleLevel(projSampler, projTexCoord.xy/projTexCoord.w, 0);//tex2Dproj(ProjTextureMap, projTexCoord); 
		}
		float2 uv = projTexCoord.xy/projTexCoord.w;

#ifndef FC3D
		if (projLight.InnerAngle <= projLight.OuterAngle)
		{
			float p = EllipticalPenumbra(projLight.InnerAngle, 
				projLight.OuterAngle, projLight.Aspect, uv);
			tex_col *= p;
		}
		else
#endif
		{
			uv = step(0, uv) * step(uv, 1);
			tex_col *= uv.x * uv.y;
		}

		float atten = attenuation(Ps, light);

		float3 modifier = atten * tex_col.rgb;
		OUT.Cld = modifier * light.Diffuse.rgb;
		OUT.Cls = modifier * light.Specular.rgb;
	}
}

//--------------------------------------------------------------------
//	IlluminateProjLight() - projected light with shadow
//--------------------------------------------------------------------
void IlluminateProjLight(in float3 Ps, in LightInfo light, in ProjLightInfo projLight, 
						 in Texture2D ProjTextureMap, in Texture2D ProjShadowMap, 
						 in uniform int nBlockerSamples, in uniform int nShadowSamples,
						 out IncidentLight OUT)
{
	OUT.L = projLight.Pos.xyz - Ps;
	if (light.Pos.w == 0) // directional projected
		OUT.L = light.ConeInfo.xyz;

	OUT.Cld = OUT.Cls = float3(0,0,0);
	float4 projTexCoord = mul(projLight.Matrix, float4(Ps,1));
	if (projTexCoord.z >= 0 && projTexCoord.z < projLight.NearFar.y)
	{
		float4 tex_col = 1;
		if( g_bHasProjMap )
		{
			tex_col = ProjTextureMap.SampleLevel(projSampler, projTexCoord.xy/projTexCoord.w, 0);//tex2Dproj(ProjTextureMap, projTexCoord); 
		}

		float2 uv = projTexCoord.xy/projTexCoord.w;

#ifndef FC3D
		if (projLight.InnerAngle <= projLight.OuterAngle)
		{
			float p = EllipticalPenumbra(projLight.InnerAngle, 
				projLight.OuterAngle, projLight.Aspect, uv);
			tex_col *= p;
		}
		else
#endif
		{
			uv = step(0, uv) * step(uv, 1);
			tex_col *= uv.x * uv.y;
		}

		float atten = attenuation(Ps, light);

		float4 shadowCoeff = 1;
		if( g_bHasShadowMap )
		{
			shadowCoeff = PCSS(ProjShadowMap, projTexCoord, nBlockerSamples, nShadowSamples);
			//shadowCoeff = SampleShadowMap(ProjShadowMap, projTexCoord, nShadowSamples, g_projLight.LightSize);
			//shadowCoeff = CalculateVarianceShadow(ProjShadowMap, projTexCoord);
		}			

		// multiply texture color with shadow color
		float3 modifier = projLight.ShadowColor.rgb * atten * tex_col.rgb;
		// OUT.Cld = modifier * light.Diffuse.rgb;
		// OUT.Cls = modifier * light.Specular.rgb;
		OUT.Cld = lerp( modifier, atten * tex_col.rgb * light.Diffuse.rgb, shadowCoeff.rgb);
		OUT.Cls = lerp( modifier, atten * tex_col.rgb * light.Specular.rgb, shadowCoeff.rgb);
	}
}

//--------------------------------------------------------------------
//	IlluminateProjLightShadowOnly() - projected light with shadow
// no attenuation
// no projection
// dim values only if in the actual shadow.
// grayscale only, light intensity 1
// shadow intensity 1, shadow color 0
//--------------------------------------------------------------------
void IlluminateProjLightShadowsOnly(in float3 Ps, in LightInfo light, in ProjLightInfo projLight, 
						 in Texture2D ProjTextureMap, in Texture2D ProjShadowMap, 
						 in uniform int nBlockerSamples, in uniform int nShadowSamples,
						 out IncidentLight OUT)
{
	OUT.L = projLight.Pos.xyz - Ps;
	if (light.Pos.w == 0) // directional projected
		OUT.L = light.ConeInfo.xyz;

	// areas outside frustum default to black. 
	OUT.Cld = OUT.Cls = float3(0,0,0);

	float4 projTexCoord = mul(projLight.Matrix, float4(Ps,1));
	if (projTexCoord.z >= 0 && projTexCoord.z < projLight.NearFar.y)
	{
		float4 tex_col = 1;

		float4 shadowCoeff = 1;
		if (g_bHasShadowMap)
		{
			shadowCoeff = PCSS(ProjShadowMap, projTexCoord, nBlockerSamples, nShadowSamples);
			//shadowCoeff = SampleShadowMap(ProjShadowMap, projTexCoord, nShadowSamples, g_projLight.LightSize);
		}			

		// multiply texture color with shadow color
		float3 modifier = 0;
		//OUT.Cld = shadowCoeff.rgb;//lerp( modifier, light.Diffuse.rgb, shadowCoeff.rgb);
		//OUT.Cls = shadowCoeff.rgb;//lerp( modifier, light.Specular.rgb, shadowCoeff.rgb);

		// Use white color to indicate there's shadow so later the bumpnormal could actually "darken" the shadow color
		OUT.Cld = 1 - shadowCoeff.rgb;
		OUT.Cls = 1 - shadowCoeff.rgb;
	}
}

//--------------------------------------------------------------------
//	IlluminateImageBasedLight()
//--------------------------------------------------------------------
void IlluminateImageBasedLight(in float3 Ps, in float3 Ns, 
							   in float3 I,
							   in float diffuseFactor,
							   in bool bHasDiffuseEnvMap,
							   in TextureCube DiffuseEnvMap,
							   in float diffuseEnvAngle,
							   in float specularFactor,
							   in bool bHasSpecularEnvMap,
							   in TextureCube SpecularEnvMap,
							   in float specularEnvAngle,
							   out IncidentLight OUT)
{
	// light incident is equal to opposite of normal,
	// for environment lights.
	OUT.L = -Ns;
	OUT.Cld = OUT.Cls = float3(0,0,0);

	// sample diffuse using normal vector
	if (diffuseFactor > 0)
	{
		if (bHasDiffuseEnvMap)
		{
			float3 Nr = rotateAboutY(Ns, diffuseEnvAngle);
			OUT.Cld = DiffuseEnvMap.Sample( CubeSampler, Nr).rgb;
		}
	}

	// sample specular using reflection vector
	if (specularFactor > 0)
	{
		if (bHasSpecularEnvMap)
		{
			float3 Nr = rotateAboutY(Ns, specularEnvAngle);

			// I points from eyepos to Ps.
			// world eye dir points FROM shade point TO eye pos so use -I
			// do world space reflection
			float NdotI = dot(-I, Nr);
			//R = 2*N*(L.N)-L
			float3 reflVect = 2.0 * NdotI * Nr - (-I);
			OUT.Cls = SpecularEnvMap.Sample( CubeSampler, reflVect).rgb;
		}
	}
}


/*************** eof ****************/
