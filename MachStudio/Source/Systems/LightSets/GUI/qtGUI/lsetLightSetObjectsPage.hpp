/*****************************************************************************
**	lsetLightSetObjectsPage.hpp
**
**	Light set objects tab page in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef LSET_LIGHTSETOBJECTSPAGE_HPP
#error lsetLightSetObjectsPage.hpp multiply included
#endif
#define LSET_LIGHTSETOBJECTSPAGE_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

class QTreeWidget;
class QTreeWidgetItem;

#ifdef USE_QT

//----------------------------------------------------------------------------
// Class lsetLightSetObjectsPage
//----------------------------------------------------------------------------
class lsetLightSetObjectsPage : public QWidget
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static lsetLightSetObjectsPage* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		lsetLightSetObjectsPage( QWidget* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~lsetLightSetObjectsPage();

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
		// An item's checkbox was toggled
		//--------------------------------------------------------------------
		void item_changed(QTreeWidgetItem* i_pItem);

		QTreeWidget* m_treeCtrl_Objects;
		nameString m_LightSetName;
		bool m_bUpdating;
};

#endif // USE_QT
