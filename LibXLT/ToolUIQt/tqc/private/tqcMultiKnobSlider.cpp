/****************************************************************************\
**	tqcMultiKnobSlider.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/private/tqcMultiKnobSlider.hpp"

#include "ToolUIQt/tqc/private/tqcColorDialog.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"

#include "Core/dbg/dbgMsg.hpp"

#include <algorithm>

#ifdef USE_QT

namespace
{
	bool SortKnobs(tqcKnob* d1, tqcKnob* d2)
	{
		return d1->GetPos() < d2->GetPos();
	}
}

//============================================================================
// tqcKnob
//============================================================================

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcKnob::tqcKnob(tqcMultiKnobSlider* i_pSlider, float i_percentage, const maFloatRGBA& i_color)
:	m_pSlider(i_pSlider),
	m_curPercentage(0.0f),
	m_color(i_color)
{
	SetPos(i_percentage);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcKnob::~tqcKnob()
{
	if (m_pColorDialog)
	{
		m_pColorDialog->SetColorChangedCallback(tqcColorDialog::ColorChangedFunction());
		m_pColorDialog->close();
	}
}

//----------------------------------------------------------------------------
//  Get the position of the knob, value from 0~1
//----------------------------------------------------------------------------
float tqcKnob::GetPos()
{
	return m_curPercentage;
}

//----------------------------------------------------------------------------
//  Set the position of the knob, value from 0~1
//----------------------------------------------------------------------------
void tqcKnob::SetPos(float i_percent)
{
	if (i_percent < 0.0f)
		i_percent = 0.0f;
	if (i_percent > 1.0f)
		i_percent = 1.0f;
	if (i_percent != m_curPercentage)
	{
		m_curPercentage = i_percent;
		m_pSlider->KnobChanged(this);
	}
}

//----------------------------------------------------------------------------
//  Set the color of the knob
//----------------------------------------------------------------------------
void tqcKnob::SetColor(const maFloatRGBA& i_color)
{
	m_color = i_color;
	m_pSlider->KnobChanged(this);
}

//----------------------------------------------------------------------------
//  Get the color of the knob
//----------------------------------------------------------------------------
maFloatRGBA& tqcKnob::GetColor()
{
	return m_color;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
QColor tqcKnob::GetBackgroundColour() const
{
	QColor color = tqcToQColor(m_color);
	color.setAlphaF(qBound(0.0f, m_color.GetAlpha(), 1.0f));
	return color;
}

//----------------------------------------------------------------------------
//  Open a color dialog for the knob
//----------------------------------------------------------------------------
void tqcKnob::EditColor()
{
	// If we already have a color dialog, then don't do anything
	if (m_pColorDialog && (m_pColorDialog == tqcColorDialog::Instance))
		return;

	// if a color picker is already up, close it
	if (tqcColorDialog::Instance)
	{
		tqcColorDialog::Instance->close();
	}

	m_pColorDialog = new tqcColorDialog(tqtSystem::g_pMainForm, m_color);
	tqcColorDialog::Instance = m_pColorDialog;
	m_pColorDialog->SetColorChangedCallback([this]() { OnColorChanged(); });
	m_pColorDialog->show();
}

//----------------------------------------------------------------------------
//  Callback: Color change from tqcColorDialog
//----------------------------------------------------------------------------
void tqcKnob::OnColorChanged()
{
	if (m_pColorDialog)
	{
		maFloatRGBA new_value = m_pColorDialog->GetValue();
		if (!(new_value == m_color))
		{
			this->SetColor( new_value );
		}
	}
}


//============================================================================
// tqcMultiKnobSlider
//============================================================================

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcMultiKnobSlider::tqcMultiKnobSlider(QWidget* i_parent,
									   const QSize& i_backgroundSize,
									   const QSize& i_knobSize)
: QWidget(i_parent),
	m_backgroundSize(i_backgroundSize),
	m_knobSize(i_knobSize),
	m_currentKnob(NULL),
	m_draggingKnob(NULL)
{
	this->setFixedSize(m_backgroundSize.width(), m_knobSize.height());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcMultiKnobSlider::~tqcMultiKnobSlider()
{
	m_KnobChanged = KnobChangedFunction();
	clear_knobs();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcMultiKnobSlider::clear_knobs()
{
	m_currentKnob = NULL;
	m_draggingKnob = NULL;
	for (size_t i = 0; i < m_knobList.size(); i++)
		delete m_knobList[i];
	m_knobList.clear();
}

//----------------------------------------------------------------------------
// Create a new knob on the slider
//----------------------------------------------------------------------------
tqcKnob* tqcMultiKnobSlider::AddKnob(float i_percentage, const maFloatRGBA& i_color)
{
	tqcKnob* knob = new tqcKnob(this, i_percentage, i_color);
	m_knobList.push_back(knob);
	std::sort(m_knobList.begin(), m_knobList.end(), SortKnobs);
	m_currentKnob = knob;
	KnobChanged(knob);
	return knob;
}
tqcKnob* tqcMultiKnobSlider::AddKnob(int i_position, const maFloatRGBA& i_color)
{
	return AddKnob(PositionToPercentage(i_position), i_color);
}

//----------------------------------------------------------------------------
//	Value
//----------------------------------------------------------------------------
void tqcMultiKnobSlider::SetValue(const maGradient& i_value)
{
	maGradient g = i_value;

	// Force the input contains at least 2 nodes
	if (g.GetSize() < 2)
	{
		DBG_WARNING("Input data doesn't contain at least size of 2");
		return;
	}

	clear_knobs();
	for (int i = 0; i < g.GetSize(); i++)
	{
		AddKnob(g[i].first, g[i].second);
	}
	SetCurrentKnobByIndex(0);
}

//----------------------------------------------------------------------------
// Set current knob to i_index
//----------------------------------------------------------------------------
void tqcMultiKnobSlider::SetCurrentKnobByIndex(int i_index)
{
	if (i_index >= 0 && (int)m_knobList.size() > i_index)
	{
		m_currentKnob = m_knobList[i_index];
		update();
	}
}

//----------------------------------------------------------------------------
// Set current knob to i_knob
//----------------------------------------------------------------------------
void tqcMultiKnobSlider::SetCurrentKnob(tqcKnob* i_knob)
{
	if (std::find(m_knobList.begin(), m_knobList.end(), i_knob) != m_knobList.end())
	{
		m_currentKnob = i_knob;
		update();
	}
}

//----------------------------------------------------------------------------
// Delete current knob
//----------------------------------------------------------------------------
void tqcMultiKnobSlider::DeleteCurrentKnob()
{
	// knobs can only be deleted when they are more than two
	if (m_knobList.size() <= 2)
	{
		return;
	}

	if (m_currentKnob)
	{
		std::vector<tqcKnob*>::iterator it;
		it = std::find(m_knobList.begin(), m_knobList.end(), m_currentKnob);
		if (it != m_knobList.end())
		{
			it = m_knobList.erase(it);
			delete m_currentKnob;
			if(it == m_knobList.begin())
				m_currentKnob = *it;
			else
				m_currentKnob = *(--it);
			KnobChanged(m_currentKnob);
		}
	}
}

//----------------------------------------------------------------------------
// Get the number of knobs on the slider
//----------------------------------------------------------------------------
int tqcMultiKnobSlider::GetKnobsNum()
{
	return (int)m_knobList.size();
}

//----------------------------------------------------------------------------
// Get knob list
//----------------------------------------------------------------------------
void tqcMultiKnobSlider::GetKnobList(std::vector<tqcKnob*>& o_knobList)
{
	o_knobList =  m_knobList;
}

//----------------------------------------------------------------------------
// Force to sort knob list
//----------------------------------------------------------------------------
void tqcMultiKnobSlider::SortKnobList()
{
	std::sort(m_knobList.begin(), m_knobList.end(), SortKnobs);
}

//----------------------------------------------------------------------------
// Set current knob position in percentage
//----------------------------------------------------------------------------
void tqcMultiKnobSlider::SetCurrentKnobPos(float i_percentage)
{
	if (m_currentKnob)
	{
		m_currentKnob->SetPos(i_percentage);
	}
}

//----------------------------------------------------------------------------
// Set current knob color
//----------------------------------------------------------------------------
void tqcMultiKnobSlider::SetCurrentKnobColor(const maFloatRGBA& i_color)
{
	if (m_currentKnob)
	{
		m_currentKnob->SetColor(i_color);
	}
}

//----------------------------------------------------------------------------
// Get current knob position in percentage
//----------------------------------------------------------------------------
float tqcMultiKnobSlider::GetCurrentKnobPos()
{
	if (m_currentKnob)
	{
		return m_currentKnob->GetPos();
	}
	return 0.0f;
}

//----------------------------------------------------------------------------
// Get current knob color
//----------------------------------------------------------------------------
void tqcMultiKnobSlider::GetCurrentKnobColor(maFloatRGBA& o_color)
{
	maFloatRGBA c;
	if (m_currentKnob)
	{
		c = m_currentKnob->GetColor();
	}
	o_color = c;
}

//----------------------------------------------------------------------------
//  Convert position on slider to percentage
//----------------------------------------------------------------------------
float tqcMultiKnobSlider::PositionToPercentage(int i_pos) const
{
	int travel = m_backgroundSize.width() - m_knobSize.width();
	if (travel <= 0)
		return 0.0f;
	return (float)(i_pos - (m_knobSize.width() / 2)) / travel;
}

//----------------------------------------------------------------------------
//  A knob changed, tell our owner
//----------------------------------------------------------------------------
void tqcMultiKnobSlider::KnobChanged(tqcKnob* i_knob)
{
	update();
	if (i_knob && m_KnobChanged)
		m_KnobChanged(i_knob);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
QRect tqcMultiKnobSlider::knob_rect(tqcKnob* i_knob) const
{
	int travel = m_backgroundSize.width() - m_knobSize.width();
	int x = (int)(i_knob->GetPos() * travel + 0.5f);
	return QRect(x, 0, m_knobSize.width(), m_knobSize.height());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcKnob* tqcMultiKnobSlider::knob_at(const QPoint& i_pos) const
{
	// current knob first, it is drawn on top
	if (m_currentKnob && knob_rect(m_currentKnob).contains(i_pos))
		return m_currentKnob;
	for (int i = (int)m_knobList.size() - 1; i >= 0; --i)
	{
		if (knob_rect(m_knobList[i]).contains(i_pos))
			return m_knobList[i];
	}
	return NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcMultiKnobSlider::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	for (size_t i = 0; i < m_knobList.size(); ++i)
	{
		if (m_knobList[i] == m_currentKnob)
			continue;
		QRect rc = knob_rect(m_knobList[i]).adjusted(1, 1, -1, -1);
		painter.setPen(QPen(palette().color(QPalette::Mid), 1));
		painter.setBrush(m_knobList[i]->GetBackgroundColour());
		painter.drawRoundedRect(rc, 2, 2);
	}

	// the current knob is outlined and drawn on top
	if (m_currentKnob)
	{
		QRect rc = knob_rect(m_currentKnob).adjusted(1, 1, -1, -1);
		painter.setPen(QPen(palette().color(QPalette::Highlight), 2));
		painter.setBrush(m_currentKnob->GetBackgroundColour());
		painter.drawRoundedRect(rc, 2, 2);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcMultiKnobSlider::mousePressEvent(QMouseEvent* i_pEvent)
{
	tqcKnob* knob = knob_at(i_pEvent->position().toPoint());
	if (!knob)
		return;

	m_currentKnob = knob;
	if (i_pEvent->button() == Qt::LeftButton)
	{
		m_draggingKnob = knob;
		KnobChanged(knob);
	}
	else if (i_pEvent->button() == Qt::RightButton)
	{
		KnobChanged(knob);
		knob->EditColor();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcMultiKnobSlider::mouseMoveEvent(QMouseEvent* i_pEvent)
{
	if (m_draggingKnob && (i_pEvent->buttons() & Qt::LeftButton))
	{
		m_draggingKnob->SetPos(PositionToPercentage((int)i_pEvent->position().x()));
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcMultiKnobSlider::mouseReleaseEvent(QMouseEvent*)
{
	m_draggingKnob = NULL;
}

#endif // USE_QT
