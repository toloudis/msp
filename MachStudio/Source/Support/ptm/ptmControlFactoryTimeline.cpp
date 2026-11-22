/****************************************************************************\
**	ptmControlFactoryTimeline.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/ptm/ptmControlFactoryTimeline.hpp"

#ifdef USE_WXWIDGETS
#ifdef MSP_GUI_QT
#include "Support/ptm/qtGUI/ptmTime_TimeEdit.hpp"
#else
#include "Support/ptm/wxGUI/ptmTime_TimeEdit.hpp"
#endif
#elif defined(USE_QT)
#include "Support/ptm/qtGUI/ptmTime_TimeEdit.hpp"
#endif


#if defined(USE_WXWIDGETS) || defined(USE_QT)

//----------------------------------------------------------------------------
//	Check for Control + Property pairing
//----------------------------------------------------------------------------
//virtual 
#ifdef USE_WXWIDGETS
pwxControl* ptmControlFactoryTimeline::CreateControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
													wxWindow* i_pParent)
#else
pqtControl* ptmControlFactoryTimeline::CreateControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
													QWidget* i_pParent)
#endif
{
	const std::string& control_name		= i_pUIInfo->GetControlName();

	// only need first one (since the rest should be the same at this point)
	const std::string& property_type	= i_pUIInfo->GetProperty(0)->GetType();

	if ( strcmp(control_name.c_str(), "TimeEdit") == 0)
	{
		if ( strcmp(property_type.c_str(), "Time") == 0)
		{
			return new ptmTime_TimeEdit(i_pUIInfo, i_pParent);
			
		}
	}
	return NULL;
}

#endif // USE_WXWIDGETS
