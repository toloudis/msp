/*----------------------------------------------------------------------------
** inTPCInkCollectorEvents.hpp
**
**		Class that will process the various events that can be captured by the 
**		Tablet Platform.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_TPCINKCOLLECTOREVENTS_HPP
#error inTPCInkCollectorEvents.hpp multiply included
#endif
#define IN_TPCINKCOLLECTOREVENTS_HPP

#ifndef IN_TPCHEADER_HPP
#include "InputTPC/in/private/inTPCHeader.hpp"
#endif

//----------------------------------------------------------------------------
//  inTPCInkCollectorEventsBase class
//----------------------------------------------------------------------------
class inTPCInkCollectorEventsBase : public _IInkCollectorEvents
{
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	inTPCInkCollectorEventsBase();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	~inTPCInkCollectorEventsBase();

	//----------------------------------------------------------------------------
	// Set up free threaded marshaller.  Needs to be called before the class can
	//	handle events.
	//----------------------------------------------------------------------------
	HRESULT Init();

	//----------------------------------------------------------------------------
	// Set up connection between sink and ink collector
	//----------------------------------------------------------------------------
	HRESULT AdviseInkCollector(IInkCollector* i_pIInkCollector);

	//----------------------------------------------------------------------------
	// Set up connection between sink and ink overlay
	//----------------------------------------------------------------------------
	HRESULT AdviseInkOverlay(IInkOverlay* i_pIInkOverlay);

	//----------------------------------------------------------------------------
	// Remove the connection between the sink and the ink collector
	//----------------------------------------------------------------------------
	HRESULT UnAdviseInkCollector();

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	HRESULT __stdcall QueryInterface(REFIID i_riid, void **ppvObject)
	{
		 // Validate the input
		if (NULL == ppvObject)
		{
			return E_POINTER;
		}

		// This object only supports IDispatch/_IInkCollectorEvents
		if ((i_riid == IID_IUnknown)
			|| (i_riid == IID_IDispatch)
			|| (i_riid == DIID__IInkCollectorEvents))
		{
			*ppvObject = (IDispatch *) this;
	        
			// Note: we do not AddRef here because the lifetime
			//  of this object does not depend on reference counting
			//  but on the duration of the connection set up by
			//  the user of this class.
	        
			return S_OK;
		}
		else if (i_riid == IID_IMarshal)
		{
			// Assert that the free threaded marshaller has been
			// initialized.  It is necessary to call Init() before
			// invoking this method.
			//assert(NULL != m_punkFTM);

			// Use free threaded marshalling.
			return m_punkFTM->QueryInterface(i_riid, ppvObject);
		}
	    
		return E_NOINTERFACE;
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	ULONG STDMETHODCALLTYPE AddRef()
	{
		// Note: we do not AddRef here because the lifetime
		//  of this object does not depend on reference counting
		//  but on the duration of the connection set up by
		//  the user of this class.
		return 1;
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	ULONG STDMETHODCALLTYPE Release()
	{
		// Note: we do not do Release here because the lifetime
		//  of this object does not depend on reference counting
		//  but on the duration of the connection set up by
		//  the user of this class.
		return 1;
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	STDMETHOD(GetTypeInfoCount)(UINT* pctinfo)
	{
		// This method is not needed for processing events.
		return E_NOTIMPL;
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	STDMETHOD(GetTypeInfo)(
		UINT itinfo, 
		LCID lcid, 
		ITypeInfo** pptinfo)
	{
		// This method is not needed for processing events.
		return E_NOTIMPL;
	}

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	STDMETHOD(GetIDsOfNames)(
		REFIID riid, 
		LPOLESTR* rgszNames, 
		UINT cNames,
		LCID lcid, 
		DISPID* rgdispid)
	{
		// This method is not needed for processing events.
		return E_NOTIMPL;
	}

	//----------------------------------------------------------------------------
	// Invoke translates from IDispatch to an event callout
	//  that can be overriden by a subclass of this class.
	//----------------------------------------------------------------------------
	STDMETHOD(Invoke)(
		DISPID dispidMember, 
		REFIID riid,
		LCID lcid, 
		WORD /*wFlags*/, 
		DISPPARAMS* pdispparams, 
		VARIANT* pvarResult,
		EXCEPINFO* /*pexcepinfo*/, 
		UINT* /*puArgErr*/)
	{
		switch(dispidMember)
		{
			case DISPID_ICEStroke:
				Stroke(
					(IInkCursor*) pdispparams->rgvarg[2].pdispVal,
					(IInkStrokeDisp*) pdispparams->rgvarg[1].pdispVal,
					(VARIANT_BOOL *)pdispparams->rgvarg[0].pboolVal);
				break;

			case DISPID_ICECursorDown:
				CursorDown(
					(IInkCursor*) pdispparams->rgvarg[1].pdispVal,
					(IInkStrokeDisp*) pdispparams->rgvarg[0].pdispVal);             
				break;

			case DISPID_IPEDblClick:
				DblClick(
					(VARIANT_BOOL *)pdispparams->rgvarg[0].pboolVal);
				break;

			case DISPID_IPEMouseMove:
				MouseMove(
					(InkMouseButton) pdispparams->rgvarg[4].lVal, 
					(InkShiftKeyModifierFlags) pdispparams->rgvarg[3].lVal, 
					pdispparams->rgvarg[2].lVal,
					pdispparams->rgvarg[1].lVal,
					(VARIANT_BOOL *)pdispparams->rgvarg[0].pboolVal); 
				break;
	        
			case DISPID_IPEMouseDown:
				MouseDown(
					(InkMouseButton) pdispparams->rgvarg[4].lVal, 
					(InkShiftKeyModifierFlags) pdispparams->rgvarg[3].lVal, 
					pdispparams->rgvarg[2].lVal,
					pdispparams->rgvarg[1].lVal,
					(VARIANT_BOOL *)pdispparams->rgvarg[0].pboolVal); 
				break;
	        
			case DISPID_IPEMouseUp:
				MouseUp(
					(InkMouseButton) pdispparams->rgvarg[4].lVal, 
					(InkShiftKeyModifierFlags) pdispparams->rgvarg[3].lVal, 
					pdispparams->rgvarg[2].lVal,
					pdispparams->rgvarg[1].lVal,
					(VARIANT_BOOL *)pdispparams->rgvarg[0].pboolVal); 
				break;
	        
			case DISPID_IPEMouseWheel:
				MouseWheel(
					(InkMouseButton) pdispparams->rgvarg[5].lVal, 
					(InkShiftKeyModifierFlags) pdispparams->rgvarg[4].lVal, 
					pdispparams->rgvarg[3].lVal,
					pdispparams->rgvarg[2].lVal,
					pdispparams->rgvarg[1].lVal,
					(VARIANT_BOOL *)pdispparams->rgvarg[0].pboolVal); 
				break;

			case DISPID_ICENewPackets:
				NewPackets(
					(IInkCursor*) pdispparams->rgvarg[3].pdispVal,
					(IInkStrokeDisp*) pdispparams->rgvarg[2].pdispVal,
					pdispparams->rgvarg[1].lVal,
					pdispparams->rgvarg[0].pvarVal);
				break;

			case DISPID_ICENewInAirPackets:
				NewInAirPackets(
					(IInkCursor*) pdispparams->rgvarg[2].pdispVal,
					pdispparams->rgvarg[1].lVal,
					pdispparams->rgvarg[0].pvarVal);
				break;

			case DISPID_ICECursorButtonDown:
				CursorButtonDown(
					(IInkCursor*) pdispparams->rgvarg[1].pdispVal,
					(IInkCursorButton*) pdispparams->rgvarg[0].pdispVal);
				break;
	        
			case DISPID_ICECursorButtonUp:
				CursorButtonUp(
					(IInkCursor*) pdispparams->rgvarg[1].pdispVal,
					(IInkCursorButton*) pdispparams->rgvarg[0].pdispVal);
				break;

			case DISPID_ICECursorInRange:
				CursorInRange(
					(IInkCursor*) pdispparams->rgvarg[2].pdispVal,
					(VARIANT_BOOL) pdispparams->rgvarg[1].iVal,
					pdispparams->rgvarg[0]);
				break;

			case DISPID_ICECursorOutOfRange:
				CursorOutOfRange(
					(IInkCursor*) pdispparams->rgvarg[0].pdispVal);
				break;

			case DISPID_ICESystemGesture:
				SystemGesture(
					(IInkCursor*) pdispparams->rgvarg[6].pdispVal,
					(InkSystemGesture) pdispparams->rgvarg[5].lVal, 
					pdispparams->rgvarg[4].lVal, 
					pdispparams->rgvarg[3].lVal,
					pdispparams->rgvarg[2].lVal,
					pdispparams->rgvarg[1].bstrVal,
					pdispparams->rgvarg[0].lVal);
				break;

			case DISPID_ICEGesture:
				Gesture(
					(IInkCursor*) pdispparams->rgvarg[3].pdispVal,
					(IInkStrokes*) pdispparams->rgvarg[2].pdispVal,
					pdispparams->rgvarg[1],
					(VARIANT_BOOL *)pdispparams->rgvarg[0].pboolVal);
				break;

			case DISPID_ICETabletAdded:
				TabletAdded(
					(IInkTablet*) pdispparams->rgvarg[0].pdispVal);
				break;

			case DISPID_ICETabletRemoved:
				TabletRemoved(
					pdispparams->rgvarg[0].lVal);
				break;
	        
			default:
				break;
		}
	    
		return S_OK;
	}

	//----------------------------------------------------------------------------
    // Events
	//----------------------------------------------------------------------------
    virtual void Stroke(
        IInkCursor* i_pCursor,
        IInkStrokeDisp* i_pStroke,
        VARIANT_BOOL *i_pCancel) = 0;

    virtual void CursorDown(
        IInkCursor* i_pCursor,
        IInkStrokeDisp* i_pStroke) = 0;

    virtual void NewPackets(
        IInkCursor* Cursor,
        IInkStrokeDisp* i_pStroke,
        long i_PacketCount,
        VARIANT* i_pPacketData) = 0;

    virtual void DblClick(
        VARIANT_BOOL *i_pCancel) = 0;
    
    virtual void MouseMove(
        InkMouseButton i_Button, 
        InkShiftKeyModifierFlags i_Shift, 
        long i_X, 
        long i_Y,
        VARIANT_BOOL *i_pCancel) = 0;
        
    virtual void MouseDown(
        InkMouseButton i_Button, 
        InkShiftKeyModifierFlags i_Shift, 
        long i_X, 
        long i_Y,
        VARIANT_BOOL *i_pCancel) = 0;
                
    virtual void MouseUp(
        InkMouseButton i_Button, 
        InkShiftKeyModifierFlags i_Shift, 
        long i_X, 
        long i_Y,
        VARIANT_BOOL *i_pCancel) = 0;
        
    virtual void MouseWheel(
        InkMouseButton i_Button, 
        InkShiftKeyModifierFlags i_Shift, 
        long i_Delta, 
        long i_X, 
        long i_Y,
        VARIANT_BOOL *i_pCancel) = 0;

    virtual void NewInAirPackets(
        IInkCursor* i_Cursor,
        long i_PacketCount,
        VARIANT* i_pPacketData) = 0;

    virtual void CursorButtonDown(
        IInkCursor* i_pCursor,
        IInkCursorButton* i_pButton) = 0;

    virtual void CursorButtonUp(
        IInkCursor* i_pCursor,
        IInkCursorButton* i_pButton) = 0;

    virtual void CursorInRange(
        IInkCursor* i_pCursor,
        VARIANT_BOOL i_NewCursor,
        VARIANT i_ButtonsState) = 0;

    virtual void CursorOutOfRange(
        IInkCursor* i_pCursor) = 0;

    virtual void SystemGesture(
        IInkCursor* i_pCursor,
        InkSystemGesture i_ID,
        long i_X, 
        long i_Y,
        long i_Modifier,
        BSTR i_Character,
        long i_CursorMode) = 0;

    virtual void Gesture(
        IInkCursor* i_pCursor,
        IInkStrokes* i_pStrokes,
        VARIANT i_Gestures,
        VARIANT_BOOL* i_pCancel) = 0;

    virtual void TabletAdded(
        IInkTablet* i_pTablet) = 0;

    virtual void TabletRemoved(
        long i_TabletId) = 0;

private:
	// Connection point on InkCollector
    IConnectionPoint *m_pIConnectionPoint;
    
    // Cookie returned from advise
    DWORD m_dwCookie;
    
    // Free threaded marshaler.
    IUnknown* m_punkFTM;

};  // end class inTPCInkCollectorEventsBase

//------------------------------------------------------------------------
//------------------------------------------------------------------------
struct EventData
{
	int m_X;
	int m_Y;
	float m_Z;
	bool m_bCursorDown;
	bool m_bErase;
	VARIANT m_pPacketData;
	int m_Pressure;
	int m_XTilt;
	int m_YTilt;
};

enum Packets
{
	e_TabX,
	e_TabY,
	e_Pressure,
	e_XTilt,
	e_YTilt,
	e_NumDesc
};

//----------------------------------------------------------------------------
// inTPCInkCollectorEvents class
//----------------------------------------------------------------------------
class inTPCInkCollectorEvents : public inTPCInkCollectorEventsBase
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	inTPCInkCollectorEvents();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	HRESULT Init(HWND i_Hwnd);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	EventData m_EData;

	//----------------------------------------------------------------------------
    // Events
	//----------------------------------------------------------------------------
    virtual void Stroke(
        IInkCursor* i_pCursor,
        IInkStrokeDisp* i_pStroke,
        VARIANT_BOOL *i_pCancel);

    virtual void CursorDown(
        IInkCursor* i_pCursor,
        IInkStrokeDisp* i_pStroke);

    virtual void NewPackets(
        IInkCursor* Cursor,
        IInkStrokeDisp* i_pStroke,
        long i_PacketCount,
        VARIANT* i_pPacketData);

    virtual void DblClick(
        VARIANT_BOOL *i_pCancel);
    
    virtual void MouseMove(
        InkMouseButton i_Button, 
        InkShiftKeyModifierFlags i_Shift, 
        long i_X, 
        long i_Y,
        VARIANT_BOOL *i_pCancel);
        
    virtual void MouseDown(
        InkMouseButton i_Button, 
        InkShiftKeyModifierFlags i_Shift, 
        long i_X, 
        long i_Y,
        VARIANT_BOOL *i_pCancel);
                
    virtual void MouseUp(
        InkMouseButton i_Button, 
        InkShiftKeyModifierFlags i_Shift, 
        long i_X, 
        long i_Y,
        VARIANT_BOOL *i_pCancel);
        
    virtual void MouseWheel(
        InkMouseButton i_Button, 
        InkShiftKeyModifierFlags i_Shift, 
        long i_Delta, 
        long i_X, 
        long i_Y,
        VARIANT_BOOL *i_pCancel);

    virtual void NewInAirPackets(
        IInkCursor* i_Cursor,
        long i_PacketCount,
        VARIANT* i_pPacketData);

    virtual void CursorButtonDown(
        IInkCursor* i_pCursor,
        IInkCursorButton* i_pButton);

    virtual void CursorButtonUp(
        IInkCursor* i_pCursor,
        IInkCursorButton* i_pButton);

    virtual void CursorInRange(
        IInkCursor* i_pCursor,
        VARIANT_BOOL i_NewCursor,
        VARIANT i_ButtonsState);

    virtual void CursorOutOfRange(
        IInkCursor* i_pCursor);

    virtual void SystemGesture(
        IInkCursor* i_pCursor,
        InkSystemGesture i_ID,
        long i_X, 
        long i_Y,
        long i_Modifier,
        BSTR i_Character,
        long i_CursorMode);

    virtual void Gesture(
        IInkCursor* i_pCursor,
        IInkStrokes* i_pStrokes,
        VARIANT i_Gestures,
        VARIANT_BOOL* i_pCancel);

    virtual void TabletAdded(
        IInkTablet* i_pTablet);

    virtual void TabletRemoved(
        long i_TabletId);

private:
	HWND m_Hwnd;

};  // end class inTPCInkCollectorEvents