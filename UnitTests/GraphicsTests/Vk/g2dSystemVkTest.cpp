/*****************************************************************************
**  g2dSystemVkTest.cpp
**
**      g2dSystemVk against GraphicsDX11's g2dSystemDX11, each under its
**      whole layer (gtBackend): what the 2D system owns is released with
**      it. Skipped without a device.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"

#ifndef G2D_RESETHANDLER_HPP
#include "Graphics/G2d/g2dResetHandler.hpp"
#endif

#include <gtest/gtest.h>

namespace
{
	//	g2dResetHandler has no virtual destructor, so a probe can't see its
	//	own deletion; it counts the calls it gets instead
	class ProbeResetHandler : public g2dResetHandler
	{
	public:
		explicit ProbeResetHandler(int& o_Calls) : m_Calls(o_Calls) {}
		virtual void Deallocate() { ++m_Calls; }
		virtual void Reallocate() {}

	private:
		int& m_Calls;
	};
}

//	The reset handlers (g3dRendererMgr's, the texture manager's, ...) are
//	deleted with the 2D system, as g2dSystemDX11 does; otherwise each
//	GraphicsLayer::Init leaves one behind
TEST(g2dSystemVkTest, DestroysResetHandlersAsDX11)
{
	for (gtBackend::Kind kind : { gtBackend::e_DX11, gtBackend::e_Vulkan })
	{
		int calls = 0;
		if (!gtBackend::Run(kind, [&]() { g2dResetHandler::AddResetHandler(new ProbeResetHandler(calls)); }))
		{
			return;
		}
		//	A handler still listed is still alive (it leaked), so this is safe
		g2dResetHandler::ResetHandlerDeallocate();
		EXPECT_EQ(0, calls) << gtBackend::Name(kind) << ": the probe outlived the 2D system";
		g2dResetHandler::DestroyResetHandlers();
	}
}
