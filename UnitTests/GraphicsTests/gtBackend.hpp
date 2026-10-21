/*****************************************************************************
**  gtBackend.hpp
**
**      gtBackend runs a piece of test code with one graphics backend up,
**      as MachStudio brings it up (mainInitGraphics): GraphicsLayer::Init,
**      the backend layer's Init on a hidden window, GraphicsLayer::
**      InitGraphics and the layer's InitGraphics; then the clean-ups in
**      reverse. Both layers install the same Graphics hooks, so a test runs
**      them one after the other: Run(e_DX11, f), then Run(e_Vulkan, f).
**
**      A backend that can't start (no Vulkan device, no Direct3D 11
**      adapter) marks the test skipped and Run returns false. The Vulkan
**      layer runs with validation on, and a validation error or warning
**      fails the test.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#include <functional>

namespace gtBackend
{
	enum Kind
	{
		e_DX11,
		e_Vulkan,
	};

	const char* Name(Kind i_Kind);

	//	The backend whose layer Run has up (asserts that one is)
	Kind Current();

	//	Runs i_Function with the backend up; false (and the test marked
	//	skipped) if the backend isn't available
	bool Run(Kind i_Kind, const std::function<void()>& i_Function);

	//	Run(e_Vulkan, ...) with g3dGfxOptions::m_bHeadless, as RenderNode
	//	starts the layer: windows have no swap chain
	bool RunVulkanHeadless(const std::function<void()>& i_Function);
}
