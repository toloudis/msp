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

#include "..\Support.h"
#include "..\Tessellate.h"

float4 solidColor
<
	string SasUiControl = "ConstantColor";
	string SasUiDescription = "color for constant solid shading";
	string SasUiLabel = "Solid Color";
>
= float4(0.5,0.5,0.5,1);

bool hasDiffuseMap = false;
Texture2D diffuseMap;
//float alphaThreshold = 0.1;

/************* DATA STRUCTS **************/

struct VertexOutput
{
    float4 HPosition	: SV_POSITION;
    float2 TexCoord0	: TEXCOORD0;
    float  Depth		: TEXCOORD1;		//linear view space depth
    float4 diffCol		: COLOR0;
};

struct VertexOutputTess
{
    float2 TexCoord0	: TEXCOORD0;
    float4 diffCol		: COLOR0;
    float3 Position		: TEXCOORD2;
	float3 Normal		: TEXCOORD3;
};

/*********** vertex shader ******/

VertexOutput MaskVS_Default( STANDARD_VERTEX In, uniform bool bUseColor, uniform bool bTransformUVs, out float ClipDist : SV_ClipDistance0 )
{
    VertexOutput OUT;
    
    // Always black in single light, all color comes from ambient pass
	OUT.diffCol = bUseColor ? solidColor : float4(0,0,0,1);
    
    // Single texture layer
	OUT.TexCoord0 = bTransformUVs ? mul(g_uvTransform, float4(In.UV,0,1)).xy : In.UV;
    
    // Output vertex position
	float4 Po = float4(In.Position, 1.0f);
    OUT.HPosition = mul( g_wvp, Po );
    OUT.Depth = mul( g_wv, Po).z;

	float3 Pw = mul( g_world, Po ).xyz;
	ClipDist = ClipWorldPos( Pw );
    
    return OUT;
}

VertexOutputTess MaskVS_Tess( STANDARD_VERTEX In, uniform bool bUseColor, uniform bool bTransformUVs )
{
    VertexOutputTess OUT;
    
    // Always black in single light, all color comes from ambient pass
	OUT.diffCol = bUseColor ? solidColor : float4(0,0,0,1);
    
    // Single texture layer
	OUT.TexCoord0 = bTransformUVs ? mul(g_uvTransform, float4(In.UV,0,1)).xy : In.UV;
    
    // Output vertex position
	float4 Po = float4(In.Position, 1.0f);
    OUT.Position = mul( g_world, Po ).xyz;
    OUT.Normal = mul( (float3x3)g_world, In.Normal );
    
    return OUT;
}

/********* pixel shader ********/

pixelOutput labPS( VertexOutput IN,
				  uniform Texture2D DiffuseMap) 
{
    pixelOutput OUT; 
    
    if (hasDiffuseMap)
    {
		// Clip pixel if the alpha value is below a threshold
		float alpha = DiffuseMap.Sample(g_DefaultSampler, IN.TexCoord0.xy).a;
		clip(alpha - g_AlphaTestRef); // clip skips pixel if value is negative
	}
	
    OUT.col = IN.diffCol;
    return OUT;
}

pixelOutput pickPS( VertexOutput IN,
				  uniform Texture2D DiffuseMap) 
{
    pixelOutput OUT; 
    
    if (hasDiffuseMap)
    {
		// Clip pixel if the alpha value is below a threshold
		float alpha = DiffuseMap.Sample(g_DefaultSampler, IN.TexCoord0.xy).a;
		clip(alpha - g_AlphaTestRef); // clip skips pixel if value is negative
	}
	
    OUT.col = float4(IN.diffCol.x, IN.TexCoord0.x, IN.TexCoord0.y, IN.Depth);
    return OUT;
}

//--------------------------------------------------------------------------------------
// Hull shader
//--------------------------------------------------------------------------------------

HS_CONSTANT_DATA_OUTPUT Mask_Constants_HS( InputPatch<VertexOutputTess, 3> inputPatch )
{
	return AdaptiveTessellate(inputPatch[0].Position,
		inputPatch[1].Position,
		inputPatch[2].Position);
}

[domain("tri")]
[partitioning(SGPU_PARTITIONING)]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("Mask_Constants_HS")]
VertexOutputTess Mask_HS( InputPatch<VertexOutputTess, 3> inputPatch, uint uCPID : SV_OutputControlPointID )
{
	return inputPatch[uCPID];
}

//--------------------------------------------------------------------------------------
// Domain Shader
//--------------------------------------------------------------------------------------

[domain("tri")]
VertexOutput Mask_DS( HS_CONSTANT_DATA_OUTPUT input, float3 BarycentricCoordinates : SV_DomainLocation, 
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

	float4 vDiffCol  = BarycentricCoordinates.x * TrianglePatch[0].diffCol + 
		BarycentricCoordinates.y * TrianglePatch[1].diffCol + 
		BarycentricCoordinates.z * TrianglePatch[2].diffCol;

	float3 vNormal  = BarycentricCoordinates.x * TrianglePatch[0].Normal + 
		BarycentricCoordinates.y * TrianglePatch[1].Normal + 
		BarycentricCoordinates.z * TrianglePatch[2].Normal;

	//need normal to displace
#ifdef APPLY_DISPLACEMENT
	vWorldPos = DisplaceVertex( vWorldPos, vNormal, TexCoord0 );
#endif

    output.diffCol = vDiffCol;
	output.TexCoord0 = TexCoord0;
	output.Depth = mul( g_view, float4( vWorldPos, 1.0f)).z;

	// Transform world position with viewprojection matrix
	output.HPosition = mul( g_vp, float4( vWorldPos, 1.0 ) );

	ClipDist = ClipWorldPos( vWorldPos );

	return output;
}

#define MASK_HULL_AND_DOMAIN_Default

#define MASK_HULL_AND_DOMAIN_Tess\
	SetHullShader(CompileShader(hs_5_0, Mask_HS()));\
	SetDomainShader(CompileShader(ds_5_0, Mask_DS()));

//-----------------------------------------------------------------------------
// TECHNIQUES
//-----------------------------------------------------------------------------


technique11 Default
{
#define PASS_DEFAULT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 MaskVS_##PassName( true, false );\
		PixelShader = compile ps_5_0 labPS(diffuseMap);\
		MASK_HULL_AND_DOMAIN_##PassName\
	}
PASS_DEFAULT(Default)
PASS_DEFAULT(Tess)
}

technique11 SingleLight
{
#define PASS_SINGLELIGHT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 MaskVS_##PassName( false, false );\
		PixelShader = compile ps_5_0 labPS(diffuseMap);\
		MASK_HULL_AND_DOMAIN_##PassName\
	}
PASS_SINGLELIGHT(Default)
PASS_SINGLELIGHT(Tess)
}
technique11 ProjectedLight
{
#define PASS_PROJECTEDLIGHT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 MaskVS_##PassName( false, false );\
		PixelShader = compile ps_5_0 labPS(diffuseMap);\
		MASK_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHT(Default)
PASS_PROJECTEDLIGHT(Tess)
}
technique11 ProjectedLightSuperSample
{
#define PASS_PROJECTEDLIGHTSS(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 MaskVS_##PassName( false, false );\
		PixelShader = compile ps_5_0 labPS(diffuseMap);\
		MASK_HULL_AND_DOMAIN_##PassName\
	}
PASS_PROJECTEDLIGHTSS(Default)
PASS_PROJECTEDLIGHTSS(Tess)
}

technique11 Glow
{
#define PASS_GLOW(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 MaskVS_##PassName( false, false );\
		PixelShader = compile ps_5_0 labPS(diffuseMap);\
		MASK_HULL_AND_DOMAIN_##PassName\
	}
PASS_GLOW(Default)
PASS_GLOW(Tess)
}
technique11 Matte
{
#define PASS_MATTE(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 MaskVS_##PassName( true, false );\
		PixelShader = compile ps_5_0 simpleMattePS();\
		MASK_HULL_AND_DOMAIN_##PassName\
	}
PASS_MATTE(Default)
PASS_MATTE(Tess)
}
technique11 Environment
{
#define PASS_ENVIRONMENT(PassName)	\
	pass P##PassName			\
	{						\
		VertexShader = compile vs_5_0 MaskVS_##PassName( true, true );\
		PixelShader = compile ps_5_0 pickPS(diffuseMap);\
		MASK_HULL_AND_DOMAIN_##PassName\
	}
PASS_ENVIRONMENT(Default)
PASS_ENVIRONMENT(Tess)
}

technique11 DOFPrep
{
#define PASS_DOFPREP(PassName)	\
	pass P##PassName			\
	{						\
		SetVertexShader(CompileShader(vs_5_0, DOFPrepVS_##PassName()));\
		SetPixelShader(CompileShader(ps_5_0, DOFPrep_PS()));\
		DOF_HULL_AND_DOMAIN_##PassName\
	}
PASS_DOFPREP(Default)
PASS_DOFPREP(Tess)
}

/***************************** eof ***/
