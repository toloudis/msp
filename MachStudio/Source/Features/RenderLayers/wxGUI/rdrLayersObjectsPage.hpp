/*****************************************************************************
**	rdrLayersObjectsPage.hpp
**
**	LightSet Lights tab page in wxWidgets
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
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Base class generated from wxFormBuilder
//----------------------------------------------------------------------------
#include "Features/RenderLayers/wxGUI/rdrLayersObjectsPageBase.h"


//----------------------------------------------------------------------------
// Class rdrLayersObjectsPage
//----------------------------------------------------------------------------
class rdrLayersObjectsPage : public rdrLayersObjectsPageBase
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static rdrLayersObjectsPage* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		rdrLayersObjectsPage( wxWindow* parent );

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
		// Item was clicked on, toggle its checked state
		//--------------------------------------------------------------------
		void toggle_checked_state(wxTreeItemId i_Item);

		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		virtual void treeCtrl_Objects_LeftMouseDown( wxMouseEvent& i_Event);
	
		nameString m_RenderLayerName;
};

#endif // USE_WXWIDGETS
