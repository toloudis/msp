/*****************************************************************************
**  mainGfxBackendTest.cpp
**
**      mainGfxBackend: the "Graphics backend: <code name> on <adapter>"
**      line gfxcompare reads.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef MAIN_GFXBACKEND_HPP
#include "MainApp/mainGfxBackend.hpp"
#endif

#include <gtest/gtest.h>

#include <string>

TEST(mainGfxBackend, BackendLine)
{
	EXPECT_EQ("Graphics backend: vulkan on NVIDIA Quadro RTX 5000",
			  mainGfxBackend::BackendLine("vulkan", "NVIDIA Quadro RTX 5000"));
	EXPECT_EQ("Graphics backend: dx11 on unknown adapter", mainGfxBackend::BackendLine("dx11", NULL));
	EXPECT_EQ("Graphics backend: unknown on GPU X", mainGfxBackend::BackendLine(NULL, "GPU X"));
}
