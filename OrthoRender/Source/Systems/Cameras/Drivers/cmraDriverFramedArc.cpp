/*****************************************************************************
**	cmraDriverFramedArc.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverFramedArc.hpp"

#include "Systems/Cameras/Drivers/cmraDriverFramedArcInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFramedArcParser.hpp"

#include "Support/mnm/mnmObject.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"

#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"


//--------------------------------------------------------------------
//	constants
//--------------------------------------------------------------------
const float c_fVIEWTOLERANCE_TOLERANCE = 0.05f;


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFramedArc::cmraDriverFramedArc( nameUID i_UID, tmlnChannelPosition &i_PChannel, tmlnChannelPosition &i_TChannel)
:	cmraDriverFramedBase( i_UID, i_PChannel, i_TChannel ),
	m_fRevolutions("Revolutions", 0.5f),
	m_bClockwise("Clockwise", true)
{
	//	initial values
	//
	this->m_StartPositionInfo.m_fDistance	= 10.0f;
	this->m_StartPositionInfo.m_fPitchAngle	= 45.0f;
	this->m_StartPositionInfo.m_fYawAngle	=  0.0f;

	// TODO UIINFO
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyPropertyUIInfo(&(m_fRevolutions), "category", "description");
	AddProperty( pPUII );
	pPUII = new prtyPropertyUIInfo(&(m_bClockwise), "category", "description");
	AddProperty( pPUII );

	//	property callbacks
	m_fRevolutions.AddCallback(new prtyCallbackWrapper<cmraDriverFramedArc>(this, &cmraDriverFramedArc::PropertyChanged));
	m_bClockwise.AddCallback(new prtyCallbackWrapper<cmraDriverFramedArc>(this, &cmraDriverFramedArc::PropertyChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFramedArc::~cmraDriverFramedArc()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string cmraDriverFramedArc::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();

	char buffer[128];
	sprintf( buffer, "Framed Arc Revolutions = %4.2f", m_fRevolutions.GetValue() );
	desc += std::string(buffer);
	return desc;
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  cmraDriverFramedArc::GetDriverInfo() const
{
	cmraDriverFramedArcInfo *pInfo = new cmraDriverFramedArcInfo(cmraDriverFramedArcParser::GetChunkName());

	this->GetBaseDriverInfo(*pInfo);

	//	set extended data (from FramedBase)
	pInfo->m_StartPositionData	= m_StartPositionInfo;
	pInfo->m_ViewTypeData		= m_ViewTypeInfo;
	pInfo->m_SubjectData		= m_SubjectInfo;

	pInfo->m_bPositionRestrict	= this->m_bPositionRestrict.GetValue();
	pInfo->m_PositionTolerance	= this->m_PositionTolerance.GetValue();
	pInfo->m_bAngleRestrict		= this->m_bAngleRestrict.GetValue();
	pInfo->m_AngleTolerance		= this->m_AngleTolerance.GetValue();
	pInfo->m_bDistanceRestrict	= this->m_bDistanceRestrict.GetValue();
	pInfo->m_DistanceTolerance	= this->m_DistanceTolerance.GetValue();
	pInfo->m_bDirectionRestrict	= this->m_bDirectionRestrict.GetValue();
	pInfo->m_fDirectionTolerance= this->m_fDirectionTolerance.GetValue();

	pInfo->m_StartPositionData.m_fBeginTime		= this->GetBeginTime();

	//	arc specific
	pInfo->m_fRevolutions	= this->m_fRevolutions.GetValue();
	pInfo->m_bClockwise		= this->m_bClockwise.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void cmraDriverFramedArc::SetDriverInfo(const cmraDriverFramedArcInfo& i_Info, 
										prtyProperty::UndoFlags i_Undoable)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info, i_Undoable);

	//	set extended data (FramedBase)
	m_StartPositionInfo	= i_Info.m_StartPositionData;
	m_ViewTypeInfo		= i_Info.m_ViewTypeData;
	m_SubjectInfo		= i_Info.m_SubjectData;

	this->m_bPositionRestrict.SetValue( i_Info.m_bPositionRestrict, i_Undoable );
	this->m_PositionTolerance.SetValue( i_Info.m_PositionTolerance, i_Undoable );
	this->m_bAngleRestrict.SetValue( i_Info.m_bAngleRestrict, i_Undoable );
	this->m_AngleTolerance.SetValue( i_Info.m_AngleTolerance, i_Undoable );
	this->m_bDistanceRestrict.SetValue( i_Info.m_bDistanceRestrict, i_Undoable );
	this->m_DistanceTolerance.SetValue( i_Info.m_DistanceTolerance, i_Undoable );
	this->m_bDirectionRestrict.SetValue( i_Info.m_bDirectionRestrict, i_Undoable );
	this->m_fDirectionTolerance.SetValue( i_Info.m_fDirectionTolerance, i_Undoable );

	this->m_StartPositionInfo.m_fBeginTime = this->GetBeginTime();

	// Store local info
	this->m_fRevolutions.SetValue( i_Info.m_fRevolutions, i_Undoable );
	this->m_bClockwise.SetValue( i_Info.m_bClockwise, i_Undoable );
}

//--------------------------------------------------------------------
//	Active() - called ONCE when a driver first goes active.  This
//	function needs to reset m_bNotifyNotActive so it doesn't get
//	anymore calls.
//--------------------------------------------------------------------
//virtual 
void cmraDriverFramedArc::Active()
{
	cmraDriverFramedBase::Active();
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  cmraDriverFramedArc::Operate(float i_fTime)
{
	if ( IsBefore(i_fTime) )
	{
		//	no blending, don't do anything
		return;
	}

	if ( i_fTime == this->GetBeginTime() )
	{
		//	Before the begin time
		//

		//m_fLastTime = i_fTime;

		BuildTargetList();
		Calculate_Target( i_fTime, true );

		//	set up this instances "start frame" variables based on the position info variable.
		//
		m_StartPositionInfo.Calculate_Position( m_Target );
		Calculate_StartVariables();

		//DBG_LOG4( "%campos( %6.3f, %6.3f, %6.3f ) targetyaw(%6.3f)", m_Position.GetX(), m_Position.GetY(), m_Position.GetZ(), m_fTargetYawAngle );
	}
	else if (IsWithin(i_fTime))
	{
		//	within the driver time range
		//

		Calculate_Target( i_fTime );
		Calculate_CameraLocation( i_fTime );
	}

	//DBG_LOG1( "TargetYaw=%6.3f", m_fTargetYawAngle );
	m_TargetChannel.SetPosition( m_Target );
	m_PositionChannel.SetPosition( m_Position );
	//GetObject()->LookAt(pos, m_Target, maVector3d(0,1,0));
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA cmraDriverFramedArc::GetClipFillColor() const
{
	return maFloatRGBA( 0.7f, 0.5f, 0.9f, 1.0f );
}

//--------------------------------------------------------------------
//	Set the begin time of the driver
//--------------------------------------------------------------------
//virtual 
void  cmraDriverFramedArc::SetBeginTime(float i_fBeginTime)
{
	tmlnDriver::SetBeginTime( i_fBeginTime );

	this->m_StartPositionInfo.m_fBeginTime = i_fBeginTime;
}

//------------------------------------------------------------------------
//	calculate the target based on the list of objects
//------------------------------------------------------------------------
//virtual
void cmraDriverFramedArc::Calculate_Target( float i_fTime, bool i_bForceSet )
{
	Calculate_TargetWorldBox();

	//	calculate where the target point is based on view type
	//
	// TODO:	base the target on the SCREEN-SPACE projection of this box and
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
	if (   ( fabs(view.GetX()) > (m_ViewTypeInfo.m_fViewTolerance) )
	    || ( fabs(view.GetY()) > (m_ViewTypeInfo.m_fViewTolerance) ) )
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

		//DBG_LOG3( "new Target( %6.3f, %6.3f, %6.3f ) ", m_Target.GetX(), m_Target.GetY(), m_Target.GetZ() );
	}
}

//------------------------------------------------------------------------
//	calculate the position, pitch, etc of the camera.
//------------------------------------------------------------------------
void cmraDriverFramedArc::Calculate_CameraLocation(float i_fTime)
{
	float yaw;
	float pitch;
	float percent = (i_fTime - this->GetBeginTime()) / this->GetDuration();
	if (percent > 1.0f)
		percent = 1.0f;

	yaw = m_fTargetYawAngle * maConstants::c_fAngleToRad + (360.0f * maConstants::c_fAngleToRad) * (this->m_fRevolutions.GetValue() * percent);
	pitch = m_fPitchAngle * maConstants::c_fAngleToRad;

	float x = float(sin(yaw) * cos(pitch));
	float y = float(sin(pitch));
	float z = float(cos(yaw) * cos(pitch));

	maVector3d dir(-x, -y, -z);

	m_Position = m_Target - dir * m_fRadius;
	//DBG_LOG5( "%4.1f campos( %6.3f, %6.3f, %6.3f ) targetyaw(%6.3f)", percent, m_Position.GetX(), m_Position.GetY(), m_Position.GetZ(), m_fTargetYawAngle );

	//
	// TODO: work out the ordering (and implementation) of the restrictions better.
	//
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverFramedArc::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{
	cmraDriverFramedBase::PropertyChanged( i_pProperty, i_bDirty );
}
