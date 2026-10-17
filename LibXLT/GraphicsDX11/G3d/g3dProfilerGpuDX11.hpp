/****************************************************************************\
**  g3dProfilerGpuDX11.hpp
**
**      g3dProfilerGpuDX11 times g3dProfiler scopes on the GPU with D3D11
**	timestamp queries (a pair per scope) inside a timestamp-disjoint
**	query per frame. Up to c_MaxFramesInFlight frames wait for their
**	results; beyond that the oldest is read back with waiting.
**
**	Only built with MSP_GFX_PROFILING (see Graphics/G3d/g3dProfiler.hpp).
**
**	Extra Large Technology
\****************************************************************************/
#ifdef G3D_PROFILERGPUDX11_HPP
#error g3dProfilerGpuDX11.hpp multiply included
#endif
#define G3D_PROFILERGPUDX11_HPP

#ifdef MSP_GFX_PROFILING

#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

#include <deque>
#include <vector>

struct ID3D11Query;

//============================================================================
//============================================================================
class g3dProfilerGpuDX11 : public g3dProfilerGpu
{
	public:
		g3dProfilerGpuDX11();
		virtual ~g3dProfilerGpuDX11();

		virtual void BeginFrame(uint64_t i_FrameId);
		virtual int  BeginScope();
		virtual void EndScope(int i_Token);
		virtual void EndFrame();
		virtual bool Resolve(bool i_bWait, uint64_t& o_FrameId,
							 std::vector<double>& o_ScopeMs, bool& o_bValid);

	private:
		static const size_t c_MaxFramesInFlight = 4;

		struct FrameQueries
		{
			uint64_t					m_Id;
			ID3D11Query*				m_pDisjoint;
			std::vector<ID3D11Query*>	m_Begin;		// per scope token
			std::vector<ID3D11Query*>	m_End;
			int							m_NumScopes;
		};

		struct Result
		{
			uint64_t			m_Id;
			std::vector<double>	m_ScopeMs;
			bool				m_bValid;
		};

		FrameQueries* get_frame();
		bool read_back(FrameQueries& i_Frame, bool i_bWait, Result& o_Result);

		FrameQueries*				m_pCurrent;
		std::deque<FrameQueries*>	m_InFlight;		// ended, oldest first
		std::vector<FrameQueries*>	m_Free;
		std::deque<Result>			m_Resolved;		// read back early (too many in flight)
};

#endif // MSP_GFX_PROFILING
