/*****************************************************************************
**  g3dProfilerGpuOptiX.hpp
**
**      g3dProfilerGpuOptiX is the OptiX backend's GPU timer for g3dProfiler:
**	it wraps GraphicsVulkan's (g3dProfilerGpuVk, whose times fill the
**	gpu_ms column) and also times each scope on the CUDA stream, with a
**	pair of CUDA events recorded on optxContext's stream at the scope's
**	start and end (the cuda_ms column, g3dProfilerGpu::GetSecondMs).
**	Vulkan's timestamps can't see CUDA's work: a Vulkan submission that
**	waits for a traced frame starts its timestamps after the wait.
**
**	A scope's CUDA time is from when the stream reached its start event to
**	when it reached its end event: the CUDA work queued inside the scope,
**	plus any wait (a semaphore, earlier work) the stream met in between.
**	A frame's CUDA times are read when Vulkan's are: by then the CUDA work
**	Vulkan waited for has finished.
**
**	Installed by GraphicsOptiXLayer over Vulkan's timer. Render thread;
**	makes the OptiX context current at each frame's start. Only built with
**	MSP_GFX_PROFILING (see Graphics/G3d/g3dProfiler.hpp).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G3DPROFILERGPUOPTIX_HPP
#error g3dProfilerGpuOptiX.hpp multiply included
#endif
#define GOX_G3DPROFILERGPUOPTIX_HPP

#ifdef MSP_GFX_PROFILING

#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif
#ifndef GOX_OPTXSDK_HPP
#include "GraphicsOptiX/optx/optxSdk.hpp"
#endif

#include <deque>
#include <vector>

namespace gox
{
	class g3dProfilerGpuOptiX : public g3dProfilerGpu
	{
		public:
			//	i_pInner: Vulkan's timer (not owned)
			explicit g3dProfilerGpuOptiX(g3dProfilerGpu* i_pInner);
			virtual ~g3dProfilerGpuOptiX();

			g3dProfilerGpu* GetInner() const { return m_pInner; }

			virtual void BeginFrame(uint64_t i_FrameId);
			virtual int  BeginScope();
			virtual void EndScope(int i_Token);
			virtual void EndFrame();
			virtual bool Resolve(bool i_bWait, uint64_t& o_FrameId, std::vector<double>& o_ScopeMs, bool& o_bValid);
			virtual void GetSecondMs(std::vector<double>& o_ScopeMs);

		private:
			struct Frame
			{
				uint64_t				m_Id = 0;
				std::vector<CUevent>	m_Events;	// start and end per token
			};

			CUevent take_event();
			void record(Frame& io_Frame, size_t i_Index);
			void recycle(Frame& io_Frame);

			g3dProfilerGpu*			m_pInner;
			Frame					m_Current;
			bool					m_bInFrame;
			std::deque<Frame>		m_Frames;		// ended, not yet resolved
			std::vector<CUevent>	m_Pool;
			std::vector<double>		m_LastMs;		// the frame Resolve last returned
	};
}

#endif
