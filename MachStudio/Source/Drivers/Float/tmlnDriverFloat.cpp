/*****************************************************************************
**	tmlnDriverFloat.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Drivers/Float/tmlnDriverFloat.hpp"

#include "Support/tmln/tmlnChannelFloat.hpp"
//#include "Support/tmln/tmlnChannelRangedFloat.hpp"
#include "Drivers/Float/tmlnDriverFloatInfo.hpp"

#include "Core/prty/prtyFloatEditUIInfo.hpp"
#include "Core/prty/prtyRangedFloatUIInfo.hpp"
#include "Core/prty/prtyUnits.hpp"

#include <sstream>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverFloat::tmlnDriverFloat(tmlnChannelFloat &i_Channel, chDefs::Name i_ChunkName)
:	m_Channel(i_Channel), 
	m_Value("Value",i_Channel.GetValue()), 
	m_ChunkName(i_ChunkName)
{
	this->SetBlendType(tmlnDriver::GetDefaultBlendType());

	m_Value.SetUseUnits( i_Channel.GetUseUnits() );

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;

	//tmlnChannelRangedFloat* pRFChnl = dynamic_cast<tmlnChannelRangedFloat*>(&i_Channel);
	//if (pRFChnl != 0)
	//{
	//	prtyRangedFloatUIInfo* pRFUII;
	//	pRFUII = new prtyRangedFloatUIInfo(&(m_Value), "Data", "Value");
	//	pRFUII->SetMinimum(pRFChnl->GetMinValue());
	//	pRFUII->SetMaximum(pRFChnl->GetMaxValue());
	//	pRFUII->SetNumTicks(100);
	//	pPUII = pRFUII;
	//}
	//else
	{
		pPUII = new prtyFloatEditUIInfo(&(m_Value), "Data", "Value");
	}

	AddProperty( pPUII );

	// Register callbacks to update dirty bit when properties change
	m_Value.AddCallback(new prtyCallbackWrapper<tmlnDriverFloat>(this, &tmlnDriverFloat::PropertyChanged));

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverFloat::~tmlnDriverFloat()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string tmlnDriverFloat::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();
	std::string str = m_Channel.GetName();
	str += " : ";
	
	float unit_scale = 1.0f;
	if( m_Value.GetUseUnits() )
		unit_scale = prtyUnits::GetUnitScaling();
	
	//char buffer[64];
	//::sprintf(buffer, "%5.2f", (m_Value.GetValue() / unit_scale));
	std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss.width(5);
	oss.precision(2);
	oss << (m_Value.GetValue() / unit_scale);
	std::string buffer(oss.str());
	str += buffer;
	desc += str;
	return desc;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverFloat::SetValue(float i_Val)
{
	this->MarkDirty();
	m_Value.SetValue(i_Val);
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  tmlnDriverFloat::GetDriverInfo() const
{
	tmlnDriverFloatInfo *pInfo = new tmlnDriverFloatInfo(m_ChunkName);

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_Value = this->m_Value.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void tmlnDriverFloat::SetDriverInfo(const tmlnDriverFloatInfo& i_Info )
{
	// Set base info
	this->SetBaseDriverInfo(i_Info);

	// Store local info
	this->m_Value.SetValue( i_Info.m_Value );
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverFloat::Operate(const maTime& i_Time)
{
	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		float goal = this->m_Value.GetValue();
		float cur = m_Channel.GetValue();
		float percent = this->GetBlendAlpha(m_Channel.GetPreviousTime(i_Time), i_Time);
		float value = goal*percent + cur*(1.0f - percent);	// linear blend

		// If smooth blend, add in influence of the gradients
		if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
		{
			AddGradientInfluence(&m_Channel, i_Time, 
				this->GetBeginTime(), this->GetEndTime(), this->GetEaseInWeight(),
				goal, value);
		}

		m_Channel.SetValue(value);
	}
	else
	{
		// within driver range
		m_Channel.SetValue(this->m_Value.GetValue());
	}
}

//--------------------------------------------------------------------
//	AlterKey - look at the values in the channels to which this 
//	driver is connected and alter the driver in order to match 
//	these values.
//	Returns true if this driver was able to alter its value.
//--------------------------------------------------------------------
bool tmlnDriverFloat::AlterKey()
{
	float cur = m_Channel.GetValue();

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
maFloatRGBA tmlnDriverFloat::GetClipFillColor() const
{
	return maFloatRGBA( 1.0f, 0.9215f, 0.8431f, 1.0f );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverFloat::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}

//--------------------------------------------------------------------
// Get the value of the driver at its begin time for this channel.
//--------------------------------------------------------------------
void tmlnDriverFloat::GetBeginValue(tmlnChannel* i_pChannel, float& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = this->m_Value.GetValue();
}

//--------------------------------------------------------------------
// Get the value of the driver at its end time for this channel.
//--------------------------------------------------------------------
void tmlnDriverFloat::GetEndValue(tmlnChannel* i_pChannel, float& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = this->m_Value.GetValue();
}

//--------------------------------------------------------------------
// Get the value of the gradient of the driver at its
// end time in order to maintain gradient continuity while blending.
//--------------------------------------------------------------------
void tmlnDriverFloat::GetEndGradient(tmlnChannel* i_pChannel, float& o_Gradient)
{
	this->ComputeKeyGradient(i_pChannel,  
		this->GetBeginTime(), 
		this->GetEndTime(), 
		this->m_Value.GetValue(), 
		o_Gradient);
}

