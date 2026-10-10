/*****************************************************************************
**	rpnPanelGrid.hpp
**
**		Group of 4 render panels that can switch between 1-, 2-, and 4-
**	panel layout styles.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef RPN_PANELGRID_HPP
#error rpnPanelGrid.hpp multiply included
#endif
#define RPN_PANELGRID_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS


//============================================================================
//============================================================================
class gpxCamera;
class rpnRenderPanel;


//============================================================================
// Define a new frame type: this is going to be our main frame
//============================================================================
class rpnPanelGrid : public wxPanel
{
public:
	//--------------------------------------------------------------------
	// Enumeration of layout style
	//--------------------------------------------------------------------
	enum LayoutStyle
	{
		e_SinglePane = 0,
		e_TwoStacked,
		e_FourPanels,
		e_TwoSideBySide
	};

	//--------------------------------------------------------------------
	//	Static pointer to the instance of the form, will be cleared
	//	when the object dialog is deleted.
	//--------------------------------------------------------------------
	static rpnPanelGrid* Instance;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	rpnPanelGrid(wxWindow* parent);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~rpnPanelGrid();

	//----------------------------------------------------------------------------
	// Access to the render panels
	//----------------------------------------------------------------------------
	rpnRenderPanel* GetRenderPanel(int i_Panel);

	//----------------------------------------------------------------------------
	// Set camera and label for the render pane with the given index.
	//----------------------------------------------------------------------------
	void SetCamera(int i_Panel, gpxCamera *i_pCamera, const std::string& i_Label);

	//----------------------------------------------------------------------------
	// Set the organization of panes in the grid
	//----------------------------------------------------------------------------
	void SetLayoutStyle(LayoutStyle i_Layout);
	LayoutStyle GetLayoutStyle() const;

	//----------------------------------------------------------------------------
	// SetMaxRenderSize - set the render resolution of the render areas, plus
	//	the 2 pixel border. Use -1 -1 in order to render at the panel size.
	//----------------------------------------------------------------------------
	void SetMaxRenderSize(int i_Width, int i_Height);
	void GetMaxRenderSize(int &o_Width, int &o_Height);

protected:
	//------------------------------------------------------------------------
	// Override the wxWidgets SetSize function in order to enforce a
	//	maximum size for the render area.
	//------------------------------------------------------------------------
	virtual void DoSetSize(int x, int y,
						   int width, int height,
						   int sizeFlags);

private:
	//--------------------------------------------------------------------
	// paint event handler
	//--------------------------------------------------------------------
	//void OnPaint(wxPaintEvent& i_Event);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void adjust_layout_sizer();

	rpnRenderPanel* m_RenderPanes[4];
	wxSizer *m_pGridSizer;
	wxSizer *m_pTopSizer;
	wxSizer *m_pBottomSizer;
	LayoutStyle m_Layout;
	int m_MaxWidth, m_MaxHeight;

	wxPanel *m_pLayoutPane;
	wxSizer *m_pLayoutSizer;

	DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
