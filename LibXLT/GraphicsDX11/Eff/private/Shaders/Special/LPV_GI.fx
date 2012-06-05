/*****************************************************************************
**  LPV_GI.fx
**
**      Light Propogation Volume: Inject VPL into radiance volume
**
**	Gigawatt Studios
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "..\Globals.h"
#include "..\Support.h"
#include "..\Tessellate.h"
/*********** support data and functions ******/
//float4x4 g_world : World;
//float4x4 g_wvp : WorldViewProjection;

float4 g_VolumeOrigin = 0;		// volume origin in world space
float4 g_InvVolumeWidth = 0;	// inv volume size in world space
//int g_Cells = 1;				// number of volume cells
float4 g_LightPos = 0;			// Light dir from world space position to light position
float g_PointWeight = 0;		// vpl weight
int g_TexelNum = 1;				// volume width
float g_GIScale = 1.0f;			// GI scale
float g_TexelSize = 1;			// inverse volume width
//int g_RSMSize = 1;
float3 g_CellWidth = 1.0f;		// cell width in world space
float4 g_LightColor = 0;
float g_GIFalloff = 1.0f;

// object original color attributes
float4 g_emissiveColor = float4(0,0,0,0);
float g_emissiveIntensity = 1.0f;
float4 g_diffuseColor = float4(0,0,0,0);
bool hasDiffuseMap = false;
bool g_bIsReceivesGI = false;

float2 ZH_Coeff = float2(0.886224f, 1.023326f);
float2 SHBasisPoly = float2(0.282095f, 0.488603f);
Texture2D g_diffuseMap;

Texture2D projRSMPosMap;
Texture2D projRSMNormalMap;
Texture2D projRSMFluxMap;
Texture3D RVVolumeMapR;
Texture3D RVVolumeMapG;
Texture3D RVVolumeMapB;
SamplerState RSMSampler
{
	FILTER = MIN_MAG_MIP_POINT;
	AddressU = Border;
	AddressV = Border;
	AddressW = Border;
	BorderColor = float4(0,0,0,0);
};

SamplerState LPVSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Border;
	AddressV = Border;
	AddressW = Border;
	BorderColor = float4(0,0,0,0);
};

/************* DATA STRUCTS **************/

/* data passed from vertex shader to pixel shader */
struct VertexOutput
{
    float4 HPosition	: SV_POSITION;
    float2 UV			: TEXCOORD0;
};

//struct GIVertexOutput
//{
    //float4 HPosition	: SV_POSITION;
    //float3 TexCoord3D	: TEXCOORD0;
    //float3 Normal		: TEXCOORD1;
    //float2 UV			: TEXCOORD2;
//};

/*********** vertex shader ******/

VertexOutput VS( float4 Pos: SV_POSITION, float2 UV: TEXCOORD0)
{
	VertexOutput Out;
	
	Out.HPosition = Pos;
	Out.UV = UV;
	
	return Out;
}

TANGENT_VERTEX_OUTPUT VS_GI_Default(STANDARD_VERTEX In, out float ClipDist : SV_ClipDistance0 )
{
	//GIVertexOutput Out;
	
	//Out.UV = mul(g_uvTransform, float4(Vtx.UV,0,1)).xy;
	//
	//float4 Po = float4(Vtx.Position, 1.0f);
	//float4 Pw = mul(g_world, Po);
	//float4 Pp = mul(g_wvp, Po);
	//
	//float4 No = float4(Vtx.Normal, 0.0f);
    //float4 Nw = mul(g_world, No);
	//
	//float3 grid_pos = saturate((Pw.xyz - g_VolumeOrigin.xyz) * g_InvVolumeWidth.xyz);
	//
	//Out.HPosition = Pp;
	//Out.TexCoord3D = float3(grid_pos.x, 1.0f - grid_pos.y, grid_pos.z);
	//Out.Normal = normalize(Nw.xyz);
	//
	//return Out;
	
	TANGENT_VERTEX_OUTPUT Out;

	float4 Pos = float4(In.Position, 1.0f);
	Out.V.WorldPos = mul(g_world, Pos ).xyz;

	Out.V.UV = In.UV;
	Out.V.TexCoord0 = mul(g_uvTransform, float4(In.UV,0,1)).xy;
	Out.V.WorldTan = TransformTangents( g_world, In.T, In.B, In.Normal );

	Out.ScreenPos = mul( g_wv, Pos ).xyz;
    Out.HPosition = TransformVertex( Pos, In.UV, g_wvp);
    
    // use unused screenpos to store 3d texture coord
    float3 grid_pos = saturate((Out.V.WorldPos.xyz - g_VolumeOrigin.xyz) * g_InvVolumeWidth.xyz);
    Out.ScreenPos = float3(grid_pos.x, 1.0f - grid_pos.y, grid_pos.z);

	ClipDist = ClipWorldPos( Out.V.WorldPos );

    return Out;
}

TANGENT_TESS_OUTPUT VS_GI_Tess( STANDARD_VERTEX In )
{
    TANGENT_TESS_OUTPUT Out;

	float4 Pos = float4(In.Position, 1.0f);

    Out.V.WorldPos = mul(g_world, Pos).xyz;

	Out.V.UV = In.UV;
	Out.V.TexCoord0 = mul(g_uvTransform, float4(In.UV,0,1)).xy;
	Out.V.WorldTan = TransformTangents( g_world, In.T, In.B, In.Normal );

    return Out;
}

//--------------------------------------------------------------------------------------
// Hull Shader
//--------------------------------------------------------------------------------------
[domain("tri")]
[partitioning(SGPU_PARTITIONING)]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("Constants_HS")]
TANGENT_TESS_OUTPUT GI_HS( InputPatch<TANGENT_TESS_OUTPUT, 3> inputPatch,uint uCPID : SV_OutputControlPointID )
{
	return inputPatch[uCPID];
}

//--------------------------------------------------------------------------------------
// Domain Shader
//--------------------------------------------------------------------------------------

[domain("tri")]
TANGENT_VERTEX_OUTPUT GI_DS( HS_CONSTANT_DATA_OUTPUT input, float3 BarycentricCoordinates : SV_DomainLocation, 
						 const OutputPatch<TANGENT_TESS_OUTPUT, 3> TrianglePatch, out float ClipDist : SV_ClipDistance0 )
{
	TANGENT_VERTEX_OUTPUT output = (TANGENT_VERTEX_OUTPUT)0;

	// Interpolate world space position with barycentric coordinates
	float3 vWorldPos = BarycentricCoordinates.x * TrianglePatch[0].V.WorldPos + 
		BarycentricCoordinates.y * TrianglePatch[1].V.WorldPos + 
		BarycentricCoordinates.z * TrianglePatch[2].V.WorldPos;

	float3 vWorldTanMatrixX = BarycentricCoordinates.x * TrianglePatch[0].V.WorldTan.X + 
		BarycentricCoordinates.y * TrianglePatch[1].V.WorldTan.X + 
		BarycentricCoordinates.z * TrianglePatch[2].V.WorldTan.X;
	float3 vWorldTanMatrixY = BarycentricCoordinates.x * TrianglePatch[0].V.WorldTan.Y + 
		BarycentricCoordinates.y * TrianglePatch[1].V.WorldTan.Y + 
		BarycentricCoordinates.z * TrianglePatch[2].V.WorldTan.Y;
	float3 vWorldTanMatrixZ = BarycentricCoordinates.x * TrianglePatch[0].V.WorldTan.Z + 
		BarycentricCoordinates.y * TrianglePatch[1].V.WorldTan.Z + 
		BarycentricCoordinates.z * TrianglePatch[2].V.WorldTan.Z;

	float2 UV = BarycentricCoordinates.x * TrianglePatch[0].V.UV + 
		BarycentricCoordinates.y * TrianglePatch[1].V.UV + 
		BarycentricCoordinates.z * TrianglePatch[2].V.UV;
	float2 TexCoord0 = BarycentricCoordinates.x * TrianglePatch[0].V.TexCoord0 + 
		BarycentricCoordinates.y * TrianglePatch[1].V.TexCoord0 + 
		BarycentricCoordinates.z * TrianglePatch[2].V.TexCoord0;

	float3 TanNormal = float3(0,0,1);
#ifdef APPLY_DISPLACEMENT
	vWorldPos = DisplaceVertexNormal( vWorldPos, vWorldTanMatrixZ, TexCoord0, TanNormal );
#endif

	output.V.UV = UV;
	output.V.TexCoord0 = TexCoord0;

	float3x3 TanMat = {vWorldTanMatrixX, vWorldTanMatrixY, vWorldTanMatrixZ};

	// Rotate tangent frame to line up to the bumped normal (for subsequent displacements or normal-mapping)
	float3x3 mBumpRotation = MakeFromToRotationMatrixFast( normalize( vWorldTanMatrixZ ), TanNormal );

	// Convert normal to world space
	output.V.WorldTan.Z = mul( TanNormal, TanMat );
	//transform tangent frame
	output.V.WorldTan.X = mul( mBumpRotation, vWorldTanMatrixX );
	output.V.WorldTan.Y = mul( mBumpRotation, vWorldTanMatrixY );
/*

#ifdef BICUBIC_DISPLACEMENT
	//alter normal and align other vectors of tangent frame basis
	vWorldTanMatrixZ = TanNormal;	//normalized tangent Z
	vWorldTanMatrixX = cross( vWorldTanMatrixZ, vWorldTanMatrixY );	//X orthonogal to new Z and Y
	vWorldTanMatrixY = cross( vWorldTanMatrixX, vWorldTanMatrixZ ); //Y orthogonal to X and Z
#endif
	//transform from tangent space to world space
	output.V.WorldTan = TransformTangents( g_world, vWorldTanMatrixX, vWorldTanMatrixY, vWorldTanMatrixZ );
*/
	output.V.WorldPos = vWorldPos;

	// Transform world position with viewprojection matrix
	output.HPosition = mul( g_vp, float4( vWorldPos, 1.0 ) );

	float3 grid_pos = saturate((vWorldPos.xyz - g_VolumeOrigin.xyz) * g_InvVolumeWidth.xyz);
    output.ScreenPos = float3(grid_pos.x, 1.0f - grid_pos.y, grid_pos.z);

	ClipDist = ClipWorldPos( vWorldPos );

	return output;
}



/*********** geometry shader ******/
struct GSOutput
{
    float4 HPosition	: SV_POSITION;
    float2 TexCoord0	: TEXCOORD0;
    float4 Normal		: TEXCOORD1;
    float4 Color		: TEXCOORD2;
    uint RTIndex        : SV_RenderTargetArrayIndex;
};

[maxvertexcount (1)]
void GS_Inject(point VertexOutput In[1], inout PointStream<GSOutput> pStream)
{
    GSOutput Out;
    
	float4 pw = projRSMPosMap.SampleLevel(LPVSampler, In[0].UV, 0);
	float4 normal = projRSMNormalMap.SampleLevel(RSMSampler, In[0].UV, 0);
	float4 flux = projRSMFluxMap.SampleLevel(LPVSampler, In[0].UV, 0);
	float3 LightDir = normalize(g_LightPos.xyz - pw.xyz);
	
	float3 grid_pos = (pw.xyz - g_VolumeOrigin.xyz) * g_InvVolumeWidth.xyz;
	grid_pos += ((normal.xyz) * g_TexelSize * 0.5f  + LightDir * g_TexelSize) * 0.5f;
	
	//grid_pos += (normal.xyz) * g_TexelSize * 1.0f;
	
	if (grid_pos.x < 0 || grid_pos.x >= 1.0f ||
		grid_pos.y < 0 || grid_pos.y >= 1.0f ||
		grid_pos.z < 0 || grid_pos.z >= 1.0f)
	{
	}else
	{
	
	Out.RTIndex = round(grid_pos.z * g_TexelNum);
	
    
    Out.HPosition		= float4(clamp(2.0f * grid_pos.xy - 1.0f, -1.0f, 1.0f), 0.5f, 1.0f); 
    Out.TexCoord0		= In[0].UV;
    Out.Normal			= normal;
    Out.Color           = flux;
    pStream.Append( Out );
    }
}

struct GSVolumeOutput
{
    float4 HPosition	: SV_POSITION;
    int3 TexCoord0		: TEXCOORD0;
    uint RTIndex        : SV_RenderTargetArrayIndex;
};

[maxvertexcount (128)]
void GS_Volume(point VertexOutput In[1], inout PointStream<GSVolumeOutput> pStream)
{
    GSVolumeOutput Out;
    
    for (int i = 0; i < g_TexelNum; i++)
    {
		Out.RTIndex = i;
		Out.HPosition = float4(In[0].HPosition.xy, 0.0f, 1.0f);
		Out.TexCoord0 = int3(floor(In[0].UV * (float)g_TexelNum), i);
		pStream.Append( Out );
    }
}

/********* pixel shader ********/
//float4 sh_rotate(float3 dir, float2 zh_coeffs) 
//{ 
	//float2 theta12_cs = 0.0f;
	//float2 phi12_cs = 0.0f; 
//
	//if (dir.z * dir.z < 0.99)
	//{
		//theta12_cs = normalize(dir.xy);
		//phi12_cs.x = sqrt(1.0 - dir.z * dir.z); 	
	//}
	//
//
	//phi12_cs.y = dir.z;
//
	//float4 result;
//
	//result.x = zh_coeffs.x;
//
	//result.y = zh_coeffs.y * phi12_cs.x * theta12_cs.y; 
	//result.z = -zh_coeffs.y * phi12_cs.y; 
	//result.w = zh_coeffs.y * phi12_cs.x * theta12_cs.x;
	//
	//return result;
//}
//
//float4 sh_project_cone(float3 dir) 
 //{
	//const float2 zh_coeffs = float2(0.25f, 0.5);
	//return sh_rotate(dir, zh_coeffs);
//}
//
//float2 GetZHCoeff(float angle)
//{
	//float2 c = float2(0.5f * (1.0f - cos(angle)), 0.75f * sin(angle) * sin(angle));
	//return c;
//}

float4 GetSHCosine(float3 dir)
{
	dir = normalize(dir);
	float4 result = 0;
	result.x = ZH_Coeff.x;
	result.yzw = float3(-ZH_Coeff.y, ZH_Coeff.y, -ZH_Coeff.y) * dir.yzx;
	
	return result;
}

struct LPVPixelOutput
{
	float4 r	: SV_Target0;
	float4 g	: SV_Target1;
	float4 b	: SV_Target2;
};

LPVPixelOutput PS_Inject( GSOutput IN) 
{
	LPVPixelOutput OUT;
	
	//float4 sh = sh_project_cone(IN.Normal.xyz);
	//
	//float3 radiosity = IN.Color.xyz * g_LightColor.xyz * g_PointWeight;
		//
	//OUT.r = radiosity.r * sh;
	//OUT.g = radiosity.g * sh;
	//OUT.b = radiosity.b * sh;
	float4 sh = GetSHCosine(IN.Normal.xyz);
	
	float3 radiosity = IN.Color.xyz * g_LightColor.xyz * g_PointWeight;
		
	OUT.r = radiosity.r * sh;
	OUT.g = radiosity.g * sh;
	OUT.b = radiosity.b * sh;
	
	return OUT;
	
}

void GatherRadiance(int3 offset, float4 dir, int3 UV,
					inout float4 cr, inout float4 cg, inout float4 cb )
{
	//float falloff = 1.0f / (1.0f + abs(dot((float3)offset, g_CellWidth)) * g_GIFalloff);
	//float falloff = 1.0f;
	float4 coeff_r = RVVolumeMapR.Load(int4(UV, 0), offset);
	float4 coeff_g = RVVolumeMapG.Load(int4(UV, 0), offset);
	float4 coeff_b = RVVolumeMapB.Load(int4(UV, 0), offset);
	
	float r = max(dot(coeff_r, dir), 0) * 0.6666f * PI * g_GIFalloff;// / (PI);// / (cellWidth * g_GIFalloff);
	float g = max(dot(coeff_g, dir), 0) * 0.6666f * PI * g_GIFalloff;// / (PI);// / (cellWidth * g_GIFalloff);
	float b = max(dot(coeff_b, dir), 0) * 0.6666f * PI * g_GIFalloff;// / (PI);// / (cellWidth * g_GIFalloff);
	
	
	cr += r * dir;
	cg += g * dir;
	cb += b * dir;
	
}

LPVPixelOutput PS_Volume(GSVolumeOutput IN)
{
	LPVPixelOutput OUT;
	float4 accum_cr = 0;
	float4 accum_cg = 0;
	float4 accum_cb = 0;
	
	float4 incoming_dir = 0;
	int3 offset = 0;
	
	//--------------------------------------------------------
	// offset (1, 0, 0)
	//--------------------------------------------------------
	offset = float3(1, 0, 0);
	incoming_dir = (IN.TexCoord0 + offset).x >= g_TexelNum ? 0.0 : float4(SHBasisPoly.x, 0, 0, SHBasisPoly.y);//GetSHCosine(float3(-1, 0, 0));//float4(0.4132f, 0, 0, -0.7273f);	// sh_rotate(float3(-1, 0, 0), GetZHCoeff(90));
	GatherRadiance(offset, incoming_dir, IN.TexCoord0, accum_cr, accum_cg, accum_cb);
	
	// --------------------------------------------------------
	// offset (-1, 0, 0)
	// --------------------------------------------------------
	offset = float3(-1, 0, 0);
	incoming_dir = (IN.TexCoord0 + offset).x < 0 ? 0.0 : float4(SHBasisPoly.x, 0, 0, -SHBasisPoly.y);//GetSHCosine(float3(1, 0, 0));	// sh_rotate(float3(1, 0, 0), GetZHCoeff(90));
	GatherRadiance(offset, incoming_dir, IN.TexCoord0, accum_cr, accum_cg, accum_cb);
	
	// --------------------------------------------------------
	// offset (0, 1, 0)
	// --------------------------------------------------------
	offset = float3(0, 1, 0);
	incoming_dir = (IN.TexCoord0 + offset).y >= g_TexelNum ? 0.0 : float4(SHBasisPoly.x, -SHBasisPoly.y, 0, 0);//GetSHCosine(float3(0, 1, 0));	// sh_rotate(float3(0, -1, 0), GetZHCoeff(90));
	GatherRadiance(offset, incoming_dir, IN.TexCoord0, accum_cr, accum_cg, accum_cb);
	
	// --------------------------------------------------------
	// offset (0, -1, 0)
	// --------------------------------------------------------
	offset = float3(0, -1, 0);
	incoming_dir = (IN.TexCoord0 + offset).y < 0 ? 0.0 : float4(SHBasisPoly.x, SHBasisPoly.y, 0, 0);//GetSHCosine(float3(0, -1, 0));	// sh_rotate(float3(0, 1, 0), GetZHCoeff(90));
	GatherRadiance(offset, incoming_dir, IN.TexCoord0, accum_cr, accum_cg, accum_cb);
	
	// --------------------------------------------------------
	// offset (0, 0, 1)
	// --------------------------------------------------------
	offset = float3(0, 0, 1);
	incoming_dir = (IN.TexCoord0 + offset).z >= g_TexelNum ? 0.0 : float4(SHBasisPoly.x, 0, -SHBasisPoly.y, 0);//GetSHCosine(float3(0, 0, -1));	// sh_rotate(float3(0, 0, -1), GetZHCoeff(90));
	GatherRadiance(offset, incoming_dir, IN.TexCoord0, accum_cr, accum_cg, accum_cb);
	
	// --------------------------------------------------------
	// offset (0, 0, -1)
	// --------------------------------------------------------
	offset = float3(0, 0, -1);
	incoming_dir = (IN.TexCoord0 + offset).z < 0 ? 0.0 : float4(SHBasisPoly.x, 0, SHBasisPoly.y, 0);//GetSHCosine(float3(0, 0, 1));	// sh_rotate(float3(0, 0, 1), GetZHCoeff(90)); 0.4132f, 0, -0.7274f
	GatherRadiance(offset, incoming_dir, IN.TexCoord0, accum_cr, accum_cg, accum_cb);
	
	OUT.r = accum_cr;
	OUT.g = accum_cg;
	OUT.b = accum_cb;
	
	return OUT;
}

float4 PS_GI(TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE) 
{	
	// occluded objects still need to render depth
	if (!g_bIsReceivesGI)
		return 0;
		
	//float3 UV = IN.TexCoord3D;
	float4 coeff_r = RVVolumeMapR.Sample(LPVSampler, IN.ScreenPos);
	float4 coeff_g = RVVolumeMapG.Sample(LPVSampler, IN.ScreenPos);
	float4 coeff_b = RVVolumeMapB.Sample(LPVSampler, IN.ScreenPos);
	
	//float4 coeff = sh_rotate(-(IN.Normal), float2(0.5, 0.75)); // ZH 90 degree
	//float4 coeff = GetSHCosine(-(IN.Normal));
	float3 Normal = GetBumpNormal( IN.V, hasNormalMap, normalMap, g_bumpMapScale, vFace );
	float4 coeff = float4(SHBasisPoly.x, SHBasisPoly.y * (Normal).y, SHBasisPoly.y * (-Normal).z, SHBasisPoly.y * (Normal).x);
	float3 gi = 0;
	gi.r = dot(coeff, coeff_r);
	gi.g = dot(coeff, coeff_g);
	gi.b = dot(coeff, coeff_b);
	gi = max(gi, 0.0f);
	
	float3 diffuse = Tex2DCombine(hasDiffuseMap, g_diffuseMap, IN.V.TexCoord0, g_diffuseColor).rgb;
	float4 objColor = g_emissiveColor * g_emissiveIntensity + float4(diffuse, 1.0f);
	
	return float4(gi * objColor.rgb * g_GIScale, 1.0f);
	//return float4(gi * g_GIScale, 1.0f);
}

float4 PS_GI_Tess( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE) : SV_Target0
{
	TangentDisplace( IN.V );
	return PS_GI( IN, vFace );
}

float4 PS_GI_Default( TANGENT_VERTEX_OUTPUT IN, bool vFace : SV_ISFRONTFACE): SV_Target0
{
	return PS_GI( IN, vFace );
}

LPVPixelOutput PS_AccumRV(GSVolumeOutput IN)
{
	LPVPixelOutput OUT;
	
	OUT.r = RVVolumeMapR.Load(int4(IN.TexCoord0, 0));
	OUT.g = RVVolumeMapG.Load(int4(IN.TexCoord0, 0));
	OUT.b = RVVolumeMapB.Load(int4(IN.TexCoord0, 0));
	
	return OUT;
}

//-----------------------------------------------------------------------------
// TECHNIQUES
//-----------------------------------------------------------------------------

#define GI_HULL_AND_DOMAIN_Default

#define GI_HULL_AND_DOMAIN_Tess\
	SetHullShader(CompileShader(hs_5_0, GI_HS()));\
	SetDomainShader(CompileShader(ds_5_0, GI_DS()));
	
technique11 Default
{
	pass P0
	{
		SetVertexShader		(CompileShader(vs_5_0, VS()));	
		SetHullShader		(NULL);
		SetDomainShader		(NULL);
		SetGeometryShader	(CompileShader(gs_5_0, GS_Inject()));				 
		SetPixelShader		(CompileShader(ps_5_0, PS_Inject()));				
	}
}

technique11 RVPropagation
{
	pass P0
	{		
		SetVertexShader		(CompileShader(vs_5_0, VS()));	
		SetHullShader		(NULL);
		SetDomainShader		(NULL);
		SetGeometryShader	(CompileShader(gs_5_0, GS_Volume()));				 
		SetPixelShader		(CompileShader(ps_5_0, PS_Volume()));				
	}

}
	
technique11 GI
{
#define PASS_LPVGI(PassName)	\
	pass P##PassName \
	{	\
		SetVertexShader( CompileShader( vs_5_0, VS_GI_##PassName()));\
        SetPixelShader( CompileShader( ps_5_0, PS_GI_##PassName()));\
		GI_HULL_AND_DOMAIN_##PassName\
	}	
PASS_LPVGI(Default)
PASS_LPVGI(Tess)
}

//SetVertexShader		(CompileShader(vs_5_0, VS_GI()));\	
		//SetHullShader		(NULL);\
		//SetDomainShader		(NULL);\
		//SetGeometryShader	(NULL);\				 
		//SetPixelShader		(CompileShader(ps_5_0, PS_GI()));\

technique11 AccumRV
{
	pass P0
	{
		SetVertexShader		(CompileShader(vs_5_0, VS()));	
		SetHullShader		(NULL);
		SetDomainShader		(NULL);
		SetGeometryShader	(CompileShader(gs_5_0, GS_Volume()));			 
		SetPixelShader		(CompileShader(ps_5_0, PS_AccumRV()));				
	}
}

/***************************** eof ***/
