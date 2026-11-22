//////////////////////////////////////////////////////////////////////////////
// Converted from DepthMap.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// DepthMap.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  MaskAlpha.fx
**
**      Fixed single color flat rendering for shader array,
**	skips pixels where the texture's alpha is below a threshold.
**
**	Gigawatt Studios
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

/*********** support data and functions ******/

#include "../Materials/Support.hlsli"
#include "../Materials/Tessellate.hlsli"
#include "../Materials/Lighting.hlsli"

// Register layout shared with the materials: see ../Materials/Globals.hlsli.
// b4: this shader's own parameters (defaults are in DepthMap.effect.json)
cbuffer MaterialParams : register(b4)
{
	float4 solidColor;				// default (0.5,0.5,0.5,1)
	//bool hasDiffuseMap = false;
	//texture diffuseMap;
	bool hasTransparencyMap;		// default false
	//float alphaThreshold = 0.1;
	float g_transparency;			// default 1

	// larger bias will make shadows MORE transparent
	bool g_bUseDither;				// default false
	float g_DitherAlphaBias;		// default 0.85
};

Texture2D transparencyMap : register(t4);
//texture2D noiseMap 
//<
//	string name="Random1024_1024.dds";
//>;
//sampler2D noiseSampler = sampler_state
//{
//	Texture = <noiseMap>;
//	MinFilter = Linear;
//	MagFilter = Linear;
//	AddressU = Wrap;
//	AddressV = Wrap;
//};

/************* DATA STRUCTS **************/

/* data passed from vertex shader to pixel shader */
struct VertexOutput
{
    float4 HPosition	: SV_POSITION;
    float2 TexCoord0	: TEXCOORD0;
};

/* data passed from vertex shader to hull and domain shader for tessellation */
struct VertexOutputTess
{
    float2 TexCoord0	: TEXCOORD0;
    float2 UV			: TEXCOORD1;
    float3 Position		: TEXCOORD2;
	float3 Normal		: TEXCOORD3;
};

/*********** vertex shader ******/

VertexOutput DepthVS_Default( STANDARD_VERTEX IN, out float ClipDist : SV_ClipDistance0 )
{
    VertexOutput OUT;
    
    OUT.TexCoord0 = mul(g_uvTransform, float4(IN.UV,0,1)).xy;
    
    // Output vertex position
	float4 Po = float4(IN.Position, 1.0f);
    
    //OUT.HPosition = mul( g_wvp, Po );
    OUT.HPosition = TransformVertex( Po, IN.UV, g_wvp );

	float3 Pw = mul( g_world, Po ).xyz;
	ClipDist = ClipWorldPos( Pw );
    
    return OUT;
}

VertexOutputTess DepthVS_Tess( STANDARD_VERTEX IN )
{
    VertexOutputTess OUT;
    
    OUT.UV = IN.UV;
    OUT.TexCoord0 = mul(g_uvTransform, float4(IN.UV,0,1)).xy;
    
    // Output vertex position
	float4 Po = float4(IN.Position, 1.0f);
    float4 Pp = mul(g_wvp, Po);
    
    OUT.Position = mul( g_world, Po ).xyz;
	OUT.Normal = mul( (float3x3)g_world, IN.Normal );
    
    return OUT;
}

/********* pixel shader ********/

float4 DistributePrecision(float2 Moments)  
{  
  float FactorInv = 1 / g_DistributeFactor;  
  // Split precision  
   float2 IntPart;  
  float2 FracPart = modf(Moments * g_DistributeFactor, IntPart);  
  // Compose outputs to make reconstruction cheap.  
   return float4(IntPart * FactorInv, FracPart);  
} 

pixelOutput labPS( VertexOutput IN) 
{
    pixelOutput OUT;

	float depth = IN.HPosition.z;	//normalized screen depth
	float Alpha = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.TexCoord0, g_transparency).r;

	float threshold = g_AlphaTestRef;
	// dither shadowmap based on alpha value:
	if (g_bUseDither)
	{
		// if (rand() > alpha) then skip the pixel.
		// select random number from texture, based on pixel coordinates.
//		float2 uv;
//		uv.x = (IN.Depth.x/IN.Depth.w + 1) * 0.5;
//		uv.y = (IN.Depth.y/IN.Depth.w + 1) * 0.5;
//		threshold += tex2D(noiseSampler, uv).x;
		threshold += g_DitherAlphaBias;
	}
	
	// low alphas will have more pixels skipped.
	// clip skips pixel if value is negative
//	clip(alpha-threshold);
	if( threshold >= Alpha ) discard;

	// put light space projected depth in shadow map color buffer.
	OUT.col = float4(depth,depth,depth,1);	

#ifdef PCSS_SAT	
	// Variance shadow map
	float vz = depth;
	float vz_s;
	// Reduce depth bias
	
	float dx = ddx(vz);
	float dy = ddy(vz);

	vz_s = vz * vz + 0.25 * (dx*dx + dy*dy);
	//vz_s = vz * vz;
	
	OUT.col = float4(vz-0.5, vz_s-0.5, 1,1);	
#endif
	//OUT.col = DistributePrecision(float2(vz, vz_s));
	
/*****
    OUT.col = IN.diffCol;
    //
    // Depth is z / w
    //
    OUT.col.r = IN.Depth.z / IN.Depth.w;
    // stick alpha value into g component.I think 
    OUT.col.g = alpha;
*****/

    return OUT;
}

pixelOutput ZFillPS( VertexOutput IN ) 
{
	float Alpha = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= Alpha ) discard;

    return (pixelOutput)float4(0,0,0,1.0f);
}

pixelOutput ZFillRGBAPS( VertexOutput IN ) 
{
	float Alpha = Tex2DCombine(hasTransparencyMap, transparencyMap, IN.TexCoord0, g_transparency).r;
	if( g_AlphaTestRef >= Alpha ) discard;

    return (pixelOutput)float4(1.0f,1.0f,1.0f,1.0f);
}

pixelOutput AlphaFillPS( VertexOutput IN ) 
{
	// assumes we are rendering opaque geometry.
	float alpha = g_transparency;
	if( hasTransparencyMap )
	{
		alpha *= transparencyMap.Sample( g_DefaultSampler, IN.TexCoord0.xy ).r;
	}
	
	return (pixelOutput)float4(0,0,0,alpha);
}

pixelOutput BinaryAlphaFillPS( VertexOutput IN) 
{
	// assumes we are rendering opaque geometry.
	float alpha = g_transparency;
	if( hasTransparencyMap )
	{
		alpha *= transparencyMap.Sample( g_DefaultSampler, IN.TexCoord0.xy ).r;
	}
	
	if ( alpha > 0.5)
	{
		alpha = 1.0f;
	}
	else
	{
		alpha = 0.0f;
	}
	
	return (pixelOutput)float4(0,0,0,alpha);
}

pixelOutput AlphaFillRGBAPS( VertexOutput IN ) 
{
	// assumes we are rendering opaque geometry.
	float alpha = g_transparency;
	if( hasTransparencyMap )
	{
		alpha *= transparencyMap.Sample( g_DefaultSampler, IN.TexCoord0.xy ).r;
	}
	
	return (pixelOutput)float4(alpha, alpha, alpha, alpha);
}

pixelOutput BinaryAlphaFillRGBAPS( VertexOutput IN) 
{
	// assumes we are rendering opaque geometry.
	float alpha = g_transparency;
	if( hasTransparencyMap )
	{
		alpha *= transparencyMap.Sample( g_DefaultSampler, IN.TexCoord0.xy ).r;
	}
	
	if ( alpha > 0.5)
	{
		alpha = 1.0f;
	}
	else
	{
		alpha = 0.0f;
	}
	
	return (pixelOutput)float4(alpha, alpha, alpha, alpha);
}

//--------------------------------------------------------------------------------------
// Hull shader
//--------------------------------------------------------------------------------------

HS_CONSTANT_DATA_OUTPUT Depth_Constants_HS( InputPatch<VertexOutputTess, 3> inputPatch )
{
	return AdaptiveTessellate(inputPatch[0].Position,
		inputPatch[1].Position,
		inputPatch[2].Position);
}

[domain("tri")]
[partitioning(SGPU_PARTITIONING)]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("Depth_Constants_HS")]
VertexOutputTess Depth_HS( InputPatch<VertexOutputTess, 3> inputPatch, uint uCPID : SV_OutputControlPointID )
{
	return inputPatch[uCPID];
}

//--------------------------------------------------------------------------------------
// Domain Shader
//--------------------------------------------------------------------------------------

[domain("tri")]
VertexOutput Depth_DS( HS_CONSTANT_DATA_OUTPUT input, float3 BarycentricCoordinates : SV_DomainLocation, 
						 const OutputPatch<VertexOutputTess, 3> TrianglePatch, out float ClipDist : SV_ClipDistance0 )
{
	VertexOutput output = (VertexOutput)0;

	// Interpolate world space position with barycentric coordinates

	float3 vWorldPos = BarycentricCoordinates.x * TrianglePatch[0].Position + 
		BarycentricCoordinates.y * TrianglePatch[1].Position + 
		BarycentricCoordinates.z * TrianglePatch[2].Position;

	float2 TexCoord0 = BarycentricCoordinates.x * TrianglePatch[0].TexCoord0 + 
		BarycentricCoordinates.y * TrianglePatch[1].TexCoord0 + 
		BarycentricCoordinates.z * TrianglePatch[2].TexCoord0;
		
	float2 UV = BarycentricCoordinates.x * TrianglePatch[0].UV + 
		BarycentricCoordinates.y * TrianglePatch[1].UV + 
		BarycentricCoordinates.z * TrianglePatch[2].UV;

	float3 vNormal  = BarycentricCoordinates.x * TrianglePatch[0].Normal + 
		BarycentricCoordinates.y * TrianglePatch[1].Normal + 
		BarycentricCoordinates.z * TrianglePatch[2].Normal;

	output.TexCoord0 = TexCoord0;

	//need normal to displace
#ifdef APPLY_DISPLACEMENT
	vWorldPos = DisplaceVertex( vWorldPos, vNormal, TexCoord0 );
#endif

	// Transform world position with viewprojection matrix
	//output.HPosition = mul( g_vp, float4( vWorldPos, 1.0 ) );
	output.HPosition = TransformVertex( float4( vWorldPos, 1.0 ), UV, g_vp );

	ClipDist = ClipWorldPos( vWorldPos );

	return output;
}


//-----------------------------------------------------------------------------
// TECHNIQUES
//-----------------------------------------------------------------------------


/***************************** eof ***/
