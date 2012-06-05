/****************************************************************************\
**	tmlnTimeEditUIInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnTimeEditUIInfo.hpp"

#include "Core/prty/prtyProperty.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tmlnTimeEditUIInfo::tmlnTimeEditUIInfo(prtyProperty* i_pProperty)
:	prtyPropertyUIInfo(i_pProperty)
{
	SetControlName("TimeEdit");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tmlnTimeEditUIInfo::tmlnTimeEditUIInfo(	prtyProperty* i_pProperty, 
											const std::string& i_Category, 
											const std::string& i_Description)
:	prtyPropertyUIInfo(i_pProperty, i_Category, i_Description)
{
	SetControlName("TimeEdit");
}

//--------------------------------------------------------------------
// Return pointer to new equivalent prtyPropertyUIInfo
//--------------------------------------------------------------------
//virtual 
prtyPropertyUIInfo* tmlnTimeEditUIInfo::Clone()
{
	return new tmlnTimeEditUIInfo( this->GetProperty(0) );
}

