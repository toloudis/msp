/*****************************************************************************
**  tqtWidgets.hpp
**
**     Wrapper for the main headers of Qt. Also provides the
**	preprocessor define for turning Qt on and off.
**
**	USE_QT is defined when the project builds the Qt GUI (MSP_GUI_QT,
**	set by ToolUIQt/qt6.props) and the build is not a batch build.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef TQT_WIDGETS_HPP
#error tqtWidgets.hpp multiply included
#endif
#define TQT_WIDGETS_HPP


#if defined(MSP_GUI_QT) && !defined(BATCH_MODE)
// This define activates Library usage of Qt
#define USE_QT 1
#endif

#ifdef USE_QT

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include <QtWidgets/QtWidgets>

#include <string>

#undef GetOpenFileName
#undef GetSaveFileName
#undef SetCurrentDirectory

// Qt needs these libraries:
#ifdef _DEBUG
#pragma comment(lib,"Qt6Cored.lib")
#pragma comment(lib,"Qt6Guid.lib")
#pragma comment(lib,"Qt6Widgetsd.lib")
#else
#pragma comment(lib,"Qt6Core.lib")
#pragma comment(lib,"Qt6Gui.lib")
#pragma comment(lib,"Qt6Widgets.lib")
#endif

//============================================================================
// String conversions between the library's UTF-8 std::string and QString
//============================================================================
inline QString tqtQString(const std::string& i_String)
{
	return QString::fromUtf8(i_String.c_str(), (qsizetype)i_String.size());
}
inline QString tqtQString(const char* i_pString)
{
	return QString::fromUtf8(i_pString ? i_pString : "");
}
inline std::string tqtStdString(const QString& i_String)
{
	return std::string(i_String.toUtf8().constData());
}

//============================================================================
// Convert a file dialog filter in the wxWidgets / Win32 style,
//	"My files (*.myf)|*.myf|All files (*.*)|*.*"
// to the Qt style,
//	"My files (*.myf);;All files (*.*)"
// Qt takes the patterns from the parentheses, so they are rebuilt from
// the pattern half of each pair.
//============================================================================
inline QString tqtFileDialogFilter(const QString& i_Filter)
{
	QStringList parts = i_Filter.split(QLatin1Char('|'));
	QStringList filters;
	for (int i = 0; i + 1 < parts.size(); i += 2)
	{
		QString label = parts[i].trimmed();
		int paren = label.lastIndexOf(QLatin1Char('('));
		if (paren > 0 && label.endsWith(QLatin1Char(')')))
			label = label.left(paren).trimmed();

		QString patterns = parts[i+1];
		patterns.replace(QLatin1Char(';'), QLatin1Char(' '));
		filters << QString("%1 (%2)").arg(label, patterns.simplified());
	}
	if (filters.isEmpty() && !i_Filter.isEmpty())
		filters << i_Filter;
	return filters.join(";;");
}

#endif // USE_QT
