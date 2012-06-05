/*****************************************************************************
**	tmlnDriverFileName.hpp
**
**		Derived driver class
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERFILENAME_HPP
#error tmlnDriverFileName.hpp multiply included
#endif
#define TMLN_DRIVERFILENAME_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif


//============================================================================
//============================================================================
class tmlnChannelFileName;
class tmlnDriverFileNameInfo;


//============================================================================
//============================================================================
class tmlnDriverFileName : public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverFileName(tmlnChannelFileName &i_Channel, chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~tmlnDriverFileName();

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse hovers over the clip.
	//--------------------------------------------------------------------
	std::string GetHoverDescription();

	//--------------------------------------------------------------------
	//	Quick Accessors
	//--------------------------------------------------------------------
	inline const itString& GetValue() const;
	inline void SetValue(const itString& i_Val);

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
	void SetDriverInfo(	const tmlnDriverFileNameInfo& i_Info );

	//--------------------------------------------------------------------
	//  Update position of things that are being driven
	//--------------------------------------------------------------------
	virtual void  Operate(const maTime& i_Time);

	//--------------------------------------------------------------------
	//	AlterKey - look at the values in the channels to which this 
	//	driver is connected and alter the driver in order to match 
	//	these values.
	//	Returns true if this driver was able to alter its value.
	//--------------------------------------------------------------------
	virtual bool AlterKey();

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void PropertyChanged(prtyProperty *i_pProperty, bool i_bDirty);

	tmlnChannelFileName &m_Channel;
	prtyFileName	m_Value;
	chDefs::Name m_ChunkName;

};

//--------------------------------------------------------------------
//	Quick Accessors
//--------------------------------------------------------------------
const itString& tmlnDriverFileName::GetValue() const
{
	return m_Value.GetValue();
}
