/*****************************************************************************
**	fcuiButtonLayout.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcui/qtGUI/fcuiButtonLayout.hpp"

#include "FCSupport/fcmd/fcmdModeMgr.hpp"
#include "FCSupport/fcui/fcuiConstants.hpp"
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#include "FCSupport/fcui/fcuiUtils.hpp"
#include "FCSupport/fcui/qtGUI/fcuiTimelineItem.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
	const char* c_CLOSEICON = "Media/GUI/Action_element_deleteimage.png";
	int l_cbSize = 30;

	//========================================================================
	//========================================================================
	class CameraButton : public QPushButton
	{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		CameraButton(QWidget* i_pParent, 
					 fcuiTimelineItemData& io_ItemData, 
					 const fsLocator& i_Locator, 
					 void (*i_CallbackFunction)(fcuiTimelineItemData& io_ItemData, const fsLocator& i_Locator ))
		:  QPushButton(i_pParent),
		   m_Locator(i_Locator),
		   m_ItemData(io_ItemData),
		   m_CallbackFunction(i_CallbackFunction)
		{
			setFlat(true);
			QString button_stylesheet(" QPushButton{border: 0px solid #8f8f91;border-radius: 0px; background-color: rgba(0,0,0,0);} QPushButton:pressed {color: rgba(0,0,0,0) }	QPushButton:checked {color:rgba(0,0,0,0)} QPushButton:hover {color:rgba(0,0,0,0)}");
			this->setStyleSheet(button_stylesheet);
		}
		
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void mouseReleaseEvent( QMouseEvent* event ) 
		{
			if(fcmdModeMgr::Instance != NULL)
			{
				if(m_CallbackFunction != NULL)
					m_CallbackFunction(m_ItemData, m_Locator);
			}
		}

	private:
		fsLocator m_Locator;
		fcuiTimelineItemData m_ItemData;
		void (*m_CallbackFunction)(fcuiTimelineItemData&, const fsLocator& );
	};
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fcuiButtonLayout::fcuiButtonLayout(QWidget* i_pParent)
:  QFrame(i_pParent),
   m_pParent(i_pParent)
{
	setStyleSheet(QString("background-color: rgba(20, 20, 20, 255)"));
	setGeometry(0, 0, i_pParent->width(), i_pParent->height());
	m_pContainer = new QWidget(this);
	m_pContainer->setStyleSheet(QString("background-color: rgba(20, 20, 20, 255)"));
	int butHeight = 73;
	int butWidth = fcuiUtils::GetWidescreenWidth(butHeight);

	m_ButtonSize = QSize(butWidth, butHeight);
	//m_ButtonSize = QSize(100, 100);
	m_ContainerSize = QSize(0,0);
	m_ContainerPadTop = 20;
	m_ContainerPadBottom = 0;
	m_ContainerPadLeft = 20;
	m_ContainerPadRight = 0;

	//top gradient border init
	m_pTopBorder = new QWidget(this);
	m_pTopBorder->setGeometry(0, 0, width() + l_cbSize, l_cbSize);
	m_pTopBorder->setStyleSheet(QString("background: QLinearGradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #888, stop: 1 #000)"));

	//close button init
	m_pCloseButton = new QPushButton(this);
	m_pCloseButton->setGeometry(width() - l_cbSize, 0, l_cbSize, l_cbSize);
	m_pCloseButton->setIcon(QIcon(c_CLOSEICON));
	m_pCloseButton->setIconSize(QSize(l_cbSize,l_cbSize));
	m_pCloseButton->setFlat(true);
	m_pCloseButton->hide();
	
	QObject::connect(m_pCloseButton, SIGNAL(clicked()), this, SLOT(CloseLayout()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fcuiButtonLayout::~fcuiButtonLayout()
{
	RemoveButtons();
}

//----------------------------------------------------------------------------
/// set the layout's boundaries based on the number of cameras that will be in
/// the layout
//----------------------------------------------------------------------------
void fcuiButtonLayout::SetupCameraLayout(int i_NumCameras)
{
	int containerWidth;
	int containerHeight;

	containerWidth = i_NumCameras * m_ButtonSize.width();
	containerHeight = m_ButtonSize.height();
	m_ContainerSize = QSize(containerWidth, containerHeight);
	ResizeLayout();
	m_pContainer->setGeometry(l_cbSize, l_cbSize + l_cbSize/2, m_ContainerSize.width(), m_ContainerSize.height());
	//m_pContainer->setGeometry(l_cbSize/2, l_cbSize, m_ContainerSize.width(), m_ContainerSize.height());
}

//----------------------------------------------------------------------------
/// Given a vector of camera directories, make a set of buttons for each
//----------------------------------------------------------------------------
void fcuiButtonLayout::PopulateCameraButtons(fcuiTimelineItemData& io_ItemData, 
											 const std::vector<fsLocator>& i_ButtonDirectories, 
											 void (*i_CallbackFunction)(fcuiTimelineItemData& io_ItemData, const fsLocator& i_Locator ))
{
	RemoveButtons();
	fsLocator icon_file;
	std::string icon_string;
	QPushButton* next_button;
	int labelOffset = m_pContainer->x();

	//for each button in the vector, we need to create a button and assign the button's
	//icon from the camera's directory
	for( int i = 0; i < i_ButtonDirectories.size(); ++i )
	{		
		next_button = new CameraButton(m_pContainer, io_ItemData, i_ButtonDirectories[i], i_CallbackFunction);
		next_button->setGeometry(0 + (i * m_ButtonSize.width()), 0, 
								  m_ButtonSize.width(), m_ButtonSize.height());
		
		icon_file = i_ButtonDirectories[i];
		icon_file.Push(fcuiConstants::c_FILE_ICON);
		if(fsFileUtil::FileExists(icon_file))
		{
			fsFileUtil::LocatorToANSIFilename(icon_file, icon_string);
			QSizePolicy policy;
			policy.setHorizontalPolicy(QSizePolicy::Policy::Expanding);
			policy.setVerticalPolicy(QSizePolicy::Policy::Expanding);
			next_button->setSizePolicy(policy);
			next_button->setIconSize( QSize((m_ButtonSize.width() * 0.95), (m_ButtonSize.height() * 0.95)) );
			next_button->setIcon(QIcon(icon_string.c_str()));
		}
		next_button->show();
		m_Buttons.push_back(next_button);

		//now create the lable for that button
		QLabel* button_label = new QLabel(this);
		button_label->setGeometry(labelOffset + (i * m_ButtonSize.width()), 0, m_ButtonSize.width(), m_pTopBorder->height());
		QString label_text = QString(itStringUtil::GetStdString(i_ButtonDirectories[i].GetLastName()).c_str());
		button_label->setText(label_text);
		QFont label_font("Raavi", 16);
		label_font.setBold(true);
		button_label->setFont(label_font);
		button_label->setAlignment(Qt::AlignCenter);
		QString style_string = QString("background-color: rgba(0, 0, 0, 0);\n");
		style_string += QString("color: white;");
		button_label->setStyleSheet(style_string);
		button_label->show();

		m_ButtonLabels.push_back(button_label);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcuiButtonLayout::RemoveButtons()
{
	envSTLHelpers::DeleteContainer(m_Buttons);
	m_Buttons.clear();
	envSTLHelpers::DeleteContainer(m_ButtonLabels);
	m_ButtonLabels.clear();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fcuiButtonLayout::ShowLayout()
{
	this->show();
	m_pParent->show();
	m_pCloseButton->show();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcuiButtonLayout::HideLayout()
{
	fcuiFormMgr::SetTimelineEditMode(false);
	RemoveButtons();
	this->hide();
	m_pParent->hide();
	m_pParent->update();
	m_pCloseButton->hide();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fcuiButtonLayout::ResizeLayout()
{
	/*int frameW = m_ContainerSize.width() + l_cbSize*2;
	int frameH = m_ContainerSize.height() + l_cbSize*2;*/
	int frameW = m_ContainerSize.width() + l_cbSize*2;
	int frameH = m_ContainerSize.height() + l_cbSize*2;
	setGeometry(x(), y(), frameW, frameH);
	m_pParent->setGeometry(m_pParent->x(), m_pParent->y(), frameW, frameH);
	m_pTopBorder->setGeometry(0, 0, width() + l_cbSize, l_cbSize);
	m_pCloseButton->setGeometry(width() - l_cbSize, 0, l_cbSize, l_cbSize);
	m_pCloseButton->setIconSize(QSize(l_cbSize,l_cbSize));
	m_pParent->update();
}

void fcuiButtonLayout::CloseLayout()
{
	HideLayout();
}