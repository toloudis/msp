/*****************************************************************************
**	fcuiTextEditOperations.hpp
**
**		Class to perform qt specific operations on panels
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef FCUI_TEXTEDITOPERATIONS_HPP
#error fcuiTextEditOperations.hpp multiply included
#endif
#define FCUI_TEXTEDITOPERATIONS_HPP

#ifndef FCUI_TIMELINEITEM_HPP
#include "FCSupport/fcui/qtGUI/fcuiTimelineItem.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif

#ifndef FIO_TITLECARDITEM_HPP
#include "FCSupport/fio/fioTitleCardItem.hpp"
#endif

#include <QtGui>

//class fcuiTimelineItemData;
//============================================================================
//============================================================================
class fcuiTextEditOperations : public QWidget
{
	Q_OBJECT

public:

	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	fcuiTextEditOperations(QWidget *parent = 0);
	//fcuiTextEditOperations(fcuiTimelineItemData& i_ItemData);

	~fcuiTextEditOperations();
	//----------------------------------------------------------------------------
	/// Create the labels to show on the billboard
	//----------------------------------------------------------------------------
	void SetItemData(const fcuiTimelineItemData& i_ItemData);

	//----------------------------------------------------------------------------
	/// Set the temp image as a background when editing text
	//----------------------------------------------------------------------------
	void setlabelBackground(QPixmap& i_pixmap);

	//----------------------------------------------------------------------------
	/// Cancel the title card edit without a button press
	//----------------------------------------------------------------------------
	void CancelEdit();
	void mergeFormatOnWordOrSelectionTitle(const QTextCharFormat &format);
	void mergeFormatOnWordOrSelectionBody(const QTextCharFormat &format);
	void setColor(const QColor& i_color);
	void SetTextEdits(const fioTitleCardItemData& i_Data);

	

private slots: 
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void ButtonOKClicked();
	void ButtonCancelClicked();
	void TextChanged();
	void ColorChanged();
//	void FontChanged();


private:
	QLabel* TitleCard_Label;
	QLabel* TitleCard_BodyLabel;
	QLabel* TitleCard_TitleLabel;
	QTextEdit* TitleCard_TitleEdit ;
	QTextEdit* TitleCard_BodyEdit;
	QPushButton* TitleCard_ColorButton;
	QPushButton* TitleCard_OKButton;
	QPushButton* TitleCard_CancelButton;
	
	QString Title;
	QString Body;
	maFloatRGBA doccolor;
	fcuiTimelineItemData m_Data;
	fcuiTimelineItemData original_Data;
	itString itStrBody;
	itString itStrTitle;
	itString objName;
	int col_number;

	int m_MaxTitleCols, m_MaxTitleRows;
	int m_MaxBodyCols, m_MaxBodyRows;
};