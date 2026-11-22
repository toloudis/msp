/****************************************************************************\
**	tqcGradientColorBase.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/private/tqcGradientColorBase.hpp"

#ifdef USE_QT

namespace
{
	//------------------------------------------------------------------------
	// Color of the gradient at a position (0-1), from its sorted nodes
	//------------------------------------------------------------------------
	maFloatRGBA color_at(const maGradient& i_Gradient, float i_Pos)
	{
		const int num_nodes = i_Gradient.GetSize();
		if (num_nodes == 0)
			return maFloatRGBA(0, 0, 0, 1);
		if (i_Pos <= i_Gradient[0].first)
			return i_Gradient[0].second;
		for (int i = 1; i < num_nodes; ++i)
		{
			maGradient::GradientNodePair prev = i_Gradient[i-1];
			maGradient::GradientNodePair next = i_Gradient[i];
			if (i_Pos <= next.first)
			{
				float span = next.first - prev.first;
				float t = (span > 0.0f) ? (i_Pos - prev.first) / span : 0.0f;
				return maFloatRGBA(prev.second.GetRed() + t * (next.second.GetRed() - prev.second.GetRed()),
								   prev.second.GetGreen() + t * (next.second.GetGreen() - prev.second.GetGreen()),
								   prev.second.GetBlue() + t * (next.second.GetBlue() - prev.second.GetBlue()),
								   prev.second.GetAlpha() + t * (next.second.GetAlpha() - prev.second.GetAlpha()));
			}
		}
		return i_Gradient[num_nodes-1].second;
	}
}

//============================================================================
// Panel that draws the gradient and adds a knob when clicked
//============================================================================
class tqcGradientPreview : public QWidget
{
public:
	tqcGradientPreview(tqcGradientColorBase* i_pOwner, const QSize& i_Size)
		: QWidget(i_pOwner), m_pOwner(i_pOwner)
	{
		this->setFixedSize(i_Size);
	}

protected:
	virtual void paintEvent(QPaintEvent*)
	{
		QPainter painter(this);
		m_pOwner->PaintGradient(painter);
	}

	virtual void mousePressEvent(QMouseEvent* i_pEvent)
	{
		if (i_pEvent->button() == Qt::LeftButton)
			m_pOwner->OnAddKnob((int)i_pEvent->position().x());
	}

private:
	tqcGradientColorBase* m_pOwner;
};


//============================================================================
// tqcGradientColorBase
//============================================================================

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcGradientColorBase::tqcGradientColorBase(QWidget* i_pParent)
:	QWidget(i_pParent),
	m_bSettingValue(false),
	m_knobSize(14),
	m_gradientPanelWidth(200),
	m_gradientPanelHeight(80),
	m_gradientPanelHeightOffset(10)
{
	QVBoxLayout* bSizer1 = new QVBoxLayout(this);
	bSizer1->setContentsMargins(0, 0, 0, 0);
	bSizer1->setSpacing(0);

	// gradient panel and multiknobs slider
	m_panel_gradientPanel = new tqcGradientPreview(this,
		QSize(m_gradientPanelWidth, m_gradientPanelHeight + m_gradientPanelHeightOffset));
	bSizer1->addWidget(m_panel_gradientPanel);

	m_slider_colorSlider = new tqcMultiKnobSlider(this,
		QSize(m_gradientPanelWidth, m_knobSize), QSize(m_knobSize, m_knobSize));
	m_slider_colorSlider->SetKnobChangedCallback([this](tqcKnob* i_knob) { KnobValueChange(i_knob); });
	bSizer1->addWidget(m_slider_colorSlider);
	bSizer1->addSpacing(5);

	// color knob property
	m_knobColorEdit = new tqcColorRGBAEdit(this);
	m_knobColorEdit->SetValueChangedCallback([this]() { ColorEditChange(); });
	bSizer1->addWidget(m_knobColorEdit);

	QHBoxLayout* bSizer5 = new QHBoxLayout;
	bSizer5->addWidget(new QLabel("Position", this));
	m_colorPosition = new tqcRangedFloat(this);
	m_colorPosition->ShowValue( true );
	m_colorPosition->SetMaximum( 1.0f );
	m_colorPosition->SetValue( 0.0f );
	m_colorPosition->SetMinimum( 0.0f );
	m_colorPosition->SetNumTicks( 500 );
	m_colorPosition->SetDecimalPlaces(3);
	m_colorPosition->AllowExpandRange(false);
	m_colorPosition->SetValueChangedCallback([this]() { OnPositionSliderChange(); });
	bSizer5->addWidget(m_colorPosition, 3);
	bSizer1->addLayout(bSizer5);

	QHBoxLayout* bSizer6 = new QHBoxLayout;
	m_button_colorDelete = new QPushButton("Delete", this);
	QObject::connect(m_button_colorDelete, &QPushButton::clicked, [this]() { OnDeleteKnobClick(); });
	bSizer6->addWidget(m_button_colorDelete);
	bSizer6->addStretch(1);
	bSizer1->addLayout(bSizer6);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcGradientColorBase::~tqcGradientColorBase()
{
	// the knobs are going away with the slider, don't listen to them
	m_slider_colorSlider->SetKnobChangedCallback(tqcMultiKnobSlider::KnobChangedFunction());
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maGradient& tqcGradientColorBase::GetValue() const
{
	return m_value;
}
void tqcGradientColorBase::SetValue(const maGradient& i_value)
{
	m_bSettingValue = true;
	m_value = i_value;
	m_slider_colorSlider->SetValue(i_value);
	WriteIntoValue();
	std::vector<tqcKnob*> knobList;
	m_slider_colorSlider->GetKnobList(knobList);
	if (!knobList.empty())
		ShowKnobProperty(knobList[0]);
	m_bSettingValue = false;
	m_panel_gradientPanel->update();
}

//----------------------------------------------------------------------------
// callback when the gradient is clicked: add a knob with the color there
//----------------------------------------------------------------------------
void tqcGradientColorBase::OnAddKnob(int i_X)
{
	QRect rc = GetGradientPanelDrawable();
	float pos = (rc.width() > 0) ? (float)(i_X - rc.left()) / rc.width() : 0.0f;
	pos = qBound(0.0f, pos, 1.0f);
	maFloatRGBA color = color_at(m_value, pos);
	m_slider_colorSlider->AddKnob(i_X, color);
}

//----------------------------------------------------------------------------
// callback any property of knob changes
//----------------------------------------------------------------------------
void tqcGradientColorBase::KnobValueChange(tqcKnob* i_knob)
{
	if (i_knob)
	{
		ShowKnobProperty(i_knob);
		m_slider_colorSlider->SetCurrentKnob(i_knob);
		m_slider_colorSlider->SortKnobList();
	}

	WriteIntoValue();

	// Any value change of knob causes panel re-paint
	m_panel_gradientPanel->update();

	// Notify parent
	if (!m_bSettingValue)
		NotifyValueChanged();
}

//----------------------------------------------------------------------------
// callback when the color of the current knob is edited
//----------------------------------------------------------------------------
void tqcGradientColorBase::ColorEditChange()
{
	m_slider_colorSlider->SetCurrentKnobColor(m_knobColorEdit->GetValue());
}

//----------------------------------------------------------------------------
// modify m_value according to the knob position/color
//----------------------------------------------------------------------------
void tqcGradientColorBase::WriteIntoValue()
{
	std::vector<tqcKnob*> knobList;
	m_slider_colorSlider->GetKnobList(knobList);
	m_value.Clear();
	for (size_t i = 0; i < knobList.size(); i++)
	{
		m_value.AddNode(knobList[i]->GetPos(), knobList[i]->GetColor());
	}
}

//----------------------------------------------------------------------------
// callback when position slider changes in knob property window
//----------------------------------------------------------------------------
void tqcGradientColorBase::OnPositionSliderChange()
{
	float percentage = (float)m_colorPosition->GetValue() / (float) m_colorPosition->GetMaximum();
	m_slider_colorSlider->SetCurrentKnobPos(percentage);
}

//----------------------------------------------------------------------------
// callback when deletes button press in knob property window
//----------------------------------------------------------------------------
void tqcGradientColorBase::OnDeleteKnobClick()
{
	m_slider_colorSlider->DeleteCurrentKnob();
}

//----------------------------------------------------------------------------
// paint gradient panel
//----------------------------------------------------------------------------
void tqcGradientColorBase::PaintGradient(QPainter& i_Painter)
{
	QRect rcPanel = m_panel_gradientPanel->rect();
	i_Painter.fillRect(rcPanel, m_panel_gradientPanel->palette().color(QPalette::Base));

	QRect rcAll = GetGradientPanelDrawable();
	rcAll.setHeight(rcAll.height() - m_gradientPanelHeightOffset);

	std::vector<tqcKnob*> knobList;
	m_slider_colorSlider->GetKnobList(knobList);
	if (!knobList.empty())
	{
		QLinearGradient gradient(rcAll.left(), 0, rcAll.right() + 1, 0);
		for (size_t i = 0; i < knobList.size(); ++i)
			gradient.setColorAt(knobList[i]->GetPos(), knobList[i]->GetBackgroundColour());
		i_Painter.fillRect(rcAll, gradient);
	}

	// Draw color marker indicator below the gradient
	QRect rcBottom = GetGradientPanelDrawable();
	int posX = (int)(m_slider_colorSlider->GetCurrentKnobPos() * rcBottom.width());
	i_Painter.fillRect(posX + m_knobSize / 2 - 1, rcBottom.top() + m_gradientPanelHeight,
					   2, m_gradientPanelHeightOffset,
					   m_panel_gradientPanel->palette().color(QPalette::WindowText));
}

//----------------------------------------------------------------------------
// Show the properties of a knob
//----------------------------------------------------------------------------
void tqcGradientColorBase::ShowKnobProperty(tqcKnob* knob)
{
	if (knob)
	{
		float pos = knob->GetPos();
		m_colorPosition->SetValue(m_colorPosition->GetMaximum() * pos);
		m_knobColorEdit->SetValue(knob->GetColor());
		m_button_colorDelete->setEnabled(m_slider_colorSlider->GetKnobsNum() > 2);
	}
}

//----------------------------------------------------------------------------
// Get drawable rect of gradient panel
//----------------------------------------------------------------------------
QRect tqcGradientColorBase::GetGradientPanelDrawable()
{
	QRect rc = m_panel_gradientPanel->rect();
	rc.setLeft(rc.left() + m_knobSize / 2);
	rc.setWidth(rc.width() - m_knobSize / 2);
	return rc;
}

#endif // USE_QT
