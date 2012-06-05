/*****************************************************************************
**	rcdDriverFloat.hpp
**
**		Derived driver class
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef RCD_DRIVERFLOAT_HPP
#error rcdDriverFloat.hpp multiply included
#endif
#define RCD_DRIVERFLOAT_HPP


#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef AN_KEYDATA_HPP
#include "Graphics/an/anKeyData.hpp"
#endif

class tmlnChannelRangedFloat;
class rcdDriverFloatInfo;

class rcdDriverFloat : public tmlnDriver
{

public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	rcdDriverFloat(tmlnChannelRangedFloat &i_Channel, chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~rcdDriverFloat();

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse hovers over the clip.
	//--------------------------------------------------------------------
	std::string GetHoverDescription();

	//--------------------------------------------------------------------
	//	Key Accessors
	//--------------------------------------------------------------------
	float GetValue(float i_Time) const;
	void SetValue(float i_Time, float i_Val);

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
	void SetDriverInfo(const rcdDriverFloatInfo& i_Info, prtyProperty::UndoFlags i_Undoable = prtyProperty::eNoUndo);

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(float i_Time);

	//--------------------------------------------------------------------
	//  Show dialog that allows user to edit this driver's properties
	//--------------------------------------------------------------------
	virtual void  DoEditProperties();

	//--------------------------------------------------------------------
	// Access to channel, in order to get/set ranges
	//--------------------------------------------------------------------
	inline tmlnChannelRangedFloat& Channel();
	inline const tmlnChannelRangedFloat& GetChannel() const;

private:
	tmlnChannelRangedFloat &m_Channel;
	chDefs::Name m_ChunkName;
	anKeyData<float> m_Keys;

};

//--------------------------------------------------------------------
// Access to channel, in order to get/set ranges
//--------------------------------------------------------------------
inline tmlnChannelRangedFloat& rcdDriverFloat::Channel()
{
	return m_Channel;
}
inline const tmlnChannelRangedFloat& rcdDriverFloat::GetChannel() const
{
	return m_Channel;
}
