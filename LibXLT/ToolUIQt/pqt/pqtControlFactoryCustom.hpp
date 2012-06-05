/****************************************************************************\
**	pqtControlFactoryCustom.hpp
**
**		Control factory for base controls.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_CONTROLFACTORYCUSTOM_HPP
#error pqtControlFactoryCustom.hpp multiply included
#endif
#define PQT_CONTROLFACTORYCUSTOM_HPP

#ifndef PQT_CONTROLFACTORY_HPP
#include "ToolUIQt/pqt/pqtControlFactory.hpp"
#endif


//============================================================================
//============================================================================
class pqtControlFactoryCustom : public pqtControlFactory
{
	public:
#ifdef USE_QT
		//----------------------------------------------------------------------------
		//	Check for Control + Property pairing
		//----------------------------------------------------------------------------
		virtual pqtControl* CreateControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										  QWidget* i_pParent);
#endif
};
