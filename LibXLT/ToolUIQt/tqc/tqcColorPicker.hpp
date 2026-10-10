/****************************************************************************\
**	tqcColorPicker.hpp
**
**		Button that display a color value and opens a modeless color 
**	dialog when pressed.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_COLORPICKER_HPP
#error tqcColorPicker.hpp multiply included
#endif
#define TQC_COLORPICKER_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifndef TQC_EVENT_HPP
#include "ToolUIQt/tqc/tqcEvent.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif 

#ifdef USE_QT

//============================================================================
//============================================================================
class tqcColorDialog;

//============================================================================
//============================================================================
class tqcColorPicker : public QPushButton, public tqcValueChangedNotifier
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcColorPicker(QWidget* i_pParent);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~tqcColorPicker();

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const maFloatRGBA& GetValue() const;
		void SetValue(const maFloatRGBA& i_Value);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnButtonClick();
		void OnColorDialogChange();

		maFloatRGBA m_Value;
		QPointer<tqcColorDialog> m_pColorDialog;
};

#endif // USE_QT
