/*****************************************************************************
**	fcuiButtonLayout.hpp
**
**		Frame that contains a list of buttons
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCUI_BUTTONLAYOUT_HPP
#error fcuiButtonLayout.hpp multiply defined
#endif
#define FCUI_BUTTONLAYOUT_HPP

#include <QFrame>
#include <QtGui>

//============================================================================
//============================================================================
class fsLocator;
class fcuiTimelineItemData;

//============================================================================
//============================================================================
class fcuiButtonLayout : public QFrame
{
	Q_OBJECT

public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	fcuiButtonLayout(QWidget* i_pParent);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~fcuiButtonLayout();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetupCameraLayout(int i_NumCameras);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void PopulateCameraButtons(fcuiTimelineItemData& io_ItemData, 
							   const std::vector<fsLocator>& i_ButtonDirectories, 
							   void (*i_CallbackFunction)(fcuiTimelineItemData& io_ItemData, const fsLocator& i_Locator ));

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void RemoveButtons();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ShowLayout();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void HideLayout();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ResizeLayout();

private slots:
	void CloseLayout();

private:
	QWidget* m_pTopBorder;
	QWidget* m_pParent;
	QPushButton* m_pCloseButton;
	QWidget* m_pContainer;
	QPoint m_ContainerPosition;
	QSize m_ContainerSize;
	QSize m_ButtonSize;
	int m_ContainerPadTop;
	int m_ContainerPadBottom;
	int m_ContainerPadLeft;
	int m_ContainerPadRight;
	std::vector<QPushButton*> m_Buttons;
	std::vector<QLabel*> m_ButtonLabels;
};