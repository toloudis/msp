/*****************************************************************************
**	lyrsLayerObjectsPage.hpp
**
**	Layer Objects tab page in wxWidgets
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
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Base class generated from wxFormBuilder
//----------------------------------------------------------------------------
#include "Systems/Layers/GUI/wxGUI/lyrsLayerObjectsPageBase.h"


//----------------------------------------------------------------------------
// Class lyrsLayerObjectsPage
//----------------------------------------------------------------------------
class lyrsLayerObjectsPage : public lyrsLayerObjectsPageBase
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static lyrsLayerObjectsPage* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		lyrsLayerObjectsPage( wxWindow* parent );

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
		virtual void checkList_Objects_Toggle( wxCommandEvent& i_Event);
	
		nameString m_LayerName;
};

#endif // USE_WXWIDGETS
