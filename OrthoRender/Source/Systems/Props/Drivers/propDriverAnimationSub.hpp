/*****************************************************************************
**	propDriverAnimationSub.hpp
**
**	Derived driver class for sub object animation
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PROP_DRIVERANIMATIONSUB_HPP
#error propDriverAnimationSub.hpp multiply included
#endif
#define PROP_DRIVERANIMATIONSUB_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif

#ifndef API3D_OBJECTENTITY_HPP
#include "Tool/api3d/api3dObjectEntity.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/Ch/chDefs.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif
#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif


//============================================================================
//============================================================================
class propChannelAnimationSub;
class propDriverAnimationSubInfo;
class tmlnDriverInfo;


//============================================================================
//============================================================================
class propDriverAnimationSub : public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	propDriverAnimationSub(propChannelAnimationSub &i_Channel);

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
	void SetDriverInfo(	const propDriverAnimationSubInfo& i_Info, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//  Show dialog that allows user to edit this driver's properties
	//--------------------------------------------------------------------
	virtual void  DoEditProperties();

	//--------------------------------------------------------------------
	// Update - update the object
	//--------------------------------------------------------------------
	void Update();

	//====================================================================
	// Get/Set data
	//====================================================================

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool IsDriverToAnimLength();
	void SetDriverToAnimLengthFlag( bool i_bSetFlag, 
									prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const itString&	GetAnimName();
	void SetAnimName( const itString& i_AnimName, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	int GetAnimIndex();
	void SetAnimIndex( int i_AnimIndex, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	// Adapter
	//--------------------------------------------------------------------
	propChannelAnimationSub& Adapter();
	const propChannelAnimationSub& GetAdapter() const;
	void  SetAdapter(const propChannelAnimationSub& i_Channel);

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

protected:
	//--------------------------------------------------------------------
	//	PerformSplit - Split up the details of this driver between itself
	//	and the driver passed in.
	//--------------------------------------------------------------------
	virtual void PerformSplit( tmlnDriver* io_pDriverAtEnd, float i_fTime );

private:
	//--------------------------------------------------------------------
	// return true if blending and if true, return amount blending time
	//--------------------------------------------------------------------
	bool get_blend_time(float i_Time, float &o_BlendTime);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	propChannelAnimationSub  &m_Channel;

	prtyBoolean		m_bDriverToAnimLength;
	prtyFileName	m_AnimName;
	prtyInt32		m_AnimIndex;
};
