//////////////////////////////////////////////////////////////////////////////
// Converted from Skinning.h (one-time conversion, by hand).
// Source of truth for the plain-HLSL material shaders from here on; the
// original Skinning.h was deleted along with the Effects (.fx) shaders.
// Register layout: see Globals.hlsli.
//////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
**  Tessellate.h
**
**      ATI Hardware Tessellation functions
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifndef _GLOBALS_
#include "Globals.hlsli"
#endif

/*********** structures ******/
struct VS_INPUT_SKINNING
{
    float3 Position	: SV_POSITION;
    float3 Normal	: NORMAL;
    float4 UV		: TEXCOORD0;
    float3 T		: TANGENT;
    float3 B		: BINORMAL;
    float4 Bones	: TEXCOORD1;
    float4 Weights	: TEXCOORD2;
};

/*********** data ******/
// g_BonePalette (semantic BONES) is the last member of ObjectParams in
// Globals.hlsli; MATRIX_PALETTE_SIZE_DEFAULT is fixed there because it is
// part of the shared cbuffer layout.

/*********** functions ******/
/*********** support functions ******/
void Skin(in VS_INPUT_SKINNING In, 
		  out STANDARD_VERTEX Out)
{
// maximum of 4 influencing transforms:

	// if Bones were passed in as a float1 instead of float4, 
	// then we can break it into an array of 4 bytes like this:
	// int4 iBones = D3DCOLORtoUBYTE4(Bones);

    float4x4 finalMatrix;
    finalMatrix = In.Weights.x * g_BonePalette[In.Bones.x];
    finalMatrix += In.Weights.y * g_BonePalette[In.Bones.y];
    finalMatrix += In.Weights.z * g_BonePalette[In.Bones.z];
    finalMatrix += In.Weights.w * g_BonePalette[In.Bones.w];
    
	Out.Position	= mul(float4(In.Position,1), finalMatrix).xyz;
	Out.Normal		= mul(In.Normal, (float3x3)finalMatrix);
	Out.T			= mul(In.T, (float3x3)finalMatrix);
	Out.B			= mul(In.B, (float3x3)finalMatrix);
	Out.UV = In.UV.xy;//?

 // In the sample file, this should do a rigid bend of the cylinder
 // as if the full cylinder was influenced only by the second bone...
//	float4 final_pos = mul(Pos, g_BonePalette[1]);
//	return final_pos;
	
	// Combining the transformed positions
//  	float4 final_pos = Weights.x * mul(Pos, g_BonePalette[Bones.x]);
//  	final_pos += Weights.y * mul(Pos, g_BonePalette[Bones.y]);
//  	final_pos += Weights.z * mul(Pos, g_BonePalette[Bones.z]);
//  	final_pos += Weights.w * mul(Pos, g_BonePalette[Bones.w]);
//  	return final_pos;
   
}

/*********** eof **************/
