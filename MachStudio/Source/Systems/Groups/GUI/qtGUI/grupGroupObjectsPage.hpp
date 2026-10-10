/*****************************************************************************
**	grupGroupObjectsPage.hpp
**
**	Group Objects tab page in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef GRUP_GROUPOBJECTSPAGE_HPP
#error grupGroupObjectsPage.hpp multiply included
#endif
#define GRUP_GROUPOBJECTSPAGE_HPP

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
// Class grupGroupObjectsPage
//----------------------------------------------------------------------------
class grupGroupObjectsPage : public QWidget
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static grupGroupObjectsPage* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		grupGroupObjectsPage( QWidget* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~grupGroupObjectsPage();

		//--------------------------------------------------------------------
		// Clear object data from form
		//--------------------------------------------------------------------
		void Clear();

		//--------------------------------------------------------------------
		// Update object data in form
		//--------------------------------------------------------------------
		void Update(const nameString& i_GroupName);
		void ReUpdate() { Update(m_GroupName); }

	private:
		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		void checkList_Objects_Toggle( QListWidgetItem* i_pItem );

		QListWidget* m_checkList_Objects;
		nameString m_GroupName;
		bool m_bUpdating;
};

#endif // USE_QT
