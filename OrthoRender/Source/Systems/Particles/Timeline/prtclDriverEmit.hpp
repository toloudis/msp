/*****************************************************************************
**	prtclDriverEmit.hpp
**
**	Derived driver class for turning on particle emissions.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef PRTCL_DRIVEREMIT_HPP
#error prtclDriverEmit.hpp multiply included
#endif
#define PRTCL_DRIVEREMIT_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


//============================================================================
//============================================================================
class prtclChannelEmit;
class prtclDriverEmitInfo;
class tmlnDriverInfo;


//============================================================================
//============================================================================
class prtclDriverEmit : public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	prtclDriverEmit(prtclChannelEmit &i_Channel);

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse hovers over the clip.
	//--------------------------------------------------------------------
	std::string GetHoverDescription();

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(float i_Time);

	//--------------------------------------------------------------------
	//  GetDriverInfo - return data structure representing state of
	//		this driver suitable for writing to a file.
	//	The returned value should be created with "new" and will
	//		be deleted by the caller.
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo*  GetDriverInfo() const;

	//--------------------------------------------------------------------
	// Set internal variables from data structure
	//--------------------------------------------------------------------
	void SetDriverInfo(	const prtclDriverEmitInfo& i_Info, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	// Update - update the object
	//--------------------------------------------------------------------
	void Update();

	//--------------------------------------------------------------------
	//	NotActive() - called ONCE when a driver goes from active to not
	//	active.  This function needs to reset m_bNotifyNotActive so it
	//	doesn't get anymore calls.
	//--------------------------------------------------------------------
	virtual void NotActive();

	//--------------------------------------------------------------------
	//	Active() - called ONCE when a driver first goes active.  This
	//	function needs to reset m_bNotifyNotActive so it doesn't get
	//	anymore calls.
	//--------------------------------------------------------------------
	virtual void Active();

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	prtclChannelEmit &m_Channel;
};
