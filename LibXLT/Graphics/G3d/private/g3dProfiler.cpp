/****************************************************************************\
**  g3dProfiler.cpp
**
**      See g3dProfiler.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <chrono>
#include <deque>
#include <fstream>
#include <iomanip>

namespace
{
	typedef std::chrono::steady_clock Clock;

	struct Scope
	{
		const char*			m_pName;
		int					m_Parent;		// index in the frame, -1 for the root
		int					m_Depth;
		int					m_GpuToken;		// -1 without a GPU timer
		Clock::time_point	m_Start;
		double				m_CpuMs;
		int					m_Draws;
	};

	struct Frame
	{
		uint64_t			m_Id;
		float				m_SimTime;
		std::vector<Scope>	m_Scopes;
		std::vector<double>	m_GpuMs;		// per GPU token
		bool				m_bGpuDone;		// GPU times known (or not expected)
		bool				m_bGpuValid;
	};

	bool				l_bEnabled = false;
	std::ofstream*		l_pOut = NULL;
	std::string			l_Backend;
	g3dProfilerGpu*		l_pGpu = NULL;
	bool				l_bInFrame = false;
	Frame				l_Current;
	std::vector<int>	l_Stack;			// open scopes, indices into l_Current.m_Scopes
	uint64_t			l_NextFrameId = 0;
	std::deque<Frame>	l_Finished;			// waiting for GPU times, in frame order

	std::string path_of(const Frame& i_Frame, int i_Scope)
	{
		std::string path = i_Frame.m_Scopes[i_Scope].m_pName;
		for (int parent = i_Frame.m_Scopes[i_Scope].m_Parent; parent >= 0;
			 parent = i_Frame.m_Scopes[parent].m_Parent)
		{
			path = std::string(i_Frame.m_Scopes[parent].m_pName) + "/" + path;
		}
		return path;
	}

	void write_frame(const Frame& i_Frame)
	{
		std::ofstream& out = *l_pOut;
		for (size_t i = 0; i < i_Frame.m_Scopes.size(); ++i)
		{
			const Scope& scope = i_Frame.m_Scopes[i];
			out << l_Backend << ','
				<< i_Frame.m_Id << ','
				<< std::fixed << std::setprecision(4) << i_Frame.m_SimTime << ','
				<< path_of(i_Frame, static_cast<int>(i)) << ','
				<< scope.m_Depth << ','
				<< scope.m_CpuMs << ',';
			if (i_Frame.m_bGpuValid && scope.m_GpuToken >= 0 &&
				scope.m_GpuToken < static_cast<int>(i_Frame.m_GpuMs.size()))
			{
				out << i_Frame.m_GpuMs[scope.m_GpuToken];
			}
			out << ',' << scope.m_Draws << '\n';
		}
	}

	//	Takes whatever GPU results are ready (all outstanding ones with
	//	i_bWait), then writes the finished frames at the front whose GPU
	//	times are known.
	void collect_and_write(bool i_bWait)
	{
		if (l_pGpu)
		{
			uint64_t frame_id = 0;
			std::vector<double> scope_ms;
			bool valid = false;
			while (l_pGpu->Resolve(i_bWait, frame_id, scope_ms, valid))
			{
				for (Frame& frame : l_Finished)
				{
					if (frame.m_Id == frame_id)
					{
						frame.m_GpuMs = scope_ms;
						frame.m_bGpuValid = valid;
						frame.m_bGpuDone = true;
						break;
					}
				}
			}
		}
		while (!l_Finished.empty() && l_Finished.front().m_bGpuDone)
		{
			write_frame(l_Finished.front());
			l_Finished.pop_front();
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g3dProfiler::Start(const std::string& i_CsvPath, const std::string& i_Backend)
{
	DBG_ASSERT(!l_bEnabled, "g3dProfiler already started");
	l_pOut = new std::ofstream(i_CsvPath.c_str(), std::ios::out | std::ios::trunc);
	if (!l_pOut->is_open())
	{
		DBG_WARNING("g3dProfiler: cannot open " << i_CsvPath.c_str());
		delete l_pOut;
		l_pOut = NULL;
		return;
	}
	*l_pOut << "backend,frame,sim_time,path,depth,cpu_ms,gpu_ms,draws\n";
	l_Backend = i_Backend;
	l_NextFrameId = 0;
	l_bInFrame = false;
	l_Stack.clear();
	l_Finished.clear();
	l_bEnabled = true;
}

void g3dProfiler::Stop()
{
	if (!l_bEnabled)
	{
		return;
	}
	DBG_ASSERT(!l_bInFrame, "g3dProfiler stopped inside a frame");
	collect_and_write(true);
	for (Frame& frame : l_Finished)
	{
		frame.m_bGpuValid = false;
		write_frame(frame);
	}
	l_Finished.clear();
	l_pOut->close();
	delete l_pOut;
	l_pOut = NULL;
	l_bEnabled = false;
	l_bInFrame = false;
}

bool g3dProfiler::IsEnabled()
{
	return l_bEnabled;
}

void g3dProfiler::SetGpu(g3dProfilerGpu* i_pGpu)
{
	DBG_ASSERT(!l_bInFrame, "g3dProfiler::SetGpu inside a frame");
	l_pGpu = i_pGpu;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g3dProfiler::BeginFrame(float i_SimTime)
{
	if (!l_bEnabled)
	{
		return;
	}
	DBG_ASSERT(!l_bInFrame, "g3dProfiler::BeginFrame inside a frame");
	l_Current = Frame();
	l_Current.m_Id = l_NextFrameId++;
	l_Current.m_SimTime = i_SimTime;
	l_Current.m_bGpuDone = (l_pGpu == NULL);
	l_Current.m_bGpuValid = false;
	l_Stack.clear();
	l_bInFrame = true;
	if (l_pGpu)
	{
		l_pGpu->BeginFrame(l_Current.m_Id);
	}
}

void g3dProfiler::EndFrame()
{
	if (!l_bInFrame)
	{
		return;
	}
	DBG_ASSERT(l_Stack.empty(), "g3dProfiler::EndFrame with open scopes");
	if (l_pGpu)
	{
		l_pGpu->EndFrame();
	}
	l_bInFrame = false;
	l_Finished.push_back(l_Current);
	collect_and_write(false);
}

bool g3dProfiler::IsInFrame()
{
	return l_bInFrame;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g3dProfiler::PushScope(const char* i_Name)
{
	if (!l_bInFrame)
	{
		return;
	}
	Scope scope;
	scope.m_pName = i_Name;
	scope.m_Parent = l_Stack.empty() ? -1 : l_Stack.back();
	scope.m_Depth = static_cast<int>(l_Stack.size());
	scope.m_GpuToken = l_pGpu ? l_pGpu->BeginScope() : -1;
	scope.m_CpuMs = 0.0;
	scope.m_Draws = 0;
	scope.m_Start = Clock::now();
	l_Current.m_Scopes.push_back(scope);
	l_Stack.push_back(static_cast<int>(l_Current.m_Scopes.size()) - 1);
}

void g3dProfiler::PopScope()
{
	if (!l_bInFrame)
	{
		return;
	}
	DBG_ASSERT(!l_Stack.empty(), "g3dProfiler::PopScope without a scope");
	if (l_Stack.empty())
	{
		return;
	}
	Scope& scope = l_Current.m_Scopes[l_Stack.back()];
	if (l_pGpu)
	{
		l_pGpu->EndScope(scope.m_GpuToken);
	}
	scope.m_CpuMs = std::chrono::duration<double, std::milli>(Clock::now() - scope.m_Start).count();
	l_Stack.pop_back();
}

void g3dProfiler::AddDraws(int i_Count)
{
	if (!l_bInFrame)
	{
		return;
	}
	for (int index : l_Stack)
	{
		l_Current.m_Scopes[index].m_Draws += i_Count;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dProfileScope::g3dProfileScope(const char* i_Name)
	: m_bPushed(g3dProfiler::IsInFrame())
{
	if (m_bPushed)
	{
		g3dProfiler::PushScope(i_Name);
	}
}

g3dProfileScope::~g3dProfileScope()
{
	if (m_bPushed)
	{
		g3dProfiler::PopScope();
	}
}

g3dProfileFrameScope::g3dProfileFrameScope(const char* i_Name, float i_SimTime)
	: m_bPushed(false)
	, m_bOwnsFrame(false)
{
	if (!g3dProfiler::IsEnabled())
	{
		return;
	}
	if (!g3dProfiler::IsInFrame())
	{
		g3dProfiler::BeginFrame(i_SimTime);
		m_bOwnsFrame = true;
	}
	g3dProfiler::PushScope(i_Name);
	m_bPushed = true;
}

g3dProfileFrameScope::~g3dProfileFrameScope()
{
	if (m_bPushed)
	{
		g3dProfiler::PopScope();
	}
	if (m_bOwnsFrame)
	{
		g3dProfiler::EndFrame();
	}
}
