/*****************************************************************************
**	fcuiWidget.h
**
**		custom code for qwidget
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef FCUIWIDGET_H
#define FCUIWIDGET_H

#include <QtGui>

//============================================================================
//============================================================================
class fcuiWidget : public QWidget
{
	Q_OBJECT

    public:
        fcuiWidget(QWidget *parent = 0);
        ~fcuiWidget();

        // Override QWidget::paintEngine to return NULL
        QPaintEngine* paintEngine() const; // Turn off QTs paint engine for the fcui widget.

   protected:
        virtual void paintEvent(QPaintEvent *e);
};

#endif FCUIWIDGET_H
