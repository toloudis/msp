/****************************************************************************\
**	chnlClipDriver.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/wxGUI/chnlClipDriver.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Channels/chnlOperations.hpp"

#include "Core/undo/undoUndoMgr.hpp"


#ifdef USE_WXWIDGETS

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlClipDriver::chnlClipDriver(	tmlnDriver& i_Driver, 
								const std::string& i_Name, 
								const maTime& i_BeginTime, 
								const maTime& i_EndTime)
:	chnlChannelClip(	i_Name, 
						i_BeginTime, 
						i_EndTime,
						(chnlChannelClip::BlendType)i_Driver.GetBlendType(), 
						i_Driver.GetBlendTime(),
						i_Driver.IsRestoreOriginalValue()),
	m_Driver(i_Driver)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlClipDriver::chnlClipDriver(const chnlClipDriver& i_Clip)
:	chnlChannelClip( i_Clip ),
	m_Driver(i_Clip.m_Driver)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlClipDriver::~chnlClipDriver()
{
}

//--------------------------------------------------------------------
// Access to driver
//--------------------------------------------------------------------
tmlnDriver&	chnlClipDriver::Driver()
{
	return m_Driver;
}

//--------------------------------------------------------------------
// Update - get values from driver and set into clip. Called when
//	a property of the driver has changed.
//--------------------------------------------------------------------
void chnlClipDriver::Update()
{
	this->SetBeginTime( m_Driver.GetBeginTime() );
	this->SetEndTime( m_Driver.GetEndTime() );
	this->SetBlendType( (chnlChannelClip::BlendType)m_Driver.GetBlendType() );
	this->SetBlendTime( m_Driver.GetBlendTime() );
	this->SetRestore( m_Driver.IsRestoreOriginalValue() );

	maFloatRGBA color;
	color = m_Driver.GetClipFillColor();
	this->SetFillColor( color.GetRed(), color.GetGreen(), color.GetBlue() );

	this->SetName( m_Driver.GetName() );
}


//============================================================================
//	Virtual functions to be overriden
//============================================================================

//--------------------------------------------------------------------
// Set begin and end times using duration
//--------------------------------------------------------------------
//virtual
void chnlClipDriver::SetTime(const maTime& i_BeginTime, const maTime& i_Duration)
{
	if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

	//DBG_LOG2("SetTime!: %f %f", i_BeginTime, i_Duration);

	// Create undo for both properties into one block
	undoUndoMgr::BeginMultipleOperationBlock("Set Driver Time");
	m_Driver.CreateUndoForProperty(m_Driver.PropertyBeginTime());
	m_Driver.CreateUndoForProperty(m_Driver.PropertyEndTime());
	undoUndoMgr::EndMultipleOperationBlock();

	m_Driver.SetBeginTime(i_BeginTime);
	m_Driver.SetEndTime(i_BeginTime + i_Duration);
	chnlChannelClip::SetTime(i_BeginTime, i_Duration);

	chnlDialogUtil::UpdateTimeTicks();
	chnlOperations::NotifyScriptObject();
}

//--------------------------------------------------------------------
// Set blend attributes
//--------------------------------------------------------------------
//virtual
void chnlClipDriver::SetBlend(BlendType i_Blend, const maTime& i_BlendTime)
{
	if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

	//DBG_LOG2("SetBlend!: %d %f", i_Blend, i_BlendTime);

	// Create undo for both properties into one block
	undoUndoMgr::BeginMultipleOperationBlock("Set Driver Blend");
	m_Driver.CreateUndoForProperty(m_Driver.PropertyBlendType());
	m_Driver.CreateUndoForProperty(m_Driver.PropertyBlendTime());
	undoUndoMgr::EndMultipleOperationBlock();

	m_Driver.SetBlendType((tmlnDriver::BlendType)i_Blend);
	m_Driver.SetBlendTime(i_BlendTime);
	chnlChannelClip::SetBlend(i_Blend, i_BlendTime);
	chnlOperations::NotifyScriptObject();
}

//--------------------------------------------------------------------
// Set the restore flag
//--------------------------------------------------------------------
//virtual
void chnlClipDriver::SetRestore(bool i_bRestore)
{
	if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

	//DBG_LOG("SetRestore!: " << i_bRestore);
	m_Driver.SetRestoreOriginalValue(i_bRestore);
	chnlChannelClip::SetRestore(i_bRestore);
	chnlOperations::NotifyScriptObject();
}

//--------------------------------------------------------------------
// display properties dialog for this clip
//--------------------------------------------------------------------
//virtual
void chnlClipDriver::ShowProperties()
{
	if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

	m_Driver.DoEditProperties();
	chnlOperations::NotifyScriptObject();
}

//--------------------------------------------------------------------
// allow driver to handle selection in its own way
//--------------------------------------------------------------------
//virtual
void chnlClipDriver::Select()
{
	if (&m_Driver == 0) return;	// if the user selected something else while this was open, don't let them crash

	m_Driver.DoSelect();
}

//--------------------------------------------------------------------
// select the 3D icon for this clip
//--------------------------------------------------------------------
//virtual
void chnlClipDriver::SelectIcon()
{
	m_Driver.DoSelectIcon();
}

//--------------------------------------------------------------------
// Return string to display when mouse hovers over control
//--------------------------------------------------------------------
//virtual
std::string chnlClipDriver::GetHoverDescription()
{
	if (&m_Driver == 0) return "";	// if the user selected something else while this was open, don't let them crash

	return m_Driver.GetHoverDescription();
}

//--------------------------------------------------------------------
// Return string to display when mouse is interacting 
//		(moving, resizing) over control
//--------------------------------------------------------------------
//virtual
std::string chnlClipDriver::GetInteractionDescription(const maTime& i_InteractStart, 
													   const maTime& i_InteractDuration)
{
	if (&m_Driver == 0) return "";	// if the user selected something else while this was open, don't let them crash

	return m_Driver.GetInteractionDescription(i_InteractStart, i_InteractDuration);
}

#endif // USE_WXWIDGETS