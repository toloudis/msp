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

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT


//============================================================================
//============================================================================
class gpxCamera;
class rpnRenderPanel;


//============================================================================
// Grid of up to four render panels
//============================================================================
class rpnPanelGrid : public QWidget
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
	rpnPanelGrid(QWidget* parent);

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
	// Lay the panels out whenever the grid changes size
	//------------------------------------------------------------------------
	virtual void resizeEvent(QResizeEvent* i_pEvent);

private:
	//--------------------------------------------------------------------
	// Position the visible panels: each keeps the aspect ratio of the
	// render size (if one is set) and is centered in its cell, and the
	// group of panels is centered in the grid.
	//--------------------------------------------------------------------
	void adjust_layout_sizer();

	rpnRenderPanel* m_RenderPanes[4];
	LayoutStyle m_Layout;
	int m_MaxWidth, m_MaxHeight;
};

#endif // USE_QT
