/*****************************************************************************
**	cmraDriverFramedBase.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverFramedBase.hpp"

#include "Systems/Cameras/Drivers/cmraDriverFramedBaseInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFramedBaseParser.hpp"

#include "Tool/cam3d/cam3dMgr.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/mnm/mnmObject.hpp"
#include "Support/tmln/tmlnChannelPosition.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/Name/NameMgr.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/ma/maConstants.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFramedBase::cmraDriverFramedBase(nameUID i_UID, tmlnChannelPosition &i_PChannel, tmlnChannelPosition &i_TChannel)
:	m_PositionChannel(i_PChannel), 
	m_TargetChannel(i_TChannel), 
	m_bPositionRestrict("Restrict Position", false),
	m_PositionTolerance("Position Tolerance", maPoint3d(0.0f,0.0f,0.0f)),
	m_bAngleRestrict("Restrict Angle", false),
	m_AngleTolerance("Angle Tolerance", maPoint3d(0.0f,0.0f,0.0f)),
	m_bDistanceRestrict("Restrict Distance", false),
	m_DistanceTolerance("Distance Tolerance", maPoint3d(0.0f,0.0f,0.0f)),
	m_bDirectionRestrict("Restrict Direction", false),
	m_fDirectionTolerance("Direction Tolerance", 0.0f),
	m_PositionOrig(0.0f,0.0f,0.0f),
	m_PositionLast(0.0f,0.0f,0.0f),
	m_fRadiusOrig(0.0f),
	m_fPitchAngleOrig(0.0f),
	m_fTargetYawAngleOrig(0.0f),
	m_TargetLast(0.0f,0.0f,0.0f),
	m_TargetOrig(0.0f,0.0f,0.0f),
	m_fLastTime(-1.0f),
	m_Target_FrameDiff(0.0f,0.0f,0.0f),
	m_fPositionToleranceLenSqr(0.0f),
	m_DirOrigNorm(0.0f,0.0f,0.0f),
	m_fDirOrigLenSqr(0.0f),
	m_fDistanceToleranceLenSqr(0.0f),
	m_TargetWorldBox(0.0f,0.1f,0.0f,0.1f,0.0f,0.1f),
	m_CameraUID( i_UID )
{
	//DBG_LOG1( "Creating DriverFramed UID = %d", i_UID );

	//	initial values
	//
	this->m_StartPositionInfo.m_fDistance	= 10.0f;
	this->m_StartPositionInfo.m_fPitchAngle	= 45.0f;
	this->m_StartPositionInfo.m_fYawAngle	=  0.0f;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFramedBase::~cmraDriverFramedBase()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string cmraDriverFramedBase::GetHoverDescription()
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
tmlnDriverInfo*  cmraDriverFramedBase::GetDriverInfo() const
{
	cmraDriverFramedBaseInfo *pInfo = new cmraDriverFramedBaseInfo(cmraDriverFramedBaseParser::GetChunkName());

	this->GetBaseDriverInfo(*pInfo);

	//	set extended data
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

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void cmraDriverFramedBase::SetDriverInfo(	const cmraDriverFramedBaseInfo& i_Info, 
											prtyProperty::UndoFlags i_Undoable)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info, i_Undoable);

	//	set extended data
	m_StartPositionInfo	= i_Info.m_StartPositionData;
	m_ViewTypeInfo		= i_Info.m_ViewTypeData;
	m_SubjectInfo		= i_Info.m_SubjectData;

	// Store local info
	this->m_bPositionRestrict.SetValue( i_Info.m_bPositionRestrict, i_Undoable );
	this->m_PositionTolerance.SetValue( i_Info.m_PositionTolerance, i_Undoable );
	this->m_bAngleRestrict.SetValue( i_Info.m_bAngleRestrict, i_Undoable );
	this->m_AngleTolerance.SetValue( i_Info.m_AngleTolerance, i_Undoable );
	this->m_bDistanceRestrict.SetValue( i_Info.m_bDistanceRestrict, i_Undoable );
	this->m_DistanceTolerance.SetValue( i_Info.m_DistanceTolerance, i_Undoable );
	this->m_bDirectionRestrict.SetValue( i_Info.m_bDirectionRestrict, i_Undoable );
	this->m_fDirectionTolerance.SetValue( i_Info.m_fDirectionTolerance, i_Undoable );

	//i_Info.display_values();

	this->m_StartPositionInfo.m_fBeginTime = this->GetBeginTime();
}

//--------------------------------------------------------------------
//	Calculate the Start variables based on the info + target
//
//	Note: this assumes the m_StartPositionInfo.m_CamTarget and
//	m_StartPositionInfo.m_CamPosition has already been set.
//--------------------------------------------------------------------
//virtual 
void cmraDriverFramedBase::Calculate_StartVariables()
{
	// Can't really know the target point here,
	// so let's assume that our radius is correct
	// and move off of camera's position along view dir
	//
	m_PositionOrig		= this->m_StartPositionInfo.m_CamPosition;
	m_Position			= m_PositionOrig;
	m_PositionLast		= m_Position;
	m_TargetOrig		= this->m_StartPositionInfo.m_CamTarget;
	m_Target			= m_TargetOrig;
	m_TargetLast		= m_Target;

	this->m_PositionChannel.SetPosition( m_PositionOrig );
	this->m_TargetChannel.SetPosition( m_TargetOrig );

	//DBG_LOG3( "pos (%6.3f,%6.3f,%6.3f)", m_Position.GetX(), m_Position.GetY(), m_Position.GetZ() );

	m_fRadiusOrig		= this->m_StartPositionInfo.m_fDistance;
	m_fRadius			= m_fRadiusOrig;
	m_fTargetYawAngle	= this->m_StartPositionInfo.m_fYawAngle;
	m_fPitchAngleOrig	= this->m_StartPositionInfo.m_fPitchAngle;
	m_fPitchAngle		= m_fPitchAngleOrig;

	//maVector3d cam_dir		= (m_Target - m_Position);
	//maPoint3d target_dir	= -cam_dir;

	//DBG_LOG1( "Radius %6.3f", m_fRadius );
	//DBG_LOG1( "Yaw    %6.3f", m_fTargetYawAngle );
	//DBG_LOG1( "Pitch    %6.3f", m_fPitchAngle );
}

//--------------------------------------------------------------------
//	Active() - called ONCE when a driver first goes active.  This
//	function needs to reset m_bNotifyNotActive so it doesn't get
//	anymore calls.
//--------------------------------------------------------------------
//virtual 
void cmraDriverFramedBase::Active()
{
	BuildTargetList();

	//	first calc the target
	//
	Calculate_Target( this->GetBeginTime(), true );

	//	set up this instances "start frame" variables based on the position info variable.
	//
	//DBG_LOG3( "TARGET!!! (%6.3f,%6.3f,%6.3f)", m_Target.GetX(), m_Target.GetY(), m_Target.GetZ() );
	m_StartPositionInfo.Calculate_Position( m_Target );
	Calculate_StartVariables();

	tmlnDriver::Active();

	Calculate_DriverData();

	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//	for test purposes only
	//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//maPoint4d View;
	//maPoint3d testPoint;
	//nameString name;
	//name.SetUID( this->m_CameraUID );

	//int x,y;
	//for ( x = -1000 ; x <= 1000 ; x=x+100 )
	//{
	//	for ( y = -1000 ; y <= 1000 ; y=y+100 )
	//	{
	//		cmraScriptObject* pObj = cmraObjectMgr::GetObject( name );
	//		testPoint = pObj->GetTarget();
	//		testPoint.SetX( testPoint.GetX() + x );
	//		testPoint.SetY( testPoint.GetY() + y );
	//		Calculate_ViewPos( testPoint, View );
	//	}
	//}
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
//virtual
void  cmraDriverFramedBase::Operate(float i_fTime)
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

	Calculate_FrameData();					// calc this data ONCE per frame so other funcs can use the results

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
maFloatRGBA cmraDriverFramedBase::GetClipFillColor() const
{
	return maFloatRGBA( 0.7f, 0.5f, 0.9f, 1.0f );
}

//--------------------------------------------------------------------
//	Set the begin time of the driver
//--------------------------------------------------------------------
//virtual 
void  cmraDriverFramedBase::SetBeginTime(float i_BeginTime)
{
	tmlnDriver::SetBeginTime( i_BeginTime );

	this->m_StartPositionInfo.m_fBeginTime = i_BeginTime;
}

//------------------------------------------------------------------------
//	Build the target list from the data subject info
//------------------------------------------------------------------------
//virtual 
void cmraDriverFramedBase::BuildTargetList()
{
	//	set-up the objects to watch
	//
	nameObject* pObj;
	m_Objects.resize( m_SubjectInfo.m_ObjectNames.size() );
	int i;
	for ( i = 0 ; i < m_SubjectInfo.m_ObjectNames.size() ; ++i )
	{
		//DBG_LOG2( "%02d) %s", i, m_SubjectInfo.m_Objects[i]->GetName().GetString().c_str() );

		pObj = nameMgr::GetObjectByName( m_SubjectInfo.m_ObjectNames[i] );

		//	if the name can't be found, try to find it by just the string
		//	this would happen if importing this driver
		if (pObj == 0)
		{
			m_SubjectInfo.m_ObjectNames[i].SetUID(-1);
			pObj = nameMgr::GetObjectByName( m_SubjectInfo.m_ObjectNames[i] );

			//	if the name was found, reset the UID to the proper one.
			if (pObj != 0)
				m_SubjectInfo.m_ObjectNames[i].SetUID( pObj->GetName().GetUID() );
		}
		DBG_ASSERT1( pObj != 0, "Couldn't find the object (%s)", m_SubjectInfo.m_ObjectNames[i].GetString().c_str() );
		m_Objects[i] = dynamic_cast<mnmObject*>(pObj);
		DBG_ASSERT1( m_Objects[i] != 0, "Couldn't cast the object (%s)", m_SubjectInfo.m_ObjectNames[i].GetString().c_str() );
	}
}

//------------------------------------------------------------------------
//	calculate the target based on the list of objects
//------------------------------------------------------------------------
//virtual
void cmraDriverFramedBase::Calculate_Target( float i_fTime, bool i_bForceSet )
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
		tempTarget.Normalize();

		float pct;
		pct = ((i_fTime - m_fLastTime) / 1.0f) * 2.0f;
		tempTarget = tempTarget * pct;
		m_Target = m_Target + tempTarget;

		//maPoint3d tempTarget;
		//Calculate_TargetPos( view, newTarget, tempTarget );
		//m_Target = tempTarget;

		//DBG_LOG3( "new Target( %6.3f, %6.3f, %6.3f ) ", m_Target.GetX(), m_Target.GetY(), m_Target.GetZ() );
	}
	else
	{
		//m_Target = newTarget;
		//	if the camera isn't centered, slowly center it.
		//
		//if (   (fabs(view.GetX()) >= 0.01f)
		//	|| (fabs(view.GetY()) >= 0.01f) )
		//{
		//}
	}

	//DBG_LOG4( "Target( %6.3f, %6.3f, %6.3f ) %6.3f", m_Target.GetX(), m_Target.GetY(), m_Target.GetZ(), offsetpct );
}

//------------------------------------------------------------------------
//	calculate the position of the point projected into the camera view.
//	values will be between -1.0 and 1.0.
//------------------------------------------------------------------------
//virtual
void cmraDriverFramedBase::Calculate_ViewPos( maPoint3d& i_Point, maPoint4d& o_View )
{
	maMatrix4x4 camprojmat;
	maMatrix4x4 cammat;
	maMatrix4x4 projmat;

	nameString name;
	name.SetUID( this->m_CameraUID );
	int camindex = camsCameraMgr::GetIndexForName( name );
	DBG_ASSERT1( camindex != -1, "Couldn't find the camera (%s)", name.GetString().c_str() );

	const camCamera* pCamera = camsCameraMgr::GetCamera(camindex);
	pCamera->GetCameraMatrix(cammat);
	pCamera->GetProjectionMatrix(projmat);
	//projmat.ScaleBy(-1.0f, 1.0f, 1.0f);
	camprojmat = cammat * projmat;

	maPoint4d vertex(i_Point.GetX(), i_Point.GetY(), i_Point.GetZ(), 1.0f);

	vertex = camprojmat * vertex;
	vertex.Wdiv();
	vertex.Normalize();

	o_View = vertex;
}

//------------------------------------------------------------------------
//	calculate a close (and valid) position based on the (normalized) view  
//	values
//------------------------------------------------------------------------
//virtual
void cmraDriverFramedBase::Calculate_TargetPos( maPoint4d& i_View, maPoint3d& i_TargetPoint, maPoint3d& o_Point )
{
	//	first, set the normalized view within the valid view range.
	//
	if ( i_View.GetX() > m_ViewTypeInfo.m_fViewTolerance )
		i_View.SetX( m_ViewTypeInfo.m_fViewTolerance );
	if ( i_View.GetY() > m_ViewTypeInfo.m_fViewTolerance )
		i_View.SetY( m_ViewTypeInfo.m_fViewTolerance );

	//	extend the valid normalized vector out to the length of the last
	//	valid target.
	//
	maVector3d lineseg;
	maPoint3d edgept;

	lineseg = (i_TargetPoint - m_Position);

	i_View = i_View * lineseg.Length();
	edgept.SetX( i_View.GetX() );
	edgept.SetY( i_View.GetY() );
	edgept.SetZ( i_View.GetZ() );

	o_Point.SetX( m_Target.GetX() + (i_TargetPoint.GetX()-edgept.GetX()) );
	o_Point.SetY( m_Target.GetY() + (i_TargetPoint.GetY()-edgept.GetY()) );
	o_Point.SetZ( m_Target.GetZ() + (i_TargetPoint.GetZ()-edgept.GetZ()) );

	//DBG_LOG4( "New Target( %6.3f, %6.3f, %6.3f ) %6.3f", o_Point.GetX(), o_Point.GetY(), o_Point.GetZ(), lineseg.Length() );
}

//------------------------------------------------------------------------
//	calculate the position, pitch, etc of the camera.
//------------------------------------------------------------------------
//virtual
void cmraDriverFramedBase::Calculate_CameraLocation(float i_fTime)
{
	bool validPos = true;

	//float percent = (i_fTime - this->GetBeginTime()) / this->GetDuration();

	maPoint3d newPos = (m_Position + m_Target_FrameDiff);

	m_PositionLast = m_Position;

	//
	// TODO: work out the ordering (and implementation) of the restrictions better.
	//

	//if ( CheckIfPositionValid( newPos ) )
	//{
	//	m_Position = newPos;
	//}
	//
	////float x,y,z;
	////float yaw;

	////	the position hasn't been set yet
	////
	////	make sure the direction is within the restriction range
	////
	//maVector3d dir( (m_Target - m_Position) );

	//if ( CheckIfDirectionValid( dir ) )
	//{
	//}
	//else
	//{
	//	validPos = false;
	//}

	//if ( CheckIfDistanceValid( dir.LengthSqr() ) )
	//{
	//}
	//else
	//{
	//	validPos = false;
	//}

	//if ( CheckIfAngleValid( 0.0f ) )
	//{
	//}
	//else
	//{
	//	validPos = false;
	//}

	////	something failed so roll back the new position
	////
	//if ( !validPos )
	//{
	//	m_Position = m_PositionLast;
	//}
	////DBG_LOG3( "Pos   ( %6.3f, %6.3f, %6.3f )", m_Position.GetX(), m_Position.GetY(), m_Position.GetZ() );
}

//------------------------------------------------------------------------
//	calculate the bounding box represented by the selected objects
//------------------------------------------------------------------------
//virtual
void cmraDriverFramedBase::Calculate_TargetWorldBox()
{
	int size = m_Objects.size();
	int i;
	maAxisBox objBox;

	//	grab each box
	for ( i = 0 ; i < size ; ++i )
	{
		DBG_ASSERT1( m_Objects[i] != 0, "Invalid object #%d", i );

		objBox = m_Objects[i]->GetWorldBox();

		if ( i == 0 )
		{
			m_TargetWorldBox = objBox;
		}
		else
		{
			if ( m_TargetWorldBox.GetMaxX() < objBox.GetMaxX() )
			{
				m_TargetWorldBox.SetMaxX( objBox.GetMaxX() );
			}
			if ( m_TargetWorldBox.GetMaxY() < objBox.GetMaxY() )
			{
				m_TargetWorldBox.SetMaxY( objBox.GetMaxY() );
			}
			if ( m_TargetWorldBox.GetMaxZ() < objBox.GetMaxZ() )
			{
				m_TargetWorldBox.SetMaxZ( objBox.GetMaxZ() );
			}
			if ( m_TargetWorldBox.GetMinX() > objBox.GetMinX() )
			{
				m_TargetWorldBox.SetMinX( objBox.GetMinX() );
			}
			if ( m_TargetWorldBox.GetMinY() > objBox.GetMinY() )
			{
				m_TargetWorldBox.SetMinY( objBox.GetMinY() );
			}
			if ( m_TargetWorldBox.GetMinZ() > objBox.GetMinZ() )
			{
				m_TargetWorldBox.SetMinZ( objBox.GetMinZ() );
			}
		}
	}
}

//------------------------------------------------------------------------
//	calculate the one-time driver data
//------------------------------------------------------------------------
//virtual
void cmraDriverFramedBase::Calculate_DriverData()
{
	m_fPositionToleranceLenSqr	= m_PositionTolerance.GetValue().LengthSqr();
	m_fDistanceToleranceLenSqr	= m_DistanceTolerance.GetValue().LengthSqr();
	m_DirOrigNorm				= (m_TargetOrig - m_PositionOrig);
	m_DirOrigNorm.Normalize();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//virtual
void cmraDriverFramedBase::Calculate_FrameData()
{
	m_Target_FrameDiff			= m_Target - m_TargetLast;
	m_fPositionToleranceLenSqr	= m_PositionTolerance.GetValue().LengthSqr();
	m_fDistanceToleranceLenSqr	= m_DistanceTolerance.GetValue().LengthSqr();
}

//------------------------------------------------------------------------
//	see if the new point is valid. if so, return true
//------------------------------------------------------------------------
//virtual
bool cmraDriverFramedBase::CheckIfPositionValid( maPoint3d& i_NewPoint )
{
	if (m_bPositionRestrict.GetValue())
	{
		maVector3d pm_TargetDiff = (i_NewPoint - m_PositionOrig);
		if (pm_TargetDiff.LengthSqr() <= m_fPositionToleranceLenSqr)
		{
			//	the new point is within the range to set the position.
			//DBG_LOG6( "Pos-Res( %6.3f, %6.3f, %6.3f )  delta( %6.3f, %6.3f, %6.3f )", m_Position.GetX(), m_Position.GetY(), m_Position.GetZ(), m_TargetDiff.GetX(), m_TargetDiff.GetY(), m_TargetDiff.GetZ() );
			return true;
		}
		else
		{
			//m_Position = xxx;	// TODO: calc position to get to edge of "tolerance area"
			return false;
		}
	}
	else
	{
		//DBG_LOG6( "Pos-NoR( %6.3f, %6.3f, %6.3f )  delta( %6.3f, %6.3f, %6.3f )", m_Position.GetX(), m_Position.GetY(), m_Position.GetZ(), m_TargetDiff.GetX(), m_TargetDiff.GetY(), m_TargetDiff.GetZ() );
		return true;
	}
}

//------------------------------------------------------------------------
//	see if the new direction is valid. if so return true
//------------------------------------------------------------------------
//virtual
bool cmraDriverFramedBase::CheckIfDirectionValid( maVector3d& i_NewDir )
{
	if (m_bDirectionRestrict.GetValue())
	{
		maVector3d dirNorm = i_NewDir;
		dirNorm.Normalize();
		double angle = (m_DirOrigNorm * dirNorm);
		if ( (float)fabs((acos( angle ) * maConstants::c_dRadToAngle)) <= this->m_fDirectionTolerance.GetValue())
		{
			// the position + target are within the direction range
			return true;
		}
		
		return false;
	}

	return true;
}

//------------------------------------------------------------------------
//	see if the new distance (squared) is valid. if so return true
//------------------------------------------------------------------------
//virtual
bool cmraDriverFramedBase::CheckIfDistanceValid( float i_fNewDistSqr )
{
	if ( m_bDistanceRestrict.GetValue() )
	{
		//	make sure m_fRadius is within it's limits
		if ( i_fNewDistSqr <= m_fDistanceToleranceLenSqr )
		{
			return true;
		}

		return false;
	}

	return true;
}

//------------------------------------------------------------------------
//	see if the new angle is valid. if so return true
//------------------------------------------------------------------------
//virtual
bool cmraDriverFramedBase::CheckIfAngleValid( float i_fNewAngle )
{
	return true;
}

//------------------------------------------------------------------------
//	calculate the each-frame driver data once
//------------------------------------------------------------------------
//virtual
void cmraDriverFramedBase::display_data()
{
	DBG_LOG0( "DISPLAY data object list FRAMED" );
	int size = m_SubjectInfo.m_ObjectNames.size();
	for ( int j = 0 ; j < size ; j++ )
	{
		DBG_LOG2( " %02d - (%s)", j, m_SubjectInfo.m_ObjectNames[j].GetString().c_str() );
	}

	DBG_LOG1( "DISPLAY view type (%d)", m_ViewTypeInfo.m_ViewType );

	DBG_LOG1( "DISPLAY restrict position (%d)", (int)m_bPositionRestrict.GetValue() );
	DBG_LOG1( "DISPLAY restrict angle (%d)", (int)m_bAngleRestrict.GetValue() );
	DBG_LOG1( "DISPLAY restrict distance (%d)", (int)m_bDistanceRestrict.GetValue() );
	DBG_LOG1( "DISPLAY restrict direction (%d)", (int)m_bDirectionRestrict.GetValue() );

	//m_PositionTolerance
	//m_AngleTolerance
	//m_DistanceTolerance
	//m_fDirectionTolerance
}

//------------------------------------------------------------------------
//	set the min and max for the form variables
//------------------------------------------------------------------------
//virtual 
//void cmraDriverFramedBase::Configure_PositionForm(StudioFramework::cmraDriverDataPositionForm * i_pPForm)
//{
//	DBG_ASSERT0( i_pPForm != 0, "Form is NULL" );
//
//	i_pPForm->SetDistanceValues( m_StartPositionInfo.m_fDistance, ( 0.01f ), ( 3.0f ) );
//	i_pPForm->SetPitchValues( m_StartPositionInfo.m_fPitch, ( 0.01f ), ( 80.0f ) );
//	i_pPForm->SetYawValues( m_StartPositionInfo.m_fYaw, ( -180.0f ), ( 180.0f ) );
//}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverFramedBase::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}
