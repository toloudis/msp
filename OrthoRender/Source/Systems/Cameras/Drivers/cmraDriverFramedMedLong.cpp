/*****************************************************************************
**	cmraDriverFramedMedLong.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverFramedMedLong.hpp"

#include "Systems/Cameras/Drivers/cmraDriverFramedBaseInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFramedMedLongParser.hpp"
#include "Systems/Cameras/Object/cmraScriptObject.hpp"

//	App
#include "Tool/cam3d/cam3dMgr.hpp"
//#include "Support/mnm/mnmObject.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"

//	Library
//#include "Core/dbg/dbgLog.hpp"
#include "Core/ma/maFunctions.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFramedMedLong::cmraDriverFramedMedLong( nameUID i_UID, 
												 tmlnChannelPosition &i_PChannel, 
												 tmlnChannelPosition &i_TChannel )
:	cmraDriverFramedBase( i_UID, i_PChannel, i_TChannel )
{
	//	initial values
	//
	this->m_StartPositionInfo.m_fDistance	= 10.0f;
	this->m_StartPositionInfo.m_fPitchAngle	= 45.0f;
	this->m_StartPositionInfo.m_fYawAngle	=  0.0f;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFramedMedLong::~cmraDriverFramedMedLong()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string cmraDriverFramedMedLong::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();
	return desc;
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  cmraDriverFramedMedLong::GetDriverInfo() const
{
	tmlnDriverInfo* pInfo = new cmraDriverFramedBaseInfo( cmraDriverFramedMedLongParser::GetChunkName() );
	GetBaseDriverInfo(*pInfo);
	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void cmraDriverFramedMedLong::SetDriverInfo(const cmraDriverFramedBaseInfo& i_Info, 
											prtyProperty::UndoFlags i_Undoable)
{
	cmraDriverFramedBase::SetDriverInfo( i_Info, i_Undoable );
}

//--------------------------------------------------------------------
//	Active() - called ONCE when a driver first goes active.  This
//	function needs to reset m_bNotifyNotActive so it doesn't get
//	anymore calls.
//--------------------------------------------------------------------
//virtual 
void cmraDriverFramedMedLong::Active()
{
	cmraDriverFramedBase::Active();
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  cmraDriverFramedMedLong::Operate(float i_fTime)
{
	if (IsBefore(i_fTime))
	{
		return;
	}

	if ( i_fTime == this->GetBeginTime() )
	{
		m_fLastTime = i_fTime;

		BuildTargetList();
		Calculate_Target( i_fTime, true );

		//	set up this instances "start frame" variables based on the position info variable.
		//
		m_StartPositionInfo.Calculate_Position( m_Target );
		Calculate_StartVariables();
	}

	if ( m_fLastTime == i_fTime )
	{
		return;
	}

	Calculate_FrameData();

	Calculate_Target( i_fTime );
	Calculate_CameraLocation( i_fTime );

	m_TargetChannel.SetPosition( m_Target );
	m_PositionChannel.SetPosition( m_Position );

	m_fLastTime = i_fTime;
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA cmraDriverFramedMedLong::GetClipFillColor() const
{
	return maFloatRGBA( 0.6f, 0.4f, 0.9f, 1.0f );
}

//------------------------------------------------------------------------
//	calculate the target based on the list of objects
//------------------------------------------------------------------------
//virtual
void cmraDriverFramedMedLong::Calculate_Target( float i_fTime, bool i_bForceSet )
{
	Calculate_TargetWorldBox();

	//	calculate where the target point is based on view type
	//
	// TODO:	base the target on the SCREEN-SPACE projetion of this box and
	//	make sure it's boundaries fall within the TOLERANCE of the view
	//	type.
	//
	float offsetpct;

	switch ( m_ViewTypeInfo.m_ViewType )
	{
		case cmraDriverDataViewTypeInfo::e_DataView_CloseUp:
		case cmraDriverDataViewTypeInfo::e_DataView_ExtremeCloseUp:
		{
			offsetpct = 0.25f;
			break;
		}
		case cmraDriverDataViewTypeInfo::e_DataView_Medium:
		{
			offsetpct = 0.50f;
			break;
		}
		case cmraDriverDataViewTypeInfo::e_DataView_MediumLong:
		{
			offsetpct = 0.75f;
			break;
		}
		case cmraDriverDataViewTypeInfo::e_DataView_Long:
		case cmraDriverDataViewTypeInfo::e_DataView_Overhead:
		case cmraDriverDataViewTypeInfo::e_DataView_Aerial:
		{
			offsetpct = 1.0f;
			break;
		}
	}

	m_TargetLast = m_Target;

	//	pick the center point
	//
	maVector3d newTarget;
	newTarget.SetX( (m_TargetWorldBox.GetMaxX() - m_TargetWorldBox.GetDiffX() / 2.0f) );
	newTarget.SetY( (m_TargetWorldBox.GetMaxY() - offsetpct * m_TargetWorldBox.GetDiffY() / 2.0f) );
	newTarget.SetZ( (m_TargetWorldBox.GetMaxZ() - m_TargetWorldBox.GetDiffZ() / 2.0f) );

	//DBG_LOG3( "NEW TARGET (%6.3f,%6.3f,%6.3f)", newTarget.GetX(), newTarget.GetY(), newTarget.GetZ() );

	//	see if still in tolerance zone.
	//
	//	if the new target screen space point is less than the tolerance than don't update the 
	//	target.
	//
	//	if it is outside the tolerance area then move it the difference.
	//
	//	if the target doesn't move then drift back to the center.
	//
	maPoint4d view;
	
	Calculate_ViewPos( newTarget, view );

	if ( i_bForceSet )
	{
		m_Target = newTarget;
		return;
	}

	//	if outside the view tolerance, then update the target.
	//
	if (   ( fabs(view.GetX()) > m_ViewTypeInfo.m_fViewTolerance )
	    || ( fabs(view.GetY()) > m_ViewTypeInfo.m_fViewTolerance ) )
	{
		maPoint3d tempTarget = newTarget - m_Target;
		if ( tempTarget.LengthSqr() > 1.0f )
		{
			tempTarget.Normalize();
		}

		float pct;
		//pct = ((i_fTime - m_fLastTime) / 1.0f);
		pct = 1.0f;
		tempTarget = tempTarget * pct;
		m_Target = m_Target + tempTarget;
	}

	//DBG_LOG4( "Target( %6.3f, %6.3f, %6.3f ) %6.3f", m_Target.GetX(), m_Target.GetY(), m_Target.GetZ(), offsetpct );
}

//------------------------------------------------------------------------
//	calculate the position, pitch, etc of the camera.
//------------------------------------------------------------------------
void cmraDriverFramedMedLong::Calculate_CameraLocation(float i_fTime)
{
	bool validPos = true;

	//float percent = (i_fTime - this->GetBeginTime()) / this->GetDuration();

	maPoint3d newPos = (m_Position + m_Target_FrameDiff);

	m_PositionLast = m_Position;

	if ( CheckIfPositionValid( newPos ) )
	{
		m_Position = newPos;
	}

	//
	// TODO: work out the ordering (and implementation) of the restrictions better.
	//

	//	the position hasn't been set yet
	//
	//	make sure the direction is within the restriction range
	//
	maVector3d dir( (m_Target - m_Position) );

	if ( CheckIfDirectionValid( dir ) )
	{
	}
	else
	{
		validPos = false;
	}

	if ( CheckIfDistanceValid( dir.LengthSqr() ) )
	{
	}
	else
	{
		validPos = false;
	}

	if ( CheckIfAngleValid( 0.0f ) )
	{
	}
	else
	{
		validPos = false;
	}

	//	something failed so roll back the new position
	//
	if ( !validPos )
	{
		m_Position = m_PositionLast;
	}
	//DBG_LOG3( "Pos   ( %6.3f, %6.3f, %6.3f )", m_Position.GetX(), m_Position.GetY(), m_Position.GetZ() );
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverFramedMedLong::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	cmraDriverFramedBase::PropertyChanged( i_pProperty, i_bDirty );
}
