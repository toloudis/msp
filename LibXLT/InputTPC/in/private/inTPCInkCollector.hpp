/*----------------------------------------------------------------------------
** inTPCInkCollector.hpp
**
**		Class that will keep track of the current strokes on the tablet.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_TPCINKCOLLECTOR_HPP
#error inTPCInkCollector.hpp multiply included
#endif
#define IN_TPCINKCOLLECTOR_HPP

#ifndef IN_TPCHEADER_HPP
#include "InputTPC/in/private/inTPCHeader.hpp"
#endif

#ifndef IN_TPCINKCOLLECTOREVENTS_HPP
#include "InputTPC/in/private/inTPCInkCollectorEvents.hpp"
#endif

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class inTPCInkCollector
{
public:
	//enumerator for the packet info we want to grab from the tablet
	enum PacketDescriptions
	{
		e_TabletX,
		e_TabletY,
		e_Pressure,
		e_XTilt,
		e_YTilt,
		e_NumDescriptions
	};

	//------------------------------------------------------------------------
	// Constructor
	//------------------------------------------------------------------------
	inTPCInkCollector();

	//------------------------------------------------------------------------
	// Destructor
	//------------------------------------------------------------------------
	~inTPCInkCollector();

	//------------------------------------------------------------------------
	// Initialize the window handle for the ink collector and its event sink
	//------------------------------------------------------------------------
	HRESULT Init(HWND i_Hwnd);

	//------------------------------------------------------------------------
	// Determine which events should be caught by the event sink
	//------------------------------------------------------------------------
	HRESULT InitEventIterests();

	//------------------------------------------------------------------------
	// Set the additional properties we would like to get data about
	//------------------------------------------------------------------------
	HRESULT SetDesiredPackets();

	//------------------------------------------------------------------------
	// Get the value of one of the additional packet properties
	//------------------------------------------------------------------------
	HRESULT GetDesiredPackets(PacketDescriptions i_PacketDesc, int& o_PacketValue);

	//------------------------------------------------------------------------
	// Change the color of the InkCollector's ink strokes
	//------------------------------------------------------------------------
	HRESULT SetColor(int i_R, int i_G, int i_B);

	//------------------------------------------------------------------------
	// Set the width of the ink collector strokes, also makes ink cursor bigger
	//------------------------------------------------------------------------
	HRESULT SetWidth(int i_Width);

	//------------------------------------------------------------------------
	// Sets the opacity of the strokes made by the ink collector
	//------------------------------------------------------------------------
	HRESULT SetTransparency(LONG i_TransparencyValue);

	//------------------------------------------------------------------------
	// Determine if the ink collector should register strokes(0), gestures(1), or both(2)
	//------------------------------------------------------------------------
	HRESULT SetCollectionMode(int i_Mode);

	//------------------------------------------------------------------------
	// Turn on/off the actual rendering of ink 
	//------------------------------------------------------------------------
	HRESULT SetInkRendering(VARIANT_BOOL i_bRenderInk);

	//------------------------------------------------------------------------
	// Turn on/off the ink collector and its event sink
	//------------------------------------------------------------------------
	HRESULT SetEnabled(VARIANT_BOOL i_bEnabled);

	//------------------------------------------------------------------------
	// Get whether or not the ink collector is active
	//------------------------------------------------------------------------
	bool GetEnabled();

	//------------------------------------------------------------------------
	// Match the property field to the ink collector's event sink
	//------------------------------------------------------------------------
	void UpdateState();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ResetState();

	//------------------------------------------------------------------------
	// Store the new state of the tablet gathered from the event sink
	//------------------------------------------------------------------------
	EventData m_Data;

private:
	IInkCollector* m_pInkCollector;
	inTPCInkCollectorEvents m_InkEvents;

};  // end class inTPCInkCollector