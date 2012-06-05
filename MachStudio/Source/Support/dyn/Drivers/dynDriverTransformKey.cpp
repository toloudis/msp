/*****************************************************************************
**	dynDriverTransformKey.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/dyn/Drivers/dynDriverTransformKey.hpp"

#include "Support/dyn/dynChannelControl.hpp"
#include "Support/dyn/Drivers/dynDriverTransformKeyInfo.hpp"
//#include "Support/dyn/Drivers/dynDriverTransformKeyForm.h"	// GUI

#include "Core/Ma/maConstants.hpp"

#include <sstream>


//--------------------------------------------------------------------
// Operators for blending the transform key unit
//--------------------------------------------------------------------
sTransformKeyUnit::sTransformKeyUnit()
: m_Rotation(0,0,0), m_Translation(0,0,0)
{
}
sTransformKeyUnit::sTransformKeyUnit(const maVector3d& i_Rotation, 
									 const maVector3d& i_Translation)
: m_Rotation(i_Rotation), m_Translation(i_Translation)
{
}
void sTransformKeyUnit::operator += ( const sTransformKeyUnit& i_A )
{
	m_Rotation += i_A.m_Rotation;
	m_Translation += i_A.m_Translation;
}
sTransformKeyUnit sTransformKeyUnit::operator - ( const sTransformKeyUnit& i_A ) const
{
	return sTransformKeyUnit(m_Rotation - i_A.m_Rotation, m_Translation - i_A.m_Translation);
}
sTransformKeyUnit sTransformKeyUnit::operator * ( float i_S ) const
{
	return sTransformKeyUnit(m_Rotation * i_S, m_Translation * i_S);
}



//--------------------------------------------------------------------
//--------------------------------------------------------------------
dynDriverTransformKey::dynDriverTransformKey(dynChannelControl &i_Channel, 
											 chDefs::Name i_ChunkName)
:	m_Channel(i_Channel), 
	m_Rotation(i_Channel.GetEulerAngles()), 
	m_Translation(i_Channel.GetTranslation()), 
	m_ChunkName(i_ChunkName)
{
	this->SetBlendType(tmlnDriver::GetDefaultBlendType());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
dynDriverTransformKey::~dynDriverTransformKey()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string dynDriverTransformKey::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();

	//char buffer[256];
	//::sprintf(buffer, "Translation %.1f %.1f %.1f \nOrientation: %.0f %.0f %.0f", 
	//	m_Translation.m_X, m_Translation.m_Y, m_Translation.m_Z, 
	//	m_Rotation.m_X, m_Rotation.m_Y, m_Rotation.m_Z);

	std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss.precision(1);
	oss << "Translation "<<m_Translation.m_X << " "<<m_Translation.m_Y<<" "<<m_Translation.m_Z<<oss.precision(0)<<"\nOrientation: "<<m_Rotation.m_X<<" "<<m_Rotation.m_Y<<" "<<m_Rotation.m_Z;
	std::string buffer(oss.str());
	desc += std::string(buffer.c_str());
	return desc;
}

//--------------------------------------------------------------------
//	Quick Accessors
//--------------------------------------------------------------------
void dynDriverTransformKey::SetRotation(const maVector3d& i_Val)
{
	m_Rotation = i_Val;
	this->MarkDirty();
}
void dynDriverTransformKey::SetTranslation(const maVector3d& i_Val)
{
	m_Translation = i_Val;
	this->MarkDirty();
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  dynDriverTransformKey::GetDriverInfo() const
{
	dynDriverTransformKeyInfo *pInfo = new dynDriverTransformKeyInfo(m_ChunkName);

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_Name = this->m_Channel.GetName();

	pInfo->m_Rotation = this->m_Rotation;
	pInfo->m_Translation = this->m_Translation;

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void dynDriverTransformKey::SetDriverInfo(const dynDriverTransformKeyInfo& i_Info )
{
	// Set base info
	this->SetBaseDriverInfo(i_Info);

	// Store local info
	this->m_Rotation = i_Info.m_Rotation;
	this->m_Translation = i_Info.m_Translation;
}

//--------------------------------------------------------------------
//  Update orientation of things that are being driven
//--------------------------------------------------------------------
void  dynDriverTransformKey::Operate(const maTime& i_Time)
{

	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		// convert Euler degree angles to quaternion
		maRotation driver_rot(m_Rotation.m_X * maConstants::c_fAngleToRad, 
					m_Rotation.m_Y * maConstants::c_fAngleToRad, 
					m_Rotation.m_Z * maConstants::c_fAngleToRad);

		// Slerp rotation
		maRotation cur = m_Channel.GetRotation();
		float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
		//maRotation ori = goal*percent + cur*(1.0f - percent);	// linear blend
		maRotation ori;
		ori.Slerp(cur, driver_rot, percent);
		m_Channel.SetRotation(ori, true);	// set value as quaternion

		// Interpolate translation
		maPoint3d pos = this->m_Translation*percent + m_Channel.GetTranslation()*(1.0f - percent);	// linear blend

		// If smooth blend, add in influence of the gradients
		if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
		{
			sTransformKeyUnit sum, goal(m_Rotation, m_Translation);
			AddGradientInfluence(&m_Channel, i_Time, 
				this->GetBeginTime(), this->GetEndTime(), this->GetEaseInWeight(),
				goal, sum);
			pos += sum.m_Translation;

			// We might be able to add in the rotation from here also,
			// but orientation doesn't blend in other drivers, so need
			// to really do this with quaternions?
		}

		m_Channel.SetTranslation(pos, true);
	}
	else 
	{
		// within driver range
		m_Channel.SetTranslation(this->m_Translation, true);
		m_Channel.SetEulerAngles(m_Rotation, true);	// set value as angles
	}
}

//--------------------------------------------------------------------
//	AlterKey - look at the values in the channels to which this 
//	driver is connected and alter the driver in order to match 
//	these values.
//	Returns true if this driver was able to alter its value.
//--------------------------------------------------------------------
bool dynDriverTransformKey::AlterKey()
{
	//TODO - needs undo operation
	m_Rotation = m_Channel.GetEulerAngles();
	m_Translation = m_Channel.GetTranslation();

	return true;
}


//--------------------------------------------------------------------
//  Show dialog that allows user to edit this driver's properties
//--------------------------------------------------------------------
//void  dynDriverTransformKey::DoEditProperties()
//{

//}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA dynDriverTransformKey::GetClipFillColor() const
{
	return maFloatRGBA( 0.1412f, 0.8f, 0.8431f, 1.0f );
}

//--------------------------------------------------------------------
// Get the value of the driver at its begin time for this channel.
//--------------------------------------------------------------------
void dynDriverTransformKey::GetBeginValue(tmlnChannel* i_pChannel, sTransformKeyUnit& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value.m_Rotation = m_Rotation;
	o_Value.m_Translation = m_Translation;
}

//--------------------------------------------------------------------
// Get the value of the driver at its end time for this channel.
//--------------------------------------------------------------------
void dynDriverTransformKey::GetEndValue(tmlnChannel* i_pChannel, sTransformKeyUnit& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value.m_Rotation = m_Rotation;
	o_Value.m_Translation = m_Translation;
}

//--------------------------------------------------------------------
// Get the value of the gradient of the driver at its
// end time in order to maintain gradient continuity while blending.
//--------------------------------------------------------------------
void dynDriverTransformKey::GetEndGradient(tmlnChannel* i_pChannel, sTransformKeyUnit& o_Gradient)
{
	sTransformKeyUnit value(m_Rotation, m_Translation);
	this->ComputeKeyGradient(i_pChannel, 
		this->GetBeginTime(), 
		this->GetEndTime(), 
		value, 
		o_Gradient);
}

