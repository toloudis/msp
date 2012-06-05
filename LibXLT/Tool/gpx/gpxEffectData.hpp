/*****************************************************************************
**	gpxEffectData.hpp
**
**	This class is a thread-safe proxy for a effDisplacementData.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_EFFECTDATA_HPP
#error gpxEffectData.hpp multiply included
#endif
#define GPX_EFFECTDATA_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 


//============================================================================
//============================================================================
class gpxEffectData : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor 
	//--------------------------------------------------------------------
	gpxEffectData();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxEffectData();

	//--------------------------------------------------------------------
	//	Source data has changed, update target now or on Update()
	//--------------------------------------------------------------------
	void SetEffectDataChanged();

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual void UpdateTargetData() = 0;
};

//============================================================================
//============================================================================
template <class EffectDataType>
class gpxEffectDataTemplate : public gpxEffectData
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to two data structures, a source
	//	and a target. When updating, the source is copied to the target.
	//--------------------------------------------------------------------
	gpxEffectDataTemplate(const EffectDataType &i_Source,
				  EffectDataType &i_Target)
	  : m_SourceEffect(i_Source), m_TargetEffect(i_Target)
	{
	}

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual void UpdateTargetData() 
	{
		m_TargetEffect = m_SourceEffect;
	}

	const EffectDataType &m_SourceEffect;
	EffectDataType &m_TargetEffect;
};

//============================================================================
//	specific implementations
//============================================================================
class effSolidData;
typedef gpxEffectDataTemplate<effSolidData> gpxSolidData;
