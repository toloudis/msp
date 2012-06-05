/****************************************************************************\
**	pwxControlFactory.hpp
**
**		Control Factory base class
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_CONTROLFACTORY_HPP
#error pwxControlFactory.hpp multiply included
#endif
#define PWX_CONTROLFACTORY_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 


#ifdef USE_WXWIDGETS 

//============================================================================
//============================================================================
class prtyPropertyUIInfo;
class pwxControl;

//============================================================================
//============================================================================
class pwxControlFactory
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual pwxControl* CreateControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										  wxWindow* i_pParent) = 0;
};

#endif	// USE_WXWIDGETS