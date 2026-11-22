//////////////////////////////////////////////////////////////////////////////
// Converted from testtess.fx by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// testtess.effect.json. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////


// testtess.fx declared no material parameters of its own: every global it
// used (g_vTessellationFactor, g_world, g_vp, g_wvp, g_uvTransform,
// g_glowSize, g_bumpMapScale) is a member of the shared cbuffers, so it takes
// them from Globals.hlsli (b0 FrameParams, b1 ObjectParams, b3 MaterialCommon)
// and the material classes set them by name/semantic as for every material.
// Defaults (including g_bumpMapScale = 1) are in testtess.effect.json.
// Register layout shared by all materials: see Globals.hlsli.

#include "Globals.hlsli"

/* data from application vertex buffer */
struct vertexData 
{
    float3 Position	: SV_POSITION;
    float3 Normal	: NORMAL;
    float2 UV		: TEXCOORD0;
    float3 T		: TANGENT;
    float3 B		: BINORMAL;
};

struct VS_OUTPUT
{
    float4 HPosition : SV_POSITION;
};

struct VS_OUTPUT_HS_INPUT 
{
//    float4 HPosition	: SV_POSITION;
    float4 TexCoord0	: TEXCOORD0;
    float4 UV			: TEXCOORD1;
    float3 WorldPos		: TEXCOORD2;
	float3 WorldTanMatrixX : TEXCOORD3;
	float3 WorldTanMatrixY : TEXCOORD4;
	float3 WorldTanMatrixZ : TEXCOORD5;
    float3 OPos		: TEXCOORD6;
};


// compute the 3x3 tranform from world space to tangent space,
// transforming basis vectors to world space
// also send transpose of this matrix to the pixel shader
// so that it can transform the normal into world space to 
// compute the reflection vector for env mapping
void getTangentToWorldSpace(uniform float3x3 objToWorld,
                                  in float3 objTangent,
								  in float3 objBinormal,
								  in float3 objNormal,
								  in float bumpMapScale,
//								  out float3x3 worldToTangentSpace,
								  out float3 tanToWorldX,
								  out float3 tanToWorldY,
								  out float3 tanToWorldZ)
{
	tanToWorldX = mul(objToWorld, objTangent).xyz;
	tanToWorldY = mul(objToWorld, objBinormal).xyz;
	tanToWorldZ = mul(objToWorld, objNormal).xyz;

/*
	float3 wTangent = mul(objToWorld, objTangent).xyz;
	float3 wBinormal = mul(objToWorld, objBinormal).xyz;
	float3 wNormal = mul(objToWorld, objNormal).xyz;
//	worldToTangentSpace[0] = wTangent * bumpMapScale;
//	worldToTangentSpace[1] = wBinormal * bumpMapScale;
//	worldToTangentSpace[2] = wNormal;

	// assumption: inverse = transpose
	// why does bumpmapscale work like this? 
	// if the above uses bumpmapscale, shouldn't this one use 1/bumpmapscale?
	// yet somehow this works.
	tanToWorldX = float3(wTangent.x*bumpMapScale, wBinormal.x*bumpMapScale, wNormal.x);
	tanToWorldY = float3(wTangent.y*bumpMapScale, wBinormal.y*bumpMapScale, wNormal.y);
	tanToWorldZ = float3(wTangent.z*bumpMapScale, wBinormal.z*bumpMapScale, wNormal.z);
*/
}

VS_OUTPUT VS_Default(vertexData IN)
{
    VS_OUTPUT OUT;
    
    // output position in proj space
    float4 Po = float4(IN.Position.xyz + IN.Normal*g_glowSize, 1.0);
    OUT.HPosition = mul( g_wvp, Po );
	    
    return OUT;
}


VS_OUTPUT_HS_INPUT VS_Tess(vertexData IN)
{
    VS_OUTPUT_HS_INPUT OUT;
    
    // output position in proj space
    float4 Po = float4(IN.Position.xyz + IN.Normal*g_glowSize, 1.0);
//    OUT.HPosition = TransformVertex(Po, IN.UV, WorldViewProj);
    
    // transform position to world space and get vector to light
    float3 Pw = mul(g_world, Po).xyz;
    OUT.WorldPos = Pw;

	// theoretically this should be using WorldIT! Who dares to change it?
	getTangentToWorldSpace((float3x3)g_world,IN.T,IN.B,IN.Normal,g_bumpMapScale, 
		OUT.WorldTanMatrixX,OUT.WorldTanMatrixY,OUT.WorldTanMatrixZ);
	
	// decal and bump texture coords
    OUT.TexCoord0 = mul(g_uvTransform, float4(IN.UV,0,0));
    OUT.UV = float4(IN.UV,0,0);

	OUT.OPos = IN.Position.xyz;
	    
    return OUT;
}

#define DISTANCE_ADAPTIVE_TESSELLATION 0

struct HS_CONSTANT_DATA_OUTPUT
{
    float    Edges[3]         : SV_TessFactor;
    float    Inside           : SV_InsideTessFactor;
};

//--------------------------------------------------------------------------------------
// Hull shader
//--------------------------------------------------------------------------------------
HS_CONSTANT_DATA_OUTPUT ConstantsHS( InputPatch<VS_OUTPUT_HS_INPUT, 3> p, uint PatchID : SV_PrimitiveID )
{
    HS_CONSTANT_DATA_OUTPUT output = (HS_CONSTANT_DATA_OUTPUT)0;
    float4 vEdgeTessellationFactors;
    
    // Tessellation level fixed by variable
    vEdgeTessellationFactors = g_vTessellationFactor.xxxy;

#if DISTANCE_ADAPTIVE_TESSELLATION==1

    // Calculate edge scale factor from vertex scale factor: simply compute 
    // average tess factor between the two vertices making up an edge
    float3 fScaleFactor;
    fScaleFactor.x = 0.5 * ( p[1].fVertexDistanceFactor + p[2].fVertexDistanceFactor );
    fScaleFactor.y = 0.5 * ( p[2].fVertexDistanceFactor + p[0].fVertexDistanceFactor );
    fScaleFactor.z = 0.5 * ( p[0].fVertexDistanceFactor + p[1].fVertexDistanceFactor );

    // Scale edge factors 
    vEdgeTessellationFactors *= fScaleFactor.xyzx;
    
#endif
    
    // Assign tessellation levels
    output.Edges[0] = vEdgeTessellationFactors.x;
    output.Edges[1] = vEdgeTessellationFactors.y;
    output.Edges[2] = vEdgeTessellationFactors.z;
    output.Inside   = vEdgeTessellationFactors.w;

    return output;
}

struct HS_CONTROL_POINT_OUTPUT
{
//    float4 HPosition	: SV_POSITION;
    float4 TexCoord0	: TEXCOORD0;
    float4 UV			: TEXCOORD1;
    float3 vWorldPos		: TEXCOORD2;
	float3 vWorldTanMatrixX : TEXCOORD3;
	float3 vWorldTanMatrixY : TEXCOORD4;
	float3 vWorldTanMatrixZ : TEXCOORD5;
	float3 OPos : TEXCOORD6;
};

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantsHS")]
HS_CONTROL_POINT_OUTPUT HS( InputPatch<VS_OUTPUT_HS_INPUT, 3> inputPatch, 
                            uint uCPID : SV_OutputControlPointID )
{
    HS_CONTROL_POINT_OUTPUT    output = (HS_CONTROL_POINT_OUTPUT)0;
    
    // Copy inputs to outputs
    output.vWorldPos =			inputPatch[uCPID].WorldPos;
    output.vWorldTanMatrixX =   inputPatch[uCPID].WorldTanMatrixX;
    output.vWorldTanMatrixY =   inputPatch[uCPID].WorldTanMatrixY;
    output.vWorldTanMatrixZ =   inputPatch[uCPID].WorldTanMatrixZ;
    output.TexCoord0 =			inputPatch[uCPID].TexCoord0;
    output.UV =					inputPatch[uCPID].UV;
    output.OPos =				inputPatch[uCPID].OPos;
//    output.HPosition =			inputPatch[uCPID].HPosition;

    return output;
}

struct DS_OUTPUT
{
    float4 TexCoord0	: TEXCOORD0;
    float4 UV			: TEXCOORD1;
    float3 vWorldPos		: TEXCOORD2;
	float3 vWorldTanMatrixX : TEXCOORD3;
	float3 vWorldTanMatrixY : TEXCOORD4;
	float3 vWorldTanMatrixZ : TEXCOORD5;
    float4 HPosition	: SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Domain Shader
//--------------------------------------------------------------------------------------
[domain("tri")]
DS_OUTPUT DS( HS_CONSTANT_DATA_OUTPUT input, float3 BarycentricCoordinates : SV_DomainLocation, 
             const OutputPatch<HS_CONTROL_POINT_OUTPUT, 3> TrianglePatch )
{
    DS_OUTPUT output = (DS_OUTPUT)0;

    // Interpolate world space position with barycentric coordinates
    float3 vWorldPos = BarycentricCoordinates.x * TrianglePatch[0].vWorldPos + 
                       BarycentricCoordinates.y * TrianglePatch[1].vWorldPos + 
                       BarycentricCoordinates.z * TrianglePatch[2].vWorldPos;
    
    float3 vWorldTanMatrixX = BarycentricCoordinates.x * TrianglePatch[0].vWorldTanMatrixX + 
                       BarycentricCoordinates.y * TrianglePatch[1].vWorldTanMatrixX + 
                       BarycentricCoordinates.z * TrianglePatch[2].vWorldTanMatrixX;
    float3 vWorldTanMatrixY = BarycentricCoordinates.x * TrianglePatch[0].vWorldTanMatrixY + 
                       BarycentricCoordinates.y * TrianglePatch[1].vWorldTanMatrixY + 
                       BarycentricCoordinates.z * TrianglePatch[2].vWorldTanMatrixY;
    float3 vWorldTanMatrixZ = BarycentricCoordinates.x * TrianglePatch[0].vWorldTanMatrixZ + 
                       BarycentricCoordinates.y * TrianglePatch[1].vWorldTanMatrixZ + 
                       BarycentricCoordinates.z * TrianglePatch[2].vWorldTanMatrixZ;

	float4 UV = BarycentricCoordinates.x * TrianglePatch[0].UV + 
                      BarycentricCoordinates.y * TrianglePatch[1].UV + 
                      BarycentricCoordinates.z * TrianglePatch[2].UV;
	float4 TexCoord0 = BarycentricCoordinates.x * TrianglePatch[0].TexCoord0 + 
                      BarycentricCoordinates.y * TrianglePatch[1].TexCoord0 + 
                      BarycentricCoordinates.z * TrianglePatch[2].TexCoord0;

	float3 OPos  = BarycentricCoordinates.x * TrianglePatch[0].OPos + 
                      BarycentricCoordinates.y * TrianglePatch[1].OPos + 
                      BarycentricCoordinates.z * TrianglePatch[2].OPos;

    // Calculate MIP level to fetch normal from
//    float fHeightMapMIPLevel = clamp( ( distance( vWorldPos, g_vEye ) - 100.0f ) / 100.0f, 0.0f, 6.0f);
    
    // Sample normal and height map
//    float4 vNormalHeight = g_nmhTexture.SampleLevel( g_samLinear, output.texCoord, fHeightMapMIPLevel );
    
    // Displace vertex along normal
//    vWorldPos += vNormal * ( g_vDetailTessellationHeightScale.x * ( vNormalHeight.w-1.0 ) );
    
   
    output.TexCoord0 = TexCoord0;
    output.UV = UV;
    output.vWorldPos = vWorldPos;
	output.vWorldTanMatrixX = vWorldTanMatrixX; 
	output.vWorldTanMatrixY = vWorldTanMatrixY; 
	output.vWorldTanMatrixZ = vWorldTanMatrixZ; 

    // Transform world position with viewprojection matrix
    output.HPosition = mul( g_wvp, float4( OPos.xyz, 1.0 ) );

    return output;
}

float4 PS() : SV_TARGET
{
	return float4(1,1,1,1);
}
