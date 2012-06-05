/****************************************************************************\
**	pqtControlFactoryBase.hpp
**
**		Control factory for base controls.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_CONTROLFACTORYBASE_HPP
#error pqtControlFactoryBase.hpp multiply included
#endif
#define PQT_CONTROLFACTORYBASE_HPP

#ifndef PQT_CONTROLFACTORY_HPP
#include "ToolUIQt/pqt/pqtControlFactory.hpp"
#endif


//============================================================================
//============================================================================
class pqtControlFactoryBase : public pqtControlFactory
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

