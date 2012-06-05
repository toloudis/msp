/*****************************************************************************
**	cmraDriverCircle.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverCircle.hpp"

#include "Support/tmln/tmlnChannelPosition.hpp"
#include "Systems/Cameras/Drivers/cmraDriverCircleInfo.hpp"
#include "Systems/Cameras/Drivers/cmraDriverCircleParser.hpp"

#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Core/prty/prtyCheckBoxUIInfo.hpp"
#include "Core/prty/prtyNumericUpDownUIInfo.hpp"

#include <sstream>


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverCircle::cmraDriverCircle(tmlnChannelPosition &i_PChannel,
								   tmlnChannelPosition &i_TChannel)
:	m_PChannel(i_PChannel), 
	m_TChannel(i_TChannel), 
	m_Revolutions("Revolutions", 1.0f),
	m_bClockwise("Clockwise", true),
	m_fStartingYaw("Starting Yaw", 0.0f),
	m_fStartingPitch("Starting Pitch", 45.0f),
	m_fRadius("Distance To Target", 10.0f)
{
	this->SetRestoreOriginalValue(true);

	//	set the value of the properties
	//
	//	Before the begin time
	//
	maVector3d vec = (m_TChannel.GetPosition() - m_PChannel.GetPosition());
	float yaw, pitch;
	maFunctions::GetYawPitch(vec, yaw, pitch);
	m_fStartingYaw.SetValue(yaw * maConstants::c_fRadToAngle);
	m_fStartingPitch.SetValue(pitch * maConstants::c_fRadToAngle);
	m_fRadius.SetValue(vec.Length());

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyCheckBoxUIInfo(&(m_bClockwise), "Circle Settings", "Clockwise");
	AddProperty( pPUII );
	prtyNumericUpDownUIInfo* pNUDUII;
	pNUDUII = new prtyNumericUpDownUIInfo(&(m_Revolutions), "Circle Settings", "Revolutions");
	pNUDUII->SetMinimum(0.0f);
	pNUDUII->SetIncrement(0.1f);
	pNUDUII->SetDecimalPlaces(2);
	AddProperty( pNUDUII );
	pPUII = new prtyNumericUpDownUIInfo(&(m_fStartingYaw), "Initial Settings", "Yaw");
	AddProperty( pPUII );
	pPUII = new prtyNumericUpDownUIInfo(&(m_fStartingPitch), "Initial Settings", "Pitch");
	AddProperty( pPUII );
	pPUII = new prtyNumericUpDownUIInfo(&(m_fRadius), "Initial Settings", "Radius");
	AddProperty( pPUII );

	// Register callbacks to update dirty bit when properties change
	m_bClockwise.AddCallback(new prtyCallbackWrapper<cmraDriverCircle>(this, &cmraDriverCircle::PropertyChanged));
	m_Revolutions.AddCallback(new prtyCallbackWrapper<cmraDriverCircle>(this, &cmraDriverCircle::PropertyChanged));
	m_fStartingYaw.AddCallback(new prtyCallbackWrapper<cmraDriverCircle>(this, &cmraDriverCircle::PropertyChanged));
	m_fStartingPitch.AddCallback(new prtyCallbackWrapper<cmraDriverCircle>(this, &cmraDriverCircle::PropertyChanged));
	m_fRadius.AddCallback(new prtyCallbackWrapper<cmraDriverCircle>(this, &cmraDriverCircle::PropertyChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverCircle::~cmraDriverCircle()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string cmraDriverCircle::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();

	//char buffer[128];
	std::string buffer;
	bool cwise = m_bClockwise.GetValue();
	//::sprintf(buffer, "Revolutions: %4.2f Clockwise = %s", m_Revolutions.GetValue(), (cwise?"true":"false"));//Clockwise: %s", m_Revolutions.GetValue(), (cwise?"true":"false") );

	std::ostringstream oss(std::ostringstream::out);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss.precision(3);
	oss.width(4);
	oss << " Revolutions: "<< m_Revolutions.GetValue() <<" Clockwise = "<< (cwise?"true":"false") ;
	buffer = oss.str();
	desc += std::string(buffer);

	return desc;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverCircle::SetClockwise(bool i_Val)
{
	m_bClockwise.SetValue(i_Val);
	this->MarkDirty();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverCircle::SetRevolutions(float i_Val)
{
	m_Revolutions.SetValue(i_Val);
	this->MarkDirty();
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  cmraDriverCircle::GetDriverInfo() const
{
	cmraDriverCircleInfo *pInfo = new cmraDriverCircleInfo(cmraDriverCircleParser::GetChunkName());

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_bClockwise = this->m_bClockwise.GetValue();
	pInfo->m_Revolutions = this->m_Revolutions.GetValue();
	pInfo->m_fStartingYaw = this->m_fStartingYaw.GetValue();
	pInfo->m_fStartingPitch = this->m_fStartingPitch.GetValue();
	pInfo->m_fRadius = this->m_fRadius.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void cmraDriverCircle::SetDriverInfo(const cmraDriverCircleInfo& i_Info)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info);

	// Store local info
	this->m_bClockwise.SetValue(i_Info.m_bClockwise);
	this->m_Revolutions.SetValue(i_Info.m_Revolutions);
	this->m_fStartingYaw.SetValue(i_Info.m_fStartingYaw);
	this->m_fStartingPitch.SetValue(i_Info.m_fStartingPitch);
	this->m_fRadius.SetValue(i_Info.m_fRadius);
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  cmraDriverCircle::Operate(const maTime& i_fTime)
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
bool cmraDriverCircle::AlterKey()
{
	//maVector3d cur = m_PChannel.GetState();

	// Store local info, using undo
	//this->m_bClockwise.SetValue( cur, bSetDirty  );

	return true;
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA cmraDriverCircle::GetClipFillColor() const
{
	return maFloatRGBA( 0.1f, 0.4f, 0.7f, 1.0f );
}

//--------------------------------------------------------------------
//	Clone - Clone this driver and return a new instace
//--------------------------------------------------------------------
//virtual 
tmlnDriver* cmraDriverCircle::Clone()
{
	return new cmraDriverCircle(*this);
}

//------------------------------------------------------------------------
//	calculate the position, pitch, etc of the camera.
//------------------------------------------------------------------------
void cmraDriverCircle::Calculate_CameraLocation(const maTime& i_fTime)
{
	maVector3d position, target;
	target = m_TChannel.GetPosition();

	float yaw = 0.0f;
	float pitch;
	float percent = (i_fTime - this->GetBeginTime()) / this->GetDuration();
	if (percent > 1.0f)
		percent = 1.0f;

	yaw = (360.0f * maConstants::c_fAngleToRad) * (this->m_Revolutions.GetValue() * percent);
	yaw += m_fStartingYaw.GetValue() * maConstants::c_fAngleToRad;
	pitch = m_fStartingPitch.GetValue() * maConstants::c_fAngleToRad;

	//DBG_LOG4("starting yaw/calc %6.3f - %6.3f    pitch %6.3f - %6.3f", m_fStartingYaw.GetValue(), yaw, m_fStartingPitch.GetValue(), pitch );

	float x = float(sin(yaw) * cos(pitch));
	float y = float(sin(pitch));
	float z = float(cos(yaw) * cos(pitch));

	maVector3d dir(-x, -y, -z);

	position = target - dir * m_fRadius.GetValue();

	//DBG_LOG5( "%4.1f campos( %6.3f, %6.3f, %6.3f ) targetyaw(%6.3f)", percent, m_Position.GetX(), m_Position.GetY(), m_Position.GetZ(), m_fTargetYawAngle );

	m_PChannel.SetPosition( position );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverCircle::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}
