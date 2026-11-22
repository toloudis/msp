// gtIsoline.hlsl
//
// Test shader for isoline tessellation (vkdIsolineTest): the hull shader
// sets its factors with ProcessIsolineTessFactors exactly as the hair's
// Hair_HS does (raw .y as the detail factor, raw .x as the density factor,
// into SV_TessFactor[0] and [1], fractional_odd), and culls a patch with
// a factor of -1 as Hair_HS culls a strand's last segment. The domain
// shader draws each tessellated point (output topology point) at a pixel
// of its own, computed from (patch, u, v), and the pixel shader writes
// (u, v, patch, 1) there. Compiled with GraphicsVulkan's DXC options
// (GraphicsTests.vcxproj) and, for D3D11, with fxc at run time.

#ifdef __spirv__
#include "IsolineTessFactors.vulkan-only.hlsli"
#endif

cbuffer CB0 : register(b0)
{
	float2 g_Raw;			// x: tessellation (density), y: interpolation count (detail), as g_HairTessellationValue
	uint g_CulledPatch;		// the patch whose SV_TessFactor[0] is -1 (none if past the last)
	uint g_Pad;
};

// the target: c_Width columns for u, c_Rows rows of v for each patch
static const float c_Width = 1024;
static const float c_Rows = 64;
static const float c_Patches = 2;

struct ControlPoint
{
	float4 pos : POSITION;
};

ControlPoint VSMain(uint i_Id : SV_VertexID)
{
	ControlPoint o;
	o.pos = float4(i_Id, 0, 0, 1);
	return o;
}

struct Factors
{
	float Edges[2] : SV_TessFactor;
};

Factors ConstantsHS(uint i_Patch : SV_PrimitiveID)
{
	Factors o = (Factors)0;
	ProcessIsolineTessFactors(g_Raw.y, g_Raw.x, o.Edges[0], o.Edges[1]);
	if (i_Patch == g_CulledPatch) o.Edges[0] = -1;
	return o;
}

[domain("isoline")]
[partitioning("fractional_odd")]
[outputtopology("point")]
[outputcontrolpoints(1)]
[patchconstantfunc("ConstantsHS")]
ControlPoint HSMain(InputPatch<ControlPoint, 1> i_Patch, uint i_Id : SV_OutputControlPointID)
{
	return i_Patch[0];
}

struct DSOut
{
	float4 pos : SV_Position;
	float4 value : TEXCOORD0;
};

[domain("isoline")]
DSOut DSMain(Factors i_Factors, float2 i_UV : SV_DomainLocation, const OutputPatch<ControlPoint, 1> i_Patch,
			 uint i_PatchId : SV_PrimitiveID)
{
	// v is line / lines with at most 64 lines, so v * 64 rounds to a row of its own
	const float x = floor(i_UV.x * (c_Width - 1) + 0.5) + 0.5;
	const float y = i_PatchId * c_Rows + floor(i_UV.y * c_Rows + 0.5) + 0.5;
	DSOut o;
	o.pos = float4(x / c_Width * 2 - 1, 1 - y / (c_Rows * c_Patches) * 2, 0.5, 1);
	o.value = float4(i_UV, i_PatchId, 1);
	return o;
}

float4 PSMain(DSOut i) : SV_Target
{
	return i.value;
}
