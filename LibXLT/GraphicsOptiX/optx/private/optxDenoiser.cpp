/*****************************************************************************
**  optxDenoiser.cpp
**
**      See optxDenoiser.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_OPTXDENOISER_HPP
#include "GraphicsOptiX/optx/optxDenoiser.hpp"
#endif
#ifndef GOX_OPTXCHECK_HPP
#include "GraphicsOptiX/optx/optxCheck.hpp"
#endif
#ifndef GOX_OPTXCONTEXT_HPP
#include "GraphicsOptiX/optx/optxContext.hpp"
#endif

#include <string>

namespace gox
{
	namespace
	{
		const char* const c_Tag = "DENOISER";

		OptixImage2D image(CUdeviceptr i_Data, unsigned i_Width, unsigned i_Height, unsigned i_PixelBytes, OptixPixelFormat i_Format)
		{
			OptixImage2D result = {};
			result.data = i_Data;
			result.width = i_Width;
			result.height = i_Height;
			result.pixelStrideInBytes = i_PixelBytes;
			result.rowStrideInBytes = i_Width * i_PixelBytes;
			result.format = i_Format;
			return result;
		}
	}

	optxDenoiser::optxDenoiser()
		: m_Denoiser(NULL)
		, m_Width(0)
		, m_Height(0)
		, m_bFailed(false)
		, m_nRuns(0)
	{
	}

	optxDenoiser::~optxDenoiser()
	{
		Release();
	}

	void optxDenoiser::fail(const std::string& i_Message)
	{
		if (!m_bFailed)
		{
			optxContext::ReportError(c_Tag, i_Message + "; the beauty pass is shown undenoised");
		}
		m_bFailed = true;
	}

	bool optxDenoiser::Prepare(unsigned i_Width, unsigned i_Height)
	{
		if (m_bFailed || i_Width == 0 || i_Height == 0)
		{
			return false;
		}
		if (m_Denoiser && i_Width == m_Width && i_Height == m_Height)
		{
			return true;
		}
		if (!m_Denoiser)
		{
			OptixDenoiserOptions options = {};
			options.guideAlbedo = 1;
			options.guideNormal = 1;
			options.denoiseAlpha = OPTIX_DENOISER_ALPHA_MODE_COPY;
			const OptixResult result =
				optixDenoiserCreate(optxContext::GetOptix(), OPTIX_DENOISER_MODEL_KIND_AOV, &options, &m_Denoiser);
			if (result != OPTIX_SUCCESS)
			{
				m_Denoiser = NULL;
				fail(std::string("the denoiser couldn't be created (") + optxResultName(result) + ")");
				return false;
			}
		}

		OptixDenoiserSizes sizes = {};
		if (OPTIX_CHECK(optixDenoiserComputeMemoryResources(m_Denoiser, i_Width, i_Height, &sizes)))
		{
			fail("the denoiser's memory needs are unknown");
			Release();
			return false;
		}
		//	A queued invocation may still use the old memory: Reserve frees
		//	a grown allocation only after the stream's work
		if (!m_State.Reserve(sizes.stateSizeInBytes) || !m_Scratch.Reserve(sizes.withoutOverlapScratchSizeInBytes))
		{
			fail("out of GPU memory for the denoiser at " + std::to_string(i_Width) + "x" + std::to_string(i_Height));
			Release();
			return false;
		}
		if (OPTIX_CHECK(optixDenoiserSetup(m_Denoiser, optxContext::GetStream(), i_Width, i_Height, m_State.Get(),
										   m_State.GetSize(), m_Scratch.Get(), m_Scratch.GetSize())))
		{
			fail("the denoiser couldn't be set up");
			Release();
			return false;
		}
		m_Width = i_Width;
		m_Height = i_Height;
		return true;
	}

	bool optxDenoiser::Run(CUdeviceptr i_Input, CUdeviceptr i_Output)
	{
		if (!m_Denoiser || !i_Input || !i_Output)
		{
			return false;
		}
		const size_t plane = size_t(m_Width) * m_Height * 4 * sizeof(float);
		const unsigned float4 = 4 * sizeof(float);
		OptixDenoiserGuideLayer guides = {};
		guides.albedo = image(i_Input + plane, m_Width, m_Height, float4, OPTIX_PIXEL_FORMAT_FLOAT4);
		guides.normal = image(i_Input + 2 * plane, m_Width, m_Height, float4, OPTIX_PIXEL_FORMAT_FLOAT4);
		OptixDenoiserLayer layer = {};
		layer.input = image(i_Input, m_Width, m_Height, float4, OPTIX_PIXEL_FORMAT_FLOAT4);
		layer.output = image(i_Output, m_Width, m_Height, 4 * sizeof(unsigned short), OPTIX_PIXEL_FORMAT_HALF4);
		layer.type = OPTIX_DENOISER_AOV_TYPE_BEAUTY;
		OptixDenoiserParams params = {};		// intensity and average colour computed by OptiX; no blending
		if (OPTIX_CHECK(optixDenoiserInvoke(m_Denoiser, optxContext::GetStream(), &params, m_State.Get(), m_State.GetSize(),
											&guides, &layer, 1, 0, 0, m_Scratch.Get(), m_Scratch.GetSize())))
		{
			fail("the denoiser couldn't be run");
			return false;
		}
		++m_nRuns;
		return true;
	}

	void optxDenoiser::Release()
	{
		if (m_Denoiser)
		{
			optxContext::Synchronize();		// a queued invocation still uses it
			OPTIX_CHECK(optixDenoiserDestroy(m_Denoiser));
			m_Denoiser = NULL;
		}
		m_State.Free();
		m_Scratch.Free();
		m_Width = m_Height = 0;
	}
}
