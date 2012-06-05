/*****************************************************************************
**	dynDriverTransformKey.hpp
**
**		Derived driver class which sets a orientation keyframe
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef DYN_DRIVERTRANSFORMKEY_HPP
#error dynDriverTransformKey.hpp multiply included
#endif
#define DYN_DRIVERTRANSFORMKEY_HPP

#ifndef TMLN_DRIVER_HPP
#include "Support/tmln/tmlnDriver.hpp"
#endif
#ifndef TMLN_BLENDDRIVER_HPP
#include "Support/tmln/tmlnBlendDriver.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif

//============================================================================
//============================================================================
class dynChannelControl;
class dynDriverTransformKeyInfo;

//============================================================================
// struct to represent the unit of info being blended in smooth blends
//============================================================================
struct sTransformKeyUnit
{
	sTransformKeyUnit();
	sTransformKeyUnit(const maVector3d& i_Rotation, 
					  const maVector3d& i_Translation);

	void operator += ( const sTransformKeyUnit& i_A );
	sTransformKeyUnit operator - ( const sTransformKeyUnit& i_A ) const;
	sTransformKeyUnit operator * ( float i_S ) const;


	maVector3d m_Rotation;	// Euler angles in degrees
	maVector3d m_Translation;
};

//============================================================================
//============================================================================
class dynDriverTransformKey : public tmlnDriver,
	public tmlnBlendDriver<sTransformKeyUnit>
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	dynDriverTransformKey(dynChannelControl &i_Channel, 
						  chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~dynDriverTransformKey();

	//--------------------------------------------------------------------
	// Return string for description of driver (include its current state)
	// to display in the gui when the mouse hovers over the clip.
	//--------------------------------------------------------------------
	std::string GetHoverDescription();

	//--------------------------------------------------------------------
	//	Quick Accessors
	//--------------------------------------------------------------------
	inline const maVector3d& GetRotation() const;
	void SetRotation(const maVector3d& i_Val);
	inline const maVector3d& GetTranslation() const;
	void SetTranslation(const maVector3d& i_Val);

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
	void SetDriverInfo(const dynDriverTransformKeyInfo& i_Info );

	//--------------------------------------------------------------------
	//  Update orientation of things that are being driven
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
	//  Show dialog that allows user to edit this driver's properties
	//--------------------------------------------------------------------
	//virtual void  DoEditProperties();

	//--------------------------------------------------------------------
	//	return the fill color to be used for this driver type
	//--------------------------------------------------------------------
	virtual maFloatRGBA GetClipFillColor() const;

//============================================================================
//	tmlnBlendDriver interface
//============================================================================

	//--------------------------------------------------------------------
	// Get the value of the driver at its begin time for this channel.
	//--------------------------------------------------------------------
	virtual void GetBeginValue(tmlnChannel* i_pChannel, sTransformKeyUnit& o_Value);

	//--------------------------------------------------------------------
	// Get the value of the driver at its end time for this channel.
	//--------------------------------------------------------------------
	virtual void GetEndValue(tmlnChannel* i_pChannel, sTransformKeyUnit& o_Value);

	//--------------------------------------------------------------------
	// Get the value of the gradient of the driver at its
	// end time in order to maintain tangent continuity while blending.
	//--------------------------------------------------------------------
	virtual void GetEndGradient(tmlnChannel* i_pChannel, sTransformKeyUnit& o_Gradient);

private:
	dynChannelControl &m_Channel;
	maVector3d m_Rotation;	// Euler angles in degrees
	maVector3d m_Translation;
	chDefs::Name m_ChunkName;

};

//--------------------------------------------------------------------
//	Quick Accessors
//--------------------------------------------------------------------
const maVector3d& dynDriverTransformKey::GetRotation() const
{
	return m_Rotation;
}
inline const maVector3d& dynDriverTransformKey::GetTranslation() const
{
	return m_Translation;
}
