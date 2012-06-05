/****************************************************************************\
**	twcGradientColorBase.hpp
**
**		Custom control with gradient image and multiknobslider
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/private/twcGradientColorBase.hpp"

#include "ToolUIWx/twc/twcEvent.hpp"

#include <algorithm>
#include <wx/dcbuffer.h>

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
twcGradientColorBase::twcGradientColorBase(wxWindow* i_pParent)
:	wxPanel(i_pParent),
m_knobSize(14),
m_gradientPanelWidth(200),
m_gradientPanelHeight(80),
m_gradientPanelHeightOffset(10)
{	

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer2;	// Contain gradient panel and multiknobs slider
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	m_panel_gradientPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, 
		wxSize(m_gradientPanelWidth, m_gradientPanelHeight + m_gradientPanelHeightOffset));
	bSizer2->Add(m_panel_gradientPanel, 0, wxALL, 0);

	m_slider_colorSlider = new twcMultiKnobSlider(this, wxPoint(0, 0), wxSize(m_gradientPanelWidth, m_knobSize), wxSize(m_knobSize, m_knobSize));
	bSizer2->Add(m_slider_colorSlider, 0, wxALL, 0);
	bSizer1->Add(bSizer2, 0, wxEXPAND|wxALL, 0);

	wxBoxSizer* bSizer3;	// Contain color knob property
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	m_knobColorEdit = new twcColorRGBAEdit(this);
	bSizer4->Add(m_knobColorEdit, 3, wxALL | wxEXPAND, 0);
	bSizer3->Add(bSizer4, 0, wxALL | wxEXPAND, 2);


	wxBoxSizer* bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	m_text_colorPosition = new wxStaticText(this, wxID_ANY, wxT("Position"));
	bSizer5->Add(m_text_colorPosition, 0, wxALL, 0);
	m_colorPosition = new twcRangedFloat(this);
	m_colorPosition->ShowValue( true );
	m_colorPosition->SetMaximum( 1.0f );
	m_colorPosition->SetValue( 0.0f );
	m_colorPosition->SetMinimum( 0.0f );
	m_colorPosition->SetNumTicks( 500 );
	m_colorPosition->SetDecimalPlaces(3);
	m_colorPosition->AllowExpandRange(false);
	
	bSizer5->Add(m_colorPosition, 3, wxALL|wxEXPAND, 0);
	bSizer3->Add(bSizer5, 0, wxALL | wxEXPAND, 2);

	wxBoxSizer* bSizer6 = new wxBoxSizer( wxHORIZONTAL );
	m_button_colorDelete = new wxButton(this, wxID_ANY, wxT("Delete"));
	
	bSizer6->Add(m_button_colorDelete);
	bSizer3->Add(bSizer6, 0, wxALL | wxEXPAND, 2);

	bSizer1->Add(bSizer3, 0, wxALL | wxEXPAND, 5);

	m_panel_gradientPanel->Connect(m_panel_gradientPanel->GetId(), wxEVT_PAINT, wxPaintEventHandler(twcGradientColorBase::OnPaint), NULL, this);
	m_panel_gradientPanel->Connect(m_panel_gradientPanel->GetId(), wxEVT_LEFT_DOWN, wxMouseEventHandler(twcGradientColorBase::OnAddKnob), NULL, this);
	m_panel_gradientPanel->Connect(m_panel_gradientPanel->GetId(), wxEVT_ERASE_BACKGROUND, wxPaintEventHandler(twcGradientColorBase::OnEraseBackground), NULL, this);
	m_button_colorDelete->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(twcGradientColorBase::OnDeleteKnobClick), NULL, this);
	this->Connect(m_colorPosition->GetId(), wxEVT_VALUE_CHANGED, wxCommandEventHandler(twcGradientColorBase::OnPositionSliderChange));
	this->Connect(m_slider_colorSlider->GetId(), wxEVT_VALUE_CHANGED, wxCommandEventHandler(twcGradientColorBase::KnobValueChange));
	this->Connect(m_knobColorEdit->GetId(), wxEVT_VALUE_CHANGED, wxCommandEventHandler(twcGradientColorBase::KnobValueChange));
	
	Initialize();
	

	this->SetSizer( bSizer1 );
	this->Layout();
}

twcGradientColorBase::~twcGradientColorBase()
{
	//delete m_buffer;
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const maGradient& twcGradientColorBase::GetValue() const
{
	return m_value;
}

void twcGradientColorBase::SetValue(const maGradient& i_value)
{
	m_value = i_value;
	m_slider_colorSlider->SetValue(i_value);
}

//--------------------------------------------------------------------
// callback when adds knob
//--------------------------------------------------------------------
void twcGradientColorBase::OnAddKnob(wxMouseEvent& i_Event)
{
	wxColour c;
	wxClientDC dc(m_panel_gradientPanel);
	dc.GetPixel(i_Event.GetX(), i_Event.GetY(), &c);
	maFloatRGBA maColor((float)c.Red() / 255.0f, (float)c.Green() / 255.0f, (float)c.Blue() / 255.0f, (float)c.Alpha() / 255.0f);

	m_slider_colorSlider->AddKnob(i_Event.GetX(), maColor);
}

//--------------------------------------------------------------------
// callback any property of knob changes
//--------------------------------------------------------------------
void twcGradientColorBase::KnobValueChange(wxCommandEvent& i_Event)
{
	// If the client data contains twcKnob object then
	// it comes from twcknob itself so we need to re-sort the knob list
	twcKnob* knob = reinterpret_cast<twcKnob*>(i_Event.GetClientData());
	if (knob)
	{
		ShowKnobProperty(knob);
		m_slider_colorSlider->SetCurrentKnob(knob);
		m_slider_colorSlider->SortKnobList();
	}
	else
	{
		// event from the color property window
		maFloatRGBA c = m_knobColorEdit->GetValue();
		m_slider_colorSlider->SetCurrentKnobColor(c);
	}
	
	WriteIntoValue();

	// Send event to notify parent
	wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
	// Use one of the child to process the command
	m_button_colorDelete->ProcessCommand(changed_event);

	// Any value change of knob causes panel re-paint
	PaintNow();

	
}

//--------------------------------------------------------------------
// modify m_value according to the knob position/color
//--------------------------------------------------------------------
void twcGradientColorBase::WriteIntoValue()
{
	std::vector<twcKnob*> knobList;
	m_slider_colorSlider->GetKnobList(knobList);

	m_value.Clear();
	for (int i = 0; i < knobList.size(); i++)
	{
		m_value.AddNode(knobList[i]->GetPos(), knobList[i]->GetColor());
	}
}

//--------------------------------------------------------------------
// callback when position slider changes in knob property window
//--------------------------------------------------------------------
void twcGradientColorBase::OnPositionSliderChange(wxCommandEvent& i_Event)
{
	//float percentage = (float)m_slider_colorPosition->GetValue()/(float)m_slider_colorPosition->GetMax();
	float percentage = (float)m_colorPosition->GetValue() / (float) m_colorPosition->GetMaximum();
	m_slider_colorSlider->SetCurrentKnobPos(percentage);
}

//--------------------------------------------------------------------
// callback when deletes button press in knob property window
//--------------------------------------------------------------------
void twcGradientColorBase::OnDeleteKnobClick(wxMouseEvent& i_Event)
{
	m_slider_colorSlider->DeleteCurrentKnob();
}

//--------------------------------------------------------------------
// callback function when paints gradient panel
//--------------------------------------------------------------------
void twcGradientColorBase::OnPaint(wxPaintEvent& i_Event)
{
	//wxPaintDC dc(m_panel_gradientPanel);
	wxBufferedPaintDC dc(m_panel_gradientPanel, m_buffer);
	//PrepareDC(dc);

	PaintGradient(dc);
	i_Event.Skip();
}

//--------------------------------------------------------------------
// force to paint gradient panel
//--------------------------------------------------------------------
void twcGradientColorBase::PaintNow()
{
	wxClientDC dc(m_panel_gradientPanel);
	//PrepareDC(dc);

	wxBufferedDC dcBuffer(&dc, m_buffer);
	PaintGradient(dcBuffer);
}

//--------------------------------------------------------------------
// paint gradient panel
//--------------------------------------------------------------------
void twcGradientColorBase::PaintGradient(wxDC& dc)
{

	wxColour c = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
	wxPen pen = dc.GetPen();
	wxBrush brush = dc.GetBrush();

	dc.SetBrush(wxBrush(c));
	dc.SetPen(wxPen(m_panel_gradientPanel->GetBackgroundColour(), 0, wxTRANSPARENT));
	dc.DrawRectangle(m_panel_gradientPanel->GetRect());
	
	dc.SetBrush(brush);
	dc.SetPen(pen);

	wxRect rcAll = GetGradientPanelDrawable();
	rcAll.SetHeight(rcAll.GetHeight() - m_gradientPanelHeightOffset);

	std::vector<twcKnob*> knobList;
	m_slider_colorSlider->GetKnobList(knobList);

	if (knobList.size() > 0)
	{
		wxRect rc(rcAll.GetLeft(), rcAll.GetTop(), 
			knobList[0]->GetPos() * rcAll.GetWidth() + 0.5f, rcAll.GetHeight());
		dc.GradientFillLinear(rc, 
			knobList[0]->GetBackgroundColour(), 
			knobList[0]->GetBackgroundColour());
	}

	if (knobList.size() > 1)
	{
		for (int i = 0; i < knobList.size() - 1; i++)
		{
			wxRect rc(knobList[i]->GetPos() * rcAll.GetWidth() + rcAll.GetLeft() - 0.5f, rcAll.GetTop(), 
				(knobList[i+1]->GetPos() - knobList[i]->GetPos()) * rcAll.GetWidth()+ 0.5f, rcAll.GetHeight());
			dc.GradientFillLinear(rc, knobList[i]->GetBackgroundColour(), knobList[i+1]->GetBackgroundColour());
		}
	}

	if (knobList.size() > 0)
	{
		wxRect rc = wxRect(knobList[knobList.size() - 1]->GetPos() * rcAll.GetWidth() + rcAll.GetLeft() - 0.5f, rcAll.GetTop(), 
			(1.0f - knobList[knobList.size() - 1]->GetPos()) * rcAll.GetWidth() + 0.5f, rcAll.GetHeight());
		dc.GradientFillLinear(rc, 
			knobList[knobList.size() - 1]->GetBackgroundColour(), 
			knobList[knobList.size() - 1]->GetBackgroundColour());
	}

	// Draw color marker indicator
	wxRect rcBottom = GetGradientPanelDrawable();
	wxRect rcIndicator = wxRect(rcBottom.GetLeft(), rcBottom.GetTop() + m_gradientPanelHeight,
		rcBottom.GetWidth(), m_gradientPanelHeightOffset);

	/*wxPen pen = dc.GetPen();
	wxBrush brush = dc.GetBrush();

	dc.SetBrush(wxBrush(m_panel_gradientPanel->GetBackgroundColour()));
	dc.SetPen(wxPen(m_panel_gradientPanel->GetBackgroundColour(), 0,wxTRANSPARENT));
	dc.DrawRectangle(rcIndicator);

	dc.SetBrush(brush);
	dc.SetPen(pen);*/
	int posX = m_slider_colorSlider->GetCurrentKnobPos() * rcBottom.GetWidth();
	dc.DrawRectangle(posX + m_knobSize / 2 - 1, rcIndicator.GetTop(), 2, rcIndicator.GetHeight());

	//this->Thaw();
}

//--------------------------------------------------------------------
// Initialize
//--------------------------------------------------------------------
void twcGradientColorBase::Initialize()
{
	InitializeKnobs(m_slider_colorSlider);

	//m_buffer = new wxBitmap(m_panel_gradientPanel->GetSize().GetWidth(), m_panel_gradientPanel->GetSize().GetHeight());
	m_buffer = wxBitmap(m_panel_gradientPanel->GetSize().GetWidth(), m_panel_gradientPanel->GetSize().GetHeight());
	/*wxMemoryDC dc;
	dc.SelectObject(m_buffer);
	dc.SetBrush(wxBrush(m_panel_gradientPanel->GetBackgroundColour()));
	dc.SetPen(wxPen(m_panel_gradientPanel->GetBackgroundColour(), wxTRANSPARENT));
	dc.DrawRectangle(m_panel_gradientPanel->GetRect());*/
}

//--------------------------------------------------------------------
// Initialize knobs
//--------------------------------------------------------------------
void twcGradientColorBase::InitializeKnobs(twcMultiKnobSlider* i_slider)
{
	/*twcKnob *knob1 = m_slider_colorSlider->AddKnob(0.0f, maFloatRGBA(0.0, 0.0, 0.0, 1.0)); 

	twcKnob *knob2 = m_slider_colorSlider->AddKnob(1.0f, maFloatRGBA(1.0, 1.0, 1.0, 1.0)); 

	WriteIntoValue();*/
}

//--------------------------------------------------------------------
// Initialize knobs
//--------------------------------------------------------------------
void twcGradientColorBase::ShowKnobProperty(twcKnob* knob)
{
	if (knob)
	{
		float pos = knob->GetPos();
		//m_slider_colorPosition->SetValue(m_slider_colorPosition->GetMax() * pos);
		m_colorPosition->SetValue(m_colorPosition->GetMaximum() * pos);
		m_knobColorEdit->SetValue(knob->GetColor());
		//DrawKnobIndicator(knob);

		if (m_slider_colorSlider->GetKnobsNum() <= 2)
		{
			m_button_colorDelete->Disable();
		}
		else
		{
			m_button_colorDelete->Enable();
		}

	}
}

//--------------------------------------------------------------------
// Get drawable rect of gradient panel
//--------------------------------------------------------------------
wxRect twcGradientColorBase::GetGradientPanelDrawable()
{
	wxRect rc = m_panel_gradientPanel->GetRect();
	rc.SetLeft(rc.GetLeft() + m_knobSize / 2);
	rc.SetWidth(rc.GetWidth() - m_knobSize);

	return rc;
}

//--------------------------------------------------------------------
// Draw knob indicator
//--------------------------------------------------------------------
void twcGradientColorBase::DrawKnobIndicator(twcKnob* knob)
{
	if (knob)
	{
		wxClientDC dc(m_panel_gradientPanel);
		wxBufferedDC dcBuffer(&dc, m_buffer);
		
		wxRect rcAll = GetGradientPanelDrawable();
		wxRect rcIndicator = wxRect(rcAll.GetLeft(), rcAll.GetTop() + m_gradientPanelHeight,
			rcAll.GetWidth(), m_gradientPanelHeightOffset);
		
		// clean indicator area
		this->Freeze();
		/*dcBuffer.SetBrush(wxBrush(m_panel_gradientPanel->GetBackgroundColour()));
		dcBuffer.SetPen(wxPen(m_panel_gradientPanel->GetBackgroundColour(), wxTRANSPARENT));
		dcBuffer.DrawRectangle(rcIndicator);*/

		int posX = knob->GetPos() * rcAll.GetWidth();
		dcBuffer.DrawRectangle(posX + m_knobSize / 2 - 1, rcIndicator.GetTop(), 2, rcIndicator.GetHeight());
		this->Thaw();
	}
}

//--------------------------------------------------------------------
// Custom event handler for erase background event
// Note: this function does nothing. It is used to achieve 
// double buffering when painting gradient panel
//--------------------------------------------------------------------
void twcGradientColorBase::OnEraseBackground(wxPaintEvent& i_event)
{
}

#endif // USE_WXWIDGETS
