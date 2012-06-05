/*****************************************************************************
**	fcuiPanel.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcui/qtGUI/fcuiPanel.hpp"

#include "FCSupport/anim/qtGUI/animCategoryPixmap.hpp"
#include "FCSupport/anim/qtGUI/animElementPixmap.hpp"
#include "FCSupport/anim/qtGUI/animMoviePixmap.hpp"
#include "FCSupport/anim/qtGUI/animModePixmap.hpp"
#include "FCSupport/anim/qtGUI/animSubjectPixmap.hpp"
#include "FCSupport/fcdc/fcdcDataMgr.hpp"
#include "FCSupport/fcdc/data/fcdcTimelineItemData.hpp"
#include "FCSupport/fcmd/fcmdConstants.hpp"
#include "FCSupport/fcmd/fcmdModeMgr.hpp"
#include "FCSupport/fcui/fcuiUtils.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"


///---------------------------------------------------------------------------
/// Constructors
///---------------------------------------------------------------------------
fcuiPanel::fcuiPanel(QWidget* i_pParent, 
					 int i_Width, int i_Height,
					 int i_PanelLevel,
					 bool i_bVerticalPanel,
					 bool i_bWideImageRatio)
:	QGraphicsView(i_pParent),
	m_OriginalGridWidth(i_Width),
	m_OriginalGridHeight(i_Height),
	m_GridWidth(i_Width),
	m_GridHeight(i_Height),
	m_pScene(NULL),
	m_pCurrentSelection(NULL),
	m_pCurrentCheckedItem(NULL),
	m_pNextItemToHighlight(NULL),
	m_Offset(0.0f),
	m_ScrollCount(0),
	m_ScrollMax(0),
	m_PanelLevelID(i_PanelLevel),
	m_pScrollButtonHome(NULL),
	m_pScrollButtonEnd(NULL),
	m_bVerticalPanel(i_bVerticalPanel),
	m_bShowCheckboxes(false),
	m_bResetCheck(false)
{
	m_SelectedElementPairs.clear();
	setGeometry(0, 0, i_pParent->width(), i_pParent->height());
	m_SavedGeometry = geometry();
	setFrameStyle(QFrame::NoFrame);
	setFrameShape(QFrame::NoFrame);
	setLineWidth(0);
	setStyleSheet(QString("background-color: rgba(255, 255, 255, 0)"));
	
	// set the Qt flags
	setTransformationAnchor(QGraphicsView::NoAnchor);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setCacheMode(CacheBackground);
	setViewportUpdateMode(FullViewportUpdate);
	setRenderHints( QPainter::Antialiasing |
					QPainter::SmoothPixmapTransform |
					QPainter::TextAntialiasing );

	//Set a background image?
	//setBackgroundBrush(QPixmap(QString::fromUtf8("Media/panel_back.jpg")));
	//create scene
	QRect bounds;
	if(i_bVerticalPanel)
		bounds = QRect(0, 0, width() - 10, height());
	else
		bounds = QRect(0, 0, width(), height());

	m_SceneOriginalGeometry = bounds;
	m_SceneNewGeometry = m_SceneOriginalGeometry;
	m_pScene = new QGraphicsScene(bounds, this);
	m_pScene->setItemIndexMethod(QGraphicsScene::NoIndex);
	setScene(m_pScene);

	//set the tile sizes for this panel
	qreal temp_width = m_pScene->width();
	//we need to make room for the scroll bar of the panel
	if(i_bVerticalPanel)
		temp_width -= 10;
	
	m_TileWidth = temp_width / m_GridWidth;
	m_TileHeight = m_pScene->height() / m_GridHeight;
	
	m_ImageWidth = m_TileWidth;
	m_ImageHeight = m_TileHeight;
	
	if(i_bWideImageRatio)
		m_ImageHeight = fcuiUtils::GetWidescreenHeight(m_ImageWidth);

	SetGrid(m_GridHeight, m_GridWidth);
}

///---------------------------------------------------------------------------
/// Destructors
///---------------------------------------------------------------------------
fcuiPanel::~fcuiPanel()
{
	CleanGrid();
}

///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
void fcuiPanel::CleanGrid()
{
	for (int y = 0; y < m_GridHeight; ++y)
		delete [] m_pGrid[y];
	delete [] m_pGrid;

	for (int y = 0; y < m_GridHeight; ++y)
		delete [] m_pLabelGrid[y];
	delete [] m_pLabelGrid;
}

///---------------------------------------------------------------------------
/// Clear the text from all visible labels
///---------------------------------------------------------------------------
void fcuiPanel::ClearText()
{
	for (int y = 0; y < m_GridHeight; ++y)
		for (int x = 0; x < m_GridWidth; ++x)
			if (m_pLabelGrid[y][x] != NULL)
				m_pLabelGrid[y][x]->setText("");
}

///---------------------------------------------------------------------------
/// Set the size of panel grid
///---------------------------------------------------------------------------
void fcuiPanel::SetGrid(int i_GridHeight, int i_GridWidth, bool i_bCleanGrid)
{
	if( i_bCleanGrid )
		CleanGrid();

	m_GridHeight = i_GridHeight;
	m_GridWidth = i_GridWidth;

	//setup the grid of animPixmapItems
	m_pGrid = new animPixmapItem **[m_GridHeight];
	m_pLabelGrid = new QLabel **[m_GridHeight];
	for ( int y = 0; y < m_GridHeight; ++y )
	{
		m_pGrid[y] = new animPixmapItem *[m_GridWidth];
		m_pLabelGrid[y] = new QLabel *[m_GridWidth];
		for ( int x = 0; x < m_GridWidth; ++x )
		{
			animPixmapItem *item = GetNextPixmapItem();
			item->setPos(GetPositionForLocation(x, y));
			m_pScene->addItem(item);
			m_pGrid[y][x] = item;
			m_pLabelGrid[y][x] = NULL;
		}
	}
}

///-----------------------------------------------------------------------
/// Set a new scroll bar for the panel
///-----------------------------------------------------------------------
void fcuiPanel::SetPanelScrollBar( QScrollBar* i_pNewScrollBar )
{
	if( m_bVerticalPanel )
	{
		this->setVerticalScrollBar(i_pNewScrollBar);
		this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
	else
	{
		this->setHorizontalScrollBar(i_pNewScrollBar);
		this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	}
}

///-----------------------------------------------------------------------
/// Set 2 scroll buttons from the ui as this panel's scroll buttons
///-----------------------------------------------------------------------
void fcuiPanel::SetPanelScrollButtons( QPushButton* i_pToHome, QPushButton* i_pToEnd )
{
	m_pScrollButtonHome = i_pToHome;
	m_pScrollButtonEnd = i_pToEnd;

	QObject::connect(m_pScrollButtonHome, SIGNAL(pressed()), this, SLOT(ScrollHome()));
	QObject::connect(m_pScrollButtonEnd, SIGNAL(pressed()), this, SLOT(ScrollEnd()));
}

///-----------------------------------------------------------------------
/// make the scroll buttons belonging to the panel visible
///-----------------------------------------------------------------------
void fcuiPanel::ShowScrollButtons(bool i_bShow)
{
	if( (m_pScrollButtonHome != NULL) && (m_pScrollButtonEnd != NULL) )
	{
		if(i_bShow)
		{
			m_pScrollButtonHome->show();
			m_pScrollButtonEnd->show();
		}
		else
		{
			m_pScrollButtonHome->hide();
			m_pScrollButtonEnd->hide();
		}
	}
}

///---------------------------------------------------------------------------
/// Given a list of image files, populate the panel with each image as
/// an animPixmapItem
///---------------------------------------------------------------------------
void fcuiPanel::LoadItems( const std::vector<fsLocator>& i_ItemList )
{
	//if necessary, resize the grid
	ClearText(); 
	QScrollBar* scrollBar;
	int decrement = 0;
	if( m_bVerticalPanel )
		scrollBar = this->verticalScrollBar();
	else
		scrollBar = this->horizontalScrollBar();
	scrollBar->setValue(0);

	if( i_ItemList.size() > (m_OriginalGridWidth * m_OriginalGridHeight) )
	{
		int newHeight, newWidth;
		if( m_bVerticalPanel )
		{
			newWidth = m_GridWidth;
			newHeight = ceil((float)i_ItemList.size() / (float)m_GridWidth);
		}
		else
		{
			newHeight = m_GridHeight;
			newWidth = ceil((float)i_ItemList.size() / (float)m_GridHeight);
		}
		QRect bounds = QRect(0, 0, 
		                 newWidth * m_TileWidth,
						 newHeight * m_TileHeight);
		m_pScene->setSceneRect(bounds);
		m_SceneNewGeometry = bounds;
		SetGrid(newHeight, newWidth, true);
		ShowScrollButtons(true);
	}
	else
	{
		m_pScene->setSceneRect(m_SceneOriginalGeometry);
		ShowScrollButtons(false);
	}

	//now place the items in the next available cell of the grid
	m_Index = 0;
	for ( int i = 0; i < i_ItemList.size(); ++i )
	{
		LoadItem(i_ItemList[i]);
	}
}

///---------------------------------------------------------------------------
/// Load an individual file into a specific index of 
/// the panel given the file's info 
///---------------------------------------------------------------------------
void fcuiPanel::LoadItemAtIndex(const fsLocator& i_Item, int i_Index)
{
	m_Index = i_Index;
	LoadItem(i_Item);
}

///---------------------------------------------------------------------------
/// Load an individual file into the panel given the file's FileInfo
///---------------------------------------------------------------------------
void fcuiPanel::LoadItem(const fsLocator& i_Item)
{
	//check if the grid is full
	if ( m_Index >= (m_GridWidth * m_GridHeight) )
		return;

	fsLocator icon_loc = i_Item;
	icon_loc.Push(fcmdConstants::c_Icon);
	if ( !fsFileUtil::FileExists(icon_loc) )
		return;
	itString file_string;
	fsFileUtil::LocatorToUnicodeString(icon_loc, file_string);
	QString file( itStringUtil::GetStdString(file_string).c_str() );

	//create the new pixmap
	QPixmap pixmap(file);
	pixmap = pixmap.scaled(QSize(m_ImageWidth, m_ImageHeight), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	
	//set the item index within the grid
	int y = m_Index / m_GridWidth;
	int x = m_Index % m_GridWidth;

	//set the pixmap
	if ( m_pGrid[y][x] != NULL )
	{
		m_pGrid[y][x]->SetPixmap(pixmap);
		m_pGrid[y][x]->SetLocator(i_Item);
		m_pGrid[y][x]->PerformEnableCheck();
		if( m_pGrid[y][x]->GetHasLabel() )
		{
			if( m_pLabelGrid[y][x] == NULL )
			{
				m_pLabelGrid[y][x] = ShowLabel(m_pGrid[y][x]->pos(), m_pGrid[y][x]->boundingRect());
				if( m_pLabelGrid[y][x] != NULL )
					m_pScene->addWidget(m_pLabelGrid[y][x]);
			}
			
			if( m_pLabelGrid[y][x] != NULL )
				SetLabelText(m_pLabelGrid[y][x], i_Item);
		}
	}
	m_Index++;
}

//----------------------------------------------------------------------------
/// Show the label text associated with this pixmap
//----------------------------------------------------------------------------
QLabel* fcuiPanel::ShowLabel(const QPointF& i_Pos, const QRectF& i_Rect)
{
	QLabel* pLabel = NULL;
	//create the label and initialized its properties
	if( pLabel == NULL )
	{
		pLabel = new QLabel("");
		int label_height = m_TileHeight - i_Rect.height();
		if( label_height < 1 )
			label_height = 20; // in case we want the label to overlay the image

		int label_y = (i_Pos.y() + m_TileHeight) - label_height;
		pLabel->setGeometry(i_Pos.x(), label_y, i_Rect.width(), label_height);

		QFont label_font;
		label_font.setPointSize(10);
		label_font.setBold(true);
		label_font.setFamily("Raavi");
		
		QPalette label_palette;
		QBrush whitebrush(QColor(255, 255, 255, 255));
		whitebrush.setStyle(Qt::SolidPattern);
		label_palette.setBrush(QPalette::Active, QPalette::WindowText, whitebrush); 
		label_palette.setBrush(QPalette::Inactive, QPalette::WindowText, whitebrush);
		label_palette.setBrush(QPalette::Disabled, QPalette::WindowText, whitebrush);

		pLabel->setPalette(label_palette);
		pLabel->setFont(label_font);
		pLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
		pLabel->setStyleSheet(QString("background-color: rgba(255, 255, 255, 0)"));
	}

	return pLabel;
}

///---------------------------------------------------------------------------
///---------------------------------------------------------------------------
void fcuiPanel::SetLabelText( QLabel* i_pLabel, const fsLocator& i_Locator )
{
	//update the text of the label
	itString str_name = i_Locator.GetLastName();
	QString item_name( itStringUtil::GetStdString(str_name).c_str() );

	i_pLabel->setText(item_name);
}

///---------------------------------------------------------------------------
/// Animate the panels, clear them, reload the images, animate panels back
///---------------------------------------------------------------------------
void fcuiPanel::ReLoadItems( const std::vector<fsLocator>& i_ItemList )
{
	m_CurrentItemList = i_ItemList;
	DoClearAnimationStart();
}

//------------------------------------------------------------------------
/// Scroll to the top of the panel
//------------------------------------------------------------------------
void fcuiPanel::ScrollHome()
{
	QScrollBar* scrollBar;
	int decrement = 0;
	if( m_bVerticalPanel )
	{
		scrollBar = verticalScrollBar();
		decrement = -m_TileHeight;
	}
	else
	{
		scrollBar = horizontalScrollBar();
		decrement = -m_TileWidth;
	}

	int val = scrollBar->value();
	int newVal = val + decrement;

	//animate the slide
	QPropertyAnimation *animation = new QPropertyAnimation(scrollBar, "sliderPosition");
	animation->setDuration(500);
	animation->setStartValue(val);
	animation->setEndValue(newVal);
	animation->setEasingCurve(QEasingCurve::Linear);
	animation->start(QPropertyAnimation::DeleteWhenStopped);
}

//------------------------------------------------------------------------
/// Scroll to the end of the panel
//------------------------------------------------------------------------
void fcuiPanel::ScrollEnd()
{
	QScrollBar* scrollBar;
	int increment = 0;
	if( m_bVerticalPanel )
	{
		scrollBar = verticalScrollBar();
		increment = m_TileHeight;
	}
	else
	{
		scrollBar = horizontalScrollBar();
		increment = m_TileWidth;
	}

	int val = scrollBar->value();
	int newVal = val + increment;

	//animate the slide
	QPropertyAnimation *animation = new QPropertyAnimation(scrollBar, "sliderPosition");
	animation->setDuration(500);
	animation->setStartValue(val);
	animation->setEndValue(newVal);
	animation->setEasingCurve(QEasingCurve::Linear);
	animation->start(QPropertyAnimation::DeleteWhenStopped);
}

///---------------------------------------------------------------------------
/// After the drop animation, Clear the grid of any images, load the new images
/// and play the drop-in animation
///---------------------------------------------------------------------------
void fcuiPanel::ClearOut()
{
	ClearItems();
	LoadItems(m_CurrentItemList);
	DoClearAnimationEnd();
	m_CurrentItemList.clear();
}

///---------------------------------------------------------------------------
/// Clear the grid of any images
///---------------------------------------------------------------------------

void fcuiPanel::ClearItems()
{
	//remove the highlight of the previous selection
	if(m_pCurrentSelection != NULL)
	{
		m_pCurrentSelection->RemoveHighlight();
	}

	QPixmap empty_pixmap;
	for ( int y = 0; y < m_GridHeight; ++y )
	{	
		for ( int x = 0; x < m_GridWidth; ++x )
		{
			m_pGrid[y][x]->SetPixmap(empty_pixmap);
			m_pGrid[y][x]->SetLocator(fsLocator());
		}
	}
}

///---------------------------------------------------------------------------
/// Hide the panel through animation
///---------------------------------------------------------------------------
void fcuiPanel::DoHideAnimation()
{
	QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
	int duration = 500;
	QRect start_val = m_SavedGeometry;
	QRect end_val = m_SavedGeometry;
    
	animation->setEasingCurve(QEasingCurve::Linear);

	//determine the new geometry for the animation
	switch((fcmdModeMgr::ModeLevel)m_PanelLevelID)
	{	
	case fcmdModeMgr::e_Mode:
		break;
	case fcmdModeMgr::e_Subject:
		break;
	case fcmdModeMgr::e_Category:
		break;
	case fcmdModeMgr::e_Element:
		break;
	case fcmdModeMgr::e_HomeScreen:
		end_val.setY( (m_SavedGeometry.y() - 200) );
		animation->setEasingCurve(QEasingCurve::InBack);
		setBackgroundBrush(QPixmap());
		break;
	default:
		break;
	}
	
	animation->setDuration(duration);
    animation->setStartValue(start_val);
	animation->setEndValue(end_val);

	animation->start(QPropertyAnimation::DeleteWhenStopped);
	m_NewGeometry = end_val;
}

///---------------------------------------------------------------------------
/// Animate the panel before removing any items
///---------------------------------------------------------------------------
void fcuiPanel::DoClearAnimationStart()
{
	QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
	int duration = 500;
	QRect start_val = m_SavedGeometry;
	QRect end_val = m_SavedGeometry;
    
	animation->setEasingCurve(QEasingCurve::Linear);

	//determine the new geometry for the animation
	switch((fcmdModeMgr::ModeLevel)m_PanelLevelID)
	{	
	case fcmdModeMgr::e_Mode:
		break;
	case fcmdModeMgr::e_Subject:
		end_val.setY( (m_SavedGeometry.y() + 50) );
		break;
	case fcmdModeMgr::e_Category:
		end_val.setY( (m_SavedGeometry.y() + 40) );
		break;
	case fcmdModeMgr::e_Element:
		end_val.setX( (m_SavedGeometry.x() + 300) );
		animation->setEasingCurve(QEasingCurve::InBack);
		break;
	case fcmdModeMgr::e_HomeScreen:
		duration = 1;
		start_val.setY( (m_SavedGeometry.y() - 200) );
		end_val = start_val;
		setBackgroundBrush(QPixmap());
		break;
	default:
		break;
	}
	
	animation->setDuration(duration);
    animation->setStartValue(start_val);
	animation->setEndValue(end_val);

	animation->start(QPropertyAnimation::DeleteWhenStopped);
    QObject::connect(animation, SIGNAL(finished()), this, SLOT(ClearOut()));

	m_NewGeometry = end_val;
}

///---------------------------------------------------------------------------
/// Animate the panel beore removing any items
///---------------------------------------------------------------------------
void fcuiPanel::DoClearAnimationEnd()
{
	QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    int duration = 500;
	QRect start_val = m_NewGeometry;
	QRect end_val = m_SavedGeometry;
	animation->setEasingCurve(QEasingCurve::Linear);

	//determine the new geometry for the animation
	switch((fcmdModeMgr::ModeLevel)m_PanelLevelID)
	{	
	case fcmdModeMgr::e_Mode:
		break;
	case fcmdModeMgr::e_Subject:
		animation->setEasingCurve(QEasingCurve::OutBounce);
		break;
	case fcmdModeMgr::e_Category:
		animation->setEasingCurve(QEasingCurve::OutBounce);
		break;
	case fcmdModeMgr::e_Element:
		animation->setEasingCurve(QEasingCurve::OutBounce);
		break;
	case fcmdModeMgr::e_HomeScreen:
		animation->setEasingCurve(QEasingCurve::OutBounce);
		break;
	default:
		break;
	}
	
	animation->setDuration(duration);
    animation->setStartValue(start_val);
	animation->setEndValue(end_val);
	animation->start(QPropertyAnimation::DeleteWhenStopped);

	QObject::connect(animation, SIGNAL(finished()), this, SLOT(FinishClearAnim()));

	if( m_pNextItemToHighlight != NULL )
	{
		UpdateSelection(m_pNextItemToHighlight);
		m_pNextItemToHighlight = NULL;
	}

	if( m_NextItemToSelect.GetNumNames() > 0 )
	{
		animPixmapItem* elem = GetPixmapByLocator(m_NextItemToSelect);
		if( elem != NULL )
			UpdateSelection(elem);
		m_NextItemToSelect.Clear();
	}
}

///-----------------------------------------------------------------------
/// Take care of any final operations that need to happen once the reset anim
/// is completely done.
///-----------------------------------------------------------------------
void fcuiPanel::FinishClearAnim()
{
	if((fcmdModeMgr::ModeLevel)m_PanelLevelID == fcmdModeMgr::e_HomeScreen)
	{
		//setBackgroundBrush(QPixmap(QString::fromUtf8("Media/GUI/bottomBAR_home_element.png")));
	}

	if(m_bShowCheckboxes)
	{
		ShowItemCheckboxes();
		m_bShowCheckboxes = false;
	}
	if(m_bResetCheck)
	{
		ResetCheckedItem(m_ResetLocator);
		m_bResetCheck = false;
		m_ResetLocator.Clear();
	}
}

///-----------------------------------------------------------------------
/// Highlight the item at the current index
///-----------------------------------------------------------------------
void fcuiPanel::HighlightItemAtIndex( int i_Index, bool i_bHighlightAfterClear )
{
	//set the item index within the grid
	int y = i_Index / m_GridWidth;
	int x = i_Index % m_GridWidth;
	m_pNextItemToHighlight = NULL;

	if ( (m_pGrid[y][x] != NULL) && !i_bHighlightAfterClear)
	{
		UpdateSelection(m_pGrid[y][x]);
	}
	else if (m_pGrid[y][x] != NULL)
	{
		m_pNextItemToHighlight = m_pGrid[y][x];
	}
}

//------------------------------------------------------------------------
// depending on the panel ID, return a pointer of the proper pixmap item type
//------------------------------------------------------------------------
animPixmapItem* fcuiPanel::GetNextPixmapItem()
{
	animPixmapItem* new_item = NULL;
	//determine the new geometry for the animation
	switch((fcmdModeMgr::ModeLevel)m_PanelLevelID)
	{	
	case fcmdModeMgr::e_Mode:
		new_item = new animModePixmap(this, m_PanelLevelID, m_ImageWidth/2, m_ImageHeight/2);
		break;
	case fcmdModeMgr::e_Subject:
		new_item = new animSubjectPixmap(this, m_PanelLevelID, m_ImageWidth/2, m_ImageHeight/2);
		break;
	case fcmdModeMgr::e_Category:
		new_item = new animCategoryPixmap(this, m_PanelLevelID, m_ImageWidth/2, m_ImageHeight/2);
		break;
	case fcmdModeMgr::e_Element:
		new_item = new animElementPixmap(this, m_PanelLevelID, m_ImageWidth/2, m_ImageHeight/2);
		break;
	case fcmdModeMgr::e_HomeScreen:
		new_item = new animMoviePixmap(this, m_PanelLevelID, m_ImageWidth/2, m_ImageHeight/2);
		break;
	default:
		new_item = new animPixmapItem(this, m_PanelLevelID, m_ImageWidth/2, m_ImageHeight/2);
		break;
	}
	return new_item;
}

///---------------------------------------------------------------------------
/// Return the animPixmap in the panel that has the matching locator
///---------------------------------------------------------------------------
animPixmapItem* fcuiPanel::GetPixmapByLocator(const fsLocator& i_Locator)
{
	for ( int y = 0; y < m_GridHeight; ++y )
	{	
		for ( int x = 0; x < m_GridWidth; ++x )
		{
			if((m_pGrid[y][x] != NULL) && 
			   (m_pGrid[y][x]->GetLocator().GetNumNames() > 0))
			{
				if(	m_pGrid[y][x]->GetLocator() == i_Locator )
					return m_pGrid[y][x];
			}
		}
	}
	return NULL;
}

///---------------------------------------------------------------------------
/// Certain images may need an ID, so we need to set that here.
///---------------------------------------------------------------------------
void fcuiPanel::SetItemID(int i_Index, int i_ItemID)
{
	//set the item index within the grid
	int y = i_Index / m_GridWidth;
	int x = i_Index % m_GridWidth;

	if (m_pGrid[y][x])
		m_pGrid[y][x]->SetItemID(i_ItemID);
}

///---------------------------------------------------------------------------
/// Set the locator to a specific item
///---------------------------------------------------------------------------
void fcuiPanel::SetItemLocator(int i_Index, const fsLocator& i_Locator)
{
	//set the item index within the grid
	int y = i_Index / m_GridWidth;
	int x = i_Index % m_GridWidth;

	if (m_pGrid[y][x])
		m_pGrid[y][x]->SetLocator(i_Locator);
}

//----------------------------------------------------------------------------
/// Update panel Selection
//----------------------------------------------------------------------------
void fcuiPanel::UpdateSelection(animPixmapItem* i_pNewSelection)
{
	//remove the highlight of the previous selection
	if(m_pCurrentSelection != NULL)
	{
		m_pCurrentSelection->RemoveHighlight();
	}
	m_pCurrentSelection = i_pNewSelection;

	//Highlight the new selected item
	m_pCurrentSelection->HighlightImage();
	m_pCurrentSelection->DoSelection();
}

//------------------------------------------------------------------------
/// If an animPixmap has it's checkbox selected, update the selection in 
/// the panel.
//------------------------------------------------------------------------
void fcuiPanel::UpdateCheckedItem(animPixmapItem* i_pNewCheckedItem, bool i_bChecked)
{
	if( !i_bChecked && (i_pNewCheckedItem == m_pCurrentCheckedItem) )
	{
		//if current checked item is turned off, reset/remove the effects
		return;
	}
	else if( !i_bChecked )
		return;   // don't do anything if the item is unknown and not checked, should never happen but just a precaution

	if(m_pCurrentCheckedItem != NULL)
	{
		m_pCurrentCheckedItem->RemoveCheckboxCheck();
	}
	m_pCurrentCheckedItem = i_pNewCheckedItem;
	if( fcmdModeMgr::Instance != NULL )
	{
		fcmdModeMgr::Instance->ProcessCheckedElement(m_pCurrentCheckedItem->GetLocator());
	}
}

//------------------------------------------------------------------------
/// When the panel changes, we need to reset the checkbox to the current item
//------------------------------------------------------------------------
void fcuiPanel::ResetCheckedItem(const fsLocator& i_Locator)
{
	// uncheck the current item
	if(m_pCurrentCheckedItem != NULL)
	{
		m_pCurrentCheckedItem->RemoveCheckboxCheck();
		m_pCurrentCheckedItem = NULL;
	}
	if(i_Locator.GetNumNames() > 0)
	{
		m_pCurrentCheckedItem = GetPixmapByLocator(i_Locator);
		if(m_pCurrentCheckedItem != NULL)
			m_pCurrentCheckedItem->ShowCheckboxCheck();
	}
}

//------------------------------------------------------------------------
/// If we need to show the checkboxes after the panel animates, we need to 
/// se the state
//------------------------------------------------------------------------
void fcuiPanel::PrepareShowCheckboxItems()
{
	m_bShowCheckboxes = true;
}

//------------------------------------------------------------------------
/// If we need to reset the checkboxes after the panel animates, we need to 
/// set the state
//------------------------------------------------------------------------
void fcuiPanel::PrepareResetCheck(const fsLocator& i_Locator)
{
	m_ResetLocator = i_Locator;
	m_bResetCheck = true;
}

//------------------------------------------------------------------------
/// Make the checkboxes for the elements visible
//------------------------------------------------------------------------
void fcuiPanel::ShowItemCheckboxes()
{
	for ( int y = 0; y < m_GridHeight; ++y )
	{	
		for ( int x = 0; x < m_GridWidth; ++x )
		{
			if((m_pGrid[y][x] != NULL) && 
			   (m_pGrid[y][x]->GetLocator().GetNumNames() > 0))
			{
				m_pGrid[y][x]->ShowCheckbox();
			}
		}
	}
}

//------------------------------------------------------------------------
/// Hide the checkboxes if they are visible
//------------------------------------------------------------------------
void fcuiPanel::HideItemCheckboxes()
{
	for ( int y = 0; y < m_GridHeight; ++y )
	{	
		for ( int x = 0; x < m_GridWidth; ++x )
		{
			if(m_pGrid[y][x] != NULL)
				m_pGrid[y][x]->HideCheckbox();
		}
	}
}

///---------------------------------------------------------------------------
/// return the position of the tile, given the x,y location within the 
/// panel's grid
///---------------------------------------------------------------------------
QPointF fcuiPanel::GetPositionForLocation(int i_X, int i_Y) const
{
	QPointF pos = QPointF(i_X * m_TileWidth, i_Y * m_TileHeight);
	return pos;
}

//----------------------------------------------------------------------------
/// Get the offset position of the panel
//----------------------------------------------------------------------------
float fcuiPanel::GetOffset()
{
	return m_Offset;
}

//----------------------------------------------------------------------------
/// Set the offset position of the panel
//----------------------------------------------------------------------------
void fcuiPanel::SetOffset(float i_Offset)
{
	m_Offset = i_Offset;
	QMatrix matrix;
    matrix.translate(0.0f, m_Offset);
    setMatrix(matrix);
    update();
}

//----------------------------------------------------------------------------
/// Get the tile width
//----------------------------------------------------------------------------
int fcuiPanel::GetTileWidth()
{
	return m_TileWidth;
}

//----------------------------------------------------------------------------
/// Get the tile height
//----------------------------------------------------------------------------
int fcuiPanel::GetTileHeight()
{
	return m_TileHeight;
}

//----------------------------------------------------------------------------
/// Get the tile width
//----------------------------------------------------------------------------
int fcuiPanel::GetImageWidth()
{
	return m_ImageWidth;
}

//----------------------------------------------------------------------------
/// Get the tile height
//----------------------------------------------------------------------------
int fcuiPanel::GetImageHeight()
{
	return m_ImageHeight;
}

//----------------------------------------------------------------------------
/// Update the selected element pairing for the panel, used only by 
/// element panel
//----------------------------------------------------------------------------
void fcuiPanel::UpdateSelectedItemPair(const fsLocator& i_Category, const fsLocator& i_SelectedElement)
{
	bool category_exists = false;
	fcdcTimelineItemData temp = fcdcDataMgr::GetData();
	int size = temp.m_HighlightData.m_HighlightItems.size();
	for (int i = 0; i < size; i++)
	{
		if(temp.m_HighlightData.m_HighlightItems[i].m_categorypath.GetValue() == i_Category)
		{
			temp.m_HighlightData.m_HighlightItems[i].m_elementpath.SetValue(i_SelectedElement);
			category_exists = true;
		}
	}
	if(!category_exists)
	{
		fcdcHighlightItem newitem;
		newitem.m_categorypath.SetValue(i_Category);
		newitem.m_elementpath.SetValue(i_SelectedElement);
		temp.m_HighlightData.m_HighlightItems.push_back(newitem);
	}
	fcdcDataMgr::SetData(temp);
	m_SelectedElementPairs[i_Category] = i_SelectedElement;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcuiPanel::Initialize() 
{
	fcdcTimelineItemData temp = fcdcDataMgr::GetData();
	int size = temp.m_HighlightData.m_HighlightItems.size();
	for (int i = 0; i < size; i++)
	{
		m_SelectedElementPairs[temp.m_HighlightData.m_HighlightItems[i].m_categorypath.GetValue()] = temp.m_HighlightData.m_HighlightItems[i].m_elementpath.GetValue() ;
	}

}
//----------------------------------------------------------------------------
/// Given the category, select the appropriate element in the panel
//----------------------------------------------------------------------------
const fsLocator& fcuiPanel::SelectItemPair(const fsLocator& i_Category)
{
	m_NextItemToSelect.Clear();
	std::map<fsLocator, fsLocator>::iterator it, end = m_SelectedElementPairs.end();
	it = m_SelectedElementPairs.find(i_Category);
	if( it != end )
	{
		m_NextItemToSelect = m_SelectedElementPairs[i_Category];	
	}
	return m_NextItemToSelect;
}