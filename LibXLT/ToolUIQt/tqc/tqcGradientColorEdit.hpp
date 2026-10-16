/****************************************************************************\
**	tqcGradientColorEdit.hpp
**
**		Custom control with GradientColorBase and color property window
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_GRADIENTCOLOREDIT_HPP
#error tqcGradientColorEdit.hpp multiply included
#endif
#define TQC_GRADIENTCOLOREDIT_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#ifndef TQC_GRADIENTCOLORBASE_HPP
#include "ToolUIQt/tqc/private/tqcGradientColorBase.hpp"
#endif

#ifndef MA_GRADIENT_HPP
#include "Core/ma/maGradient.hpp"
#endif

//#ifndef TQC_PICTUREBOX_HPP
//#include "ToolUIQt/tqc/private/tqcPictureBox.hpp"
//#endif

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif


#ifdef USE_QT

//============================================================================
// Value changed callback (tqcEvent.hpp): this control calls it when the
//	value in the control has changed aftere.
//============================================================================
class tqcGradientColorEdit : public QWidget, public tqcValueChangedNotifier
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcGradientColorEdit(QWidget* i_pParent);
		virtual ~tqcGradientColorEdit();

		//--------------------------------------------------------------------
		//	Value
		//--------------------------------------------------------------------
		const maGradient& GetValue() const;
		void SetValue(const maGradient& i_Value);

	private:

		tqcGradientColorBase* m_gradient_gradientEditor;
};

#endif // USE_QT
