/*****************************************************************************
**	envtEnvironmentSwlPage.hpp
**
**	Environment software lighting tab page in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef ENVT_ENVIRONMENTSWLPAGE_HPP
#error envtEnvironmentSwlPage.hpp multiply included
#endif
#define ENVT_ENVIRONMENTSWLPAGE_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

class swlScriptObject;
class nameString;

//----------------------------------------------------------------------------
// Class envtEnvironmentSwlPage
//----------------------------------------------------------------------------
class envtEnvironmentSwlPage : public QWidget
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static envtEnvironmentSwlPage* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		envtEnvironmentSwlPage( QWidget* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~envtEnvironmentSwlPage();

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
		QWidget* m_tabPage_SoftwareLighting;
		nameString m_EnvironmentName;
};

#endif // USE_QT
