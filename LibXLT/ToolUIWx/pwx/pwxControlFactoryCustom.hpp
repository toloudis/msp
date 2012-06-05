/****************************************************************************\
**	pwxControlFactoryCustom.hpp
**
**		Control factory for base controls.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_CONTROLFACTORYCUSTOM_HPP
#error pwxControlFactoryCustom.hpp multiply included
#endif
#define PWX_CONTROLFACTORYCUSTOM_HPP

#ifndef PWX_CONTROLFACTORY_HPP
#include "ToolUIWx/pwx/pwxControlFactory.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class pwxControlFactoryCustom : public pwxControlFactory
{
	public:
		//----------------------------------------------------------------------------
		//	Check for Control + Property pairing
		//----------------------------------------------------------------------------
		virtual pwxControl* CreateControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										  wxWindow* i_pParent);
};

#endif // USE_WXWIDGETS
