/*----------------------------------------------------------------------------
** inTPCInkCollector.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "InputTPC/in/private/inTPCInkCollector.hpp"

#include "Core/dbg/DbgMsg.hpp"

#include <msinkaut_i.c>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inTPCInkCollector::inTPCInkCollector()
{
	m_pInkCollector = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inTPCInkCollector::~inTPCInkCollector()
{
	if (m_pInkCollector != NULL)
    {
        m_InkEvents.UnAdviseInkCollector();
        m_pInkCollector->put_Enabled(VARIANT_FALSE);
        m_pInkCollector->Release();
    }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT inTPCInkCollector::Init(HWND i_Hwnd)
{
	// Initialize event sink. This consists of setting
    //  up the free threaded marshaler.
    HRESULT hr = m_InkEvents.Init(i_Hwnd);
    
    if (FAILED(hr))
    {
        return hr;
    }
    
    // Create the ink collector
    hr = CoCreateInstance(CLSID_InkCollector, NULL, CLSCTX_ALL,
        IID_IInkCollector, (void **) &m_pInkCollector);

    if (FAILED(hr))
    {
        return hr;
    }

	//turn off the visibility of the ink
	hr = SetTransparency(255);
	if (FAILED(hr))
		return hr;

	hr = SetInkRendering(VARIANT_FALSE);
	if (FAILED(hr))
		return hr;

	//Set each event interest that we want the ink event class
	//to catch
	hr = InitEventIterests();
	if (FAILED(hr))
		return hr;

	//Set the special tablet GUIDS that we can extract from the tablet
	hr = SetDesiredPackets();
	if (FAILED(hr))
		return hr;
    
	// Set up connection between Ink Collector and our event sink        
    hr = m_InkEvents.AdviseInkCollector(m_pInkCollector);
    if (FAILED(hr))
		return hr;

    // Attach Ink Collector to window
    hr = m_pInkCollector->put_hWnd((LONG_PTR) i_Hwnd);
    if (FAILED(hr))
		return hr;
    
    // Allow Ink Collector to receive input.
    return SetEnabled(VARIANT_TRUE);
}

//----------------------------------------------------------------------------
// Set up the various events that can be caught by the collector event class
//----------------------------------------------------------------------------
HRESULT inTPCInkCollector::InitEventIterests()
{
	HRESULT hr = S_OK;

	//(Stroke, CursorInRange, CursorOutOfRange are the only event interests that are on by 
    // default).  In order to handle an event that is not on by default, 
    // it is necessary to set the event interest at this point.  

	//Turn off defaults
	hr = m_pInkCollector->SetEventInterest(ICEI_Stroke, VARIANT_FALSE);
    if (FAILED(hr))
        return hr;
	
	/*hr = m_pInkCollector->SetEventInterest(ICEI_CursorInRange, VARIANT_FALSE);
    if (FAILED(hr))
        return hr;
	
	hr = m_pInkCollector->SetEventInterest(ICEI_CursorOutOfRange, VARIANT_FALSE);
    if (FAILED(hr))
        return hr;*/

	//*************************************
	//	To enable an interest of the following
	//	events, uncomment out its block of code
	//*************************************
	hr = m_pInkCollector->SetEventInterest(ICEI_CursorDown, VARIANT_TRUE);
    if (FAILED(hr))
        return hr;

	hr = m_pInkCollector->SetEventInterest(ICEI_NewPackets, VARIANT_TRUE);
    if (FAILED(hr))
        return hr;

	//hr = m_pInkCollector->SetEventInterest(ICEI_NewInAirPackets, VARIANT_TRUE);
    //if (FAILED(hr))
    //    return hr;

	//hr = m_pInkCollector->SetEventInterest(ICEI_CursorButtonDown, VARIANT_TRUE);
    //if (FAILED(hr))
    //    return hr;

	//hr = m_pInkCollector->SetEventInterest(ICEI_CursorButtonUp, VARIANT_TRUE);
    //if (FAILED(hr))
    //    return hr;

	//hr = m_pInkCollector->SetEventInterest(ICEI_SystemGesture, VARIANT_TRUE);
    //if (FAILED(hr))
    //    return hr;

	//hr = m_pInkCollector->SetEventInterest(ICEI_TabletAdded, VARIANT_TRUE);
    //if (FAILED(hr))
    //    return hr;

	//hr = m_pInkCollector->SetEventInterest(ICEI_TabletRemoved, VARIANT_TRUE);
    //if (FAILED(hr))
    //    return hr;

	hr = m_pInkCollector->SetEventInterest(ICEI_MouseDown, VARIANT_TRUE);
    if (FAILED(hr))
        return hr;

	hr = m_pInkCollector->SetEventInterest(ICEI_MouseMove, VARIANT_TRUE);
    if (FAILED(hr))
        return hr;

	hr = m_pInkCollector->SetEventInterest(ICEI_MouseUp, VARIANT_TRUE);
    if (FAILED(hr))
        return hr;

	hr = m_pInkCollector->SetEventInterest(ICEI_MouseWheel, VARIANT_TRUE);
    if (FAILED(hr))
        return hr;

	//hr = m_pInkCollector->SetEventInterest(ICEI_DblClick, VARIANT_TRUE);
    //if (FAILED(hr))
    //    return hr;

	return hr;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
HRESULT inTPCInkCollector::SetDesiredPackets()
{
	//Tablet X, Tablet Y, Normal Pressure, X Tilt, Y Tilt
	//Note: Tablet X-Y are the X,Y coordinates of the pen on the physical tablet,
	//		so this may be different than the X,Y coordinates returned from the 
	//		pen on the current window handle.
	VARIANT desc;
	desc.vt = VT_BSTR | VT_ARRAY;
	SAFEARRAYBOUND safeBound;
	BSTR* packets; 
	safeBound.cElements = e_NumDescriptions;
	safeBound.lLbound = 0;

	desc.parray = SafeArrayCreate(VT_BSTR, 1, &safeBound);
	SafeArrayAccessData(desc.parray,(void**)&packets);

	packets[e_TabletX] = STR_GUID_X;
	packets[e_TabletY] = STR_GUID_Y;
	packets[e_Pressure] = STR_GUID_NORMALPRESSURE;
	packets[e_XTilt] = STR_GUID_XTILTORIENTATION;
	packets[e_YTilt] = STR_GUID_YTILTORIENTATION;

	SafeArrayUnaccessData(desc.parray);

	return m_pInkCollector->put_DesiredPacketDescription(desc);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
HRESULT inTPCInkCollector::GetDesiredPackets(PacketDescriptions i_PacketDesc, int& o_PacketValue)
{
	HRESULT hr = S_OK;
	switch(i_PacketDesc)
	{
	case e_Pressure:
		o_PacketValue = m_Data.m_Pressure;
		break;
	case e_XTilt:
		o_PacketValue = m_Data.m_XTilt;
		break;
	case e_YTilt:
		o_PacketValue = m_Data.m_YTilt;
		break;
	}
	return hr;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
HRESULT inTPCInkCollector::SetColor(int i_R, int i_G, int i_B)
{
	IInkDrawingAttributes* attr = NULL;
	HRESULT hr = m_pInkCollector->get_DefaultDrawingAttributes(&attr);

	if (SUCCEEDED(hr))
	{
		COLORREF color = RGB(i_R, i_G, i_B);
		attr->put_Color(color);
		hr = m_pInkCollector->putref_DefaultDrawingAttributes(attr);
	}
	return hr;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
HRESULT inTPCInkCollector::SetWidth(int i_Width)
{
	IInkDrawingAttributes* attr = NULL;
	HRESULT hr = m_pInkCollector->get_DefaultDrawingAttributes(&attr);

	if (SUCCEEDED(hr))
	{
		attr->put_Width((float)i_Width);
		hr = m_pInkCollector->putref_DefaultDrawingAttributes(attr);
	}
	return hr;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
HRESULT inTPCInkCollector::SetTransparency(LONG i_TransparencyValue)
{
	IInkDrawingAttributes* attr = NULL;
	HRESULT hr = m_pInkCollector->get_DefaultDrawingAttributes(&attr);

	if (SUCCEEDED(hr))
	{
		//0 to 255 (opaque to totally transparent)
		attr->put_Transparency(i_TransparencyValue);
		hr = m_pInkCollector->putref_DefaultDrawingAttributes(attr);
	}
	return hr;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
HRESULT inTPCInkCollector::SetCollectionMode(int i_Mode)
{
	HRESULT hr = S_OK;
	
	VARIANT_BOOL bCollectingInk;
	m_pInkCollector->get_CollectingInk(&bCollectingInk);
	if (bCollectingInk == VARIANT_FALSE)
	{
		hr = m_pInkCollector->put_CollectionMode(InkCollectionMode(i_Mode));
	}

	return hr;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
HRESULT inTPCInkCollector::SetInkRendering(VARIANT_BOOL i_bRenderInk)
{
	HRESULT hr = S_OK;

	VARIANT_BOOL bCollectingInk;
	m_pInkCollector->get_CollectingInk(&bCollectingInk);
	if (bCollectingInk == VARIANT_FALSE)
	{
		hr = m_pInkCollector->put_DynamicRendering(i_bRenderInk);
	}

	return hr;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
HRESULT inTPCInkCollector::SetEnabled(VARIANT_BOOL i_bEnabled)
{
	//reset the properties of the ink collector if we are turning it off
	if(i_bEnabled == VARIANT_FALSE)
		ResetState();
	
	return m_pInkCollector->put_Enabled(i_bEnabled);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool inTPCInkCollector::GetEnabled()
{
	VARIANT_BOOL bEnabled;
	HRESULT hr = m_pInkCollector->get_Enabled(&bEnabled);
	if(FAILED(hr))
		return false;

	if(bEnabled == VARIANT_TRUE)
		return true;
	
	return false;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void inTPCInkCollector::UpdateState()
{
	m_Data = m_InkEvents.m_EData;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void inTPCInkCollector::ResetState()
{
	m_InkEvents.m_EData.m_X = 0;
	m_InkEvents.m_EData.m_Y = 0;
	m_InkEvents.m_EData.m_Z = 0;
	m_InkEvents.m_EData.m_bCursorDown = false;
	m_InkEvents.m_EData.m_bErase = false;
	m_InkEvents.m_EData.m_Pressure = 0;
	m_InkEvents.m_EData.m_XTilt = 0;	
	m_InkEvents.m_EData.m_YTilt = 0;

	m_Data = m_InkEvents.m_EData;
}