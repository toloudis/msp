/****************************************************************************\
**	tqtCursor.cpp
**
**	class to invoke the Qt wait cursor
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtCursor.hpp"

#ifdef USE_QT

void tqtCursor::SetWaitCursor()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
}

void tqtCursor::EndWaitCursor()
{
	// Unwind every wait cursor, like wxSetCursor(wxNullCursor) did.
	while (QApplication::overrideCursor() != NULL)
		QApplication::restoreOverrideCursor();
}

#endif
