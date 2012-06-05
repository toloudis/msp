/*****************************************************************************
**	tmlnDriverPosition.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Drivers/Position/tmlnDriverPosition.hpp"

#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Drivers/Position/tmlnDriverPositionInfo.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/ma/maConstants.hpp"
#include "Support/pnt/pntPointObject.hpp"
#include "Support/pnt/pntPointMgr.hpp"
#include "Support/pnt/pntPointSelect.hpp"
#include "Core/prty/prtyVector3dEditUpDownUIInfo.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"


//============================================================================
//============================================================================
const char* c_PointGroup = "DriverPosition";


//--------------------------------------------------------------------
// The parent object pointer should be the tmlnScriptObject that
//	 this driver will control. It is used to associate the 
//	 driver icons back to the main object.
//--------------------------------------------------------------------
tmlnDriverPosition::tmlnDriverPosition(tmlnChannelPosition &i_Channel, 
									   chDefs::Name i_ChunkName, 
									   pick3dPickObject* i_pParent)
:	m_PChannel(i_Channel), 
	m_ChunkName(i_ChunkName),
	m_Position("Position", i_Channel.GetPosition())
{
	m_Point.SetPosition( i_Channel.GetPosition() );
	m_Point.SetCallback(this);
	this->SetBlendType(tmlnDriver::GetDefaultBlendType());

	std::string pt_name = i_Channel.GetName() + " Static Key";
	pntPointMgr::AddPoint(c_PointGroup, &m_Point, i_pParent, pt_name);

	// Register the property so it can be displayed to the user
	prtyVector3dEditUpDownUIInfo* pPUII;
	pPUII = new prtyVector3dEditUpDownUIInfo(&m_Position, "Transform", "Position");
	pPUII->SetIncrement(0.1f, 0.1f, 0.1f);
	AddProperty( pPUII );

	// Register callbacks to update dirty bit when properties change
	m_Position.AddCallback(new prtyCallbackWrapper<tmlnDriverPosition>(this, &tmlnDriverPosition::PositionChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverPosition::~tmlnDriverPosition()
{
	pntPointMgr::RemovePoint(&m_Point);
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string tmlnDriverPosition::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();

	char buffer[128];
	::sprintf(buffer, "Position: %.3f %.3f %.3f", m_Position.GetValue().GetX(), m_Position.GetValue().GetY(), m_Position.GetValue().GetZ());
	desc += std::string(buffer);
	return desc;
}

//--------------------------------------------------------------------
// Set Position value in driver
//--------------------------------------------------------------------
void tmlnDriverPosition::SetValue(const maPoint3d& i_Val)
{
	m_Position.SetValue(i_Val);
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  tmlnDriverPosition::GetDriverInfo() const
{
	tmlnDriverPositionInfo *pInfo = new tmlnDriverPositionInfo(m_ChunkName);

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_Value = m_Position.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void tmlnDriverPosition::SetDriverInfo(const tmlnDriverPositionInfo& i_Info, prtyProperty::UndoFlags i_Undoable)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info, i_Undoable);

	// Store local info
	m_Position.SetValue( i_Info.m_Value );
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverPosition::Operate(float i_Time)
{
	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		maPoint3d goal = m_Position.GetValue();
		maPoint3d cur = m_PChannel.GetPosition();
		float percent = this->GetBlendAlpha(m_PChannel.GetPreviousTime(i_Time), i_Time);
		maPoint3d pos = goal*percent + cur*(1.0f - percent);	

		// If smooth blend, add in influence of the gradients
		if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
		{
			AddGradientInfluence(&m_PChannel, i_Time, 
				this->GetBeginTime(), this->GetEndTime(), this->GetEaseInWeight(),
				goal, pos);
		}

		m_PChannel.SetPosition(pos);
	}
	else
	{
		m_PChannel.SetPosition(m_Position.GetValue());
	}
}

//--------------------------------------------------------------------
//	AlterKey - look at the values in the channels to which this 
//	driver is connected and alter the driver in order to match 
//	these values.
//	Returns true if this driver was able to alter its value.
//--------------------------------------------------------------------
bool tmlnDriverPosition::AlterKey()
{
	maPoint3d cur = m_PChannel.GetPosition();
	m_Position.SetValue(cur);
	return true;
}


//--------------------------------------------------------------------
//  This is called when a driver is selected. It lets the driver
//		select one of its icons or whatever else it wants to do.
//--------------------------------------------------------------------
//virtual 
void  tmlnDriverPosition::DoSelect()
{
	// commented out because... when this object is selected, 
	// it is re-selecting the top object
	// and causing the driver clip selection to be cleared.
	//sel3dMgr::Select( pntPointSelect::GetPointObject( &m_Point ) );
}

//--------------------------------------------------------------------
//  Driver should select its 3D icon
//--------------------------------------------------------------------
//virtual 
void  tmlnDriverPosition::DoSelectIcon()
{
	sel3dMgr::CreateUndoOperation();
	sel3dMgr::Select( pntPointSelect::GetPointObject( &m_Point ) );
}

//--------------------------------------------------------------------
//	ShowIcons - show or hide icons that are not part of real scene.
//--------------------------------------------------------------------
void tmlnDriverPosition::ShowIcons( bool i_bVisible )
{
	pntPointMgr::SetRenderable(&m_Point, i_bVisible);
}


//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA tmlnDriverPosition::GetClipFillColor() const
{
	return maFloatRGBA( 0.1412f, 0.4442f, 0.3431f, 1.0f );
}

//--------------------------------------------------------------------
// Callback when point has been changed from manipulator
//--------------------------------------------------------------------
void tmlnDriverPosition::PointChanged(pntPoint *i_pPoint)
{
	m_Position.SetValue( m_Point.GetPosition() );
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverPosition::PositionChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	// Use a threshold for changing the 3D icon in order to prevent infinite loops
	if ( (m_Position.GetValue() - m_Point.GetPosition()).LengthSqr() > maConstants::c_fEpsilon)
	{
		// Alter 3D point icon's position
		m_Point.SetPosition( m_Position.GetValue() );
		pntPointMgr::UpdatePoint(&m_Point);
	}

	this->MarkDirty();
}

//--------------------------------------------------------------------
// Get the value of the driver at its begin time for this channel.
//--------------------------------------------------------------------
void tmlnDriverPosition::GetBeginValue(tmlnChannel* i_pChannel, maVector3d& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_Position.GetValue();
}

//--------------------------------------------------------------------
// Get the value of the driver at its end time for this channel.
//--------------------------------------------------------------------
void tmlnDriverPosition::GetEndValue(tmlnChannel* i_pChannel, maVector3d& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = m_Position.GetValue();
}

//--------------------------------------------------------------------
// Get the value of the gradient of the driver at its
// end time in order to maintain gradient continuity while blending.
//--------------------------------------------------------------------
void tmlnDriverPosition::GetEndGradient(tmlnChannel* i_pChannel, maVector3d& o_Gradient)
{
	this->ComputeKeyGradient(i_pChannel, 
		this->GetBeginTime(), 
		this->GetEndTime(), 
		m_Position.GetValue(), 
		o_Gradient);
}

