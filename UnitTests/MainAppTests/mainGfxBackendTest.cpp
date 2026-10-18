/*****************************************************************************
**  mainGfxBackendTest.cpp
**
**      mainGfxBackend: the name each graphics backend goes by (profiler
**      CSV, the "Graphics backend: <name> on <adapter>" line gfxcompare
**      reads) and the main window title suffix, which is empty for DX11 so
**      a DX11 run looks exactly as before.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef MAIN_GFXBACKEND_HPP
#include "MainApp/mainGfxBackend.hpp"
#endif

#include <gtest/gtest.h>

#include <string>

TEST(mainGfxBackend, Names)
{
	EXPECT_EQ(std::string("dx11"), mainGfxBackend::Name(g3dGfxOptions::e_DX11));
	EXPECT_EQ(std::string("vulkan"), mainGfxBackend::Name(g3dGfxOptions::e_Vulkan));
}

TEST(mainGfxBackend, TitleSuffixOnlyForVulkan)
{
	EXPECT_EQ(std::wstring(L""), mainGfxBackend::TitleSuffix(g3dGfxOptions::e_DX11));
	EXPECT_EQ(std::wstring(L" [Vulkan]"), mainGfxBackend::TitleSuffix(g3dGfxOptions::e_Vulkan));
}

TEST(mainGfxBackend, BackendLine)
{
	EXPECT_EQ("Graphics backend: vulkan on NVIDIA Quadro RTX 5000",
			  mainGfxBackend::BackendLine(g3dGfxOptions::e_Vulkan, "NVIDIA Quadro RTX 5000"));
	EXPECT_EQ("Graphics backend: dx11 on unknown adapter", mainGfxBackend::BackendLine(g3dGfxOptions::e_DX11, NULL));
}
