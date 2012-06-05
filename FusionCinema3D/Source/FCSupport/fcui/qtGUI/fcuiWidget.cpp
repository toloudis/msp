/*****************************************************************************
**	fcuiWidget.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "fcuiwidget.h"

//============================================================================
//============================================================================
fcuiWidget::fcuiWidget(QWidget *parent)
:QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_PaintOnScreen);
    setMinimumSize(240,240);
    setFocusPolicy(Qt::ClickFocus);
}

fcuiWidget::~fcuiWidget()
{
}

//------------------------------------------------------------------
// We don't want another paint engine to get in the way for our fcui 
//based paint engine. So we return nothing.
//------------------------------------------------------------------
QPaintEngine* fcuiWidget::paintEngine() const
{
	return NULL;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void fcuiWidget::paintEvent(QPaintEvent *e)
{
   e->accept();
}

