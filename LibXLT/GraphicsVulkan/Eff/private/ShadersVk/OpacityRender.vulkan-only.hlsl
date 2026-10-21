// OpacityRender.vulkan-only.hlsl
//
// Vulkan only: the unchanged ../Shaders/Special/OpacityRender.hlsl
// with ProcessIsolineTessFactors given (IsolineTessFactors.vulkan-only.hlsli).
// EmbedShadersVk.ps1 compiles the effect's Hair_HS from this file and
// every other stage from the copy itself.

#include "IsolineTessFactors.vulkan-only.hlsli"
#include "../Shaders/Special/OpacityRender.hlsl"
