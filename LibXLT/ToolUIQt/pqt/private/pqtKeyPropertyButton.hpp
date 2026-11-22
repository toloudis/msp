/****************************************************************************\
**	pqtKeyPropertyButton.hpp
**
**	Button next to an animatable property that keys the property
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_KEYPROPERTYBUTTON_HPP
#error pqtKeyPropertyButton.hpp multiply included
#endif
#define PQT_KEYPROPERTYBUTTON_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

//============================================================================
//============================================================================
class pqtKeyPropertyButton : public QToolButton
{
	public:
		//--------------------------------------------------------------------
		// Function signature as callback when the "Key" button
		// next to a property is pressed.
		//--------------------------------------------------------------------
		typedef void (*KeyPropertyFunction)(const std::string& /*i_PropertyName*/);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pqtKeyPropertyButton(QWidget* i_pParent,
							 const std::string& i_PropertyName,
							 KeyPropertyFunction i_KeyPropertyFunction);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void DoKeyProperty();

		std::string m_PropertyName;
		KeyPropertyFunction m_KeyPropertyFunction;
};

#endif // USE_QT
