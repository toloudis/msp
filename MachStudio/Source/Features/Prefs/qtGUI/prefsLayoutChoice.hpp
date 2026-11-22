/*****************************************************************************
**  prefsLayoutChoice.hpp
**
**     Choice box displaying current and available cameras and
**	director's cuts
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef PREFS_LAYOUTCHOICE_HPP
#error prefsLayoutChoice.hpp multiply included
#endif
#define PREFS_LAYOUTCHOICE_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT


//============================================================================
// Derive a new class in order to handle callbacks when layout
// choice changes.
//============================================================================
class prefsLayoutChoice : public QComboBox
{
public:
	//--------------------------------------------------------------------
	//	Static pointer to the instance of the form, will be cleared
	//	when the object dialog is deleted.
	//--------------------------------------------------------------------
	static prefsLayoutChoice* Instance;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
    prefsLayoutChoice(QWidget* parent);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
    ~prefsLayoutChoice();
		
	//------------------------------------------------------------------------
	// Set list of camera names
	//------------------------------------------------------------------------
	void Update();

	//------------------------------------------------------------------------
	// Select the layout with the given name, if it is in the list
	//------------------------------------------------------------------------
	void SetStringSelection(const std::string& i_Name);
private:
	//------------------------------------------------------------------------
	// event callbacks
	//------------------------------------------------------------------------
	void SelectedIndexChanged();

	bool m_bDisableNotify;
};

#endif // USE_QT
