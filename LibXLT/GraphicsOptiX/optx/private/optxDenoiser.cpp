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

#include <algorithm>
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
		, m_Overlap(0)
		, m_bMeasured(false)
		, m_bFailed(false)
		, m_nRuns(0)
		, m_nMeasures(0)
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

	//	The denoiser itself (nothing if made), and the overlap its tiles need
	bool optxDenoiser::create()
	{
		if (m_bFailed)
		{
			return false;
		}
		if (m_Denoiser)
		{
			return true;
		}
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
		//	The overlap doesn't depend on the size
		OptixDenoiserSizes sizes = {};
		if (OPTIX_CHECK(optixDenoiserComputeMemoryResources(m_Denoiser, 64, 64, &sizes)))
		{
			fail("the denoiser's memory needs are unknown");
			Release();
			return false;
		}
		m_Overlap = sizes.overlapWindowSizeInPixels;
		m_bMeasured = false;
		return true;
	}

	bool optxDenoiser::GetOverlap(unsigned& o_Pixels)
	{
		o_Pixels = 0;
		if (!create())
		{
			return false;
		}
		o_Pixels = m_Overlap;
		return true;
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
		if (!create())
		{
			return false;
		}

		OptixDenoiserSizes sizes = {};
		if (OPTIX_CHECK(optixDenoiserComputeMemoryResources(m_Denoiser, i_Width, i_Height, &sizes)))
		{
			fail("the denoiser's memory needs are unknown");
			Release();
			return false;
		}
		//	Scratch for tiles with overlap (the larger), and for measuring
		//	the exposure; a queued invocation may still use the old memory:
		//	Reserve frees a grown allocation only after the stream's work
		size_t scratch = std::max(sizes.withOverlapScratchSizeInBytes, sizes.withoutOverlapScratchSizeInBytes);
		scratch = std::max(scratch, std::max(sizes.computeIntensitySizeInBytes, sizes.computeAverageColorSizeInBytes));
		if (!m_State.Reserve(sizes.stateSizeInBytes) || !m_Scratch.Reserve(scratch) || !m_Exposure.Reserve(4 * sizeof(float)))
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

	//	The exposure of the noisy colour, into m_Exposure
	bool optxDenoiser::measure(const OptixImage2D& i_Color)
	{
		const CUstream stream = optxContext::GetStream();
		if (OPTIX_CHECK(optixDenoiserComputeIntensity(m_Denoiser, stream, &i_Color, m_Exposure.Get(), m_Scratch.Get(),
													  m_Scratch.GetSize())) ||
			OPTIX_CHECK(optixDenoiserComputeAverageColor(m_Denoiser, stream, &i_Color, m_Exposure.Get() + sizeof(float),
														 m_Scratch.Get(), m_Scratch.GetSize())))
		{
			fail("the denoiser couldn't measure the image");
			return false;
		}
		m_bMeasured = true;
		++m_nMeasures;
		return true;
	}

	bool optxDenoiser::Run(CUdeviceptr i_Input, CUdeviceptr i_Output, const Window& i_Window, bool i_bMeasure)
	{
		if (!m_Denoiser || !i_Input || !i_Output || i_Window.m_Width == 0 || i_Window.m_Height == 0 ||
			i_Window.m_X + i_Window.m_Width > m_Width || i_Window.m_Y + i_Window.m_Height > m_Height)
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
		layer.output = image(i_Output, i_Window.m_Width, i_Window.m_Height, 4 * sizeof(unsigned short), OPTIX_PIXEL_FORMAT_HALF4);
		layer.type = OPTIX_DENOISER_AOV_TYPE_BEAUTY;
		if ((i_bMeasure || !m_bMeasured) && !measure(layer.input))
		{
			return false;
		}
		OptixDenoiserParams params = {};		// no blending
		params.hdrIntensity = m_Exposure.Get();
		params.hdrAverageColor = m_Exposure.Get() + sizeof(float);
		if (OPTIX_CHECK(optixDenoiserInvoke(m_Denoiser, optxContext::GetStream(), &params, m_State.Get(), m_State.GetSize(),
											&guides, &layer, 1, i_Window.m_X, i_Window.m_Y, m_Scratch.Get(),
											m_Scratch.GetSize())))
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
		m_Exposure.Free();
		m_Width = m_Height = 0;
		m_Overlap = 0;
		m_bMeasured = false;
	}
}
