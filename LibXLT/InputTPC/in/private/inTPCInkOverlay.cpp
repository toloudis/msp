/*----------------------------------------------------------------------------
** inTPCInkOverlay.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "InputTPC/in/private/inTPCInkOverlay.hpp"

#include "Core/dbg/DbgMsg.hpp"

//#include <msinkaut_i.c>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inTPCInkOverlay::inTPCInkOverlay()
{
	m_pInkOverlay = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inTPCInkOverlay::~inTPCInkOverlay()
{
	if (m_pInkOverlay != NULL)
    {
        m_pInkOverlay->put_Enabled(VARIANT_FALSE);
        m_pInkOverlay->Release();
    }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT inTPCInkOverlay::Init(HWND i_Hwnd)
{
	HRESULT hr = S_OK;

    // Create the ink overlay
    hr = CoCreateInstance(CLSID_InkOverlay, NULL, CLSCTX_ALL,
        IID_IInkOverlay, (void **) &m_pInkOverlay);
    if (FAILED(hr))
        return hr;

    // Attach Ink Overlay to window
    hr = m_pInkOverlay->put_hWnd((LONG_PTR) i_Hwnd);
    if (FAILED(hr))
		return hr;
    
	IInkDrawingAttributes* att = NULL;
	hr = m_pInkOverlay->get_DefaultDrawingAttributes(&att);

	if (SUCCEEDED(hr))
	{
		att->put_Width((float)100);
		COLORREF color = RGB(255,0,0);
		att->put_Color(color);
		m_pInkOverlay->putref_DefaultDrawingAttributes(att);
	}

    // Allow Ink Overlay to receive input.
    return m_pInkOverlay->put_Enabled(VARIANT_TRUE);
}
