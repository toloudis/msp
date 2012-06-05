/*****************************************************************************
**	cmraDriverFollow.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverFollow.hpp"

#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFollowInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverFollowParser.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/prty/prtyVector3dEditUIInfo.hpp"
#include "ToolUIManaged/prtym/prtyFormControlBuilder.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFollow::cmraDriverFollow(tmlnChannelPosition &i_PChannel,
								   tmlnChannelPosition &i_TChannel)
:	m_PChannel(i_PChannel), 
	m_TChannel(i_TChannel),
	m_Direction("Direction")
{
	this->SetRestoreOriginalValue(true);

	//	set the starting values
	maVector3d vec = (m_PChannel.GetPosition() - m_TChannel.GetPosition());
	m_Direction.SetValue(vec);

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyVector3dEditUIInfo(&(m_Direction), "Follow Settings", "Direction");
	AddProperty( pPUII );

	// Register callbacks to update dirty bit when properties change
	m_Direction.AddCallback(new prtyCallbackWrapper<cmraDriverFollow>(this, &cmraDriverFollow::PropertyChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFollow::~cmraDriverFollow()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string cmraDriverFollow::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();

	char buffer[128];
	::sprintf(buffer, "Direction (%5.2f, %5.2f, %5.2f) ", m_Direction.GetValue().GetX(), m_Direction.GetValue().GetY(), m_Direction.GetValue().GetZ());
	desc += std::string(buffer);

	return desc;
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  cmraDriverFollow::GetDriverInfo() const
{
	cmraDriverFollowInfo *pInfo = new cmraDriverFollowInfo(cmraDriverFollowParser::GetChunkName());

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_Direction = this->m_Direction.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void cmraDriverFollow::SetDriverInfo(const cmraDriverFollowInfo& i_Info, 
									 prtyProperty::UndoFlags i_Undoable)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info, i_Undoable);

	// Store local info
	this->m_Direction.SetValue(i_Info.m_Direction, i_Undoable);
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  cmraDriverFollow::Operate(float i_fTime)
{
	if ( IsBefore(i_fTime) )
	{
		//maPoint3d goal = i_Goal;
		//maPoint3d cur = i_Channel.GetPosition();

		//float prevtime = i_Channel.GetPreviousTime(i_Time);
		//float percent = this->GetBlendAlpha( prevtime, i_Time );
		////maFunctions::Clamp( percent, 0.0f, 100.0f );

		//maPoint3d pos = goal*percent + cur*(1.0f - percent);	// linear blend

		////DBG_LOG3( "camkey TIME = %9.4f prev(%9.4f)  percent(%6.3f%%)", i_Time, prevtime, percent );
		////DBG_LOG3( "  curr (%6.2f,%6.2f,%6.2f)", cur.GetX(), cur.GetY(), cur.GetZ() );
		////DBG_LOG3( "  goal (%6.2f,%6.2f,%6.2f)", goal.GetX(), goal.GetY(), goal.GetZ() );
		////DBG_LOG3( "  pos  (%6.2f,%6.2f,%6.2f)", pos.GetX(), pos.GetY(), pos.GetZ() );

		//// If smooth blend, add in influence of the gradients
		//if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
		//{
		//	AddGradientInfluence(&i_Channel, i_Time, 
		//		this->GetBeginTime(), this->GetEndTime(), this->GetEaseInWeight(),
		//		goal, pos);
		//}

		//i_Channel.SetPosition(pos);
		return;
	}
	else if (IsWithin(i_fTime))
	{
		//	within the driver time range
		//
		Calculate_CameraLocation( i_fTime );
	}
}

//--------------------------------------------------------------------
//	AlterKey - look at the values in the channels to which this 
//	driver is connected and alter the driver in order to match 
//	these values.
//	Returns true if this driver was able to alter its value.
//--------------------------------------------------------------------
bool cmraDriverFollow::AlterKey()
{
	//maVector3d cur = m_PChannel.GetState();

	// Store local info, using undo
	//this->m_bClockwise.SetValue( cur, prtyProperty::eNewUndo  );

	return true;
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA cmraDriverFollow::GetClipFillColor() const
{
	return maFloatRGBA( 0.4f, 0.1f, 0.7f, 1.0f );
}

//--------------------------------------------------------------------
//	Clone - Clone this driver and return a new instace
//--------------------------------------------------------------------
//virtual 
tmlnDriver* cmraDriverFollow::Clone()
{
	return new cmraDriverFollow(*this);
}

//------------------------------------------------------------------------
//	calculate the position, pitch, etc of the camera.
//------------------------------------------------------------------------
void cmraDriverFollow::Calculate_CameraLocation(float i_fTime)
{
	maVector3d position, target;
	target = m_TChannel.GetPosition();

	position = target + m_Direction.GetValue();

	////DBG_LOG5( "%4.1f campos( %6.3f, %6.3f, %6.3f ) targetyaw(%6.3f)", percent, m_Position.GetX(), m_Position.GetY(), m_Position.GetZ(), m_fTargetYawAngle );

	m_PChannel.SetPosition( position );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverFollow::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}
