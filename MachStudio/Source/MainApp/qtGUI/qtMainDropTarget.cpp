/*****************************************************************************
**  qtMainDropTarget.cpp
**
**     see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "StdAfx.h"
#include "MainApp/qtGUI/qtMainDropTarget.hpp"

#include "Features/MaterialBrowse/mbrwPaintUtil.hpp"
#include "Core/Dbg/dbgMsg.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsLocator.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Tool/tma3d/tma3dRenderView.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Systems/Character/Undo/chtrOperations.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
qtMainDropTarget::qtMainDropTarget( QWidget* i_pWidget, tma3dRenderView *i_pRenderView )
: QObject(i_pWidget),
	m_pWidget(i_pWidget),
	m_pRenderView(i_pRenderView)
{
	m_pWidget->setAcceptDrops(true);
	m_pWidget->installEventFilter(this);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
qtMainDropTarget::~qtMainDropTarget()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//virtual
bool qtMainDropTarget::eventFilter(QObject* i_pWatched, QEvent* i_pEvent)
{
	if (i_pWatched == m_pWidget)
	{
		switch (i_pEvent->type())
		{
			case QEvent::DragEnter:
			case QEvent::DragMove:
			{
				QDropEvent* pDrop = static_cast<QDropEvent*>(i_pEvent);
				if (pDrop->mimeData()->hasUrls())
				{
					pDrop->acceptProposedAction();
					return true;
				}
				break;
			}
			case QEvent::Drop:
			{
				QDropEvent* pDrop = static_cast<QDropEvent*>(i_pEvent);
				QStringList filenames;
				const QList<QUrl> urls = pDrop->mimeData()->urls();
				for (const QUrl& url : urls)
				{
					if (url.isLocalFile())
						filenames << QDir::toNativeSeparators(url.toLocalFile());
				}
				const qreal ratio = m_pWidget->devicePixelRatioF();
				QPointF pos = pDrop->position();
				OnDropFiles(qRound(pos.x() * ratio), qRound(pos.y() * ratio), filenames);
				pDrop->acceptProposedAction();
				return true;
			}
			default:
				break;
		}
	}
	return QObject::eventFilter(i_pWatched, i_pEvent);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool qtMainDropTarget::OnDropFiles(int x, int y, const QStringList& filenames)
{
	if (filenames.size() == 1)
	{
		fsLocator file_loc;
		fsFileUtil::UnicodeStringToLocator( itString(filenames[0].toStdWString().c_str()), file_loc );
		itString filename = file_loc.GetLastName();
		itString ext;
		filename.GetExtension( ext );
		if (itStringUtil::Equal(ext, itString("mtl")))
		{
			// convert the panel position to a pixel in the rendered image
			int render_x = x;
			int render_y = y;
			m_pRenderView->PanelToRenderPixel(render_x, render_y);
			mbrwPaintUtil::PasteMaterialFile( m_pRenderView, render_x, render_y, file_loc);
		}
		// a scene, or a scene file of another format (glTF), opened as a new scene
		if(itStringUtil::Equal(ext, itString("mab")) || docSingleTypeMgr::IsForeignFile(file_loc))
		{
			guiSingleDocHandler::Open(file_loc, false);
		}
		if(itStringUtil::Equal(ext, itString("gxb")))
		{
			chtrOperations::AddObject(chtrScriptData(file_loc));
		}
	}

	// We are never really receiving a file; just loading it.
	return false;
}

#endif	// USE_QT
