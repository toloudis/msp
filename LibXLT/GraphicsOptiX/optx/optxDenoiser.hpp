/*****************************************************************************
**  optxDenoiser.hpp
**
**      optxDenoiser runs the OptiX AI denoiser over the beauty pass's
**	resolved HDR image (g3dTraceRendererOptiX). It uses OptiX's built-in
**	single-image model, OPTIX_DENOISER_MODEL_KIND_AOV (OptiX 9.1 maps the
**	deprecated HDR model onto it), guided by albedo and normals; alpha is
**	copied, not denoised. The image's HDR intensity and average colour
**	(the exposure OptiX scales the input by) are measured from the input
**	when Run is asked to, and kept for the invocations after it.
**
**	Tiles: the input may be a traced region larger than the output, a tile
**	of a bigger image with a border of up to GetOverlap pixels on each
**	side that has neighbours (OptiX's tiled denoising: the border gives
**	the tile's edges their surroundings and is dropped from the output).
**	Every tile of one image should be denoised with the first tile's
**	exposure (measured once, then reused), so tiles don't each guess
**	their own.
**
**	Input: three planes of Width x Height float4, one after the other
**	(InputBytes):
**	- the noisy colour, linear (alpha: coverage, copied to the output);
**	- the albedo guide (rgb in [0, 1]; w unused);
**	- the normal guide, world space (xyz; w unused), as the AOV model wants.
**	Output: the window of the input Run is given, as RGBA16F (the frame
**	share's e_ColorHalf layout), its rows packed.
**
**	The denoiser is created on the first Prepare. Its state and scratch
**	memory are sized for one image size and remade when it changes. If the
**	driver has no denoiser (or memory runs out), the failure is reported
**	once (optxContext::ReportError, which counts it as an error) and
**	HasFailed stays true: callers then show the image undenoised.
**
**	Work goes on optxContext's stream and isn't waited for. Render thread,
**	with the OptiX context current.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXDENOISER_HPP
#error optxDenoiser.hpp multiply included
#endif
#define GOX_OPTXDENOISER_HPP

#ifndef GOX_OPTXSDK_HPP
#include "GraphicsOptiX/optx/optxSdk.hpp"
#endif
#ifndef GOX_CUBUFFER_HPP
#include "GraphicsOptiX/cu/cuBuffer.hpp"
#endif

#include <cstddef>
#include <string>

namespace gox
{
	class optxDenoiser
	{
		public:
			optxDenoiser();
			~optxDenoiser();
			optxDenoiser(const optxDenoiser&) = delete;
			optxDenoiser& operator=(const optxDenoiser&) = delete;

			//--------------------------------------------------------------------
			//	InputBytes - the size of the input planes for an image
			//--------------------------------------------------------------------
			static size_t InputBytes(unsigned i_Width, unsigned i_Height)
			{
				return size_t(i_Width) * i_Height * 4 * sizeof(float) * 3;
			}

			//--------------------------------------------------------------------
			//	Prepare - the denoiser, with state and scratch for i_Width x
			//	i_Height (nothing if ready for that size). False if it can't be
			//	made (reported once; HasFailed).
			//--------------------------------------------------------------------
			bool Prepare(unsigned i_Width, unsigned i_Height);

			//--------------------------------------------------------------------
			//	GetOverlap - the border, in pixels, a tile's input needs on each
			//	side that has neighbours (OptixDenoiserSizes::
			//	overlapWindowSizeInPixels). False if the denoiser can't be made
			//	(reported once; HasFailed).
			//--------------------------------------------------------------------
			bool GetOverlap(unsigned& o_Pixels);

			//	Where the output lies in the input (Prepare's size)
			struct Window
			{
				unsigned	m_X = 0;
				unsigned	m_Y = 0;
				unsigned	m_Width = 0;
				unsigned	m_Height = 0;
			};

			//--------------------------------------------------------------------
			//	Run - denoises the input planes at i_Input (Prepare's size) into
			//	the RGBA16F image at i_Output, the input's i_Window, queued on
			//	the stream. With i_bMeasure (or nothing measured yet since
			//	Prepare made the denoiser) the exposure is measured from this
			//	input first; else the last one is used. False (reported) if it
			//	can't be queued.
			//--------------------------------------------------------------------
			bool Run(CUdeviceptr i_Input, CUdeviceptr i_Output, const Window& i_Window, bool i_bMeasure = true);

			//--------------------------------------------------------------------
			//	Release - destroys the denoiser and frees its memory (after the
			//	stream's work); Prepare makes them again. A failure stays.
			//--------------------------------------------------------------------
			void Release();

			bool HasFailed() const { return m_bFailed; }
			unsigned GetWidth() const { return m_Width; }
			unsigned GetHeight() const { return m_Height; }
			//	For tests and the profiler: invocations so far, and exposure
			//	measurements so far
			int GetRunCount() const { return m_nRuns; }
			int GetMeasureCount() const { return m_nMeasures; }

		private:
			void fail(const std::string& i_Message);
			bool create();
			bool measure(const OptixImage2D& i_Color);

			OptixDenoiser	m_Denoiser;
			cuBuffer		m_State;
			cuBuffer		m_Scratch;
			cuBuffer		m_Exposure;			// the intensity (1 float), then the average colour (3)
			unsigned		m_Width;
			unsigned		m_Height;
			unsigned		m_Overlap;
			bool			m_bMeasured;
			bool			m_bFailed;
			int				m_nRuns;
			int				m_nMeasures;
	};
}
