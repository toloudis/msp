/****************************************************************************\
**	ptmControlFactoryTimeline.hpp
**
**		Control factory for timeline related controls.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PTM_CONTROLFACTORYTIMELINE_HPP
#error ptmControlFactoryTimeline.hpp multiply included
#endif
#define PTM_CONTROLFACTORYTIMELINE_HPP

#ifndef PWX_CONTROLFACTORY_HPP
#include "ToolUIWx/pwx/pwxControlFactory.hpp"
#endif

#ifndef PRTY_CONTROLFACTORY_HPP
#include "ToolUIManaged/prtym/prtyControlFactory.hpp"
#endif
#ifndef PRTY_FLOAT_TIMEEDIT_HPP
#include "Support/ptm/mGUI/prtyFloat_TimeEdit.hpp"
#endif 


//============================================================================
// wxWidgets version - implements special time controls that recognize
//	and use time format settings.
//============================================================================
#ifdef USE_WXWIDGETS
class ptmControlFactoryTimeline : public pwxControlFactory
{
	public:
		//----------------------------------------------------------------------------
		//	Check for Control + Property pairing
		//----------------------------------------------------------------------------
		virtual pwxControl* CreateControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										  wxWindow* i_pParent);
};
#endif // USE_WXWIDGETS


//============================================================================
// Managed version - uses regular float edit for time in seconds
//============================================================================
#ifdef _MANAGED
public ref class ptmControlFactoryTimeline : public prtyControlFactory
{
	public:
		//----------------------------------------------------------------------------
		//	Check for Control + Property pairing
		//----------------------------------------------------------------------------
		virtual prtyControl^ CreateControl(prtyPropertyUIInfo* i_pUIInfo) override
		{
			prtyControl^ pPropertyControl = nullptr;
			const std::string& control_name		= i_pUIInfo->GetControlName();

			// only need first one (since the rest should be the same at this point)
			const std::string& property_type	= i_pUIInfo->GetProperty(0)->GetType();

			if ( strcmp(control_name.c_str(),"TimeEdit") == 0)
			{
				if ( strcmp(property_type.c_str(),"Float") == 0)
				{
					pPropertyControl = gcnew prtyFloat_TimeEdit(i_pUIInfo);
					return pPropertyControl;
				}
			}
			return nullptr;
		};
};

#endif // _MANAGED
