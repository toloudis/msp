/*****************************************************************************
**  g3dProfilerGpuVk.hpp
**
**      g3dProfilerGpuVk times g3dProfiler scopes on the GPU with Vulkan
**	timestamp queries (a pair per scope, a query pool per frame), the
**	counterpart of g3dProfilerGpuDX11. A frame's times are read once the
**	submission that contains it has completed.
**
**	Only built with MSP_GFX_PROFILING (see Graphics/G3d/g3dProfiler.hpp).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_G3DPROFILERGPUVK_HPP
#error g3dProfilerGpuVk.hpp multiply included
#endif
#define GVK_G3DPROFILERGPUVK_HPP

#ifdef MSP_GFX_PROFILING

#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

#include <volk.h>

#include <deque>
#include <vector>

namespace gvk
{
	class vkdDevice;

	class g3dProfilerGpuVk : public g3dProfilerGpu
	{
		public:
			static const uint32_t c_MaxScopes = 1024;		// per frame; more are reported invalid

			explicit g3dProfilerGpuVk(vkdDevice& i_Device);
			virtual ~g3dProfilerGpuVk();

			virtual void BeginFrame(uint64_t i_FrameId);
			virtual int  BeginScope();
			virtual void EndScope(int i_Token);
			virtual void EndFrame();
			virtual bool Resolve(bool i_bWait, uint64_t& o_FrameId,
								 std::vector<double>& o_ScopeMs, bool& o_bValid);

		private:
			struct Frame
			{
				uint64_t	m_Id = 0;
				VkQueryPool	m_Pool = VK_NULL_HANDLE;
				uint32_t	m_Scopes = 0;
				uint64_t	m_Value = 0;		// timeline value of the submission holding it
			};

			vkdDevice&			m_Device;
			Frame*				m_pCurrent = nullptr;
			std::deque<Frame*>	m_InFlight;
			std::vector<Frame*>	m_Free;
			double				m_NsPerTick;
			uint64_t			m_ValidMask;
	};
}

#endif // MSP_GFX_PROFILING
