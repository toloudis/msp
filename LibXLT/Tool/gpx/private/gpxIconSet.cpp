/*****************************************************************************
**	gpxIconSet.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxIconSet.hpp"

#include "Tool/gpx/gpxProxyMgr.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"
#include "Tool/icn/icnIconLayer.hpp"
#include "Tool/icn/icnIconSet.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to object it will control
//--------------------------------------------------------------------
gpxIconSet::gpxIconSet(icnIconSet &i_Object)
:	gpxSceneObject(i_Object),
	m_Object(i_Object)
{
#if USE_PROXIES
	const int num_layers = icnIconLayer::GetNumIconLayers();
	m_LayerScale.resize(num_layers);
	m_bLayerRenderable.resize(num_layers);

	for (int i=0; i<num_layers; ++i)
	{
		m_LayerScale[i] = i_Object.GetLayerScale(i);
		m_bLayerRenderable[i] = i_Object.GetLayerRenderable(i);
	}

#endif

	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxIconSet::~gpxIconSet()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//	Proxies functions just set the data internally. The changes are
//	only pushed through to the light when "Update()" is called.
//--------------------------------------------------------------------
void gpxIconSet::SetLayerScale(int i_IconLayerIndex, const maPoint3d& i_Scale)
{
#if USE_PROXIES
	if (m_LayerScale[i_IconLayerIndex] != i_Scale)
	{
		m_LayerScale[i_IconLayerIndex] = i_Scale;
		this->SetNeedsUpdate(true);
	}
#else
	m_Object.SetLayerScale( i_IconLayerIndex, i_Scale );
#endif

}
void gpxIconSet::SetLayerRenderable(int i_IconLayerIndex, bool i_Renderable)
{
#if USE_PROXIES
	if (m_bLayerRenderable[i_IconLayerIndex] != i_Renderable)
	{
		m_bLayerRenderable[i_IconLayerIndex] = i_Renderable;
		this->SetNeedsUpdate(true);
	}
#else
	m_Object.SetLayerRenderable( i_IconLayerIndex, i_Renderable );
#endif
}

//--------------------------------------------------------------------
// SetScale and SetRenderable are special because of the way the 
// gpxIconSet proxy keeps track of the icon layers scale and
// renderable flags. 
//--------------------------------------------------------------------
void  gpxIconSet::SetScale(const maVector3d& i_Scale)
{
	// Call base class
	gpxSceneObject::SetScale(i_Scale);

#if USE_PROXIES
	// Set this scale for all layers also
	const int num_layers = icnIconLayer::GetNumIconLayers();
	for (int i=0; i<num_layers; ++i)
	{
		m_LayerScale[i] = i_Scale;	
	}
#endif
}
void  gpxIconSet::SetRenderable(bool i_Renderable)
{
	// Call base class
	gpxSceneObject::SetRenderable(i_Renderable);

#if USE_PROXIES
	// Set this scale for all layers also
	const int num_layers = icnIconLayer::GetNumIconLayers();
	for (int i=0; i<num_layers; ++i)
	{
		m_bLayerRenderable[i] = i_Renderable;
	}
#endif
}

//--------------------------------------------------------------------
//	Get functions just return the data internally based on
//	the "Set" calls earlier.
//--------------------------------------------------------------------
maPoint3d gpxIconSet::GetLayerScale(int i_IconLayerIndex) const
{
#if USE_PROXIES
	return m_LayerScale[i_IconLayerIndex];
#else
	return m_Object.GetLayerScale( i_IconLayerIndex );
#endif
}
bool gpxIconSet::GetLayerRenderable(int i_IconLayerIndex) const
{
#if USE_PROXIES
	return m_bLayerRenderable[i_IconLayerIndex];
#else
	return m_Object.GetLayerRenderable( i_IconLayerIndex );
#endif
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxIconSet::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	gpxSceneObject::Update();

	const int num_layers = icnIconLayer::GetNumIconLayers();
	for (int i=0; i<num_layers; ++i)
	{
		m_Object.SetLayerScale(i, m_LayerScale[i]);
		m_Object.SetLayerRenderable(i, m_bLayerRenderable[i]);
	}

	this->SetNeedsUpdate(false);
#endif

	return true;
}
