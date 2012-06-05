/********************************************************************************************\
**  tmlnDriverFileNameInfo.hpp
**
**	Data structure for parsing sound drivers
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef TMLN_DRIVERFILENAMEINFO_HPP
#error tmlnDriverFileNameInfo.hpp multiply included
#endif
#define TMLN_DRIVERFILENAMEINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


class tmlnDriverFileNameInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverFileNameInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriverFileNameInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();


	itString m_Value;

};

