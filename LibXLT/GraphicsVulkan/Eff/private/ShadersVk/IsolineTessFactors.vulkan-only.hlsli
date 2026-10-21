// IsolineTessFactors.vulkan-only.hlsli
//
// Vulkan only: ProcessIsolineTessFactors, which DXC's SPIR-V backend
// doesn't implement ("intrinsic function unimplemented"). Included first
// by the wrappers in this folder (and by GraphicsTests' gtIsoline.hlsl),
// so the unchanged shaders in ../Shaders compile for Vulkan.
//
// Only fractional_odd partitioning is given: Hair_HS (SupportHair.hlsli)
// is the only caller, and it is fractional_odd. What D3D11 runs is fxc's
// lowering of the intrinsic, which for an isoline hull shader with
// fractional_odd partitioning is (hs_5_0 disassembly):
//
//     max r0.x, <raw detail>, l(1.0)    min o0.x, r0.x, l(63.0)   // finalLineDensityTessFactor
//     max r0.x, <raw density>, l(1.0)   min o1.x, r0.x, l(63.0)   // finalLineDetailTessFactor
//
// Each factor is clamped to [1, 63] and not rounded; the tessellator
// rounds them (the detail one by the partitioning, the density one to an
// integer), as Vulkan's does for TessLevelOuter[0] and [1]. A raw factor
// of 0 or less is clamped to 1, so the patch isn't culled. D3D's min and
// max return the other operand when one is NaN, so NaN gives 1; the
// comparisons below do the same (SPIR-V's FMin/FMax leave NaN undefined).
// vkdIsolineTest (GraphicsTests) checks the result against D3D11.

void gvkProcessIsolineTessFactorsFractionalOdd(float i_RawDetailFactor, float i_RawDensityFactor,
	out float o_RoundedDetailFactor, out float o_RoundedDensityFactor)
{
	const float detail = (i_RawDetailFactor > 1.0) ? i_RawDetailFactor : 1.0;
	const float density = (i_RawDensityFactor > 1.0) ? i_RawDensityFactor : 1.0;
	o_RoundedDetailFactor = (detail < 63.0) ? detail : 63.0;
	o_RoundedDensityFactor = (density < 63.0) ? density : 63.0;
}

#define ProcessIsolineTessFactors gvkProcessIsolineTessFactorsFractionalOdd
