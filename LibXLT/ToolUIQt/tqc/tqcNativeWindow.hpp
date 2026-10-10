/*****************************************************************************
**	tqcNativeWindow.hpp
**
**		Widget with its own native window that DirectX draws into. Qt
**	does not paint it at all.
**
**	Qt sizes are in device independent pixels; the native window and the
**	DirectX swap chain are in physical pixels (see PhysicalWidth/Height).
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_NATIVEWINDOW_HPP
#error tqcNativeWindow.hpp multiply included
#endif
#define TQC_NATIVEWINDOW_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

#include <windows.h>
// windows.h renames GetObject, which the object managers use as a name
#undef GetObject

//============================================================================
//============================================================================
class tqcNativeWindow : public QWidget
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	tqcNativeWindow(QWidget* i_pParent)
		: QWidget(i_pParent)
	{
		this->setAttribute(Qt::WA_NativeWindow);
		this->setAttribute(Qt::WA_PaintOnScreen);
		this->setAttribute(Qt::WA_NoSystemBackground);
		this->setAttribute(Qt::WA_OpaquePaintEvent);
		this->setAutoFillBackground(false);
		this->setFocusPolicy(Qt::StrongFocus);
		this->setMouseTracking(true);
	}

	//------------------------------------------------------------------------
	// The native window handle for DirectX
	//------------------------------------------------------------------------
	HWND GetHandle() const
	{
		return (HWND)this->winId();
	}

	//------------------------------------------------------------------------
	// Size of the native window in physical pixels
	//------------------------------------------------------------------------
	int PhysicalWidth() const
	{
		return qRound(this->width() * this->devicePixelRatioF());
	}
	int PhysicalHeight() const
	{
		return qRound(this->height() * this->devicePixelRatioF());
	}

	//------------------------------------------------------------------------
	// Mouse position in physical pixels of the native window
	//------------------------------------------------------------------------
	QPoint ToPhysical(const QPointF& i_Pos) const
	{
		const qreal ratio = this->devicePixelRatioF();
		return QPoint(qRound(i_Pos.x() * ratio), qRound(i_Pos.y() * ratio));
	}

	//------------------------------------------------------------------------
	// Qt must not paint over DirectX
	//------------------------------------------------------------------------
	virtual QPaintEngine* paintEngine() const
	{
		return NULL;
	}
};

#endif // USE_QT
