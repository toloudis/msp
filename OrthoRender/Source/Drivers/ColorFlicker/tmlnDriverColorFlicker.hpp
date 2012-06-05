/*****************************************************************************
**	tmlnDriverColorFlicker.hpp
**
**		Derived driver class
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERCOLORFLICKER_HPP
#error tmlnDriverColorFlicker.hpp multiply included
#endif
#define TMLN_DRIVERCOLORFLICKER_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef TMLN_BLENDDRIVER_HPP
#include "Support/tmln/tmlnBlendDriver.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif


//============================================================================
//============================================================================
class tmlnChannelColor;
class tmlnDriverColorFlickerInfo;


//============================================================================
//============================================================================
class tmlnDriverColorFlicker : public tmlnDriver,
	public tmlnBlendDriver<maFloatRGBA>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverColorFlicker(tmlnChannelColor &i_Channel, chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~tmlnDriverColorFlicker();

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse hovers over the clip.
	//--------------------------------------------------------------------
	std::string GetHoverDescription();

	//--------------------------------------------------------------------
	//  GetDriverInfo - return data structure representing state of
	//		this driver suitable for writing to a file.
	//	The returned value should be created with "new" and will
	//		be deleted by the caller.
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo*  GetDriverInfo() const;

	//--------------------------------------------------------------------
	// Set internal variables from data structure
	//--------------------------------------------------------------------
	void SetDriverInfo(	const tmlnDriverColorFlickerInfo& i_Info, 
						prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(float i_Time);

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

//============================================================================
//============================================================================

	//--------------------------------------------------------------------
	//	StartColor1
	//--------------------------------------------------------------------
	inline const maFloatRGBA& GetStartColor1() const;
	void SetStartColor1(const maFloatRGBA& i_Val);

	//--------------------------------------------------------------------
	//	StartColor2
	//--------------------------------------------------------------------
	inline const maFloatRGBA& GetStartColor2() const;
	void SetStartColor2(const maFloatRGBA& i_Val);

	//--------------------------------------------------------------------
	//	EndColor1
	//--------------------------------------------------------------------
	inline const maFloatRGBA& GetEndColor1() const;
	void SetEndColor1(const maFloatRGBA& i_Val);

	//--------------------------------------------------------------------
	//	EndColor2
	//--------------------------------------------------------------------
	inline const maFloatRGBA& GetEndColor2() const;
	void SetEndColor2(const maFloatRGBA& i_Val);

	//--------------------------------------------------------------------
	//	Frequency
	//--------------------------------------------------------------------
	inline const float GetFrequency() const;
	void SetFrequency(const float i_fVal);

//============================================================================
//	tmlnBlendDriver interface
//============================================================================

	//--------------------------------------------------------------------
	// Get the value of the driver at its begin time for this channel.
	//--------------------------------------------------------------------
	virtual void GetBeginValue(tmlnChannel* i_pChannel, maFloatRGBA& o_Value);

	//--------------------------------------------------------------------
	// Get the value of the driver at its end time for this channel.
	//--------------------------------------------------------------------
	virtual void GetEndValue(tmlnChannel* i_pChannel, maFloatRGBA& o_Value);

	//--------------------------------------------------------------------
	// Get the value of the gradient of the driver at its
	// end time in order to maintain tangent continuity while blending.
	//--------------------------------------------------------------------
	virtual void GetEndGradient(tmlnChannel* i_pChannel, maFloatRGBA& o_Gradient);

private:
	//--------------------------------------------------------------------
	//	Compte color at given time within BeginTime and EndTime
	//--------------------------------------------------------------------
	maFloatRGBA compute_color(float i_Time);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

private:
	tmlnChannelColor &m_Channel;
	chDefs::Name m_ChunkName;

	//	static information
	prtyColor	m_StartColor1;
	prtyColor	m_StartColor2;
	prtyColor	m_EndColor1;
	prtyColor	m_EndColor2;
	prtyFloat	m_fFrequency;
};

//--------------------------------------------------------------------
//	StartColor1
//--------------------------------------------------------------------
inline 
const maFloatRGBA& tmlnDriverColorFlicker::GetStartColor1() const
{
	return m_StartColor1.GetValue();
}

//--------------------------------------------------------------------
//	StartColor2
//--------------------------------------------------------------------
inline 
const maFloatRGBA& tmlnDriverColorFlicker::GetStartColor2() const
{
	return m_StartColor2.GetValue();
}

//--------------------------------------------------------------------
//	EndColor1
//--------------------------------------------------------------------
inline 
const maFloatRGBA& tmlnDriverColorFlicker::GetEndColor1() const
{
	return m_EndColor1.GetValue();
}

//--------------------------------------------------------------------
//	EndColor2
//--------------------------------------------------------------------
inline 
const maFloatRGBA& tmlnDriverColorFlicker::GetEndColor2() const
{
	return m_EndColor2.GetValue();
}

//--------------------------------------------------------------------
//	Frequency
//--------------------------------------------------------------------
inline 
const float tmlnDriverColorFlicker::GetFrequency() const
{
	return m_fFrequency.GetValue();
}
