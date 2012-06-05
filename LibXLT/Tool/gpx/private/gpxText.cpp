/*****************************************************************************
**	gpxText.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxText.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxText::gpxText(scrText &i_Text)
:	m_Text(i_Text)
{
#if USE_PROXIES
	m_String = i_Text.GetText();
	m_Color = i_Text.GetForegroundColor();
	m_bRenderable = i_Text.GetRenderable();
	m_Position = i_Text.GetPosition();
#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxText::~gpxText()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the character when "Update()" is called.
//--------------------------------------------------------------------
void gpxText::SetText(const itString& i_Text)
{
	PROXY_SET_OR_STORE(m_Text, SetText, m_String, i_Text);
}
void gpxText::SetForegroundColor( const maFloatRGBA& i_Color )
{
	PROXY_SET_OR_STORE(m_Text, SetForegroundColor, m_Color, i_Color);
}
void gpxText::SetRenderable(bool i_bRenderable)
{
	PROXY_SET_OR_STORE(m_Text, SetRenderable, m_bRenderable, i_bRenderable);
}
void gpxText::SetPosition(const maPoint3d& i_Position)
{
	PROXY_SET_OR_STORE(m_Text, SetPosition, m_Position, i_Position);
}

//--------------------------------------------------------------------
//	Get functions just return the data internally based on
//	the "Set" calls earlier.
//--------------------------------------------------------------------
const maPoint3d& gpxText::GetPosition() const
{
	return PROXY_GET(m_Text, GetPosition, m_Position);
}
const maFloatRGBA& gpxText::GetForegroundColor() const
{
	return PROXY_GET(m_Text, GetForegroundColor, m_Color);
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxText::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	m_Text.SetText( m_String );
	m_Text.SetForegroundColor( m_Color );
	m_Text.SetRenderable( m_bRenderable );
	m_Text.SetPosition( m_Position );

	this->SetNeedsUpdate(false);
#endif

	return true;
}
