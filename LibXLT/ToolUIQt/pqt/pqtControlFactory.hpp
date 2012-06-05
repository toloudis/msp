/****************************************************************************\
**	pqtControlFactory.hpp
**
**		Control Factory base class
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_CONTROLFACTORY_HPP
#error pqtControlFactory.hpp multiply included
#endif
#define PQT_CONTROLFACTORY_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 

#ifdef USE_QT
#include <QtGui/QWidget>
#endif


//============================================================================
//============================================================================
class prtyPropertyUIInfo;
class pqtControl;


//============================================================================
//============================================================================
class pqtControlFactory
{
	public:
#ifdef USE_QT
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual pqtControl* CreateControl(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
										  QWidget* i_pParent) = 0;
#endif	// USE_QT
};

