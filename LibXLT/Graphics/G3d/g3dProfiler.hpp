/****************************************************************************\
**  g3dProfiler.hpp
**
**      g3dProfiler times rendering per frame: nested named scopes with CPU
**	time, GPU time (from a backend's g3dProfilerGpu) and draw counts,
**	written to a CSV file, one row per scope per frame:
**
**	  backend,frame,sim_time,path,depth,cpu_ms,gpu_ms,draws
**
**	path joins scope names from the frame's root with '/'. GPU times
**	arrive frames later; rows are written in frame order once a frame's
**	GPU times are known, and on Stop() whatever never resolved is written
**	with an empty gpu_ms. Times and draws are inclusive of child scopes.
**
**	Markers go through the G3D_PROFILE_* macros, which compile to nothing
**	unless MSP_GFX_PROFILING is defined (Debug builds, or Release built
**	with MspGfxProfiling=true). Everything does nothing until Start().
**	Use from the render thread only.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef G3D_PROFILER_HPP
#error g3dProfiler.hpp multiply included
#endif
#define G3D_PROFILER_HPP

#include <cstdint>
#include <string>
#include <vector>

//============================================================================
//	g3dProfilerGpu - GPU timing, implemented by each graphics backend
//============================================================================
class g3dProfilerGpu
{
	public:
		virtual ~g3dProfilerGpu() {}

		virtual void BeginFrame(uint64_t i_FrameId) = 0;

		//	BeginScope returns a token: 0..n-1 in the order of BeginScope
		//	calls within the frame.
		virtual int  BeginScope() = 0;
		virtual void EndScope(int i_Token) = 0;
		virtual void EndFrame() = 0;

		//	Resolve fills the GPU ms per token for the oldest finished frame
		//	and returns true, or returns false if none is ready. With
		//	i_bWait it blocks until the oldest submitted frame is ready (if
		//	there is one). o_bValid false means the times are unusable (the
		//	GPU clock was disjoint); the frame is still reported.
		virtual bool Resolve(bool i_bWait, uint64_t& o_FrameId,
							 std::vector<double>& o_ScopeMs, bool& o_bValid) = 0;
};

//============================================================================
//============================================================================
class g3dProfiler
{
	public:
		//	Start opens i_CsvPath and writes the header; i_Backend fills the
		//	backend column ("dx11", "vulkan").
		static void Start(const std::string& i_CsvPath, const std::string& i_Backend);

		//	Stop waits for outstanding GPU results, writes every recorded
		//	frame and closes the file.
		static void Stop();
		static bool IsEnabled();

		//	SetGpu installs the backend's GPU timer (not owned); NULL means
		//	CPU times only.
		static void SetGpu(g3dProfilerGpu* i_pGpu);

		static void BeginFrame(float i_SimTime);
		static void EndFrame();
		static bool IsInFrame();

		//	i_Name must outlive the frame (use string literals).
		static void PushScope(const char* i_Name);
		static void PopScope();

		//	AddDraws counts draws in every open scope.
		static void AddDraws(int i_Count);
};

//============================================================================
//	g3dProfileScope - a scope for the lifetime of the object
//============================================================================
class g3dProfileScope
{
	public:
		explicit g3dProfileScope(const char* i_Name);
		~g3dProfileScope();

	private:
		bool m_bPushed;
};

//============================================================================
//	g3dProfileFrameScope - begins and ends a frame (root scope i_Name) if
//	none is open; otherwise it is an ordinary nested scope.
//============================================================================
class g3dProfileFrameScope
{
	public:
		g3dProfileFrameScope(const char* i_Name, float i_SimTime);
		~g3dProfileFrameScope();

	private:
		bool m_bPushed;
		bool m_bOwnsFrame;
};

#define G3D_PROFILE_CONCAT2(a, b) a##b
#define G3D_PROFILE_CONCAT(a, b) G3D_PROFILE_CONCAT2(a, b)
#define G3D_PROFILE_UNIQUE(prefix) G3D_PROFILE_CONCAT(prefix, __LINE__)

#ifdef MSP_GFX_PROFILING
#define G3D_PROFILE_SCOPE(name)				g3dProfileScope G3D_PROFILE_UNIQUE(g3dProfScope_)(name)
#define G3D_PROFILE_FRAME_SCOPE(name, t)	g3dProfileFrameScope G3D_PROFILE_UNIQUE(g3dProfFrame_)(name, t)
#define G3D_PROFILE_DRAWS(n)				g3dProfiler::AddDraws(n)
#else
#define G3D_PROFILE_SCOPE(name)				((void)0)
#define G3D_PROFILE_FRAME_SCOPE(name, t)	((void)0)
#define G3D_PROFILE_DRAWS(n)				((void)0)
#endif
