/*****************************************************************************
**  optxFrameShare.hpp
**
**      optxFrameShare carries traced frames from CUDA to a Vulkan render
**	target: a colour buffer (a 32-bit texel per pixel, in the target's
**	byte order, R8G8B8A8 or B8G8R8A8) and a depth buffer (a 32-bit texel
**	per pixel, as the target's depth aspect is copied from a buffer: D24
**	in the low 24 bits for D24_UNORM_S8_UINT, float for D32_SFLOAT). Both
**	are exportable vkdBuffers that CUDA writes in place
**	(cuExternalMemoryCache), and Vulkan copies into the target's images
**	(vkCmdCopyBufferToImage). One owner per target size: a renderer.
**
**	Double buffered: each BeginWrite moves to the other set of frame
**	outputs (FrameOutputs: a colour and a depth buffer), so CUDA writes
**	frame N+1 while Vulkan still copies frame N (with one set, each side
**	waited for the other every frame). Color() and Depth() are the outputs
**	being written, or after EndWrite the outputs last written, which
**	CopyTo copies.
**
**	Synchronization, one timeline semaphore each way:
**	- BeginWrite, before CUDA overwrites a set of outputs, puts a wait on
**	  the OptiX stream for Vulkan's timeline value of the last copy from
**	  that set (submitting Vulkan's open command buffer first if the copy is
**	  still in it; nothing if that value has completed, as it usually has:
**	  the copy was two frames ago).
**	- EndWrite, after the work that writes them is queued, signals the
**	  frame semaphore (made here by Vulkan, exportable, imported into
**	  CUDA) with the next value.
**	- CopyTo records the copies in Vulkan's open command buffer, and the
**	  first copy after a write adds a wait for that value to the next
**	  Vulkan submission (vkdDevice::AddSubmitWait).
**
**	Render thread, with the OptiX context current. The work goes on
**	optxContext's stream and vkdContext's command stream.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_OPTXFRAMESHARE_HPP
#error optxFrameShare.hpp multiply included
#endif
#define GOX_OPTXFRAMESHARE_HPP

#ifndef GOX_CUEXTERNALSEMAPHORE_HPP
#include "GraphicsOptiX/cu/cuExternalSemaphore.hpp"
#endif
#ifndef GOX_OPTXAOPARAMS_H
#include "GraphicsOptiX/optx/private/Programs/optxAOParams.h"
#endif

#include <volk.h>

#include <cstdint>
#include <memory>

namespace gvk
{
	class vkdBuffer;
	class vkdContext;
	class vkdDevice;
	class vkdImage;
}

namespace gox
{
	class cuExternalMemoryCache;

	class optxFrameShare
	{
		public:
			//--------------------------------------------------------------------
			//	The colour and depth formats frames can be copied into, and
			//	what their texels are. False for any other format (a float
			//	render target, say).
			//--------------------------------------------------------------------
			static bool GetColorOrder(VkFormat i_Format, optxColorOrder& o_Order);
			static bool GetDepthLayout(VkFormat i_Format, optxDepthLayout& o_Layout);

			//--------------------------------------------------------------------
			//	i_Device must have external memory; i_Memory and i_VulkanTimeline
			//	(i_Device's timeline imported into CUDA) are the GAS cache's
			//	(g3dGasCacheOptiX::GetMemoryCache, GetVulkanTimeline). The
			//	first Resize makes the frame semaphore.
			//--------------------------------------------------------------------
			optxFrameShare(gvk::vkdDevice& i_Device, gvk::vkdContext& i_Context, cuExternalMemoryCache& i_Memory,
						   cuExternalSemaphore& i_VulkanTimeline);
			~optxFrameShare();
			optxFrameShare(const optxFrameShare&) = delete;
			optxFrameShare& operator=(const optxFrameShare&) = delete;

			//--------------------------------------------------------------------
			//	Resize - buffers for i_Width x i_Height (nothing if they are
			//	that size). Their contents are undefined after a change. False
			//	(logged) if they or the semaphore can't be made.
			//--------------------------------------------------------------------
			bool Resize(unsigned i_Width, unsigned i_Height);
			unsigned GetWidth() const { return m_Width; }
			unsigned GetHeight() const { return m_Height; }

			//--------------------------------------------------------------------
			//	Release - frees the buffers and the semaphore (after the work
			//	using them); Resize makes them again
			//--------------------------------------------------------------------
			void Release();

			//--------------------------------------------------------------------
			//	The CUDA side: BeginWrite (which picks the outputs to write), then
			//	the work writing Color() and Depth() on the OptiX stream, then
			//	EndWrite (see above)
			//--------------------------------------------------------------------
			bool BeginWrite();
			CUdeviceptr Color() const { return m_Outputs[m_Current].m_ColorPtr; }
			CUdeviceptr Depth() const { return m_Outputs[m_Current].m_DepthPtr; }
			bool EndWrite();

			//--------------------------------------------------------------------
			//	CopyTo - the Vulkan side: copies the colour buffer into i_pColor
			//	and, if given, the depth buffer into i_pDepth's depth aspect, as
			//	the next commands of the context. The images must be the
			//	buffers' size, single-sampled, of formats GetColorOrder and
			//	GetDepthLayout accept, with TRANSFER_DST usage.
			//--------------------------------------------------------------------
			void CopyTo(gvk::vkdImage* i_pColor, gvk::vkdImage* i_pDepth);

			//	For tests: the last value the frame semaphore was signalled with
			uint64_t GetFrameValue() const { return m_FrameValue; }
			const gvk::vkdBuffer* GetColorBuffer() const { return m_Outputs[m_Current].m_pColor.get(); }
			const gvk::vkdBuffer* GetDepthBuffer() const { return m_Outputs[m_Current].m_pDepth.get(); }

		private:
			//	What CUDA writes for one frame and Vulkan copies: a colour and a depth buffer
			struct FrameOutputs
			{
				std::unique_ptr<gvk::vkdBuffer>	m_pColor;
				std::unique_ptr<gvk::vkdBuffer>	m_pDepth;
				CUdeviceptr						m_ColorPtr = 0;
				CUdeviceptr						m_DepthPtr = 0;
				uint64_t						m_LastReadValue = 0;	// Vulkan's timeline value of the last copy (0: none)
			};
			static const int c_OutputCount = 2;

			bool make_semaphore();
			void free_buffers();

			gvk::vkdDevice&				m_Device;
			gvk::vkdContext&			m_Context;
			cuExternalMemoryCache&		m_Memory;
			cuExternalSemaphore&		m_VulkanTimeline;
			unsigned					m_Width;
			unsigned					m_Height;
			FrameOutputs				m_Outputs[c_OutputCount];
			int							m_Current;			// the outputs written, or last written
			VkSemaphore					m_Frames;			// signalled by CUDA
			cuExternalSemaphore			m_CudaFrames;		// the same, imported
			uint64_t					m_FrameValue;		// the last value signalled
			bool						m_bWaitPending;		// no copy has waited for m_FrameValue yet
	};
}
