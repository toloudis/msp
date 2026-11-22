/*****************************************************************************
**	tqcColorDialog.hpp
**
**		Modeless color dialog that reports each color change as the user
**	edits it. Based on QColorDialog, which provides the HSV and RGB
**	editing that the wxWidgets version had to build itself.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_COLORDIALOG_HPP
#error tqcColorDialog.hpp multiply included
#endif
#define TQC_COLORDIALOG_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <functional>

#ifdef USE_QT

//============================================================================
// Class tqcColorDialog
//============================================================================
class tqcColorDialog : public QColorDialog
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the dialog is deleted.
		//--------------------------------------------------------------------
		static tqcColorDialog* Instance;

		//--------------------------------------------------------------------
		// The dialog deletes itself when closed
		//--------------------------------------------------------------------
		tqcColorDialog( QWidget* parent, const maFloatRGBA& i_Value );
		~tqcColorDialog();

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const maFloatRGBA& GetValue() const;
		void SetValue(const maFloatRGBA& i_Value);

		//--------------------------------------------------------------------
		// Called when the color changes
		//--------------------------------------------------------------------
		typedef std::function<void()> ColorChangedFunction;
		void SetColorChangedCallback(const ColorChangedFunction& i_FuncPtr);

	private:
		void set_color(const QColor& i_NewColor);
		void set_color(const maFloatRGBA& i_NewColor);
		void do_notify();

		maFloatRGBA m_Value;
		maFloatRGBA m_OriginalValue;
		ColorChangedFunction m_Callback;
};

//============================================================================
// Conversions between the float colors and Qt colors
//============================================================================
QColor tqcToQColor(const maFloatRGBA& i_Color);

#endif // USE_QT
