/*****************************************************************************
**	gpxPointLight.hpp
**
**	This class is a thread-safe proxy for a g3dPointLight.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_POINTLIGHT_HPP
#error gpxPointLight.hpp multiply included
#endif
#define GPX_POINTLIGHT_HPP

#ifndef GPX_LIGHT_HPP
#include "Tool/gpx/gpxLight.hpp"
#endif 
#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif 


//============================================================================
//	forward references
//============================================================================
class g3dPointLight;


//============================================================================
//============================================================================
class gpxPointLight : public gpxLight
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to light it will control
	//--------------------------------------------------------------------
	explicit gpxPointLight(g3dPointLight &i_Light);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxPointLight();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the light when "Update()" is called.
	//--------------------------------------------------------------------
	void SetPosition(const maPoint3d& i_Position);
	void SetFalloff0(float i_Val);
	void SetFalloff1(float i_Val);
	void SetFalloff2(float i_Val);
	void SetFalloff3(float i_Val);
	void SetFalloffStart(float i_Val);
	void SetRange(float i_Range);

	//--------------------------------------------------------------------
	//	Get functions just return the data internally based on
	//	the "Set" calls earlier.
	//--------------------------------------------------------------------
	const maPoint3d& GetPosition() const;

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	g3dPointLight &m_Light;

// Extra copy of data if using proxy info
#if USE_PROXIES
	maPoint3d m_Position;
	float m_f0, m_f1, m_f2, m_f3;
	float m_fStart;
	float m_fRange;
#endif
};
