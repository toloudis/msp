/*****************************************************************************
**	tmlnDriverColorFlicker.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Drivers/ColorFlicker/tmlnDriverColorFlicker.hpp"

#include "Support/tmln/tmlnChannelColor.hpp"
#include "Drivers/ColorFlicker/tmlnDriverColorFlickerInfo.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#include "Core/dbg/dbgLog.hpp"

#include "Core/ma/maFunctions.hpp"
#include "Core/prty/prtyColorRGBAEditUIInfo.hpp"
#include "Core/prty/prtyFloatEditUIInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverColorFlicker::tmlnDriverColorFlicker(tmlnChannelColor &i_Channel, chDefs::Name i_ChunkName)
:	m_Channel(i_Channel), 
	m_ChunkName(i_ChunkName), 
	m_StartColor1("Start Color 1", i_Channel.GetOriginalColor()),
	m_StartColor2("Start Color 2", i_Channel.GetOriginalColor()),
	m_EndColor1("End Color 1", i_Channel.GetOriginalColor()),
	m_EndColor2("End Color 2", i_Channel.GetOriginalColor()),
	m_fFrequency("Frequency (secs)", 1.0f)
{
	this->SetBlendType(tmlnDriver::GetDefaultBlendType());

	// Register the properties so they can be displayed to the user
	//
	prtyPropertyUIInfo* pPUII;
	pPUII = new prtyColorRGBAEditUIInfo(&(m_StartColor1), "Parameters", "Start Color");
	AddProperty( pPUII );
	pPUII = new prtyColorRGBAEditUIInfo(&(m_StartColor2), "Parameters", "Alternate Start Color");
	AddProperty( pPUII );
	pPUII = new prtyColorRGBAEditUIInfo(&(m_EndColor1), "Parameters", "End Color");
	AddProperty( pPUII );
	pPUII = new prtyColorRGBAEditUIInfo(&(m_EndColor2), "Parameters", "Alternate End Color");
	AddProperty( pPUII );
	pPUII = new prtyFloatEditUIInfo(&(m_fFrequency), "Parameters", "Time in seconds for a full cycle.  Start to Start again.");
	AddProperty( pPUII );

	// Register callbacks to update dirty bit when properties change
	m_StartColor1.AddCallback(new prtyCallbackWrapper<tmlnDriverColorFlicker>(this, &tmlnDriverColorFlicker::PropertyChanged));
	m_StartColor2.AddCallback(new prtyCallbackWrapper<tmlnDriverColorFlicker>(this, &tmlnDriverColorFlicker::PropertyChanged));
	m_EndColor1.AddCallback(new prtyCallbackWrapper<tmlnDriverColorFlicker>(this, &tmlnDriverColorFlicker::PropertyChanged));
	m_EndColor2.AddCallback(new prtyCallbackWrapper<tmlnDriverColorFlicker>(this, &tmlnDriverColorFlicker::PropertyChanged));
	m_fFrequency.AddCallback(new prtyCallbackWrapper<tmlnDriverColorFlicker>(this, &tmlnDriverColorFlicker::PropertyChanged));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverColorFlicker::~tmlnDriverColorFlicker()
{
}

//--------------------------------------------------------------------
// Return string for description of driver (include its current state)
// to display in the gui when the mouse hovers over the clip.
//--------------------------------------------------------------------
std::string tmlnDriverColorFlicker::GetHoverDescription()
{
	std::string desc;
	desc = tmlnDriver::GetHoverDescription();

	char buffer[128];
	::sprintf(buffer, "ColorFlicker: %.3f seconds", m_fFrequency.GetValue());

	desc += std::string(buffer);
	return desc;
}

//--------------------------------------------------------------------
//  GetDriverInfo - return data structure representing state of
//		this driver suitable for writing to a file.
//	The returned value should be created with "new" and will
//		be deleted by the caller.
//--------------------------------------------------------------------
tmlnDriverInfo*  tmlnDriverColorFlicker::GetDriverInfo() const
{
	tmlnDriverColorFlickerInfo *pInfo = new tmlnDriverColorFlickerInfo(m_ChunkName);

	this->GetBaseDriverInfo(*pInfo);

	pInfo->m_StartColor1	= this->m_StartColor1.GetValue();
	pInfo->m_StartColor2	= this->m_StartColor2.GetValue();
	pInfo->m_EndColor1		= this->m_EndColor1.GetValue();
	pInfo->m_EndColor2		= this->m_EndColor2.GetValue();
	pInfo->m_fFrequency		= this->m_fFrequency.GetValue();

	return pInfo;
}

//--------------------------------------------------------------------
// Set internal variables from data structure
//--------------------------------------------------------------------
void tmlnDriverColorFlicker::SetDriverInfo(	const tmlnDriverColorFlickerInfo& i_Info, 
											prtyProperty::UndoFlags i_Undoable)
{
	// Set base info
	this->SetBaseDriverInfo(i_Info, i_Undoable);

	// Store local info
	this->m_StartColor1.SetValue(i_Info.m_StartColor1, i_Undoable);
	this->m_StartColor2.SetValue(i_Info.m_StartColor2, i_Undoable);
	this->m_EndColor1.SetValue(i_Info.m_EndColor1, i_Undoable);
	this->m_EndColor2.SetValue(i_Info.m_EndColor2, i_Undoable);
	this->m_fFrequency.SetValue(i_Info.m_fFrequency, i_Undoable);
}

//--------------------------------------------------------------------
//  Update position of things that are being driven
//--------------------------------------------------------------------
void  tmlnDriverColorFlicker::Operate(float i_Time)
{
	// Check and handle blend into driver
	if (IsBefore(i_Time))
	{
		maFloatRGBA goal = m_StartColor1.GetValue();
		maFloatRGBA cur = m_Channel.GetColor();
		float percent = this->GetBlendAlpha( m_Channel.GetPreviousTime(i_Time), i_Time);
		maFloatRGBA color = goal*percent + cur*(1.0f - percent);	

		// If smooth blend, add in influence of the gradients
		if (this->GetBlendType() == tmlnDriver::e_SmoothBlend)
		{
			// We know that our gradient is flat, 
			// so pass it in to the blending function
			maFloatRGBA flat_gradient(0,0,0,0);
			AddGradientInfluence(&m_Channel, i_Time, 
				this->GetBeginTime(), this->GetEndTime(), this->GetEaseInWeight(),
				goal, color, &flat_gradient);
		}
	
		m_Channel.SetColor(color);
	}
	else
	{	// within (or after) driver range
		maFloatRGBA color = this->compute_color(i_Time);
		m_Channel.SetColor(color);
	}
}

//--------------------------------------------------------------------
//	return the fill color to be used for this driver type
//--------------------------------------------------------------------
//virtual
maFloatRGBA tmlnDriverColorFlicker::GetClipFillColor() const
{
	return maFloatRGBA( 0.1111f, 0.1111f, 0.7111f, 1.0f );
}

//--------------------------------------------------------------------
//	StartColor1
//--------------------------------------------------------------------
void tmlnDriverColorFlicker::SetStartColor1(const maFloatRGBA& i_Val)
{
	m_StartColor1.SetValue(i_Val);
	this->MarkDirty();
}

//--------------------------------------------------------------------
//	StartColor2
//--------------------------------------------------------------------
void tmlnDriverColorFlicker::SetStartColor2(const maFloatRGBA& i_Val)
{
	m_StartColor2.SetValue(i_Val);
	this->MarkDirty();
}

//--------------------------------------------------------------------
//	EndColor1
//--------------------------------------------------------------------
void tmlnDriverColorFlicker::SetEndColor1(const maFloatRGBA& i_Val)
{
	m_EndColor1.SetValue(i_Val);
	this->MarkDirty();
}

//--------------------------------------------------------------------
//	EndColor2
//--------------------------------------------------------------------
void tmlnDriverColorFlicker::SetEndColor2(const maFloatRGBA& i_Val)
{
	m_EndColor2.SetValue(i_Val);
	this->MarkDirty();
}

//--------------------------------------------------------------------
//	Frequency
//--------------------------------------------------------------------
void tmlnDriverColorFlicker::SetFrequency(const float i_fVal)
{
	m_fFrequency.SetValue(i_fVal);
	this->MarkDirty();
}


//--------------------------------------------------------------------
// Get the value of the driver at its begin time for this channel.
//--------------------------------------------------------------------
void tmlnDriverColorFlicker::GetBeginValue(tmlnChannel* i_pChannel, maFloatRGBA& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = this->m_StartColor1.GetValue();
}

//--------------------------------------------------------------------
// Get the value of the driver at its end time for this channel.
//--------------------------------------------------------------------
void tmlnDriverColorFlicker::GetEndValue(tmlnChannel* i_pChannel, maFloatRGBA& o_Value)
{
	// Single channel driver can ignore channel pointer
	o_Value = compute_color(this->GetEndTime());
}

//--------------------------------------------------------------------
// Get the value of the gradient of the driver at its
// end time in order to maintain gradient continuity while blending.
//--------------------------------------------------------------------
void tmlnDriverColorFlicker::GetEndGradient(tmlnChannel* i_pChannel, maFloatRGBA& o_Gradient)
{
	// By using ease in/out in the flicker itself, we make it
	// so that the flicker driver has a flat tangent at entry.
	o_Gradient = maFloatRGBA(0,0,0,0);
}

//--------------------------------------------------------------------
//	Compte color at given time within BeginTime and EndTime
//--------------------------------------------------------------------
maFloatRGBA tmlnDriverColorFlicker::compute_color(float i_Time)
{
	float cur_time = (i_Time > this->GetEndTime()) ? this->GetEndTime() : i_Time;

	// Compute where we are in the cycle frequency
	float cycle_pos = ::fmodf(cur_time - this->GetBeginTime(), m_fFrequency.GetValue());
	const float half_frequency = m_fFrequency.GetValue() / 2.0f;
	float percent = 0.0f;
	if (cycle_pos <= half_frequency)
	{
		// State going up
		percent = cycle_pos / half_frequency;
	}
	else
	{
		// State going down
		percent = (m_fFrequency.GetValue() - cycle_pos) / half_frequency;
	}
			
	// Do cubic formula in order to ease in/out through start and end colors
	float cubic_alpha = 3*percent*percent - 2*percent*percent*percent;
	maFunctions::Clamp(cubic_alpha, 0.0f, 1.0f);
	return m_EndColor1.GetValue()*cubic_alpha + m_StartColor1.GetValue()*(1.0f - cubic_alpha);

	//Note: [bga] - this code never uses the "???Color2" colors. I reorganized this
	// code to remove the Active/NotActive functions and to remove the state member
	// variables. It did not use the #2 colors before and I have not added code
	// to use them now.
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tmlnDriverColorFlicker::PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	this->MarkDirty();
}
