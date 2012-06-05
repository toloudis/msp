/*****************************************************************************
**	gpxEffectDisplacement.hpp
**
**	This class is a thread-safe proxy for a effDisplacementData.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_EFFECTDISPLACEMENT_HPP
#error gpxEffectDisplacement.hpp multiply included
#endif
#define GPX_EFFECTDISPLACEMENT_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 
#ifndef MA_VECTOR2D_HPP
#include "Core/Ma/maVector2d.hpp"
#endif 


//============================================================================
//	forward references
//============================================================================
class effDisplacementData;


//============================================================================
//============================================================================
class gpxEffectDisplacement : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxEffectDisplacement(effDisplacementData &i_Effect);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxEffectDisplacement();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the effect when "Update()" is called.
	//--------------------------------------------------------------------
	void SetScale(float i_Scale);
	void SetBias(float i_Bias);
	void SetBlur(float i_Blur);
	void SetTessellationValue(float i_TessellationValue);
	void SetObjUVScale(const maVector2d& i_ObjUVScale);

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	effDisplacementData &m_Effect;

#if USE_PROXIES
	float m_Scale;
	float m_Bias;
	float m_Blur;
	float m_TessellationValue;
	maVector2d m_ObjUVScale;
#endif
};
