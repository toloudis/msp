/****************************************************************************\
**	tqcColorRGBAEdit.hpp
**
**		Custom control with slider and flaot edit controls
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_COLORRGBAEDIT_HPP
#error tqcColorRGBAEdit.hpp multiply included
#endif
#define TQC_COLORRGBAEDIT_HPP

#ifndef TQC_FLOATEDIT_HPP
#include "ToolUIQt/tqc/tqcFloatEdit.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef TQC_COLORPICKER_HPP
#include "ToolUIQt/tqc/tqcColorPicker.hpp"
#endif 

#ifdef USE_QT

//============================================================================
// Value changed callback (tqcEvent.hpp): this control calls it when the
//	value in the control has changed aftere ENTER is pressed, or the
//	focus leaves the text box. Users should set this callback, 
//	not watch for the individual enter pressed and leave events.
//============================================================================

//============================================================================
//============================================================================
class tqcColorRGBAEdit : public QWidget, public tqcValueChangedNotifier
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcColorRGBAEdit(QWidget* i_pParent);

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const maFloatRGBA& GetValue() const;
		void SetValue(const maFloatRGBA& i_Value);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnTextChange();
		void OnColorChange();

		tqcFloatEdit* m_pFloatEditR;
		tqcFloatEdit* m_pFloatEditG;
		tqcFloatEdit* m_pFloatEditB;
		tqcFloatEdit* m_pFloatEditA;
		tqcColorPicker* m_pColorPicker;
		
		void update_edits();

		maFloatRGBA m_Value;
};

#endif // USE_QT
