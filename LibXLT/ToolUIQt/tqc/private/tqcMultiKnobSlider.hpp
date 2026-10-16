/****************************************************************************\
**	tqcMultiKnobSlider.hpp
**
**		Slider with several knobs, each with a position (0-1) and a color.
**	Used by the gradient editor. Dragging a knob moves it, clicking selects
**	it and right-clicking opens a color dialog for it.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_MULTIKNOBSLIDER_HPP
#error tqcMultiKnobSlider.hpp multiply included
#endif
#define TQC_MULTIKNOBSLIDER_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#ifndef MA_GRADIENT_HPP
#include "Core/ma/maGradient.hpp"
#endif

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <functional>
#include <vector>

#ifdef USE_QT

class tqcColorDialog;
class tqcMultiKnobSlider;

//============================================================================
// One knob of the slider
//============================================================================
class tqcKnob
{
public:
	tqcKnob(tqcMultiKnobSlider* i_pSlider, float i_percentage, const maFloatRGBA& i_color);
	~tqcKnob();

	//  Get the position of the knob, value from 0~1
	float GetPos();
	//  Set the position of the knob, value from 0~1
    void SetPos(float i_percent);
	//  Set the color of the knob
	void SetColor(const maFloatRGBA& i_color);
	//  Get the color of the knob
	maFloatRGBA& GetColor();
	//  Color of the knob for drawing
	QColor GetBackgroundColour() const;

	//  Open a color dialog to edit the knob color
	void EditColor();

private:
	//  Callback: Color change from tqcColorDialog
	void OnColorChanged();

	tqcMultiKnobSlider* m_pSlider;
	float m_curPercentage;
	maFloatRGBA m_color;
	QPointer<tqcColorDialog> m_pColorDialog;
};

//============================================================================
//============================================================================
class tqcMultiKnobSlider : public QWidget
{
public:
	//--------------------------------------------------------------------
	// i_knobSize is the size of each knob, the slider is as tall as a knob
	//--------------------------------------------------------------------
    tqcMultiKnobSlider(QWidget* i_parent, const QSize& i_backgroundSize, const QSize& i_knobSize);
	~tqcMultiKnobSlider();

	//--------------------------------------------------------------------
	// Called with the knob whenever a knob is added, moved, recolored, or
	// becomes current after a delete.
	//--------------------------------------------------------------------
	typedef std::function<void(tqcKnob*)> KnobChangedFunction;
	void SetKnobChangedCallback(KnobChangedFunction i_Callback) { m_KnobChanged = i_Callback; }

	// Create a new knob on the slider
    tqcKnob* AddKnob(float i_percentage, const maFloatRGBA& i_color);
	tqcKnob* AddKnob(int i_position, const maFloatRGBA& i_color);
	//	Value
	void SetValue(const maGradient& i_value);
	// Set current knob according to i_index
	void SetCurrentKnobByIndex(int i_index);
	// Set current knob to i_knob
	void SetCurrentKnob(tqcKnob* i_knob);
	// Delete current knob
	void DeleteCurrentKnob();
	// Get the number of knobs on the slider
	int GetKnobsNum();
	// Get knob list
	void GetKnobList(std::vector<tqcKnob*>& o_knobList);
	// Force to sort knob list
	void SortKnobList();
	// Set current knob position in percentage
	void SetCurrentKnobPos(float i_percentage);
	// Set current knob color
	void SetCurrentKnobColor(const maFloatRGBA& i_color);
	// Get current knob position in percentage
	float GetCurrentKnobPos();
	// Get current knob color
	void GetCurrentKnobColor(maFloatRGBA& o_color);

	//  Convert position on slider to percentage
	float PositionToPercentage(int i_pos) const;

	//  Notification from a knob that it changed
	void KnobChanged(tqcKnob* i_knob);

protected:
	virtual void paintEvent(QPaintEvent* i_pEvent);
	virtual void mousePressEvent(QMouseEvent* i_pEvent);
	virtual void mouseMoveEvent(QMouseEvent* i_pEvent);
	virtual void mouseReleaseEvent(QMouseEvent* i_pEvent);

private:
	QRect knob_rect(tqcKnob* i_knob) const;
	tqcKnob* knob_at(const QPoint& i_pos) const;
	void clear_knobs();

	QSize m_backgroundSize;
	QSize m_knobSize;
	std::vector<tqcKnob*> m_knobList;
	tqcKnob* m_currentKnob;
	tqcKnob* m_draggingKnob;
	KnobChangedFunction m_KnobChanged;
};

#endif // USE_QT
