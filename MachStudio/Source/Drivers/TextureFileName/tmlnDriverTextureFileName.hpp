/*****************************************************************************
**	tmlnDriverTextureFileName.hpp
**
**		Derived driver class
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_DRIVERTEXTUREFILENAME_HPP
#error tmlnDriverTextureFileName.hpp multiply included
#endif
#define TMLN_DRIVERTEXTUREFILENAME_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif

#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif


//============================================================================
//============================================================================
class tmlnChannelTextureFileName;
class tmlnDriverTextureFileNameInfo;
class tmlnDriverFilePathInfo;

//============================================================================
//============================================================================
class tmlnDriverTextureFileName : public tmlnDriver
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverTextureFileName(tmlnChannelTextureFileName &i_Channel, chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~tmlnDriverTextureFileName();

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse hovers over the clip.
	//--------------------------------------------------------------------
	std::string GetHoverDescription();

	//--------------------------------------------------------------------
	//	Quick Accessors
	//--------------------------------------------------------------------
	inline const prtyTextureFileName& GetValue() const;
	inline void SetValue(const prtyTextureFileName& i_Val);

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
	void SetDriverInfo(	const tmlnDriverTextureFileNameInfo& i_Info );
	void SetDriverInfo(	const tmlnDriverFilePathInfo& i_Info );

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

	tmlnChannelTextureFileName &m_Channel;
	prtyTextureFileName	m_Value;
	chDefs::Name m_ChunkName;

};

//--------------------------------------------------------------------
//	Quick Accessors
//--------------------------------------------------------------------
const prtyTextureFileName& tmlnDriverTextureFileName::GetValue() const
{
	return m_Value;
}
