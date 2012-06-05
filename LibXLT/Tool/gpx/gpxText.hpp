/*****************************************************************************
**	gpxText.hpp
**
**		This class is a thread-safe proxy for a scrText.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_TEXT_HPP
#error gpxText.hpp multiply included
#endif
#define GPX_TEXT_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 

#ifndef SCR_TEXT_HPP
#include "Graphics/Scr/scrText.hpp"
#endif 


//============================================================================
//============================================================================
class gpxText : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxText(scrText &i_Text);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxText();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the character when "Update()" is called.
	//--------------------------------------------------------------------
	void SetText(const itString& i_Text);
	void SetForegroundColor( const maFloatRGBA& i_Color );
	void SetRenderable(bool i_bRenderable);
	void SetPosition(const maPoint3d& i_Position);

	//--------------------------------------------------------------------
	//	Get functions just return the data internally based on
	//	the "Set" calls earlier.
	//--------------------------------------------------------------------
	const maPoint3d& GetPosition() const;
	const maFloatRGBA& GetForegroundColor() const;

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	scrText &m_Text;

#if USE_PROXIES
	itString m_String;
	maFloatRGBA m_Color;
	bool m_bRenderable;
	maPoint3d m_Position;
#endif
};
