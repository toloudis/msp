/*****************************************************************************
**	tmlnDriverAttachOrient.hpp
**
**	Derived driver class which deals with attachments with position and
**	orientation.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef TMLN_DRIVERATTACHORIENT_HPP
#error tmlnDriverAttachOrient.hpp multiply included
#endif
#define TMLN_DRIVERATTACHORIENT_HPP

#ifndef TMLN_DRIVERATTACHBASE_HPP
#include "Drivers/Attach/tmlnDriverAttachBase.hpp"
#endif 
#ifndef PRTY_ROTATION_HPP
#include "Core/prty/prtyRotation.hpp"
#endif


//============================================================================
//	Forward References
//============================================================================
class tmlnChannelPosition;
class tmlnChannelOrientation;
class tmlnDriverAttachOrientInfo;


//============================================================================
//============================================================================
class tmlnDriverAttachOrient : public tmlnDriverAttachBase
{

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverAttachOrient(tmlnChannelPosition &i_PosChannel, 
						   tmlnChannelOrientation &i_OrientationChannel, 
						   chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~tmlnDriverAttachOrient();

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
	void SetDriverInfo(	const tmlnDriverAttachOrientInfo& i_Info );

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
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	tmlnChannelPosition &m_ChannelPosition;
	tmlnChannelOrientation &m_ChannelOrientation;
	chDefs::Name m_ChunkName;

	prtyRotation	m_AttachOrientation;
	maPoint3d m_LastPosition;
};
