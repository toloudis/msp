/****************************************************************************\
**	ptmControlFactoryTimeline.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/ptm/ptmControlFactoryTimeline.hpp"

#include "Support/ptm/wxGUI/ptmFloat_TimeEdit.hpp"


#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//	Check for Control + Property pairing
//----------------------------------------------------------------------------
//virtual 
pwxControl* ptmControlFactoryTimeline::CreateControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
													wxWindow* i_pParent)
{
	const std::string& control_name		= i_pUIInfo->GetControlName();

	// only need first one (since the rest should be the same at this point)
	const std::string& property_type	= i_pUIInfo->GetProperty(0)->GetType();

	if ( strcmp(control_name.c_str(),"TimeEdit") == 0)
	{
		if ( strcmp(property_type.c_str(),"Float") == 0)
		{
			return new ptmFloat_TimeEdit(i_pUIInfo, i_pParent);
			
		}
	}
	return NULL;
}

#endif // USE_WXWIDGETS
