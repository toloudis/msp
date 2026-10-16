/*****************************************************************************
**	tqtPaneMgr.hpp
**
**		Manages dockable panes of the main window (QDockWidgets in a
**	QMainWindow), replacing the wxAUI pane manager.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQT_PANEMGR_HPP
#error tqtPaneMgr.hpp multiply included
#endif
#define TQT_PANEMGR_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

//============================================================================
// Description of a pane, with the builder style of wxAuiPaneInfo:
//	tqtPaneInfo().Name("Scene").Caption("Scene").Show().Left()
//============================================================================
class tqtPaneInfo
{
public:
	enum Dock
	{
		e_Center = 0,
		e_Left,
		e_Right,
		e_Top,
		e_Bottom
	};

	tqtPaneInfo()
		: m_Dock(e_Left), m_bShown(true), m_bFloat(false), m_bDestroyOnClose(false),
		  m_Layer(0), m_BestSize(-1, -1), m_MinSize(-1, -1) {}

	tqtPaneInfo& Name(const QString& i_Name)			{ m_Name = i_Name; return *this; }
	tqtPaneInfo& Name(const std::string& i_Name)		{ m_Name = tqtQString(i_Name); return *this; }
	tqtPaneInfo& Name(const char* i_pName)				{ m_Name = tqtQString(i_pName); return *this; }
	tqtPaneInfo& Name(const wchar_t* i_pName)			{ m_Name = QString::fromWCharArray(i_pName); return *this; }
	tqtPaneInfo& Caption(const QString& i_Caption)		{ m_Caption = i_Caption; return *this; }
	tqtPaneInfo& Caption(const std::string& i_Caption)	{ m_Caption = tqtQString(i_Caption); return *this; }
	tqtPaneInfo& Caption(const char* i_pCaption)		{ m_Caption = tqtQString(i_pCaption); return *this; }
	tqtPaneInfo& Caption(const wchar_t* i_pCaption)		{ m_Caption = QString::fromWCharArray(i_pCaption); return *this; }
	tqtPaneInfo& Show(bool i_bShow = true)				{ m_bShown = i_bShow; return *this; }
	tqtPaneInfo& Hide()									{ m_bShown = false; return *this; }
	tqtPaneInfo& Layer(int i_Layer)						{ m_Layer = i_Layer; return *this; }
	tqtPaneInfo& Left()									{ m_Dock = e_Left; return *this; }
	tqtPaneInfo& Right()								{ m_Dock = e_Right; return *this; }
	tqtPaneInfo& Top()									{ m_Dock = e_Top; return *this; }
	tqtPaneInfo& Bottom()								{ m_Dock = e_Bottom; return *this; }
	tqtPaneInfo& CenterPane()							{ m_Dock = e_Center; return *this; }
	tqtPaneInfo& Float()								{ m_bFloat = true; return *this; }
	tqtPaneInfo& DestroyOnClose(bool i_b = true)		{ m_bDestroyOnClose = i_b; return *this; }
	tqtPaneInfo& MinimizeButton(bool = true)			{ return *this; }
	tqtPaneInfo& BestSize(const QSize& i_Size)			{ m_BestSize = i_Size; return *this; }
	tqtPaneInfo& BestSize(int i_W, int i_H)				{ m_BestSize = QSize(i_W, i_H); return *this; }
	tqtPaneInfo& MinSize(const QSize& i_Size)			{ m_MinSize = i_Size; return *this; }
	tqtPaneInfo& MinSize(int i_W, int i_H)				{ m_MinSize = QSize(i_W, i_H); return *this; }

	QString m_Name;
	QString m_Caption;
	Dock	m_Dock;
	bool	m_bShown;
	bool	m_bFloat;
	bool	m_bDestroyOnClose;
	int		m_Layer;
	QSize	m_BestSize;
	QSize	m_MinSize;
};

//============================================================================
//============================================================================
namespace tqtPaneMgr
{
	//---------------------------------------------------------------------------
	// Create the pane manager
	//---------------------------------------------------------------------------
	void Init();

	//---------------------------------------------------------------------------
	// Shut down the pane management
	//---------------------------------------------------------------------------
	void CleanUp();

	//---------------------------------------------------------------------------
	// Add new pane with given info. The widget is put in a dock widget
	//	(or becomes the central widget for a center pane).
	//---------------------------------------------------------------------------
	void AddPane(QWidget *i_pWindow, const tqtPaneInfo &i_PaneInfo);

	//---------------------------------------------------------------------------
	// Refresh panes (kept for the wx interface, Qt lays out by itself)
	//---------------------------------------------------------------------------
	void Update();

	//---------------------------------------------------------------------------
	// Show the pane with the given name or window
	// To Hide the pane, call with i_bShow==false
	//---------------------------------------------------------------------------
	void Show(const char* i_PaneName, bool i_bShow = true);
	void Show(QWidget *i_pWindow, bool i_bShow = true);

	//---------------------------------------------------------------------------
	// Return true if the pane is visible currently
	//---------------------------------------------------------------------------
	bool IsVisible(const char* i_PaneName);
	bool IsVisible(QWidget *i_pWindow);

	//---------------------------------------------------------------------------
	// SetCaption - alter caption for pane
	//---------------------------------------------------------------------------
	void SetCaption(QWidget *i_pWindow, const std::string& i_Caption);

	//---------------------------------------------------------------------------
	// Resize the pane for the given window
	//---------------------------------------------------------------------------
	void ResizePane(QWidget *i_pWindow, int i_Width, int i_Height);

	//---------------------------------------------------------------------------
	// Dock widget holding the given pane window, or NULL
	//---------------------------------------------------------------------------
	QDockWidget* GetDockWidget(QWidget *i_pWindow);

	//---------------------------------------------------------------------------
	// A Perspective is a string that represents the configuration of panes
	//	and toolbars. Using these functions the layout can be saved and
	//	restored.
	//---------------------------------------------------------------------------
	bool GetCurrentPerspective(std::string& o_Perspective);
	void SetCurrentPerspective(const std::string& i_Perspective);
}

#endif // USE_QT
