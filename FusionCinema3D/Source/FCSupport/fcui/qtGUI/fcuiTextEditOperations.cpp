/*****************************************************************************
**	fcuiTextEditOperations.hpp
**
**		Class to perform qt specific operations on panels
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/fcui/qtGUI/fcuiTextEditOperations.hpp"

#include "Core/it/itStringUtil.hpp"
#include "FCSupport/fcui/fcuiUtils.hpp"
#include "FCSupport/fcui/fcuiFormMgr.hpp"
#include "FCSupport/fcmd/fcmdConstants.hpp"
#include "FCSupport/actn/actnTitleCardMgr.hpp"
#include "FCSupport/fcui/qtGUI/qtfc3d.h"
#include "FCSupport/path/pathDirectoryParser.hpp"

#include <QtGui/QtGui>


//fcuiTextEditOperations::fcuiTextEditOperations(fcuiTimelineItemData& i_ItemData):
////QObject(),
//m_Data(i_ItemData) 
//{
//}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fcuiTextEditOperations::fcuiTextEditOperations(QWidget *parent)
:  QWidget(parent),
   TitleCard_Label(NULL),
   TitleCard_BodyLabel(NULL),
   TitleCard_TitleLabel(NULL),
   TitleCard_TitleEdit(NULL),
   TitleCard_BodyEdit(NULL)
{
	
		QString button_stylesheet1("  QPushButton{color:rgba(0,255,0,255); text-align:center;font:13px;border: 0px solid #8f8f91;border-radius: 0px; background-image: url(Media/Gui/ok.png); background-color: rgba(0,0,0,0);} QPushButton:pressed {color: rgba(255,0,0,255) }	QPushButton:checked {color:rgba(0,0,0,0)} QPushButton:hover {color:rgba(255,255,0,255)}");
		QString button_stylesheet2(" QPushButton{color:rgba(0,255,0,255); text-align:center;font:13px;border: 0px solid #8f8f91;border-radius: 0px; background-image: url(Media/Gui/cancel.png); background-color: rgba(0,0,0,0);} QPushButton:pressed {color: rgba(255,0,0,255) }	QPushButton:checked {color:rgba(0,0,0,0)} QPushButton:hover {color:rgba(255,255,0,255)}");
		QString button_stylesheet3("  QPushButton{color:rgba(0,255,0,255); text-align:center;font:13px;border: 0px solid #8f8f91;border-radius: 0px; background-image: url(Media/Gui/color.png); background-color: rgba(0,0,0,0); } QPushButton:pressed {color: rgba(255,0,0,255) }	QPushButton:checked {color:rgba(0,0,0,0)} QPushButton:hover {color:rgba(255,255,0,255)}");
	
		//m_Data = io_ItemData;
		col_number = 5;
		doccolor.Set(255.0,255.0,255.0,255.0);
		QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
		TitleCard_Label = new QLabel(i_ui->ui.Edit_TitleCardWidget);
        TitleCard_Label->setObjectName(QString::fromUtf8("TitleCard_Label"));
        TitleCard_Label->setGeometry(QRect(0, 0, 782, 440));
        TitleCard_Label->setScaledContents(true);

      
		TitleCard_OKButton = new QPushButton(i_ui->ui.Edit_TitleCardWidget);
		TitleCard_OKButton->setObjectName(QString::fromUtf8("TitleCard_OKButton"));
        TitleCard_OKButton->setGeometry(QRect(568, 420, 107, 20));
		TitleCard_OKButton->setText("OK");
		TitleCard_OKButton->setStyleSheet(button_stylesheet1);
       
		TitleCard_CancelButton = new QPushButton(i_ui->ui.Edit_TitleCardWidget);
		TitleCard_CancelButton->setObjectName(QString::fromUtf8("TitleCard_CancelButton"));
        TitleCard_CancelButton->setGeometry(QRect(675, 420, 107, 20));
		TitleCard_CancelButton->setText("Cancel");
        TitleCard_CancelButton->setStyleSheet(button_stylesheet2);

		TitleCard_ColorButton = new QPushButton(i_ui->ui.Edit_TitleCardWidget);
		TitleCard_ColorButton->setObjectName(QString::fromUtf8("TitleCard_ColorButton"));
		TitleCard_ColorButton->setGeometry(QRect(461,420,107,20));
		TitleCard_ColorButton->setText("Color");
		TitleCard_ColorButton->setStyleSheet(button_stylesheet3);

		i_ui->ui.Edit_TitleCardWidget->setVisible(false);
		QObject::connect(TitleCard_CancelButton, SIGNAL(clicked()), this, SLOT(ButtonCancelClicked()));
		QObject::connect(TitleCard_OKButton, SIGNAL(clicked()), this, SLOT(ButtonOKClicked()));
		QObject::connect(TitleCard_ColorButton, SIGNAL(clicked()), this,SLOT(ColorChanged()));
		//QObject::connect(TitleCard_FontButton, SIGNAL(clicked()), this,SLOT(FontChanged()));
		TitleCard_TitleEdit = new QTextEdit("TEXT1",i_ui->ui.Edit_TitleCardWidget);
		TitleCard_BodyEdit = new QTextEdit("TEXT2",i_ui->ui.Edit_TitleCardWidget);

		QFont title_font;
		title_font.setPointSize(30);
		title_font.setFamily("Baskerville");
		TitleCard_TitleEdit->setFont(title_font);

		QFont body_font;
		body_font.setPointSize(15);
		body_font.setFamily("Baskerville");
		TitleCard_BodyEdit->setFont(body_font);
		TitleCard_TitleEdit->setFrameStyle(0);
		TitleCard_BodyEdit->setFrameStyle(0);

		m_MaxTitleCols = 19;
		m_MaxTitleRows = 1;
		m_MaxBodyCols = 36;
		m_MaxBodyRows = 5;
}

fcuiTextEditOperations::~fcuiTextEditOperations()
{
	
} 

void fcuiTextEditOperations::SetTextEdits(const fioTitleCardItemData& i_Data)
{
	QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
	i_ui->ui.Edit_TitleCardWidget->setVisible(true);
	


	QPalette palette1;
	QBrush brush1(QColor(0, 255, 255, 255));
    brush1.setStyle(Qt::SolidPattern);
	QBrush brush2(QColor(120, 120, 120, 255));
    brush2.setStyle(Qt::SolidPattern);
	QBrush brush3(QColor(240, 240, 240, 255));
    brush3.setStyle(Qt::SolidPattern);
	QBrush brush4(QColor(i_Data.DocumentColor[0], i_Data.DocumentColor[1], i_Data.DocumentColor[2], 255));
    //doccolor.Set(i_Data.DocumentColor[0], i_Data.DocumentColor[1], i_Data.DocumentColor[2], 255);
	brush4.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::Text, brush4);
    palette1.setBrush(QPalette::Active, QPalette::Base, brush1);
    palette1.setBrush(QPalette::Inactive, QPalette::Text, brush4);
    palette1.setBrush(QPalette::Inactive, QPalette::Base, brush1);
    palette1.setBrush(QPalette::Disabled, QPalette::Text, brush2);
    palette1.setBrush(QPalette::Disabled, QPalette::Base, brush3);
	m_MaxTitleCols = i_Data.TitleMaxCols;
	m_MaxTitleRows = i_Data.TitleMaxRows;
	m_MaxBodyCols = i_Data.BodyMaxCols;
	m_MaxBodyRows = i_Data.BodyMaxRows;
	
    TitleCard_TitleEdit->setObjectName(QString::fromUtf8("TitleCard_TitleEdit"));
	TitleCard_TitleEdit->setGeometry(QRect(i_Data.TitleX, i_Data.TitleY, i_Data.TitleWidth, i_Data.TitleHeight));
	TitleCard_TitleEdit->setPalette(palette1);
	QTextCharFormat fmt;
	fmt.setFontPointSize(i_Data.TitleSize);
	fmt.setFontFamily(QString(itStringUtil::GetStdString(i_Data.FontFamily).c_str()));
	//fmt.setFontWeight(i_Data.isBold);
	fmt.setForeground(QColor(doccolor.GetRed(), doccolor.GetGreen(), doccolor.GetBlue(), 255.0));
	mergeFormatOnWordOrSelectionTitle(fmt);
    TitleCard_TitleEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	TitleCard_TitleEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  	TitleCard_TitleEdit->setAlignment(Qt::AlignCenter);	
	TitleCard_TitleEdit->setLineWrapMode(QTextEdit::FixedColumnWidth);
	TitleCard_TitleEdit->setLineWrapColumnOrWidth(m_MaxTitleCols);
	
	TitleCard_TitleEdit->setFocus();
	
	TitleCard_BodyEdit->setObjectName(QString::fromUtf8("TitleCard_BodyEdit"));
	TitleCard_BodyEdit->setGeometry(QRect(i_Data.BodyX, i_Data.BodyY, i_Data.BodyWidth, i_Data.BodyHeight));
	TitleCard_BodyEdit->setPalette(palette1);
	QTextCharFormat bfmt;
	bfmt.setFontPointSize(i_Data.BodySize);
	bfmt.setFontFamily(QString(itStringUtil::GetStdString(i_Data.FontFamily).c_str()));
	//bfmt.setFontWeight(i_Data.isBold);
	bfmt.setForeground(QColor(doccolor.GetRed(), doccolor.GetGreen(), doccolor.GetBlue(), 255.0));
	mergeFormatOnWordOrSelectionBody(bfmt);
	TitleCard_BodyEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	TitleCard_BodyEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	TitleCard_BodyEdit->setAlignment(Qt::AlignHCenter);
	TitleCard_BodyEdit->setLineWrapMode(QTextEdit::FixedColumnWidth);
	TitleCard_BodyEdit->setLineWrapColumnOrWidth(m_MaxBodyCols);
	

 	

	QFont title_font;
	title_font.setPointSize(i_Data.TitleSize);
	title_font.setFamily(QString(itStringUtil::GetStdString(i_Data.FontFamily).c_str()));
	TitleCard_TitleEdit->setFont(title_font);

	QFont body_font;
	body_font.setPointSize(i_Data.BodySize);
	body_font.setFamily(QString(itStringUtil::GetStdString(i_Data.FontFamily).c_str()));
	TitleCard_BodyEdit->setFont(body_font);

	QObject::connect(TitleCard_TitleEdit, SIGNAL(clicked()), this, SLOT(TitleEditClicked()));
	QObject::connect(TitleCard_TitleEdit, SIGNAL(cursorPositionChanged()), this,SLOT(TextChanged()));
	QObject::connect(TitleCard_BodyEdit,  SIGNAL(clicked()), this, SLOT(BodyEditClicked()));
	QObject::connect(TitleCard_BodyEdit, SIGNAL(cursorPositionChanged()), this,SLOT(TextChanged()));
		
}
//----------------------------------------------------------------------------
/// When the OK button is clicked
//------------------ ----------------------------------------------------------
void fcuiTextEditOperations::ButtonOKClicked()
{
	QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
	
	i_ui->ui.Edit_TitleCardWidget->setVisible(false);
	i_ui->ui.widget_3->setVisible(true);
	if(TitleCard_TitleEdit != NULL)
	{
		Title = TitleCard_TitleEdit->toPlainText();
		itStrTitle = (const itString::CharType*)Title.data();
		m_Data.m_TitleText = itStrTitle;
		
	//	TitleCard_TitleEdit->clear();
	}

	if(TitleCard_BodyEdit != NULL)
	{
		Body = TitleCard_BodyEdit->toPlainText();
		itStrBody = (const itString::CharType*)Body.data();
		m_Data.m_BodyText = itStrBody;
	//	TitleCard_BodyEdit->clear();
	}

	std::vector<fsLocator> files;
	pathDirectoryParser parser;
	parser.GetDirectoryFiles(m_Data.m_ItemLocator, files);
	itString l_PNGExt("png");
	itString icon( fcmdConstants::c_Icon );
	itString icon_check, extension;
	for ( int i = 0; i < files.size(); ++i )
	{
		icon_check = files[i].GetLastName();
		//don't process the icon file of the asset
		if ( icon_check == icon )
			continue;
		if ( icon_check == itString("temp.jpg"))
			continue;

		files[i].GetLastName().GetExtension(extension);
		if ( extension == l_PNGExt) 
		{
			if (actnTitleCardMgr::Instance != NULL)
			{
				actnTitleCardMgr::Instance->WriteOnTitleCard( files[i], m_Data.m_BodyText, m_Data.m_TitleText, doccolor );
				itString ObjName, tempItName;
				
				m_Data.m_TitlecardColor = doccolor;
				ObjName = original_Data.m_BillboardName;
				actnTitleCardMgr::Instance->ReloadTexture(ObjName, files[i]);
				fcuiFormMgr::UpdateTimelineItemData(original_Data, m_Data);
			}
		}
	}
	fcuiFormMgr::SetTimelineEditMode(false);

}

//----------------------------------------------------------------------------
/// When the Cancel button is clicked
//------------------ ----------------------------------------------------------
void fcuiTextEditOperations::ButtonCancelClicked()
{
	QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
	i_ui->ui.Edit_TitleCardWidget->setVisible(false);
	i_ui->ui.widget_3->setVisible(true);
	if(TitleCard_TitleEdit !=NULL)
	{
		TitleCard_TitleEdit->clear();
	}
	if(TitleCard_BodyEdit !=NULL)
	{
		TitleCard_BodyEdit->clear(); 
	}
	fcuiFormMgr::SetTimelineEditMode(false);
	
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcuiTextEditOperations::SetItemData(const fcuiTimelineItemData& i_ItemData)
{
	objName = i_ItemData.m_BillboardName;
	m_Data = i_ItemData;
	original_Data = i_ItemData;

	if(i_ItemData.m_TitleText == itString(""))
		m_Data.m_TitleText = itString("TEXT1");
	if(i_ItemData.m_BodyText == itString(""))
		m_Data.m_BodyText = itString("TEXT2");
	if(i_ItemData.m_TitlecardColor.GetAlpha() == 0.0) // No color has been set
		m_Data.m_TitlecardColor.Set(255.0,255.0,255.0,255.0);
    
	
    QPalette palette1;
	QBrush brush1(QColor(255, 255, 255, 0));
    brush1.setStyle(Qt::SolidPattern);
	QBrush brush2(QColor(120, 120, 120, 255));
    brush2.setStyle(Qt::SolidPattern);
	QBrush brush3(QColor(240, 240, 240, 255));
    brush3.setStyle(Qt::SolidPattern);
    QBrush brush(QColor(m_Data.m_TitlecardColor.GetRed(), m_Data.m_TitlecardColor.GetGreen(), m_Data.m_TitlecardColor.GetBlue(), m_Data.m_TitlecardColor.GetAlpha()));
    brush.setStyle(Qt::SolidPattern);
    palette1.setBrush(QPalette::Active, QPalette::Text, brush);
    palette1.setBrush(QPalette::Active, QPalette::Base, brush1);
    palette1.setBrush(QPalette::Inactive, QPalette::Text, brush);
    palette1.setBrush(QPalette::Inactive, QPalette::Base, brush1);
    palette1.setBrush(QPalette::Disabled, QPalette::Text, brush2);
    palette1.setBrush(QPalette::Disabled, QPalette::Base, brush3);
    TitleCard_TitleEdit->setPalette(palette1);
	TitleCard_BodyEdit->setPalette(palette1);

	QString TitleText = QString((itStringUtil::GetStdString(m_Data.m_TitleText).c_str()));
	QString BodyText  = QString((itStringUtil::GetStdString(m_Data.m_BodyText).c_str()));
	TitleCard_TitleEdit->setPlainText(TitleText);
	TitleCard_BodyEdit->setPlainText(BodyText);

	doccolor = m_Data.m_TitlecardColor;
	QTextCharFormat fmt;
	fmt.setForeground(QColor(m_Data.m_TitlecardColor.GetRed(), m_Data.m_TitlecardColor.GetGreen(), m_Data.m_TitlecardColor.GetBlue(), m_Data.m_TitlecardColor.GetAlpha()));
	mergeFormatOnWordOrSelectionTitle(fmt);
	QTextCharFormat bfmt;
	bfmt.setForeground(QColor(m_Data.m_TitlecardColor.GetRed(), m_Data.m_TitlecardColor.GetGreen(), m_Data.m_TitlecardColor.GetBlue(), m_Data.m_TitlecardColor.GetAlpha()));
	mergeFormatOnWordOrSelectionBody(bfmt);


}


//----------------------------------------------------------------------------
/// Set the temp image as a background when editing text
//----------------------------------------------------------------------------
void fcuiTextEditOperations::setlabelBackground(QPixmap& i_pixmap)
{
	QtFC3D *i_ui = fcuiFormMgr::GetMainWindow();
	i_ui->ui.Edit_TitleCardWidget->setVisible(true);
	TitleCard_Label->setPixmap(i_pixmap);
}

//----------------------------------------------------------------------------
/// Cancel the title card edit without a button press
//----------------------------------------------------------------------------
void fcuiTextEditOperations::CancelEdit()
{
	ButtonCancelClicked();
}

//----------------------------------------------------------------------------
/// Handle all the restrictions and the color changes here
//----------------------------------------------------------------------------
void fcuiTextEditOperations::TextChanged()
{
	int title_cur_line_number = 0;
	int title_col_number = 5;
	int body_cur_line_number = 0;
	int body_col_number = 4;
	QTextCursor cursor = TitleCard_TitleEdit->textCursor();
	
	// QTextEdit doesnot provide any functionality to restrict the text
	// to certain number of lines. The code below uses the line numbers
	// and the columns to prevent writing more than 2 lines for Title
	// and 5 lines for Body.

	if(!cursor.isNull())
	{
		
		title_cur_line_number = cursor.blockNumber();
		title_col_number = cursor.columnNumber();
		if((title_col_number >= m_MaxTitleCols ))
		{
			TitleCard_TitleEdit->moveCursor(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor);
			cursor.deletePreviousChar();
		}

		if(title_cur_line_number >m_MaxTitleRows)
		{
			TitleCard_TitleEdit->moveCursor(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor);
			cursor.deletePreviousChar();
		}
		/*else if((title_cur_line_number == m_MaxTitleRows) && (title_col_number > m_MaxTitleCols))
		{
			TitleCard_TitleEdit->moveCursor(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor);
			cursor.deletePreviousChar();
		}*/
		//TitleCard_TitleEdit->setAlignment(Qt::AlignHCenter);
	}

	// The values might change if we use a different font.
	// Need to move all the values to a namespace.

	QTextCursor body_cursor = TitleCard_BodyEdit->textCursor();
	if(!body_cursor.isNull())
	{
		body_cur_line_number = body_cursor.blockNumber();
		body_col_number = body_cursor.columnNumber();
		if((body_col_number >= m_MaxBodyCols ))
		{
			TitleCard_BodyEdit->moveCursor(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor);
			body_cursor.deletePreviousChar();
		}

		if(body_cur_line_number >m_MaxBodyRows)
		{
			TitleCard_BodyEdit->moveCursor(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor);
			body_cursor.deletePreviousChar();
		}
		/*else if((body_cur_line_number == m_MaxBodyRows) && (body_col_number > m_MaxBodyCols))
		{
			TitleCard_BodyEdit->moveCursor(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor);
			body_cursor.deletePreviousChar();
		}*/
		//TitleCard_BodyEdit->setAlignment(Qt::AlignHCenter);
		
	}
	
	
}

//----------------------------------------------------------------------------
/// Change text color
//----------------------------------------------------------------------------
void fcuiTextEditOperations::ColorChanged()
{
	QColor doc_color = QColorDialog::getColor(TitleCard_TitleEdit->textColor(), this);
    if (!doc_color.isValid())
        return;
	setColor(doc_color);
	QTextCharFormat fmt;
    fmt.setForeground(doc_color);
    mergeFormatOnWordOrSelectionTitle(fmt);
	mergeFormatOnWordOrSelectionBody(fmt);
	m_Data.m_colorChanged = true;
}

//----------------------------------------------------------------------------
/// Set the color to be written onto titlecard
//----------------------------------------------------------------------------

void fcuiTextEditOperations::setColor(const QColor& i_color)
{
	doccolor.Set(i_color.red(), i_color.green(), i_color.blue(), i_color.alpha());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcuiTextEditOperations::mergeFormatOnWordOrSelectionTitle(const QTextCharFormat &format)
 {
     QTextCursor cursor = TitleCard_TitleEdit->textCursor();
	 if(!cursor.isNull())
	 {
		//if (!cursor.hasSelection())
		{
			cursor.select(QTextCursor::Document);
			TitleCard_TitleEdit->setTextCursor(cursor);
			TitleCard_TitleEdit->setAlignment(Qt::AlignCenter);
		}

		cursor.mergeCharFormat(format);
		TitleCard_TitleEdit->mergeCurrentCharFormat(format);
		TitleCard_TitleEdit->setAlignment(Qt::AlignCenter);
	 }
	
	
 }

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void fcuiTextEditOperations::mergeFormatOnWordOrSelectionBody(const QTextCharFormat &format)
 {
     QTextCursor body_cursor = TitleCard_BodyEdit->textCursor();
	 if(!body_cursor.isNull())
	 {
		//if (!body_cursor.hasSelection())
		{
			
			body_cursor.select(QTextCursor::Document);
			TitleCard_BodyEdit->setTextCursor(body_cursor);
			TitleCard_BodyEdit->setAlignment(Qt::AlignHCenter);
		}
		body_cursor.mergeCharFormat(format);
		TitleCard_BodyEdit->setAlignment(Qt::AlignHCenter);
		TitleCard_BodyEdit->mergeCurrentCharFormat(format);
		 
	 }
	
 }