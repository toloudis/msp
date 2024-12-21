/****************************************************************************\
**	twcMultiKnobSlider.hpp
**
**		Custom control with a slider that contains multiple knobs
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/private/twcMultiKnobSlider.hpp"

#include "ToolUIWx/twc/private/twcColorDialog.hpp"
#include "ToolUIWx/twc/twcEvent.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"


#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
BEGIN_EVENT_TABLE(twcKnob, wxButton)
EVT_MOUSE_EVENTS(twcKnob::OnMouseOver)
EVT_RIGHT_DOWN(twcKnob::OnMouseRightClick)
END_EVENT_TABLE()


namespace
{
	bool SortKnobs(twcKnob* d1, twcKnob* d2)
	{
		return d1->GetPos() < d2->GetPos();
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
twcKnob::twcKnob(wxWindow* i_parent, const float i_percentage, const wxSize& i_size, const wxString& i_label, 
				 int i_parentWidth, int i_parentHeight, maFloatRGBA& i_color)
				 : wxButton(i_parent, wxID_ANY, i_label , wxDefaultPosition, i_size),
				 m_parentWidth(i_parentWidth), m_parentHeight(i_parentHeight),
				 m_width(i_size.GetWidth()), m_height(i_size.GetHeight()),
				 m_color(i_color),
				 m_lastEventTime(0.0f)
{
	this->SetPos(i_percentage);
	this->SetColor(i_color);

	//this->Connect(this->GetId(), wxEVT_MOTION, wxMouseEventHandler(twcKnob::OnMouseOver), NULL, this);
	//i_parent->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(twcKnob::OnMouseLeftDown), NULL, this);
	//this->Connect(this->GetId(), wxEVT_RIGHT_DOWN, wxMouseEventHandler(twcKnob::OnMouseRightClick), NULL, this);
	
	/*this->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(twcKnob::OnMouseLeftDown), NULL, this);
	this->Connect(wxEVT_LEFT_UP, wxMouseEventHandler(twcKnob::OnMouseLeftUp), NULL, this);*/
}

//--------------------------------------------------------------------
//  Get the position of the knob, value from 0~1
//--------------------------------------------------------------------
float twcKnob::GetPos()
{
	return m_curPercentage; //return the result
}

//--------------------------------------------------------------------
//  Set the position of the knob, value from 0~1
//--------------------------------------------------------------------
void twcKnob::SetPos(float i_percent){
	if (i_percent < 0.0f)
		i_percent = 0.0f;

	if (i_percent > 1.0f)
		i_percent = 1.0f;

	if ((float)(clock() - m_lastEventTime) / CLOCKS_PER_SEC > 0.1f)
	{
		m_curPercentage = i_percent;
		m_curPos = (i_percent * (m_parentWidth - m_width));

		this->GetParent()->Freeze();
		this->Move(wxPoint(m_curPos, (m_parentHeight / 2) - (m_height / 2)));
		this->GetParent()->Thaw();

		SendValueChangeEvent();
		m_lastEventTime = clock();
	}
}

//--------------------------------------------------------------------
//  Set the position of the knob in pixel
//--------------------------------------------------------------------
void twcKnob::SetPos(int i_x)
{
	int X = m_width - i_x;

	//m_curPos = m_curPos - X + (m_width / 2);
	int pos = m_curPos - X + (m_width / 2);
	/*pos = (pos < 0) ? 0 : pos;
	pos = (pos > m_parentWidth) ? m_parentWidth : pos;
	*/
	m_curPercentage = (float)(pos) / (m_parentWidth - m_width);

	if(pos < 0)
	{
		//m_curPos = 0; 
		m_curPercentage = 0.0f;
	}

	if(pos > (m_parentWidth - m_width))
	{
		//m_curPos = m_parentWidth;
		m_curPercentage = 1.0f;	
	}

	SetPos(m_curPercentage);
}

//--------------------------------------------------------------------
//  Set the color of the knob using maFloatRGBA
//--------------------------------------------------------------------
void twcKnob::SetColor(maFloatRGBA& i_color)
{
	m_color = i_color;
	this->SetBackgroundColour(wxColour(i_color.GetRed() * 255, 
		i_color.GetGreen() * 255,
		i_color.GetBlue() * 255,
		i_color.GetAlpha() * 255));

	SendValueChangeEvent();
}

//--------------------------------------------------------------------
//  Set the color of the knob in wxColour
//--------------------------------------------------------------------
void twcKnob::SetColor(wxColour& i_color)
{
	m_color = maFloatRGBA((float)i_color.Red() / 255.0f, 
		(float)i_color.Green() / 255.0f,
		(float)i_color.Blue() / 255.0f,
		(float)i_color.Alpha() / 255.0f);
	SetColor(m_color);
}

//--------------------------------------------------------------------
//  Get the color of the knob
//--------------------------------------------------------------------
maFloatRGBA& twcKnob::GetColor()
{
	return m_color;
}

//--------------------------------------------------------------------
//  Callback: Color change from twcColorDialog
//--------------------------------------------------------------------
void twcKnob::OnColorChanged()
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

//--------------------------------------------------------------------
//  Callback: position change
//--------------------------------------------------------------------
void twcKnob::OnMouseOver(wxMouseEvent &i_event)
{
	wxPoint point = i_event.GetPosition();
	if (i_event.LeftIsDown()){
		this->SetFocus();
		this->WarpPointer(point.x, (m_height / 2.0f));  //DOES NOT WORK ON MAC
		this->SetPos(i_event.GetX());
	}
	i_event.Skip();
}

//--------------------------------------------------------------------
//  Callback: right click on knob
//--------------------------------------------------------------------
void twcKnob::OnMouseRightClick(wxMouseEvent& i_event)
{
	this->SetFocus();

	// If we already have a color dialog, then don't do anything
	if (m_pColorDialog && (m_pColorDialog == twcColorDialog::Instance))
		return;

	// if a color picker is already up, close it
	if (twcColorDialog::Instance)
	{
		twcColorDialog::Instance->Close();
	}

	//DBG_ASSERT(twxSystem::g_pMainForm, "MainForm not yet initialized.");
	m_pColorDialog = new twcColorDialog(twxSystem::g_pMainForm, m_color);
	twcColorDialog::Instance = m_pColorDialog;
	//this->Connect( m_pColorDialog->GetId(), wxEVT_VALUE_CHANGED,
	//				wxCommandEventHandler(twcColorPicker::OnColorDialogChange) );
	m_pColorDialog->SetColorChangedCallback(std::bind(
		&twcKnob::OnColorChanged, this));
	m_pColorDialog->Show();

	SendValueChangeEvent();
}


//--------------------------------------------------------------------
//  Send out value change event for this control
//--------------------------------------------------------------------
void twcKnob::SendValueChangeEvent()
{
	wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
	changed_event.SetClientData((void*)this);
	this->ProcessCommand(changed_event);
}

///////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
// Background of the slider
//--------------------------------------------------------------------

twcSliderBackground::twcSliderBackground(wxWindow* i_parent, const wxPoint& i_pos, const wxSize& i_size)
: wxPanel(i_parent, wxID_ANY, i_pos, i_size)
{
	m_width = i_size.GetWidth();
	m_height = i_size.GetHeight();
}


///////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------
//--------------------------------------------------------------------

twcMultiKnobSlider::twcMultiKnobSlider(wxWindow* i_parent, const wxPoint& i_pos, 
									   const wxSize& i_backgroundSize, const wxSize& i_knobSize)
: wxPanel(i_parent)
{
	m_backgroundSize = i_backgroundSize;
	m_knobSize = i_knobSize;

	m_sliderBackground = new twcSliderBackground(this, i_pos, m_backgroundSize);
	m_currentKnob = NULL;
}

//--------------------------------------------------------------------
// Create a new knob on the slider
//--------------------------------------------------------------------
twcKnob* twcMultiKnobSlider::AddKnob(float i_percentage, maFloatRGBA& i_color)
{
	twcKnob* knob = new twcKnob(m_sliderBackground, 
		i_percentage,
		m_knobSize, 
		wxT(""), 
		m_backgroundSize.GetWidth(), 
		m_backgroundSize.GetHeight(), 
		i_color);

	m_knobList.push_back(knob);
	std::sort(m_knobList.begin(), m_knobList.end(), SortKnobs);	
	m_currentKnob = knob;
	
	this->Connect(knob->GetId(), wxEVT_VALUE_CHANGED, 
		wxCommandEventHandler(twcMultiKnobSlider::OnValueChange));
	SendValueChangeEvent(knob);
	return knob;
} 

twcKnob* twcMultiKnobSlider::AddKnob(int i_position, maFloatRGBA& i_color)
{
	return AddKnob(PositionToPercentage(i_position), i_color);
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
void twcMultiKnobSlider::SetValue(const maGradient& i_value)
{
	maGradient g = i_value;

	// Force the input contains at least 2 nodes
	if (g.GetSize() < 2)
	{
		DBG_WARNING("Input data doesn't contain at least size of 2");
		return;
	}

	m_currentKnob = NULL;
	for (int i = 0; i < m_knobList.size(); i++)
	{
		this->Disconnect(m_knobList[i]->GetId(), wxEVT_VALUE_CHANGED, wxCommandEventHandler(twcMultiKnobSlider::OnValueChange));
		delete m_knobList[i];
		m_knobList[i] = NULL;
	}
	m_knobList.clear();

	for (int i = 0; i < g.GetSize(); i++)
	{
		AddKnob(g[i].first, g[i].second);
	}
	SetCurrentKnobByIndex(0);
}

//--------------------------------------------------------------------
// Set current knob to i_index
//--------------------------------------------------------------------
void twcMultiKnobSlider::SetCurrentKnobByIndex(int i_index)
{
	if (m_knobList.size() > i_index)
	{
		m_currentKnob = m_knobList[i_index];
	}
}

//--------------------------------------------------------------------
// Set current knob to i_knob
//--------------------------------------------------------------------
void twcMultiKnobSlider::SetCurrentKnob(twcKnob* i_knob)
{
	std::vector<twcKnob*>::iterator it;
	it = std::find(m_knobList.begin(), m_knobList.end(), m_currentKnob);
	if (it != m_knobList.end())
	{
		m_currentKnob = i_knob;
	}
}

//--------------------------------------------------------------------
// Delete current knob
//--------------------------------------------------------------------
void twcMultiKnobSlider::DeleteCurrentKnob()
{
	// knobs can only be deleted when they are more than two
	if (m_knobList.size() <= 2)
	{
		return;
	}

	if (m_currentKnob)
	{
		std::vector<twcKnob*>::iterator it;
		it = std::find(m_knobList.begin(), m_knobList.end(), m_currentKnob);
		if (it != m_knobList.end())
		{
			// need to disconnect event here?
			this->Disconnect(m_currentKnob->GetId(), wxEVT_VALUE_CHANGED, wxCommandEventHandler(twcMultiKnobSlider::OnValueChange));
			m_knobList.erase(it);
			delete m_currentKnob;

			if(it == m_knobList.begin())
			{
				m_currentKnob = *it;
			}
			else
			{
				m_currentKnob = *(--it);
			}
			//ShowKnobProperty(m_CurrentKnob);
			//PaintNow();

			SendValueChangeEvent(m_currentKnob);
		}
	}
}

//--------------------------------------------------------------------
// Get the number of knobs on the slider
//--------------------------------------------------------------------
int twcMultiKnobSlider::GetKnobsNum()
{
	return m_knobList.size();
}

//--------------------------------------------------------------------
// Get knob list
//--------------------------------------------------------------------
void twcMultiKnobSlider::GetKnobList(std::vector<twcKnob*>& o_knobList)
{
	o_knobList =  m_knobList;
}

//--------------------------------------------------------------------
// Force to sort knob list
//--------------------------------------------------------------------
void twcMultiKnobSlider::SortKnobList()
{
	std::sort(m_knobList.begin(), m_knobList.end(), SortKnobs);
}

//--------------------------------------------------------------------
// Set current knob position in percentage
//--------------------------------------------------------------------
void twcMultiKnobSlider::SetCurrentKnobPos(float i_percentage)
{
	if (m_currentKnob)
	{
		m_currentKnob->SetPos(i_percentage);
	}
}

//--------------------------------------------------------------------
// Set current knob color
//--------------------------------------------------------------------
void twcMultiKnobSlider::SetCurrentKnobColor(maFloatRGBA& i_color)
{
	if (m_currentKnob)
	{
		m_currentKnob->SetColor(i_color);
	}
}

//--------------------------------------------------------------------
// Get current knob position in percentage
//--------------------------------------------------------------------
float twcMultiKnobSlider::GetCurrentKnobPos()
{
	if (m_currentKnob)
	{
		return m_currentKnob->GetPos();
	}

	return 0.0f;
}

//--------------------------------------------------------------------
// Get current knob color
//--------------------------------------------------------------------
void twcMultiKnobSlider::GetCurrentKnobColor(maFloatRGBA& o_color)
{
	maFloatRGBA c;

	if (m_currentKnob)
	{
		c = m_currentKnob->GetColor();
	}

	o_color = c;
}

//--------------------------------------------------------------------
//  Gather knob value change event and send one to parent
//--------------------------------------------------------------------
void twcMultiKnobSlider::OnValueChange(wxCommandEvent& i_Event)
{
	twcKnob* knob = reinterpret_cast<twcKnob*>(i_Event.GetClientData());
	if (knob)
	{
		SendValueChangeEvent(knob);
	}
}

//--------------------------------------------------------------------
//  Send out value change event for this control
//--------------------------------------------------------------------
void twcMultiKnobSlider::SendValueChangeEvent(twcKnob* i_knob)
{
	if (i_knob)
	{
		wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
		changed_event.SetClientData((void*)i_knob);
		i_knob->ProcessCommand(changed_event);
	}
}

//--------------------------------------------------------------------
//  Convert position on slider to percentage
//--------------------------------------------------------------------
float twcMultiKnobSlider::PositionToPercentage(int i_pos)
{
	float percentage = (float)(i_pos - (m_knobSize.GetWidth() / 2)) / 
		(m_backgroundSize.GetWidth() - m_knobSize.GetWidth());

	return percentage;
}


#endif // USE_WXWIDGETS
