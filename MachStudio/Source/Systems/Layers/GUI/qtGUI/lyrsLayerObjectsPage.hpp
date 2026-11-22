/*****************************************************************************
**	lyrsLayerObjectsPage.hpp
**
**	Layer Objects tab page in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef LYRS_LAYEROBJECTSPAGE_HPP
#error lyrsLayerObjectsPage.hpp multiply included
#endif
#define LYRS_LAYEROBJECTSPAGE_HPP

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
// Class lyrsLayerObjectsPage
//----------------------------------------------------------------------------
class lyrsLayerObjectsPage : public QWidget
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static lyrsLayerObjectsPage* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		lyrsLayerObjectsPage( QWidget* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~lyrsLayerObjectsPage();

		//--------------------------------------------------------------------
		// Clear object data from form
		//--------------------------------------------------------------------
		void Clear();

		//--------------------------------------------------------------------
		// Update object data in form
		//--------------------------------------------------------------------
		void Update(const nameString& i_LayerName);
		void ReUpdate() { Update(m_LayerName); }

	private:
		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		void checkList_Objects_Toggle( QListWidgetItem* i_pItem );

		QListWidget* m_checkList_Objects;
		nameString m_LayerName;
		bool m_bUpdating;
};

#endif // USE_QT
