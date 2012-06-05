/*****************************************************************************
**  VelocityRender.fx
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

/***********************************************/
/********** SUPPORT DATA AND FUNCTIONS *********/
/***********************************************/
#include "..\Support.h"
#include "..\Tessellate.h"

/*******************************************/
/************** DATA STRUCTS ***************/
/*******************************************/

struct VertexOutput
{
    float4 HPosition	: SV_POSITION;
    float2 velocityVec	: TEXCOORD1;
};

struct VertexOutputTess
{
    float3 Position		: TEXCOORD0;
    float3 PositionPre	: TEXCOORD1;
    float3 Normal		: TEXCOORD2;
    float3 NormalPre	: TEXCOORD3;
    float2 texCoord		: TEXCOORD4;
};

struct appdataVertexAnim
{
    float3 Cur_Pos	: SV_POSITION;
    float3 Normal	: NORMAL;
    float2 UV		: TEXCOORD0;
    float3 T		: TANGENT;
    float3 B		: BINORMAL;    
    float3 Prev_Pos	: POSITION1;
    float3 Prev_Norm: NORMAL1;
};

struct PostProc_VSOut
{
    float4 pos   : SV_POSITION;
    float2 texUV : TEXCOORD0;
};

/*************************************/
/************** GLOBALS **************/
/*************************************/
float currIteration;
float4x4 g_world_old;
float4x4 g_vp_old;
float4x4 g_wvp_old;

Texture2D velocityBuffer;
SamplerState velocityBufferSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};

/********************************************/
/************** VERTEX SHADERS **************/
/********************************************/
VertexOutput StoreMorphableObjectVelocityVS_Default( appdataVertexAnim IN )
{
    VertexOutput OUT;
	//handle screen space velocity vector
	float4 CurPosS = mul( g_wvp, float4(IN.Cur_Pos.xyz,1) );		//Current screen space position
	float4 OldPosS = mul( g_wvp_old , float4(IN.Prev_Pos.xyz,1) );  //previous screen space position
    
	float3 Pold = OldPosS.xyz / OldPosS.w;
	float3 Pnew = CurPosS.xyz / CurPosS.w;

	float3 VelVec = (Pnew - Pold) * 0.5f;	//scale NDC coordinated from (-2,2) to (-1,1)

	OUT.velocityVec = float2( VelVec.x, VelVec.y );

	OUT.HPosition = CurPosS;
    return OUT;
}

VertexOutputTess StoreMorphableObjectVelocityVS_Tess( appdataVertexAnim IN )
{
    VertexOutputTess OUT;

    OUT.texCoord = mul( g_uvTransform, float4(IN.UV,0,1)).xy;
	OUT.Position = mul( g_world, float4( IN.Cur_Pos, 1.0f)).xyz;
	OUT.PositionPre = mul( g_world_old, float4( IN.Prev_Pos, 1.0f)).xyz;
	OUT.Normal = mul( (float3x3)g_world, IN.Normal );
	OUT.NormalPre = mul( (float3x3)g_world_old, IN.Prev_Norm );

	return OUT;
}

VertexOutput StoreNonMorphableObjectVelocityVS_Default( STANDARD_VERTEX IN, out float ClipDist : SV_ClipDistance0 )
{	
	appdataVertexAnim AIN;
	AIN.Cur_Pos = IN.Position;
	AIN.Normal = IN.Normal;
	AIN.UV = IN.UV;
	AIN.T = IN.T;
	AIN.B = IN.B;
	AIN.Prev_Pos = IN.Position;
	AIN.Prev_Norm = IN.Normal;

	VertexOutput Out;
	Out = StoreMorphableObjectVelocityVS_Default( AIN );	

	ClipDist = ClipWorldPos( mul( g_world, float4(IN.Position,1)).xyz );

	return Out;
}

VertexOutputTess StoreNonMorphableObjectVelocityVS_Tess( STANDARD_VERTEX IN, out float ClipDist : SV_ClipDistance0 )
{	
	appdataVertexAnim AIN;
	AIN.Cur_Pos = IN.Position;
	AIN.Normal = IN.Normal;
	AIN.UV = IN.UV;
	AIN.T = IN.T;
	AIN.B = IN.B;
	AIN.Prev_Pos = IN.Position;
	AIN.Prev_Norm = IN.Normal;

	VertexOutputTess Out;
	Out = StoreMorphableObjectVelocityVS_Tess( AIN );	

	ClipDist = ClipWorldPos( mul( g_world, float4(IN.Position,1)).xyz );

	return Out;
}

PostProc_VSOut DisplayVelocityVS(float4 Pos:SV_POSITION, float2 UV:TEXCOORD0 )
{
    PostProc_VSOut output = (PostProc_VSOut)0.0f;
	Pos.xy = sign(Pos.xy);    
    output.pos = float4( Pos.xy, 0.0f, 1.0f );
	output.texUV = UV;
    return output;
}

/*******************************************/
/************** PIXEL SHADERS **************/
/*******************************************/
pixelOutput StoreVelocityPS( VertexOutput IN )
{   
    pixelOutput OUT = (pixelOutput)0;
    OUT.col.rgba = float4( IN.velocityVec.xy, 0, 0 );
    return OUT;
}

float4 DisplayVelocityPS( PostProc_VSOut IN ) : SV_TARGET
{     
	float4 pixel = velocityBuffer.SampleLevel(velocityBufferSampler, IN.texUV, 0);
	pixel *= 100.0f;
	pixel += 0.5f;
	pixel.z = 0;
	pixel.a = 1;
    return saturate( pixel );	//hack to scale up the velocities
}

//--------------------------------------------------------------------------------------
// Hull shader
//--------------------------------------------------------------------------------------

HS_CONSTANT_DATA_OUTPUT Anim_Constants_HS( InputPatch<VertexOutputTess, 3> inputPatch )
{
	return AdaptiveTessellate(inputPatch[0].Position,
		inputPatch[1].Position,
		inputPatch[2].Position);
}

[domain("tri")]
[partitioning(SGPU_PARTITIONING)]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("Anim_Constants_HS")]
VertexOutputTess Anim_HS( InputPatch<VertexOutputTess, 3> inputPatch, uint uCPID : SV_OutputControlPointID )
{
	return inputPatch[uCPID];
}

//--------------------------------------------------------------------------------------
// Domain Shader
//--------------------------------------------------------------------------------------

[domain("tri")]
VertexOutput Anim_DS( HS_CONSTANT_DATA_OUTPUT input, float3 BarycentricCoordinates : SV_DomainLocation, 
						 const OutputPatch<VertexOutputTess, 3> TrianglePatch, out float ClipDist : SV_ClipDistance0 )
{
	VertexOutput output = (VertexOutput)0;

	// Interpolate world space position with barycentric coordinates

	float3 vWorldPos = BarycentricCoordinates.x * TrianglePatch[0].Position + 
		BarycentricCoordinates.y * TrianglePatch[1].Position + 
		BarycentricCoordinates.z * TrianglePatch[2].Position;

	float3 vWorldPosPre = BarycentricCoordinates.x * TrianglePatch[0].PositionPre + 
		BarycentricCoordinates.y * TrianglePatch[1].PositionPre + 
		BarycentricCoordinates.z * TrianglePatch[2].PositionPre;

	float2 TexCoord0 = BarycentricCoordinates.x * TrianglePatch[0].texCoord + 
		BarycentricCoordinates.y * TrianglePatch[1].texCoord + 
		BarycentricCoordinates.z * TrianglePatch[2].texCoord;

	float3 vWorldNormal  = BarycentricCoordinates.x * TrianglePatch[0].Normal + 
		BarycentricCoordinates.y * TrianglePatch[1].Normal + 
		BarycentricCoordinates.z * TrianglePatch[2].Normal;

	float3 vWorldNormalPre  = BarycentricCoordinates.x * TrianglePatch[0].NormalPre + 
		BarycentricCoordinates.y * TrianglePatch[1].NormalPre + 
		BarycentricCoordinates.z * TrianglePatch[2].NormalPre;

	//need normal to displace
#ifdef APPLY_DISPLACEMENT
	vWorldPos = DisplaceVertex( vWorldPos, vWorldNormal, TexCoord0 );
	vWorldPosPre = DisplaceVertex( vWorldPosPre, vWorldNormalPre, TexCoord0 );
#endif

	//handle screen space velocity vector
	float4 CurPosS = mul( g_vp, float4(vWorldPos,1) );		//Current screen space position
	float4 OldPosS = mul( g_vp_old , float4(vWorldPosPre,1) );  //previous screen space position
    
	float3 Pold = OldPosS.xyz / OldPosS.w;
	float3 Pnew = CurPosS.xyz / CurPosS.w;

	float3 VelVec = (Pnew - Pold) * 0.5f;	//scale NDC coordinated from (-2,2) to (-1,1)

	output.velocityVec = float2( VelVec.x, VelVec.y );

	// Transform world position with viewprojection matrix
	output.HPosition = CurPosS;

	ClipDist = ClipWorldPos( vWorldPos );

	return output;
}

#define ANIM_HULL_AND_DOMAIN_Default

#define ANIM_HULL_AND_DOMAIN_Tess\
	SetHullShader(CompileShader(hs_5_0, Anim_HS()));\
	SetDomainShader(CompileShader(ds_5_0, Anim_DS()));


/****************************************/
/************** TECHNIQUES **************/
/****************************************/
technique11 StoreMorphableObjectVelocity
{
#define PASS_STOREMORPHABLEOBJECTVELOCITY(PassName)	\
	pass P##PassName			\
	{						\
        SetVertexShader( CompileShader( vs_5_0, StoreMorphableObjectVelocityVS_##PassName()));\
        SetPixelShader( CompileShader( ps_5_0, StoreVelocityPS()));\
		ANIM_HULL_AND_DOMAIN_##PassName\
	}
PASS_STOREMORPHABLEOBJECTVELOCITY(Default)
PASS_STOREMORPHABLEOBJECTVELOCITY(Tess)
}

technique11 StoreNonMorphableObjectVelocity
{
#define PASS_STORENONMORPHABLEOBJECTVELOCITY(PassName)	\
	pass P##PassName			\
	{						\
        SetVertexShader( CompileShader( vs_5_0, StoreNonMorphableObjectVelocityVS_##PassName()));\
        SetPixelShader( CompileShader( ps_5_0, StoreVelocityPS()));\
		ANIM_HULL_AND_DOMAIN_##PassName\
	}
PASS_STORENONMORPHABLEOBJECTVELOCITY(Default)
PASS_STORENONMORPHABLEOBJECTVELOCITY(Tess)
}

technique11 DisplayVelocity
{
	pass P0
	{
        SetVertexShader( CompileShader( vs_5_0, DisplayVelocityVS()));
        SetPixelShader( CompileShader( ps_5_0, DisplayVelocityPS()));
	}
}
