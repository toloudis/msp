/****************************************************************************\
**	ptmControlFactoryTimeline.hpp
**
**		Control factory for timeline related controls.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PTM_CONTROLFACTORYTIMELINE_HPP
#error ptmControlFactoryTimeline.hpp multiply included
#endif
#define PTM_CONTROLFACTORYTIMELINE_HPP

#ifdef MSP_GUI_QT
#ifndef PQT_CONTROLFACTORY_HPP
#include "ToolUIQt/pqt/pqtControlFactory.hpp"
#endif
#else
#ifndef PWX_CONTROLFACTORY_HPP
#include "ToolUIWx/pwx/pwxControlFactory.hpp"
#endif
#endif

//============================================================================
// Implements special time controls that recognize
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
#elif defined(USE_QT)
class ptmControlFactoryTimeline : public pqtControlFactory
{
	public:
		//----------------------------------------------------------------------------
		//	Check for Control + Property pairing
		//----------------------------------------------------------------------------
		virtual pqtControl* CreateControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										  QWidget* i_pParent);
};
#endif // USE_WXWIDGETS
