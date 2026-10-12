/*****************************************************************************
**	envtEnvironmentObjectsPage.hpp
**
**	Environment Objects tab page in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef ENVT_ENVIRONMENTOBJECTSPAGE_HPP
#error envtEnvironmentObjectsPage.hpp multiply included
#endif
#define ENVT_ENVIRONMENTOBJECTSPAGE_HPP

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
// Class envtEnvironmentObjectsPage
//----------------------------------------------------------------------------
class envtEnvironmentObjectsPage : public QWidget
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static envtEnvironmentObjectsPage* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		envtEnvironmentObjectsPage( QWidget* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~envtEnvironmentObjectsPage();

		//--------------------------------------------------------------------
		// Clear object data from form
		//--------------------------------------------------------------------
		void Clear();

		//--------------------------------------------------------------------
		// Update object data in form
		//--------------------------------------------------------------------
		void Update(const nameString& i_EnvironmentName);
		void ReUpdate() { Update(m_EnvironmentName); }

	private:
		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		void checkList_Objects_Toggle( QListWidgetItem* i_pItem );

		QListWidget* m_checkList_Objects;
		nameString m_EnvironmentName;
		bool m_bUpdating;
};

#endif // USE_QT
