/*****************************************************************************
**	lsetLightSetLightsPage.hpp
**
**	LightSet Lights tab page in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef LSET_LIGHTSETLIGHTSPAGE_HPP
#error lsetLightSetLightsPage.hpp multiply included
#endif
#define LSET_LIGHTSETLIGHTSPAGE_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

class QListWidget;
class QListWidgetItem;

#ifdef USE_QT

//----------------------------------------------------------------------------
// Class lsetLightSetLightsPage
//----------------------------------------------------------------------------
class lsetLightSetLightsPage : public QWidget
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static lsetLightSetLightsPage* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		lsetLightSetLightsPage( QWidget* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~lsetLightSetLightsPage();

		//--------------------------------------------------------------------
		// Clear object data from form
		//--------------------------------------------------------------------
		void Clear();

		//--------------------------------------------------------------------
		// Update object data in form
		//--------------------------------------------------------------------
		void Update(const nameString& i_LightSetName);
		void ReUpdate() { Update(m_LightSetName); }

	private:
		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		void checkList_Lights_Toggle( QListWidgetItem* i_pItem );

		QListWidget* m_checkList_Lights;
		nameString m_LightSetName;
		bool m_bUpdating;
};

#endif // USE_QT
