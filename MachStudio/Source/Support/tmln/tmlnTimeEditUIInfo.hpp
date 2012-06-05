/****************************************************************************\
**	tmlnTimeEditUIInfo.hpp
**
**		Text box for entering time fields. Uses the current time format,
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_TIMEEDITUIINFO_HPP
#error tmlnTimeEditUIInfo.hpp multiply included
#endif
#define TMLN_TIMEEDITUIINFO_HPP

#ifndef PRTY_PROPERTYUIINFO_HPP
#include "Core/prty/prtyPropertyUIInfo.hpp"
#endif


//============================================================================
//============================================================================
class prtyProperty;


//============================================================================
//============================================================================
class tmlnTimeEditUIInfo : public prtyPropertyUIInfo
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tmlnTimeEditUIInfo(prtyProperty* i_pProperty);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tmlnTimeEditUIInfo(prtyProperty* i_pProperty, 
							const std::string& i_Category, 
							const std::string& i_Description);

		//--------------------------------------------------------------------
		// Return pointer to new equivalent prtyPropertyUIInfo
		//--------------------------------------------------------------------
		virtual prtyPropertyUIInfo* Clone();

	private:
};
