/********************************************************************************************\
**  tmlnBaseData.hpp
**
**		Universal base data 
**
**  TODO: phase out this class/file
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#ifdef TMLN_BASEDATA_HPP
#error tmlnBaseData.hpp multiply included
#endif
#define TMLN_BASEDATA_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif

#ifndef MNM_BASEDATA_HPP
#include "Support/mnm/mnmBaseData.hpp"
#endif

#include <vector>


//
//
class tmlnBaseData
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	tmlnBaseData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	bool operator == (const tmlnBaseData& i_Item);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	tmlnBaseData& operator = (const tmlnBaseData& i_Data);

	//------------------------------------------------------------------------
	// FIX: [rjk] temporary until all scene files are converted.
	//------------------------------------------------------------------------
	tmlnBaseData& operator = (const mnmBaseData& i_Data);

	//
	//	data
	//
	std::vector<tmlnDriverInfo*> m_Drivers;
};

