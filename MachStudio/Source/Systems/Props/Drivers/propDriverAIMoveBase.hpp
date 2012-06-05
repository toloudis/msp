/*****************************************************************************
**	propDriverAIMoveBase.hpp
**
**	Derived driver class for AI movement
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef PROP_DRIVERAIMOVEBASE_HPP
#error propDriverAIMoveBase.hpp multiply included
#endif
#define PROP_DRIVERAIMOVEBASE_HPP

#ifndef PROP_DRIVERANIMATIONFULL_HPP
#include "propDriverAnimationFull.hpp"
#endif
#ifndef TMLN_DRIVERSPLINEORIENTED_HPP
#include "tmlnDriverSplineOriented.hpp"
#endif
//#ifndef TMLN_DRIVER_HPP
//#include "tmlnDriver.hpp"
//#endif

#ifndef CH_DEFS_HPP
#include "chDefs.hpp"
#endif
#ifndef IT_STRING_HPP
#include "itString.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class propChannelPosition;
class propChannelOrientation;
class propChannelAnimationFull;
class propDriverAIMoveBaseInfo;
class propScriptObject;
class tmlnDriverInfo;


//============================================================================
//============================================================================
class propDriverAIMoveBase :public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	propDriverAIMoveBase( propScriptObject* i_pObject );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~propDriverAIMoveBase();

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
	void SetDriverInfo(const propDriverAIMoveBaseInfo& i_Info, prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

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
	void SetDriverToAnimLengthFlag( bool i_bSetFlag );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	const std::string& GetAnimName() const;
	void SetAnimName( const std::string& i_AnimName );

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//int GetAnimIndex();
	//void SetAnimIndex( int i_AnimIndex );

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

	//--------------------------------------------------------------------
	// Adapters
	//--------------------------------------------------------------------
	//propChannelPosition& AdapterPosition();
	//const propChannelPosition& GetAdapterPosition() const;
	//void  SetAdapterPosition(const propChannelPosition& i_ChannelP);
	//propChannelOrientation& AdapterOrientation();
	//const propChannelOrientation& GetAdapterOrientation() const;
	//void  SetAdapterOrientation(const propChannelOrientation& i_ChannelO);
	//propChannelAnimationFull& AdapterAnimationFull();
	//const propChannelAnimationFull& GetAdapterAnimationFull() const;
	//void  SetAdapterAnimationFull(const propChannelAnimationFull& i_ChannelAF);

private:
	//--------------------------------------------------------------------
	// return true if blending and if true, return amount blending time
	//--------------------------------------------------------------------
	bool get_blend_time(float i_Time, float &o_BlendTime);

	propDriverAnimationFull*	m_pDriverAnimationFull;
	tmlnDriverSplineOriented*	m_pDriverSpline;
};

