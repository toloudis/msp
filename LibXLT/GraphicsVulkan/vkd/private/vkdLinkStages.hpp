/*****************************************************************************
**  vkdLinkStages.hpp
**
**      vkdTrimUnconsumedOutputs removes, from a copy of a vertex (or
**	domain, or geometry) shader, the outputs that the pixel shader drawn
**	with it doesn't take. It is a link step, run when a pipeline is made;
**	the HLSL and the embedded SPIR-V don't change.
**
**	Why. GraphicsDX11's shaders (copied unchanged) often share one vertex
**	shader between several pixel shaders that each take only some of its
**	outputs. For example, a vertex shader writes
**		SV_Position, TEXCOORD0 (location 0), NORMAL (1), COLOR0 (2)
**	and the pixel shader of one pass takes only TEXCOORD0. D3D11 matches
**	the stages when the pipeline is bound and silently drops NORMAL and
**	COLOR0. Vulkan allows it too, but the validation layer reports each
**	such output as WARNING-Shader-OutputNotConsumed, and a successful
**	port means zero warnings. Most passes have this (613 of the 778
**	passes of the embedded effects).
**
**	What changes in the shader. In the copy of the vertex shader, the
**	instructions that write NORMAL and COLOR0 are removed, and so are the
**	two output variables (and their locations). Everything else stays:
**	TEXCOORD0 keeps location 0, the code that computes NORMAL and COLOR0
**	still runs (its results go nowhere, as before), and the pixel shader
**	isn't touched. What reaches the pixel shader, and so the image, is
**	the same: the removed values were thrown away anyway
**	(vkdLinkStagesTest.TrimmedImagesMatchUntrimmed checks bytes).
**
**	Which outputs go. Those at locations the pixel shader doesn't
**	declare as inputs (read with SPIRV-Reflect). "Declare", not "read":
**	the layer compares declarations, and DXC already drops the inputs a
**	pixel shader never reads, so a pixel shader's declarations are what it
**	really takes. The removal itself is SPIRV-Tools'
**	EliminateDeadOutputStores, RemoveUnusedInterfaceVariables and
**	DeadVariableElimination passes.
**
**	Built-ins (SV_Position, SV_ClipDistance, ...) always stay: they go to
**	the rasterizer, not the pixel shader. (SPIRV-Tools would remove
**	ClipDistance, CullDistance and PointSize because a pixel shader
**	doesn't take them, which would stop SV_ClipDistance from clipping, so
**	they are marked as used.)
**
**	vkdPipelineCache calls this once per (vertex shader, pixel shader)
**	pair and keeps the result. Pure CPU code: needs no device.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_VKDLINKSTAGES_HPP
#error vkdLinkStages.hpp multiply included
#endif
#define GVK_VKDLINKSTAGES_HPP

#include <cstdint>
#include <vector>

namespace gvk
{
	//	i_Producer: the last pre-rasterization stage; i_Consumer: the pixel
	//	shader (both SPIR-V words). Returns i_Producer unchanged when the
	//	pixel shader takes every location it writes, or when either module
	//	can't be processed (logged).
	std::vector<uint32_t> vkdTrimUnconsumedOutputs(const std::vector<uint32_t>& i_Producer,
												   const std::vector<uint32_t>& i_Consumer);
}
