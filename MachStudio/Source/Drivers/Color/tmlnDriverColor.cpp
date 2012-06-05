/*****************************************************************************
**	tmlnDriverColor.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Drivers/Color/tmlnDriverColor.hpp"

#include "Support/tmln/tmlnChannelColor.hpp"
#include "Drivers/Color/tmlnDriverColorInfo.hpp"

#include "Core/ma/maFunctions.hpp"
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"

#include <sstream>
//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverColor::tmlnDriverColor(tmlnChannelColor &i_Channel, chDefs::Name i_ChunkName)
:	m_Channel(i_Channel), 
	m_Value("Color", i_Channel.GetColor()), 
	m_ChunkName(i_ChunkName)
{
	this->SetBlendType(tmlnDriver::GetDefaultBlendType());

	//	set the value of the properties
	//
	//m_Value.SetValue(i_Channel.GetColor());

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyColorRGBAEditUIInfo(&(m_Value), "Value", "Color");
	AddProperty( pPUII );

	// Register callbacks to update dirty bit when properties change
	m_Value.AddCallback(new prtyCallbackWrapper<tmlnDriverColor>(this, &tmlnDriverColor::PropertyChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverColor::~tmlnDriverColor()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string tmlnDriverColor::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();

	//char buffer[128];
	//::sprintf(buffer, "Color: %.3f %.3f %.3f", m_Value.GetValue().GetRed(), m_Value.GetValue().GetGreen(), m_Value.GetValue().GetBlue());
	std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss.precision(3);
	oss << "Color: " <<  m_Value.GetValue().GetRed() << " " << m_Value.GetValue().GetGreen() << " " <<  m_Value.GetValue().GetBlue();
	std::string buffer(oss.str());
	desc += buffer;
	return desc;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverColor::SetValue(const maFloatRGBA& i_Val)
{
	m_Value.SetValue(i_Val);
	this->MarkDirty();
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  tmlnDriverColor::GetDriverInfo() const
{
	tmlnDriverColorInfo *pInfo = new tmlnDriverColorInfo(m_ChunkName);

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_Value = this->m_Value.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void tmlnDriverColor::SetDriverInfo(const tmlnDriverColorInfo& i_Info)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info);

	// Store local info
	this->m_Value.SetValue(i_Info.m_Value);
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverColor::Operate(const maTime& i_Time)
{
	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		maFloatRGBA goal = this->m_Value.GetValue();
		maFloatRGBA cur = m_Channel.GetColor();
		float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
		maFloatRGBA color = goal*percent + cur*(1.0f - percent);	

		// If smooth blend, add in influence of the gradients
		if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
		{
			AddGradientInfluence(&m_Channel, i_Time, 
				this->GetBeginTime(), this->GetEndTime(), this->GetEaseInWeight(),
				goal, color);
		}

		// Sometimes the gradient makes the color go above 1 or below 0, so clamp it now
		maFunctions::Clamp(color.m_Red, 0.0f, 1.0f);
		maFunctions::Clamp(color.m_Blue, 0.0f, 1.0f);
		maFunctions::Clamp(color.m_Green, 0.0f, 1.0f);
		maFunctions::Clamp(color.m_Alpha, 0.0f, 1.0f);

		m_Channel.SetColor(color);
	}
	else
	{
		// within driver range
		m_Channel.SetColor(this->m_Value.GetValue());
	}
}


//--------------------------------------------------------------------
//	AlterKey - look at the values in the channels to which this 
//	driver is connected and alter the driver in order to match 
//	these values.
//	Returns true if this driver was able to alter its value.
//--------------------------------------------------------------------
bool tmlnDriverColor::AlterKey()
{
	maFloatRGBA cur = m_Channel.GetColor();

	// Store local info, using undo
	this->CreateUndoForProperty(m_Value);
	const bool bSetDirty = true;
	this->m_Value.SetValue( cur, bSetDirty  );
	return true;
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA tmlnDriverColor::GetClipFillColor() const
{
	return maFloatRGBA( 0.6392f, 1.0f, 0.5412f, 1.0f );
}

//--------------------------------------------------------------------
// Get the value of the driver at its begin time for this channel.
//--------------------------------------------------------------------
void tmlnDriverColor::GetBeginValue(tmlnChannel* i_pChannel, maFloatRGBA& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = this->m_Value.GetValue();
}

//--------------------------------------------------------------------
// Get the value of the driver at its end time for this channel.
//--------------------------------------------------------------------
void tmlnDriverColor::GetEndValue(tmlnChannel* i_pChannel, maFloatRGBA& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = this->m_Value.GetValue();
}

//--------------------------------------------------------------------
// Get the value of the gradient of the driver at its
// end time in order to maintain gradient continuity while blending.
//--------------------------------------------------------------------
void tmlnDriverColor::GetEndGradient(tmlnChannel* i_pChannel, maFloatRGBA& o_Gradient)
{
	this->ComputeKeyGradient(i_pChannel, 
		this->GetBeginTime(), 
		this->GetEndTime(), 
		this->m_Value.GetValue(), 
		o_Gradient);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverColor::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}

