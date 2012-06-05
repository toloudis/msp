/********************************************************************************************\
**  dynDriverTransformKeyInfo.hpp
**
**	Data structure for parsing sound drivers
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/

#ifdef DYN_DRIVERTRANSFORMKEYINFO_HPP
#error dynDriverTransformKeyInfo.hpp multiply included
#endif
#define DYN_DRIVERTRANSFORMKEYINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif


class dynDriverTransformKeyInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	dynDriverTransformKeyInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~dynDriverTransformKeyInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	maVector3d m_Rotation;
	maVector3d m_Translation;
};

