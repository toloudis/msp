/****************************************************************************\
**	chnlChannelClip.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/wxGUI/chnlChannelClip.hpp"

#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Core/ma/maFunctions.hpp"

#include <sstream>

#ifdef USE_WXWIDGETS


//============================================================================
//============================================================================
namespace
{
	const wxColour c_AntiqueWhite(250,235,215);
	const wxColour c_Yellow(255,255,0);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chnlChannelClip::chnlChannelClip()
:	m_Name("Clip"),
	m_BeginTime(maTime::c_ZeroTime),
	m_EndTime(maTime::FromSeconds(10)),
	m_Blend(e_NoBlend),
	m_BlendTime(maTime::c_ZeroTime),
	m_bRestore(false),
	m_bUseHighlightFill(false),
	m_StandardFillColor(c_AntiqueWhite),
	m_HighlightFillColor(c_Yellow),
	m_InteractStart(maTime::c_ZeroTime),
	m_InteractDuration(maTime::FromSeconds(10)),
	m_InteractRoot(0),
	m_pChangedCallback(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chnlChannelClip::chnlChannelClip(const std::string& i_Name, 
								 const maTime& i_BeginTime, 
								 const maTime& i_EndTime,
								 BlendType i_Blend, 
								 const maTime& i_BlendTime, 
								 bool i_Restore)
:	m_Name(i_Name),
	m_BeginTime(i_BeginTime),
	m_EndTime(i_EndTime),
	m_Blend(i_Blend),
	m_BlendTime(i_BlendTime),
	m_bRestore(i_Restore),
	m_bUseHighlightFill(false),
	m_StandardFillColor(c_AntiqueWhite),
	m_HighlightFillColor(c_Yellow),
	m_InteractStart(i_BeginTime),
	m_InteractDuration(i_EndTime-i_BeginTime),
	m_InteractRoot(0),
	m_pChangedCallback(NULL)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlChannelClip::chnlChannelClip(const chnlChannelClip& i_Clip)
{
	m_Name = i_Clip.m_Name;
	m_Category = i_Clip.m_Category;
	m_BeginTime = i_Clip.m_BeginTime;
	m_EndTime = i_Clip.m_EndTime;
	m_Blend = i_Clip.m_Blend;
	m_BlendTime = i_Clip.m_BlendTime;
	m_bRestore = i_Clip.m_bRestore;
	m_bUseHighlightFill = i_Clip.m_bUseHighlightFill;
	m_StandardFillColor = i_Clip.m_StandardFillColor;
	m_HighlightFillColor = i_Clip.m_HighlightFillColor;
}

//--------------------------------------------------------------------
// Set callback pointer for when clip changes
//--------------------------------------------------------------------
void chnlChannelClip::SetChangedCallback(ChangedCallback* i_pCallback)
{
	m_pChangedCallback = i_pCallback;
}

//--------------------------------------------------------------------
// Name
//--------------------------------------------------------------------
void chnlChannelClip::SetName(const std::string& i_Name)
{
	m_Name = i_Name;
}
const std::string& chnlChannelClip::GetName() const
{
	return m_Name;
}

//--------------------------------------------------------------------
// Category
//--------------------------------------------------------------------
void chnlChannelClip::SetCategory(const std::string& i_Category)
{
	m_Category = i_Category;
}
const std::string& chnlChannelClip::GetCategory() const
{
	return m_Category;
}

//--------------------------------------------------------------------
// BeginTime
//--------------------------------------------------------------------
void chnlChannelClip::SetBeginTime(const maTime& i_BeginTime)
{
	m_BeginTime = i_BeginTime;
	m_InteractStart = m_BeginTime;
	notify();
}
maTime chnlChannelClip::GetBeginTime() const
{
	return m_BeginTime;
}

//--------------------------------------------------------------------
// EndTime
//--------------------------------------------------------------------
void chnlChannelClip::SetEndTime(const maTime& i_EndTime)
{
	m_EndTime = i_EndTime;
	m_InteractDuration = m_EndTime - m_BeginTime;
	notify();
}
maTime chnlChannelClip::GetEndTime() const
{
	return m_EndTime;
}

//--------------------------------------------------------------------
// Duration - end time minus begin time
//--------------------------------------------------------------------
maTime chnlChannelClip::GetDuration() const
{
	return m_EndTime - m_BeginTime;
}

//--------------------------------------------------------------------
// Blend
//--------------------------------------------------------------------
void chnlChannelClip::SetBlendType(BlendType i_Blend)
{
	m_Blend = i_Blend;
	notify();
}
chnlChannelClip::BlendType chnlChannelClip::GetBlendType() const
{
	return m_Blend;
}

//--------------------------------------------------------------------
// BlendTime
//--------------------------------------------------------------------
void chnlChannelClip::SetBlendTime(const maTime& i_BlendTime)
{
	m_BlendTime = i_BlendTime;
	notify();
}
maTime chnlChannelClip::GetBlendTime() const
{
	return m_BlendTime;
}

//--------------------------------------------------------------------
// Restore
//--------------------------------------------------------------------
bool chnlChannelClip::GetRestore() const
{
	return m_bRestore;
}

//--------------------------------------------------------------------
// FillColor - return either standard of highlight fill color,
//	depending on settings
//--------------------------------------------------------------------
wxColour chnlChannelClip::GetFillColor() const
{
	return (m_bUseHighlightFill ? m_HighlightFillColor : m_StandardFillColor);
}

//--------------------------------------------------------------------
// UseHighlightFill - use either standard of highlight fill color
//--------------------------------------------------------------------
void chnlChannelClip::SetUseHighlightFill(bool i_bUseHighlight)
{
	m_bUseHighlightFill = i_bUseHighlight;
}
bool chnlChannelClip::GetUseHighlightFill()const
{
	return m_bUseHighlightFill;
}

//--------------------------------------------------------------------
// StandardFillColor
//--------------------------------------------------------------------
void chnlChannelClip::SetStandardFillColor(const wxColour& i_Color)
{
	m_StandardFillColor = i_Color;
}
const wxColour& chnlChannelClip::GetStandardFillColor() const
{
	return m_StandardFillColor;
}

//--------------------------------------------------------------------
// HighlightFillColor
//--------------------------------------------------------------------
void chnlChannelClip::SetHighlightFillColor(const wxColour& i_Color)
{
	m_HighlightFillColor = i_Color;
}
const wxColour& chnlChannelClip::GetHighlightFillColor() const
{
	return m_HighlightFillColor;
}

//--------------------------------------------------------------------
// Interaction values - potential new position for the clip
//	while interacting with the mouse
//--------------------------------------------------------------------
void chnlChannelClip::SetInteraction(const maTime& i_InteractStart, const maTime& i_InteractDuration)
{
	m_InteractStart = i_InteractStart;
	m_InteractDuration = i_InteractDuration;
}
maTime chnlChannelClip::GetInteractStart() const
{
	return m_InteractStart;
}
maTime chnlChannelClip::GetInteractDuration() const
{
	return m_InteractDuration;
}
maTime chnlChannelClip::GetInteractEnd() const
{
	return m_InteractStart+m_InteractDuration;
}
int chnlChannelClip::GetInteractRoot() const
{
	return m_InteractRoot;
}

//--------------------------------------------------------------------
// While moving the clips, the begin and end times may be snapped
//	to important times. The snapping is always done by altering the
//	InteractStart value, but you can align either the begin or end.
//--------------------------------------------------------------------
void chnlChannelClip::AlignInteractStart(const maTime& i_Time)
{
	m_InteractStart = i_Time;
}
void chnlChannelClip::AlignInteractEnd(const maTime& i_Time)
{
	m_InteractStart = i_Time - m_InteractDuration;

	// Is this check necessary?
	if (m_InteractStart < maTime::c_ZeroTime)
		m_InteractStart = maTime::c_ZeroTime;
}

//============================================================================
//	Virtual functions to be overriden
//============================================================================

//--------------------------------------------------------------------
// Set begin and end times using duration
//--------------------------------------------------------------------
//virtual
void chnlChannelClip::SetTime(const maTime& i_BeginTime, const maTime& i_Duration)
{
	m_BeginTime = i_BeginTime;
	m_EndTime = i_BeginTime + i_Duration;
	m_InteractStart = i_BeginTime;
	m_InteractDuration = i_Duration;
	notify();
}

//--------------------------------------------------------------------
// Set blend attributes
//--------------------------------------------------------------------
//virtual
void chnlChannelClip::SetBlend(BlendType i_Blend, const maTime& i_BlendTime)
{
	m_Blend = i_Blend;
	m_BlendTime = i_BlendTime;
	notify();
}

//--------------------------------------------------------------------
// Set standard filll color
//--------------------------------------------------------------------
//virtual
void chnlChannelClip::SetFillColor( float i_R, float i_G, float i_B )
{
	int red = (int)(i_R * 255.0f);
	maFunctions::Clamp(red, 0, 255);
	int green = (int)(i_G * 255.0f);
	maFunctions::Clamp(green, 0, 255);
	int blue = (int)(i_B * 255.0f);
	maFunctions::Clamp(blue, 0, 255);
	m_StandardFillColor.Set(red, green, blue);
}

//--------------------------------------------------------------------
// Set the restore flag
//--------------------------------------------------------------------
//virtual
void chnlChannelClip::SetRestore(bool i_bRestore)
{
	m_bRestore = i_bRestore;
	notify();
}

//--------------------------------------------------------------------
// display properties dialog for this clip
//--------------------------------------------------------------------
//virtual
void chnlChannelClip::ShowProperties()
{
}

//--------------------------------------------------------------------
// allow driver to handle selection in its own way
//--------------------------------------------------------------------
//virtual
void chnlChannelClip::Select()
{
}

//--------------------------------------------------------------------
// select the 3D icon for this clip
//--------------------------------------------------------------------
//virtual
void chnlChannelClip::SelectIcon()
{
}

//--------------------------------------------------------------------
// Return string to display when mouse hovers over control
//--------------------------------------------------------------------
//virtual
std::string chnlChannelClip::GetHoverDescription()
{
	return m_Name;
}

//--------------------------------------------------------------------
// Return string to display when mouse is interacting 
//		(moving, resizing) over control
//--------------------------------------------------------------------
//virtual
std::string chnlChannelClip::GetInteractionDescription( const maTime& i_InteractStart, 
														const maTime& i_InteractDuration)
{
	std::ostringstream str;
	str.precision(5);
	std::string int_start_str, int_dur_str;
	tmlnTimeUtil::GetTimeString(i_InteractStart, int_start_str);
	tmlnTimeUtil::GetTimeString(i_InteractDuration, int_dur_str);
	str << "Start " << int_start_str << " Duration " << int_dur_str;
	return str.str();
}

//--------------------------------------------------------------------
// Notify callback
//--------------------------------------------------------------------
void chnlChannelClip::notify()
{
	if (m_pChangedCallback)
		m_pChangedCallback->ClipChanged(this);
}


#endif // USE_WXWIDGETS