/****************************************************************************\
**  g3dProfilerGpuDX11.cpp
**
**      See g3dProfilerGpuDX11.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef MSP_GFX_PROFILING

#ifndef G3D_PROFILERGPUDX11_HPP
#include "GraphicsDX11/G3d/g3dProfilerGpuDX11.hpp"
#endif
#ifndef G2D_DX11GLOBALWIN_HPP
#include "GraphicsDX11/G2d/g2dDX11GlobalWin.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <d3d11.h>

namespace
{
	ID3D11Query* create_query(D3D11_QUERY i_Type)
	{
		D3D11_QUERY_DESC desc;
		desc.Query = i_Type;
		desc.MiscFlags = 0;
		ID3D11Query* query = NULL;
		HRESULT hr = g2dDX11Global::g_pDevice->CreateQuery(&desc, &query);
		DBG_ASSERT(SUCCEEDED(hr), "g3dProfilerGpuDX11: CreateQuery failed");
		return SUCCEEDED(hr) ? query : NULL;
	}

	void release(ID3D11Query*& io_Query)
	{
		if (io_Query)
		{
			io_Query->Release();
			io_Query = NULL;
		}
	}

	//	get_data reads a query; with i_bWait it flushes and spins until the
	//	result is there. Returns false if not ready (or failed).
	template <typename T>
	bool get_data(ID3D11Query* i_pQuery, T& o_Data, bool i_bWait)
	{
		ID3D11DeviceContext* context = g2dDX11Global::g_pDeviceContext;
		for (;;)
		{
			HRESULT hr = context->GetData(i_pQuery, &o_Data, sizeof(T),
										  i_bWait ? 0 : D3D11_ASYNC_GETDATA_DONOTFLUSH);
			if (hr == S_OK)
			{
				return true;
			}
			if (hr != S_FALSE || !i_bWait)
			{
				return false;
			}
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dProfilerGpuDX11::g3dProfilerGpuDX11()
	: m_pCurrent(NULL)
{
}

g3dProfilerGpuDX11::~g3dProfilerGpuDX11()
{
	std::vector<FrameQueries*> all(m_Free);
	all.insert(all.end(), m_InFlight.begin(), m_InFlight.end());
	if (m_pCurrent)
	{
		all.push_back(m_pCurrent);
	}
	for (FrameQueries* frame : all)
	{
		release(frame->m_pDisjoint);
		for (ID3D11Query*& query : frame->m_Begin)
		{
			release(query);
		}
		for (ID3D11Query*& query : frame->m_End)
		{
			release(query);
		}
		delete frame;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dProfilerGpuDX11::FrameQueries* g3dProfilerGpuDX11::get_frame()
{
	if (!m_Free.empty())
	{
		FrameQueries* frame = m_Free.back();
		m_Free.pop_back();
		return frame;
	}
	FrameQueries* frame = new FrameQueries;
	frame->m_Id = 0;
	frame->m_pDisjoint = create_query(D3D11_QUERY_TIMESTAMP_DISJOINT);
	frame->m_NumScopes = 0;
	return frame;
}

void g3dProfilerGpuDX11::BeginFrame(uint64_t i_FrameId)
{
	//	Too many frames waiting: read the oldest back now and keep its
	//	result for the next Resolve.
	while (m_InFlight.size() >= c_MaxFramesInFlight)
	{
		FrameQueries* oldest = m_InFlight.front();
		m_InFlight.pop_front();
		Result result;
		read_back(*oldest, true, result);
		m_Resolved.push_back(result);
		m_Free.push_back(oldest);
	}

	m_pCurrent = get_frame();
	m_pCurrent->m_Id = i_FrameId;
	m_pCurrent->m_NumScopes = 0;
	if (m_pCurrent->m_pDisjoint)
	{
		g2dDX11Global::g_pDeviceContext->Begin(m_pCurrent->m_pDisjoint);
	}
}

int g3dProfilerGpuDX11::BeginScope()
{
	DBG_ASSERT(m_pCurrent, "g3dProfilerGpuDX11::BeginScope outside a frame");
	int token = m_pCurrent->m_NumScopes++;
	if (token >= static_cast<int>(m_pCurrent->m_Begin.size()))
	{
		m_pCurrent->m_Begin.push_back(create_query(D3D11_QUERY_TIMESTAMP));
		m_pCurrent->m_End.push_back(create_query(D3D11_QUERY_TIMESTAMP));
	}
	if (m_pCurrent->m_Begin[token])
	{
		g2dDX11Global::g_pDeviceContext->End(m_pCurrent->m_Begin[token]);
	}
	return token;
}

void g3dProfilerGpuDX11::EndScope(int i_Token)
{
	DBG_ASSERT(m_pCurrent && i_Token < m_pCurrent->m_NumScopes, "g3dProfilerGpuDX11::EndScope: bad token");
	if (m_pCurrent->m_End[i_Token])
	{
		g2dDX11Global::g_pDeviceContext->End(m_pCurrent->m_End[i_Token]);
	}
}

void g3dProfilerGpuDX11::EndFrame()
{
	DBG_ASSERT(m_pCurrent, "g3dProfilerGpuDX11::EndFrame outside a frame");
	if (m_pCurrent->m_pDisjoint)
	{
		g2dDX11Global::g_pDeviceContext->End(m_pCurrent->m_pDisjoint);
	}
	m_InFlight.push_back(m_pCurrent);
	m_pCurrent = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool g3dProfilerGpuDX11::read_back(FrameQueries& i_Frame, bool i_bWait, Result& o_Result)
{
	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjoint;
	if (!i_Frame.m_pDisjoint || !get_data(i_Frame.m_pDisjoint, disjoint, i_bWait))
	{
		if (!i_bWait)
		{
			return false;
		}
		disjoint.Disjoint = TRUE;		// unreadable: report the frame without times
		disjoint.Frequency = 0;
	}

	o_Result.m_Id = i_Frame.m_Id;
	o_Result.m_bValid = !disjoint.Disjoint && disjoint.Frequency != 0;
	o_Result.m_ScopeMs.assign(i_Frame.m_NumScopes, 0.0);
	for (int i = 0; i < i_Frame.m_NumScopes; ++i)
	{
		UINT64 begin = 0;
		UINT64 end = 0;
		//	The disjoint query ended after every timestamp, so these are
		//	ready (wait anyway in case the driver says otherwise).
		if (!i_Frame.m_Begin[i] || !i_Frame.m_End[i] ||
			!get_data(i_Frame.m_Begin[i], begin, true) ||
			!get_data(i_Frame.m_End[i], end, true))
		{
			o_Result.m_bValid = false;
			continue;
		}
		if (o_Result.m_bValid)
		{
			o_Result.m_ScopeMs[i] = 1000.0 * static_cast<double>(end - begin) / static_cast<double>(disjoint.Frequency);
		}
	}
	return true;
}

bool g3dProfilerGpuDX11::Resolve(bool i_bWait, uint64_t& o_FrameId,
								 std::vector<double>& o_ScopeMs, bool& o_bValid)
{
	if (!m_Resolved.empty())
	{
		const Result& result = m_Resolved.front();
		o_FrameId = result.m_Id;
		o_ScopeMs = result.m_ScopeMs;
		o_bValid = result.m_bValid;
		m_Resolved.pop_front();
		return true;
	}
	if (m_InFlight.empty())
	{
		return false;
	}
	Result result;
	if (!read_back(*m_InFlight.front(), i_bWait, result))
	{
		return false;
	}
	m_Free.push_back(m_InFlight.front());
	m_InFlight.pop_front();
	o_FrameId = result.m_Id;
	o_ScopeMs = result.m_ScopeMs;
	o_bValid = result.m_bValid;
	return true;
}

#endif // MSP_GFX_PROFILING
