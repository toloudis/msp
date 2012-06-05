/*****************************************************************************
**	gpxEffectOutline.hpp
**
**	This class is a thread-safe proxy for a effOutlineData.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_EFFECTOUTLINE_HPP
#error gpxEffectOutline.hpp multiply included
#endif
#define GPX_EFFECTOUTLINE_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif 


//============================================================================
//	forward references
//============================================================================
class effOutlineData;


//============================================================================
//============================================================================
class gpxEffectOutline : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxEffectOutline(effOutlineData &i_Effect);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxEffectOutline();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the effect when "Update()" is called.
	//--------------------------------------------------------------------
	void SetOutlineDepthScale(float i_OutlineDepthScale);
	void SetOutlineMinAngle(float i_OutlineMinAngle);
	void SetOutlineMaxAngle(float i_OutlineMaxAngle);
	void SetOutlineThickness(float i_OutlineThickness);
	void SetOutlineColor(const maFloatRGBA &i_OutlineColor);
	void SetUseDepths(bool i_UseDepths);
	void SetUseNormals(bool i_UseNormals);
	void SetOutlineMinWidth(float i_OutlineMinWidth);
	void SetOutlineMaxWidth(float i_OutlineMaxWidth);

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	effOutlineData &m_Effect;

#if USE_PROXIES
	float m_OutlineDepthScale;
	float m_OutlineMinAngle;
	float m_OutlineMaxAngle;
	float m_OutlineThickness;
	maFloatRGBA	m_OutlineColor;
	bool m_bUseDepths;
	bool m_bUseNormals;
	float m_OutlineMinWidth;
	float m_OutlineMaxWidth;
#endif
};
