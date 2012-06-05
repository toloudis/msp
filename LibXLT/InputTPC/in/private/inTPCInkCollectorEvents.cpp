/*----------------------------------------------------------------------------
** inTPCInkCollectorEvents.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "InputTPC/in/private/inTPCInkCollectorEvents.hpp"

#include "Core/dbg/DbgMsg.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inTPCInkCollectorEventsBase::inTPCInkCollectorEventsBase()
{
	m_pIConnectionPoint = NULL;
    m_punkFTM = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inTPCInkCollectorEventsBase::~inTPCInkCollectorEventsBase()
{
	UnAdviseInkCollector();
    if (m_punkFTM != NULL)
    {
        m_punkFTM->Release();
    }
}

//----------------------------------------------------------------------------
// Set up free threaded marshaller.  Needs to be called before the class can
//	handle events.
//----------------------------------------------------------------------------
HRESULT inTPCInkCollectorEventsBase::Init()
{
	return CoCreateFreeThreadedMarshaler(this, &m_punkFTM);
}

//----------------------------------------------------------------------------
// Set up connection between sink and ink collector
//----------------------------------------------------------------------------
HRESULT inTPCInkCollectorEventsBase::AdviseInkCollector(IInkCollector* i_pIInkCollector)
{
	HRESULT hr = S_OK;

    // Check to ensure that the sink is not currently connected
    // with another Ink Collector...
    if (NULL == m_pIConnectionPoint)
    {
        // Get the connection point container
        IConnectionPointContainer *pIConnectionPointContainer;
        hr = i_pIInkCollector->QueryInterface(
            IID_IConnectionPointContainer, 
            (void **) &pIConnectionPointContainer);
        
        if (FAILED(hr))
        {
            return hr;
        }
        
        // Find the connection point for Ink Collector events
        hr = pIConnectionPointContainer->FindConnectionPoint(
            __uuidof(_IInkCollectorEvents), &m_pIConnectionPoint);
        
        if (SUCCEEDED(hr))
        {
            // Hook up sink to connection point
            hr = m_pIConnectionPoint->Advise(this, &m_dwCookie);
        }
        
        if (FAILED(hr))
        {
            // Clean up after an error.
            if (m_pIConnectionPoint)
            {
                m_pIConnectionPoint->Release();
                m_pIConnectionPoint = NULL;
            }
        }
        
        // We don't need the connection point container any more.
        pIConnectionPointContainer->Release();
    }
    // If the sink is already connected to an Ink Collector, return a 
    // failure; only one Ink Collector can be attached at any given time.
    else
    {
        hr = E_FAIL;
    }

    return hr;
}

//----------------------------------------------------------------------------
// Remove the connection between the sink and the ink collector
//----------------------------------------------------------------------------
HRESULT inTPCInkCollectorEventsBase::UnAdviseInkCollector()
{
	HRESULT hr = S_OK;
	
    // If there the ink collector is connected to the sink,
    // remove it.  Otherwise, do nothing (there is nothing
    // to unadvise).
    if (m_pIConnectionPoint != NULL)
    {
        hr = m_pIConnectionPoint->Unadvise(m_dwCookie);
        m_pIConnectionPoint->Release();
        m_pIConnectionPoint = NULL;
    }

    return hr;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
inTPCInkCollectorEvents::inTPCInkCollectorEvents()
{
	m_Hwnd = NULL;
	m_EData.m_X = 0;
	m_EData.m_Y = 0;
	m_EData.m_Z = 0;
	m_EData.m_bCursorDown = false;
	//m_EData.m_pPacketData.parray = NULL;
	m_EData.m_Pressure = 0;
	m_EData.m_XTilt = 0;	
	m_EData.m_YTilt = 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HRESULT inTPCInkCollectorEvents::Init(HWND i_Hwnd)
{
	m_Hwnd = i_Hwnd;
	return inTPCInkCollectorEventsBase::Init();
}

//----------------------------------------------------------------------------
// Events - Define what to do on the occurrence of the captured event
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Stroke - Occurs when the user finishes drawing a new stroke on any tablet.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::Stroke(
    IInkCursor* i_pCursor,
    IInkStrokeDisp* i_pStroke,
    VARIANT_BOOL *i_pCancel)
{
	//define what to do at the end of a stroke event

}

//----------------------------------------------------------------------------
// CursorDown - Occurs when the cursor tip contacts the digitizing tablet surface.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::CursorDown(
    IInkCursor* i_pCursor,
    IInkStrokeDisp* i_pStroke)
{
	m_EData.m_bCursorDown = true;
	
	VARIANT_BOOL inverted;
	HRESULT hr = i_pCursor->get_Inverted(&inverted);

	//Check for eraser
	if(SUCCEEDED(hr) && inverted == VARIANT_TRUE)
		m_EData.m_bErase = true;
	else
		m_EData.m_bErase = false;
}

//----------------------------------------------------------------------------
// NewPackets - Occurs when the InkCollector object receives packets.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::NewPackets(
    IInkCursor* Cursor,
    IInkStrokeDisp* i_pStroke,
    long i_PacketCount,
    VARIANT* i_pPacketData)
{
	//extract the pressure and tilt values from the packet data's safearray
	if(i_pPacketData->parray->cDims == 1)
	{
		long* iptr;

		SafeArrayAccessData(i_pPacketData->parray, (void**)&iptr);
		
		m_EData.m_Pressure = (int)*(iptr+(e_Pressure));
		m_EData.m_XTilt = (int)*(iptr+(e_XTilt));
		m_EData.m_YTilt = (int)*(iptr+(e_YTilt));
		
		SafeArrayUnaccessData(i_pPacketData->parray);
	}
}

//----------------------------------------------------------------------------
// DblClick - Occurs when the InkCollector object is double-clicked.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::DblClick(
    VARIANT_BOOL *i_pCancel)
{
	
}

//----------------------------------------------------------------------------
// MouseMove - 	Occurs when the mouse pointer is moved over the InkCollector object.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::MouseMove(
    InkMouseButton i_Button, 
    InkShiftKeyModifierFlags i_Shift, 
    long i_X, 
    long i_Y,
    VARIANT_BOOL *i_pCancel)
{
	//Event when mouse movement occurs over the app window
	m_EData.m_X = i_X;
	m_EData.m_Y = i_Y;
}
    
//----------------------------------------------------------------------------
// MouseDown - Occurs when the mouse pointer is over the InkCollector object and a mouse button is pressed.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::MouseDown(
    InkMouseButton i_Button, 
    InkShiftKeyModifierFlags i_Shift, 
    long i_X, 
    long i_Y,
    VARIANT_BOOL *i_pCancel)
{
	m_EData.m_X = i_X;
	m_EData.m_Y = i_Y;
	m_EData.m_bCursorDown = true;
}
    
//----------------------------------------------------------------------------
// MouseUp - Occurs when the mouse pointer is over the InkCollector object and a mouse button is released.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::MouseUp(
    InkMouseButton i_Button, 
    InkShiftKeyModifierFlags i_Shift, 
    long i_X, 
    long i_Y,
    VARIANT_BOOL *i_pCancel)
{
	m_EData.m_X = 0;
	m_EData.m_Y = 0;
	m_EData.m_Z = 0;
	m_EData.m_bCursorDown = false;
	m_EData.m_bErase = false;
	//m_EData.m_pPacketData.parray = NULL;
	m_EData.m_Pressure = 0;
	m_EData.m_XTilt = 0;	
	m_EData.m_YTilt = 0;
}
    
//----------------------------------------------------------------------------
// MouseWheel - Occurs when the mouse wheel moves while the InkCollector object has focus.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::MouseWheel(
    InkMouseButton i_Button, 
    InkShiftKeyModifierFlags i_Shift, 
    long i_Delta, 
    long i_X, 
    long i_Y,
    VARIANT_BOOL *i_pCancel)
{
	if	(m_EData.m_X == 0 && m_EData.m_Y == 0)
	{
		m_EData.m_X = i_X;
		m_EData.m_Y = i_Y;
	}

	m_EData.m_Z = (float)i_Delta;
}

//----------------------------------------------------------------------------
// NewInAirPackets - Occurs when an in-air packet is seen, which happens when a
//	user moves a pen near the tablet and the cursor is within the InkCollector object's 
//	window or the user moves a mouse within the InkCollector object object's associated window.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::NewInAirPackets(
    IInkCursor* i_Cursor,
    long i_PacketCount,
    VARIANT* i_pPacketData)
{

}

//----------------------------------------------------------------------------
// CursorButtonDown - Occurs when the InkCollector detects a cursor button that is down.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::CursorButtonDown(
    IInkCursor* i_pCursor,
    IInkCursorButton* i_pButton)
{

}

//----------------------------------------------------------------------------
// CursorButtonUp - Occurs when the InkCollector detects a cursor button that is up.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::CursorButtonUp(
    IInkCursor* i_pCursor,
    IInkCursorButton* i_pButton)
{
	m_EData.m_X = 0;
	m_EData.m_Y = 0;
	m_EData.m_Z = 0;
	m_EData.m_bCursorDown = false;
	m_EData.m_bErase = false;
	//m_EData.m_pPacketData.parray = NULL;
	m_EData.m_Pressure = 0;
	m_EData.m_XTilt = 0;	
	m_EData.m_YTilt = 0;
}

//----------------------------------------------------------------------------
// CursorInRange - Occurs when a cursor enters the physical detection range (proximity) of the tablet context.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::CursorInRange(
    IInkCursor* i_pCursor,
    VARIANT_BOOL i_NewCursor,
    VARIANT i_ButtonsState)
{
	VARIANT_BOOL inverted;
	HRESULT hr = i_pCursor->get_Inverted(&inverted);

	if(SUCCEEDED(hr) && inverted == VARIANT_TRUE)
	{
		//This is the eraser side
	}

}

//----------------------------------------------------------------------------
// CursorOutOfRange - Occurs when the cursor leaves the physical detection range (proximity) of the tablet context.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::CursorOutOfRange(
    IInkCursor* i_pCursor)
{
	m_EData.m_X = 0;
	m_EData.m_Y = 0;
	m_EData.m_Z = 0;
	m_EData.m_bCursorDown = false;
	m_EData.m_bErase = false;
	//m_EData.m_pPacketData.parray = NULL;
	m_EData.m_Pressure = 0;
	m_EData.m_XTilt = 0;	
	m_EData.m_YTilt = 0;
}

//----------------------------------------------------------------------------
// SystemGesture - 	Occurs when a system gesture is recognized.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::SystemGesture(
    IInkCursor* i_pCursor,
    InkSystemGesture i_ID,
    long i_X, 
    long i_Y,
    long i_Modifier,
    BSTR i_Character,
    long i_CursorMode)
{

}

//----------------------------------------------------------------------------
// Gesture - Occurs when an application-specific gesture is recognized.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::Gesture(
    IInkCursor* i_pCursor,
    IInkStrokes* i_pStrokes,
    VARIANT i_Gestures,
    VARIANT_BOOL* i_pCancel)
{

}

//----------------------------------------------------------------------------
// TabletAdded - Occurs when a  Tablet is added to the system.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::TabletAdded(
    IInkTablet* i_pTablet)
{

}

//----------------------------------------------------------------------------
// TabletRemoved - Occurs when a  Tablet is removed from the system.
//----------------------------------------------------------------------------
void inTPCInkCollectorEvents::TabletRemoved(
    long i_TabletId)
{

}