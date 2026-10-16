/*****************************************************************************
**  rpnPanelGrid.cpp
**
**     Grid of render panels using Qt for doing MachStudio rendering
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPanels/qtGUI/rpnPanelGrid.hpp"

#include "Features/RenderPanels/qtGUI/rpnRenderPanel.hpp"


#ifdef USE_QT

namespace
{
	// Border around each render panel
	const int c_PanelBorder = 1;

	//------------------------------------------------------------------------
	// Largest rectangle with the given aspect ratio that fits in the area,
	// centered in it.
	//------------------------------------------------------------------------
	QRect fit_aspect(const QRect& i_Area, int i_AspectWidth, int i_AspectHeight)
	{
		if (i_AspectWidth <= 0 || i_AspectHeight <= 0 || i_Area.isEmpty())
			return i_Area;
		QSize size(i_AspectWidth, i_AspectHeight);
		size.scale(i_Area.size(), Qt::KeepAspectRatio);
		QRect fitted(QPoint(0, 0), size);
		fitted.moveCenter(i_Area.center());
		return fitted;
	}
}

//--------------------------------------------------------------------
//	Static pointer to the instance of the control, will be cleared
//	when the panel grid is deleted.
//--------------------------------------------------------------------
rpnPanelGrid* rpnPanelGrid::Instance = NULL;

//--------------------------------------------------------------------
// canvas constructor
//--------------------------------------------------------------------
rpnPanelGrid::rpnPanelGrid(QWidget* parent)
: QWidget(parent),
	m_Layout(e_FourPanels),
	m_MaxWidth(-1),
	m_MaxHeight(-1)
{
	rpnPanelGrid::Instance = this;

	for (int i=0; i<4; ++i)
	{
		m_RenderPanes[i] = new rpnRenderPanel(this);
	}

	// allow render pass context menu in main panel
	m_RenderPanes[0]->SetCanChangeRenderPass(true);

	adjust_layout_sizer();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
rpnPanelGrid::~rpnPanelGrid()
{
	if (rpnPanelGrid::Instance == this)
		rpnPanelGrid::Instance = NULL;
}

//----------------------------------------------------------------------------
// Access to the render panels
//----------------------------------------------------------------------------
rpnRenderPanel* rpnPanelGrid::GetRenderPanel(int i_Panel)
{
	return m_RenderPanes[i_Panel];
}

//----------------------------------------------------------------------------
// Set camera and label for the render pane with the given index.
//----------------------------------------------------------------------------
void rpnPanelGrid::SetCamera(int i_Panel, gpxCamera *i_pCamera, const std::string& i_Label)
{
	m_RenderPanes[i_Panel]->SetCamera(i_pCamera, i_Label);
}

//----------------------------------------------------------------------------
// Set the organization of panes in the grid
//----------------------------------------------------------------------------
void rpnPanelGrid::SetLayoutStyle(LayoutStyle i_Layout)
{
	switch (i_Layout)
	{
	default:
	case e_SinglePane:
		m_RenderPanes[1]->EnableViewer(false);
		m_RenderPanes[2]->EnableViewer(false);
		m_RenderPanes[3]->EnableViewer(false);
		break;
	case e_TwoStacked:
		m_RenderPanes[1]->EnableViewer(true);
		m_RenderPanes[2]->EnableViewer(false);
		m_RenderPanes[3]->EnableViewer(false);
		break;
	case e_TwoSideBySide:
		m_RenderPanes[1]->EnableViewer(false);
		m_RenderPanes[2]->EnableViewer(true);
		m_RenderPanes[3]->EnableViewer(false);
		break;
	case e_FourPanels:
		m_RenderPanes[1]->EnableViewer(true);
		m_RenderPanes[2]->EnableViewer(true);
		m_RenderPanes[3]->EnableViewer(true);
		break;
	}
	m_Layout = i_Layout;
	adjust_layout_sizer();
}
rpnPanelGrid::LayoutStyle rpnPanelGrid::GetLayoutStyle() const
{
	return m_Layout;
}

//----------------------------------------------------------------------------
// SetMaxRenderSize - set the render resolution of the render areas, plus
//	the 2 pixel border. Use -1 -1 in order to render at the panel size.
//----------------------------------------------------------------------------
void rpnPanelGrid::SetMaxRenderSize(int i_Width, int i_Height)
{
	m_MaxWidth = i_Width;
	m_MaxHeight = i_Height;

	// Each panel renders at the chosen resolution, scaled to fit the
	// panel. The grid size includes a one pixel border around the render
	// area, so subtract 2 for the render size.
	for (int i=0; i<4; i++)
	{
		if (i_Width > 2 && i_Height > 2)
			m_RenderPanes[i]->SetRenderResolution(i_Width - 2, i_Height - 2);
		else
			m_RenderPanes[i]->SetRenderResolution(-1, -1);
	}
	adjust_layout_sizer();
}
void rpnPanelGrid::GetMaxRenderSize(int &o_Width, int &o_Height)
{
	o_Width = m_MaxWidth;
	o_Height = m_MaxHeight;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void rpnPanelGrid::resizeEvent(QResizeEvent* i_pEvent)
{
	QWidget::resizeEvent(i_pEvent);
	adjust_layout_sizer();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rpnPanelGrid::adjust_layout_sizer()
{
	// Panels are arranged in the cells (row, column):
	//	0 = (0,0), 2 = (0,1), 1 = (1,0), 3 = (1,1)
	const int c_Row[4] = { 0, 1, 0, 1 };
	const int c_Column[4] = { 0, 0, 1, 1 };

	bool bVisible[4] = { true, false, false, false };
	int rows = 1, columns = 1;
	switch (m_Layout)
	{
	default:
	case e_SinglePane:
		break;
	case e_TwoStacked:
		bVisible[1] = true;
		rows = 2;
		break;
	case e_TwoSideBySide:
		bVisible[2] = true;
		columns = 2;
		break;
	case e_FourPanels:
		bVisible[1] = bVisible[2] = bVisible[3] = true;
		rows = columns = 2;
		break;
	}

	// If we are given a resolution to match, then the group of panels
	// keeps that aspect ratio
	const bool bHaveAspect = (m_MaxWidth > 0 && m_MaxHeight > 0);
	QRect group = this->rect();
	if (bHaveAspect)
		group = fit_aspect(group, m_MaxWidth * columns, m_MaxHeight * rows);

	const int cell_width = group.width() / columns;
	const int cell_height = group.height() / rows;
	for (int i = 0; i < 4; ++i)
	{
		if (!bVisible[i])
		{
			m_RenderPanes[i]->hide();
			continue;
		}

		QRect cell(group.left() + c_Column[i] * cell_width,
				   group.top() + c_Row[i] * cell_height,
				   cell_width, cell_height);
		cell.adjust(c_PanelBorder, c_PanelBorder, -c_PanelBorder, -c_PanelBorder);
		if (bHaveAspect)
			cell = fit_aspect(cell, m_MaxWidth, m_MaxHeight);

		m_RenderPanes[i]->setGeometry(cell);
		m_RenderPanes[i]->show();
	}
}

#endif // USE_QT
