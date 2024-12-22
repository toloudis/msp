/*****************************************************************************
**	rmpObject.hpp
**
**		property object for the ramp data
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef RMP_OBJECT_HPP
#error rmpObject multipy included
#endif
#define RMP_OBJECT_HPP

#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif

#ifndef RMP_DATA_HPP
#include "Support/rmp/rmpData.hpp"
#endif

#ifndef PRTY_BUTTONUIINFO_HPP
#include "Core/prty/prtyButtonUIInfo.hpp"
#endif

#include <functional>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class rmpData;

//============================================================================
//============================================================================
class rmpObject : public prtyObject
{
public:
	typedef std::function<void (bool)> RampChangedFunction;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	rmpObject();
	~rmpObject();	
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetRampChangedCallback(const RampChangedFunction& i_FuncPtr);

private:

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void RegisterProperties();
	RampChangedFunction m_Callback;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void RampPropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);
	void RampCopyRequest(prtyProperty *i_pProperty, bool i_bDirty);
	void RampPasteRequest(prtyProperty *i_pProperty, bool i_bDirty);
public:
	rmpData m_Data;
	prtyButtonUIInfo* m_pPasteButton;
};