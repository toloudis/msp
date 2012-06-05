/****************************************************************************\
**	pqtKeyPropertyButton.hpp
**
**		Label that expands and compresses the properties beneath it.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_KEYPROPERTYBUTTON_HPP
#error pqtKeyPropertyButton.hpp multiply included
#endif
#define PQT_KEYPROPERTYBUTTON_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <string>

#ifdef USE_QT
#include <QtGui/QWidget>


//============================================================================
//============================================================================
class pqtKeyPropertyButton 
#ifdef QT_FINISH_PORT
	: public wxBitmapButton
#endif
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
#ifdef QT_FINISH_PORT
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void DoKeyProperty(wxCommandEvent &i_Event);
#endif

	private:
		std::string m_PropertyName;
		KeyPropertyFunction m_KeyPropertyFunction;

#ifdef QT_FINISH_PORT
	DECLARE_EVENT_TABLE()
#endif
};

#endif // USE_QT
