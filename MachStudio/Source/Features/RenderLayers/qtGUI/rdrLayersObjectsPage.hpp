/*****************************************************************************
**	rdrLayersObjectsPage.hpp
**
**	Render layer objects tab page in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef RDR_LAYERSOBJECTSPAGE_HPP
#error rdrLayersObjectsPage.hpp multiply included
#endif
#define RDR_LAYERSOBJECTSPAGE_HPP

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
// Class rdrLayersObjectsPage - objects and their fragments that are in
//	the render layer
//----------------------------------------------------------------------------
class rdrLayersObjectsPage : public QWidget
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static rdrLayersObjectsPage* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		rdrLayersObjectsPage( QWidget* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~rdrLayersObjectsPage();

		//--------------------------------------------------------------------
		// Clear object data from form
		//--------------------------------------------------------------------
		void Clear();

		//--------------------------------------------------------------------
		// Update object data in form
		//--------------------------------------------------------------------
		void Update(const nameString& i_RenderLayerName);
		void ReUpdate() { Update(m_RenderLayerName); }

	private:
		//--------------------------------------------------------------------
		// A check box was toggled. Toggling a selected item toggles all of
		// the selected items.
		//--------------------------------------------------------------------
		void item_changed(QTreeWidgetItem* i_pItem);
		void set_checked_state(QTreeWidgetItem* i_pItem, bool i_bChecked);

		QTreeWidget* m_treeCtrl_Objects;
		nameString m_RenderLayerName;
		bool m_bUpdating;
};

#endif // USE_QT
