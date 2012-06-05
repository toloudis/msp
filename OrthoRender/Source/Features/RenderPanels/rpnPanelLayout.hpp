/*****************************************************************************
**  rpnPanelLayout.hpp
**
**      The managed system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef RPN_PANELLAYOUT_HPP
#error rpnPanelLayout.hpp multiply included
#endif
#define RPN_PANELLAYOUT_HPP

#ifndef RPN_RENDERPANE_HPP
#include "Features/RenderPanels/rpnRenderPane.hpp"
#endif

#ifdef _MANAGED

namespace StudioFramework
{

public ref class rpnPanelLayout
{
public:
	static TerawattManagedControls::PanelLayout ^ g_pPanelLayout = nullptr;
	
	static cli::array<rpnRenderPane^>^		g_RenderPanes = nullptr;

	static void CreateRenderPanes(TerawattManagedControls::PanelLayout ^ i_pPanelLayout)
	{
		g_pPanelLayout = i_pPanelLayout;

		g_RenderPanes = gcnew cli::array<rpnRenderPane^> (4);

		g_RenderPanes[0] = gcnew rpnRenderPane();
		g_pPanelLayout->SetControl(0, g_RenderPanes[0]);
		g_RenderPanes[1] = gcnew rpnRenderPane();
		g_pPanelLayout->SetControl(1, g_RenderPanes[1]);
		g_RenderPanes[2] = gcnew rpnRenderPane();
		g_pPanelLayout->SetControl(2, g_RenderPanes[2]);
		g_RenderPanes[3] = gcnew rpnRenderPane();
		g_pPanelLayout->SetControl(3, g_RenderPanes[3]);
	}
};

}

#endif // _MANAGED
