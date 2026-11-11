/*****************************************************************************
**  gtOptiX.hpp
**
**      gtOptiX is a test fixture for GraphicsOptiX: it loads CUDA and
**	OptiX and creates the OptiX context (optxContext, validation on) once
**	per test suite, and makes it current for each test. Each test fails on
**	any OptiX message at error or warning level it caused. Skips every
**	test when there is no NVIDIA driver or no usable device.
**
**	Compiled only when the OptiX SDK and the CUDA toolkit were both found
**	(GraphicsTests.vcxproj), as GraphicsOptiX's own sources are.
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#include "GraphicsOptiX/cu/cuLoader.hpp"
#include "GraphicsOptiX/optx/optxContext.hpp"
#include "GraphicsOptiX/optx/optxLoader.hpp"

#include <gtest/gtest.h>

#include <string>

class gtOptiX : public ::testing::Test
{
	public:
		static void SetUpTestSuite()
		{
			std::string& error = Error();
			error.clear();
			if (!gox::cuLoader::Load(error))
			{
				return;
			}
			if (!gox::optxLoader::Load(error))
			{
				gox::cuLoader::Unload();
				return;
			}
			if (!gox::optxContext::Create(true, error))
			{
				gox::optxLoader::Unload();
				gox::cuLoader::Unload();
			}
		}

		static void TearDownTestSuite()
		{
			if (gox::optxContext::Exists())
			{
				gox::optxContext::Destroy();
				gox::optxLoader::Unload();
				gox::cuLoader::Unload();
			}
		}

	protected:
		void SetUp() override
		{
			if (!gox::optxContext::Exists())
			{
				GTEST_SKIP() << "No OptiX device: " << Error();
			}
			gox::optxContext::MakeCurrent();
			m_nErrors = gox::optxContext::GetErrorCount();
			m_nWarnings = gox::optxContext::GetWarningCount();
		}

		void TearDown() override
		{
			if (gox::optxContext::Exists())
			{
				EXPECT_TRUE(gox::optxContext::Synchronize());
				EXPECT_EQ(m_nErrors, gox::optxContext::GetErrorCount()) << "OptiX errors (see the log)";
				EXPECT_EQ(m_nWarnings, gox::optxContext::GetWarningCount()) << "OptiX warnings (see the log)";
			}
		}

	private:
		static std::string& Error()
		{
			static std::string s_Error;
			return s_Error;
		}

		int m_nErrors = 0;
		int m_nWarnings = 0;
};
