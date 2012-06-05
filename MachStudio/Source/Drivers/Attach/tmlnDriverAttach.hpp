/*****************************************************************************
**	tmlnDriverAttach.hpp
**
**	Derived driver class which deals with attachments.
**
**	StudioGPU
**	Copyright(C) 2003-7 - All Rights Reserved
\****************************************************************************/

#ifdef TMLN_DRIVERATTACH_HPP
#error tmlnDriverAttach.hpp multiply included
#endif
#define TMLN_DRIVERATTACH_HPP

#ifndef TMLN_DRIVERATTACHBASE_HPP
#include "Drivers/Attach/tmlnDriverAttachBase.hpp"
#endif 


//============================================================================
//	Forward References
//============================================================================
class tmlnDriverAttachInfo;

//============================================================================
//============================================================================
class tmlnDriverAttach : public tmlnDriverAttachBase
{

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverAttach(tmlnChannelPosition &i_Adapter, chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~tmlnDriverAttach();


	//--------------------------------------------------------------------
	//  GetDriverInfo - return data structure representing state of
	//		this driver suitable for writing to a file.
	//	The returned value should be created with "new" and will
	//		be deleted by the caller.
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo*  GetDriverInfo() const;

	//--------------------------------------------------------------------
	// Set internal variables from data structure.
	// If i_bPreservePosition is true, then try to
	// maintain the position of the object when changing the attachment.
	//--------------------------------------------------------------------
	void SetDriverInfo(	const tmlnDriverAttachInfo& i_Info );

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(const maTime& i_Time);

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

private:
	//--------------------------------------------------------------------
	// Property callbacks
	//--------------------------------------------------------------------
	void UseBBoxChanged(prtyProperty *i_pProperty, bool i_bDirty);

	tmlnChannelPosition &m_Channel;
	chDefs::Name m_ChunkName;
	maPoint3d m_LastPosition;
	
	prtyBoolean m_bUseBoundingBox;
};
