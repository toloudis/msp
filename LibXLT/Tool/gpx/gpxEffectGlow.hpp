/*****************************************************************************
**	gpxEffectGlow.hpp
**
**	This class is a thread-safe proxy for a effGlowData.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_EFFECTGLOW_HPP
#error gpxEffectGlow.hpp multiply included
#endif
#define GPX_EFFECTGLOW_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 
#ifndef MA_VECTOR4D_HPP
#include "Core/Ma/maVector4d.hpp"
#endif 


//============================================================================
//	forward references
//============================================================================
class effGlowData;


//============================================================================
//============================================================================
class gpxEffectGlow : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxEffectGlow(effGlowData &i_Effect);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxEffectGlow();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the effect when "Update()" is called.
	//--------------------------------------------------------------------
	void SetGlowAmount(float i_GlowAmount);
	void SetGlowScale(const maVector4d& i_GlowScale);
	void SetGlowSize(float i_GlowSize);
	void SetConstantGlow(bool i_bConstantGlow);

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	effGlowData &m_Effect;

#if USE_PROXIES
	float m_GlowAmount;
	maVector4d m_GlowScale;
	float m_GlowSize;
	bool m_bConstantGlow;
#endif
};
