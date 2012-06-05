/*****************************************************************************
**	fcuiPanel.hpp
**
**		the panel of the main window that will hold various animated icons
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCUI_PANEL_HPP
#error fcuiPanel.hpp multiply included
#endif
#define FCUI_PANEL_HPP

#ifndef ANIM_PIXMAPITEM_HPP
#include "FCSupport/anim/qtGUI/animPixmapItem.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <QtGui>
#include <vector>
#include <map>

//============================================================================
//============================================================================
class fcuiPanel : public QGraphicsView
{
	Q_OBJECT
	
	//------------------------------------------------------------------------
	// Property function used to offset the position of the panel
	//------------------------------------------------------------------------
	Q_PROPERTY(float offset READ GetOffset WRITE SetOffset)

public:
	///-----------------------------------------------------------------------
	/// Constructors
	///-----------------------------------------------------------------------
	fcuiPanel(QWidget* i_pParent, 
			  int i_GridWidth, int i_GridHeight, 
			  int i_PanelLevel, bool i_bVerticalPanel = false,
			  bool i_bWideImageRatio = false);
	
	///-----------------------------------------------------------------------
	/// Destructors
	///-----------------------------------------------------------------------
	~fcuiPanel();

	///-----------------------------------------------------------------------
	///-----------------------------------------------------------------------
	void CleanGrid();

	///-----------------------------------------------------------------------
	/// Clear the text from all visible labels
	///-----------------------------------------------------------------------
	void ClearText();

	///-----------------------------------------------------------------------
	///-----------------------------------------------------------------------
	void SetGrid(int i_GridHeight, int i_GridWidth, bool i_bCleanGrid = false);

	///-----------------------------------------------------------------------
	/// Set a new scroll bar for the panel
	///-----------------------------------------------------------------------
	void SetPanelScrollBar( QScrollBar* i_pNewScrollBar );

	///-----------------------------------------------------------------------
	/// Set 2 scroll buttons from the ui as this panel's scroll buttons
	///-----------------------------------------------------------------------
	void SetPanelScrollButtons( QPushButton* i_pToHome, QPushButton* i_pToEnd );
	
	///-----------------------------------------------------------------------
	/// make the scroll buttons belonging to the panel visible
	///-----------------------------------------------------------------------
	void ShowScrollButtons(bool i_bShow = true);

	///-----------------------------------------------------------------------
	/// Given a list of image files, populate the panel with each image as
	/// an animPixmapItem
	///-----------------------------------------------------------------------
	void LoadItems( const std::vector<fsLocator>& i_ItemList );

	///-----------------------------------------------------------------------
	/// Load an individual file into a specific index of 
	/// the panel given the file's info 
	///-----------------------------------------------------------------------
	void LoadItemAtIndex(const fsLocator& i_Item, int i_Index);

	///-----------------------------------------------------------------------
	/// Animate the panels, clear them, reload the images, animate panels back
	///-----------------------------------------------------------------------
	void ReLoadItems( const std::vector<fsLocator>& i_ItemList );

	///-----------------------------------------------------------------------
	/// Clear the grid of any images, initiates clear animation
	///-----------------------------------------------------------------------
	void ClearItems();

	///-----------------------------------------------------------------------
	/// Highlight the item at the current index
	///-----------------------------------------------------------------------
	void HighlightItemAtIndex( int i_Index, bool i_bHighlightAfterClear = false );

	///-----------------------------------------------------------------------
	/// Certain images may need an ID, so we need to set that here.
	///-----------------------------------------------------------------------
	void SetItemID(int i_Index, int i_ItemID);

	///-----------------------------------------------------------------------
	/// Set the locator to a specific item
	///-----------------------------------------------------------------------
	void SetItemLocator(int i_Index, const fsLocator& i_Locator);

	//------------------------------------------------------------------------
	/// Update panel Selection
	//------------------------------------------------------------------------
	void UpdateSelection(animPixmapItem* i_pNewSelection);

	//------------------------------------------------------------------------
	/// If an animPixmap has it's checkbox selected, update the selection in 
	/// the panel.
	//------------------------------------------------------------------------
	void UpdateCheckedItem(animPixmapItem* i_pNewCheckedItem, bool i_bChecked = true);

	//------------------------------------------------------------------------
	/// When the panel changes, we need to reset the checkbox to the current item
	//------------------------------------------------------------------------
	void ResetCheckedItem(const fsLocator& i_Locator);

	//------------------------------------------------------------------------
	/// If we need to show the checkboxes after the panel animates, we need to 
	/// se the state
	//------------------------------------------------------------------------
	void PrepareShowCheckboxItems();

	//------------------------------------------------------------------------
	/// If we need to reset the checkboxes after the panel animates, we need to 
	/// set the state
	//------------------------------------------------------------------------
	void PrepareResetCheck(const fsLocator& i_Locator);

	//------------------------------------------------------------------------
	/// Make the checkboxes of the current elements visible
	//------------------------------------------------------------------------
	void ShowItemCheckboxes();

	//------------------------------------------------------------------------
	/// Hide the checkboxes if they are visible
	//------------------------------------------------------------------------
	void HideItemCheckboxes();

	//------------------------------------------------------------------------
	/// Hide the panel through animation
	//------------------------------------------------------------------------
	void DoHideAnimation();

	//------------------------------------------------------------------------
	/// Get the tile width/height
	//------------------------------------------------------------------------
	int GetTileWidth();
	int GetTileHeight();

	//------------------------------------------------------------------------
	/// Get the tile image width/height
	//------------------------------------------------------------------------
	int GetImageWidth();
	int GetImageHeight();

	//------------------------------------------------------------------------
	/// Update the selected element pairing for the panel, used only by 
	/// element panel
	//------------------------------------------------------------------------
	void UpdateSelectedItemPair(const fsLocator& i_Category, const fsLocator& i_SelectedElement);

	//------------------------------------------------------------------------
	/// Given the category, select the appropriate element in the panel
	//------------------------------------------------------------------------
	const fsLocator& SelectItemPair(const fsLocator& i_Category);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Initialize();

private:
	///-----------------------------------------------------------------------
	/// Load an individual file into the panel given the file's FileInfo
	///-----------------------------------------------------------------------
	void LoadItem(const fsLocator& i_Item);

	//----------------------------------------------------------------------------
	/// Show the label text associated with this pixmap
	//----------------------------------------------------------------------------
	QLabel* ShowLabel(const QPointF& i_Pos, const QRectF& i_Rect);

	///---------------------------------------------------------------------------
	///---------------------------------------------------------------------------
	void SetLabelText( QLabel* i_pLabel, const fsLocator& i_Locator );

	///-----------------------------------------------------------------------
	/// return the position of the tile, given the x,y location within the 
	/// panel's grid
	///-----------------------------------------------------------------------
	QPointF GetPositionForLocation(int i_X, int i_Y) const;

	//------------------------------------------------------------------------
	/// Get the offset position of the panel
	//------------------------------------------------------------------------
	float GetOffset();

	//------------------------------------------------------------------------
	/// Set the offset position of the panel
	//------------------------------------------------------------------------
	void SetOffset(float i_Offset);

	//------------------------------------------------------------------------
	/// Animate the panel beore removing any items
	//------------------------------------------------------------------------
	void DoClearAnimationStart();

	//------------------------------------------------------------------------
	/// Animate the panel beore removing any items
	//------------------------------------------------------------------------
	void DoClearAnimationEnd();

	//------------------------------------------------------------------------
	// depending on the panel ID, return a pointer of the proper pixmap item type
	//------------------------------------------------------------------------
	animPixmapItem* GetNextPixmapItem();

	//------------------------------------------------------------------------
	/// Return the animPixmap in the panel that has the matching locator
	//------------------------------------------------------------------------
	animPixmapItem* GetPixmapByLocator(const fsLocator& i_Locator);

	

private slots:
	//------------------------------------------------------------------------
	/// Do the action clearing of the panel
	//------------------------------------------------------------------------
	void ClearOut();

	///-----------------------------------------------------------------------
	/// Take care of any final operations that need to happen once the reset anim
	/// is completely done.
	///-----------------------------------------------------------------------
	void FinishClearAnim();

	//------------------------------------------------------------------------
	/// Scroll the view towards the proper direction
	//------------------------------------------------------------------------
	void ScrollHome();
	void ScrollEnd();

private:
	int m_OriginalGridWidth;
	int m_OriginalGridHeight;
	int m_GridWidth;
	int m_GridHeight;
	int m_Index;
	int m_TileWidth;
	int m_TileHeight;
	int m_ImageWidth;
	int m_ImageHeight;
	int m_PanelLevelID;
	int m_ScrollCount;
	int m_ScrollMax;
	float m_Offset;
	bool m_bVerticalPanel;
	bool m_bShowCheckboxes;
	bool m_bResetCheck;
	fsLocator m_ResetLocator;
	QRect m_SavedGeometry;
	QRect m_NewGeometry;
	QRect m_SceneNewGeometry;
	QRect m_SceneOriginalGeometry;
	std::vector<fsLocator> m_CurrentItemList;

	QGraphicsScene* m_pScene;
	animPixmapItem* m_pCurrentSelection;
	animPixmapItem* m_pCurrentCheckedItem;
	animPixmapItem* m_pNextItemToHighlight;
	animPixmapItem*** m_pGrid;
	QLabel*** m_pLabelGrid;
	fsLocator m_NextItemToSelect;
	QPushButton* m_pScrollButtonHome;
	QPushButton* m_pScrollButtonEnd;
	std::map<fsLocator, fsLocator> m_SelectedElementPairs;

};