/*****************************************************************************
**	cmraDriverTarget.hpp
**
**		Derived driver class
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef CMRA_DRIVERTARGET_HPP
#error cmraDriverTarget.hpp multiply included
#endif
#define CMRA_DRIVERTARGET_HPP


#ifndef CMRA_ADAPTERREFERENCE_HPP
#include "Systems/Cameras/Timeline/cmraAdapterReference.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif
#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif

#include <string>
#include <vector>


//============================================================================
//============================================================================
class tmlnAdapterGetPosition;
class tmlnChannelPosition;
class cmraDriverTargetInfo;


//============================================================================
//============================================================================
class cmraDriverTarget : public tmlnDriver
{

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverTarget(tmlnChannelPosition &i_Adapter);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~cmraDriverTarget();

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse hovers over the clip.
	//--------------------------------------------------------------------
	std::string GetHoverDescription();

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
	void SetDriverInfo(	const cmraDriverTargetInfo& i_Info, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(float i_Time);

	//--------------------------------------------------------------------
	//  Get list of possible reference attachment points.
	//--------------------------------------------------------------------
	void  GetReferenceList(std::vector<std::string> &o_List);

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	tmlnChannelPosition &m_Channel;
	cmraAdapterReference m_Reference;

	prtyName	m_ObjectName;
	prtyText	m_AttachName;
	prtyPoint3d	m_TargetOffset;
};
