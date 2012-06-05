/*****************************************************************************
**	gpxLight.hpp
**
**	This class is a thread-safe proxy for a g3dLight.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_LIGHT_HPP
#error gpxLight.hpp multiply included
#endif
#define GPX_LIGHT_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif 


//============================================================================
//	forward references
//============================================================================
class g3dLight;


//============================================================================
//============================================================================
class gpxLight : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to light it will control
	//--------------------------------------------------------------------
	explicit gpxLight(g3dLight &i_Light);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxLight() = 0;

	//----------------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are only
	//	pushed through to the light when "Update()" is called.
	//----------------------------------------------------------------------------
	void SetEnable( bool i_bEnable );
	void SetIntensity(const maFloatRGBA& i_Intensity);
	void SetIntensityFactor(float i_IntensityFactor);
	void SetCastsShadow(bool i_bShadow);
	void SetDiffuseEnabled(bool i_bEnabled);
	void SetSpecularEnabled(bool i_bEnabled);
	void SetAffectsGlow(bool i_bAffectsGlow);

	//--------------------------------------------------------------------
	// Update() for gpxLight only sets its values and does not
	//	ask for a lock on the mutex. The assumption is that this
	//	will be called from within the lock of the derived class.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	g3dLight  &m_Light;

// Extra copy of data if using proxy info
#if USE_PROXIES
	bool m_bEnabled;
	bool m_bShadow;
	maFloatRGBA m_Color;
	float m_IntensityFactor;
	bool m_bDiffuseEnabled;
	bool m_bSpecularEnabled;
	bool m_bAffectsGlow;
#endif
};
