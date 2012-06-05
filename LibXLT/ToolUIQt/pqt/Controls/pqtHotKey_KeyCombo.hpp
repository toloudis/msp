/****************************************************************************\
**	pqtHotKey_KeyCombo.hpp
**
**		Intermediate class between the property (prtyHotKey) and 
**	the control (HotKeyCtrl).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_HOTKEY_KEYCOMBO_HPP
#error pqtHotKey_KeyCombo.hpp multiply included
#endif
#define PQT_HOTKEY_KEYCOMBO_HPP

#ifndef PQT_CONTROL_HPP
#include "ToolUIQt/pqt/pqtControl.hpp"
#endif
#ifndef TQC_HOTKEYCTRL_HPP
#include "ToolUIQt/tqc/tqcHotKeyCtrl.hpp"
#endif 


//============================================================================
//============================================================================
class pqtHotKey_KeyCombo : public pqtControl
{
#ifdef QT_FINISH_PORT
	public:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		pqtHotKey_KeyCombo(shared_ptr<prtyPropertyUIInfo>& i_pUIInfo,
							QWidget* i_pParent);

		//----------------------------------------------------------------------------
		//	Destructor - dispose of controls
		//----------------------------------------------------------------------------
		~pqtHotKey_KeyCombo();

		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		virtual QWidget* GetControl();

		//----------------------------------------------------------------------------
		//	Note: right now, PropertyChanged will not create a circular update because
		//	of the m_bLocalChangeNoUpdate flag.  This IS NOT true the other way
		//	around.  If a control calls its "ValueChanged" then the property will
		//	call all of its callback controls and one of them could have been the one
		//	that originally updated the property value(s).  By checking the diff of 
		//	the values we can avoid a repetitive setting of the control's values.
		//----------------------------------------------------------------------------
		virtual void PropertyChanged(prtyProperty* i_pProperty, bool i_bDirty);

		//----------------------------------------------------------------------------
		// Update control to match the changes in the UI Info
		//----------------------------------------------------------------------------
		virtual void UpdateControl(prtyPropertyUIInfo* i_pUIInfo);

	private:
		//----------------------------------------------------------------------------
		//----------------------------------------------------------------------------
		void HotKeyCtrl_ValueChanged(wxCommandEvent& i_Event);

		tqcHotKeyCtrl* m_pActualControl;
#endif // USE_QT
};

