/*****************************************************************************
**	ResourceTrackerDialog.hpp
**
**		implementation of the Resource Tracker dialog
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef RESOURCETRACKERDIALOG_HPP
#error ResourceTrackerDialog.hpp multiply included
#endif
#define RESOURCETRACKERDIALOG_HPP


//	needed before App so USE_WXWIDGETS is set
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

//	base dialog
#ifdef USE_WXWIDGETS
#include "Features/ResourceTracker/wxGUI/ResourceTrackerDialogBase.h"
#endif

#include <set>
#include <string>


#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class fsLocator;
class fsResourceTrackerData;


//============================================================================
// Class RenderStatsDialog
//============================================================================
class ResourceTrackerDialog : public ResourceTrackerDialogBase
{
	public:
		static ResourceTrackerDialog* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		ResourceTrackerDialog( wxWindow* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~ResourceTrackerDialog();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Update();

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void OnInitDialog( wxInitDialogEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void button_refresh_OnButtonClick( wxCommandEvent& event );

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void update_tree_resources(const fsResourceTrackerData& i_ResourceTrackerData);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void update_tree_hierarchy(const fsResourceTrackerData& i_ResourceTrackerData);
};

#endif
