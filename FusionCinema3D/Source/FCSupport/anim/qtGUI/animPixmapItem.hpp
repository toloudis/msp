/*****************************************************************************
**	animPixmapItem.hpp
**
**		a Qt image that will act as a button and process events when clicked.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef ANIM_PIXMAPITEM_HPP
#error animPixmapItem.hpp multiply included
#endif
#define ANIM_PIXMAPITEM_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
//#ifndef FCUI_PANEL_HPP
//#include "FCSupport/fcui/qtGUI/fcuiPanel.hpp"
//#endif

#include <QGraphicsPixmapItem>
#include <QtGui/QtGui>

//============================================================================
// class forwards
//============================================================================
class QPropertyAnimation;
class fcuiPanel;

//============================================================================
//============================================================================
class animPixmapItem : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT

	//Properties used to animate the pixmap item
	Q_PROPERTY(qreal rotation READ getRotate WRITE setRotate)
	Q_PROPERTY(qreal scale READ GetScale WRITE SetScale)


public:

	//------------------------------------------------------------------------
	/// Pass in parent widget which is the containing panel
	/// also pass in the operation level for this item
	//------------------------------------------------------------------------
    animPixmapItem(fcuiPanel* i_pParentPanel, int i_ModeLevel, double i_TileWidth, double i_TileHeight);
	~animPixmapItem();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	qreal getRotate();
	void setRotate(qreal r);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	qreal GetScale();
	void SetScale(qreal s);

	///-----------------------------------------------------------------------
	/// Set the locator that is associated with this anim pixmap
	///-----------------------------------------------------------------------
	virtual void SetLocator(const fsLocator& i_Locator);

	///-----------------------------------------------------------------------
	/// Get the locator that is associated with this anim pixmap
	///-----------------------------------------------------------------------
	const fsLocator& GetLocator();

	///-----------------------------------------------------------------------
	/// Set the ID for this pixmap if necessary
	///-----------------------------------------------------------------------
	void SetItemID(const int& i_ItemID);

	///-----------------------------------------------------------------------
	/// Get the ID for this pixmap
	///-----------------------------------------------------------------------
	const int& GetItemID();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	QWidget* GetParent();

	//------------------------------------------------------------------------
	/// paint overrides parent paint function and currently sets a border 
	/// around the pixmap image
	//------------------------------------------------------------------------
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetHighlightColor(const QColor& i_Color);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const QColor& GetHighlightColor();

	//------------------------------------------------------------------------
	/// Highlight the image
	//------------------------------------------------------------------------
	virtual void HighlightImage(int i_Duration = 100, float i_EndStrength = 1.0f);

	//------------------------------------------------------------------------
	/// Use an animation to remove the highlight
	//------------------------------------------------------------------------
	virtual void AnimRemoveHighlightImage(int i_Duration = 100, float i_StartStrength = 1.0f);

	//------------------------------------------------------------------------
	/// Remove any color effect placed on the image
	//------------------------------------------------------------------------
	virtual void RemoveHighlight();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void SetHasLabel(bool i_bHasLabel);
	virtual bool GetHasLabel();

	//------------------------------------------------------------------------
	/// If there is some special operation to do when the pixmap is selected,
	/// do that here
	//------------------------------------------------------------------------
	virtual void DoSelection();
	
	//------------------------------------------------------------------------
	/// Get/set the selected state of the pixmap
	//------------------------------------------------------------------------
	bool IsSelected();
	void SetSelected(bool i_bSelected);

	//------------------------------------------------------------------------
	/// Enable the checkbox of the pixmap if it requires one
	//------------------------------------------------------------------------
	void ShowCheckbox();

	//----------------------------------------------------------------------------
	/// Show the label text associated with this pixmap
	//----------------------------------------------------------------------------
	void ShowLabel();

	//------------------------------------------------------------------------
	/// Hide/remove the checkbox from the animpixmap
	//------------------------------------------------------------------------
	void HideCheckbox();

	//----------------------------------------------------------------------------
	/// Hide the pixmap's label
	//----------------------------------------------------------------------------
	void HideLabel();

	//------------------------------------------------------------------------
	/// show the checkbox check selection from the animpixmap
	//------------------------------------------------------------------------
	void ShowCheckboxCheck();

	//------------------------------------------------------------------------
	/// remove the checkbox check selection from the animpixmap
	//------------------------------------------------------------------------
	void RemoveCheckboxCheck();

	//------------------------------------------------------------------------
	/// Perform a shake effect on the item
	//------------------------------------------------------------------------
	void Shake();
	
	//------------------------------------------------------------------------
	/// Do the scale animation 
	//------------------------------------------------------------------------
	void Scale(float i_ScaleAmount, int i_Duration);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void flip(QGraphicsSceneHoverEvent  *event);
	
	// Virtual method overrides
 	void hoverEnterEvent(QGraphicsSceneHoverEvent  *event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent  *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	//------------------------------------------------------------------------
	/// Checks if the item should be enabled, if not, it will disable it
	/// and grey it out.  The item will not respond to button clicks
	/// Sub-classes must override
	//------------------------------------------------------------------------
	virtual void PerformEnableCheck(){};

	//------------------------------------------------------------------------
	/// will call the base class setPixmap function, but also allow child classes
	/// to perform custom steps before the pixmap is set
	//------------------------------------------------------------------------
	virtual void SetPixmap(const QPixmap &i_Pixmap); 

	//------------------------------------------------------------------------
	/// Get the tile width/height
	//------------------------------------------------------------------------
	qreal GetTileWidth();
	qreal GetTileHeight();

private slots:
	void anim_rotate();
	void anim_scale();
	void clean_rotate_anim();
	void clean_scale_anim();
	void StartAnimScale();
	void StartAnimUnScale();
	void ProcessCheck(int i_NewState);
	void clean_highlight();

private:
	QTimer *m_pTimer;
	QColor m_HighlightColor;
	fcuiPanel* m_pParentPanel;
	qreal m_myRotate;
	qreal m_myScale;
	Qt::Axis m_axis;
	static int s_zBackup;
	static qreal s_xPosDown;
	static qreal s_yPosDown;
	static bool s_isDragDrop;

	int m_ModeLevel;
	int m_ItemID;
	qreal m_TileWidth;
	qreal m_TileHeight;
	double m_rotateAmount;
	int m_RotationTime;
	int m_RotateLoopCount;
	double m_ScaleAmount;
	int m_ScaleTime;
	int m_ScaleLoopCount;
	QPropertyAnimation* m_pRotateAnimation;
	QPropertyAnimation* m_pScaleAnimation;
	QPropertyAnimation* m_pColorAnimation;
	fsLocator m_Locator;
	QCheckBox* m_pCheckbox;
	QLabel* m_pLabel;
	bool m_bSelected;
	bool m_bHasLabel;
signals:
    void rotated();
    void rotationFinish();
};
