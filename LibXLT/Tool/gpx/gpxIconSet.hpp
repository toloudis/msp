/*****************************************************************************
**	gpxIconSet.hpp
**
**	This class is a thread-safe proxy for a icnIconSet.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_ICONSET_HPP
#error gpxIconSet.hpp multiply included
#endif
#define GPX_ICONSET_HPP

#ifndef GPX_SCENEOBJECT_HPP
#include "Tool/gpx/gpxSceneObject.hpp"
#endif 

#include <vector>


//============================================================================
//	forward references
//============================================================================
class icnIconSet;


//============================================================================
//============================================================================
class gpxIconSet : public gpxSceneObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to object it will control
	//--------------------------------------------------------------------
	explicit gpxIconSet(icnIconSet &i_Object);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxIconSet();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the object when "Update()" is called.
	//--------------------------------------------------------------------
	void SetLayerScale(int i_IconLayerIndex, const maPoint3d& i_Scale);
	void SetLayerRenderable(int i_IconLayerIndex, bool i_bRender);

	//--------------------------------------------------------------------
	// SetScale and SetRenderable are special because of the way the 
	// gpxIconSet proxy keeps track of the icon layers scale and
	// renderable flags. 
	//--------------------------------------------------------------------
	virtual void  SetScale(const maVector3d& i_Scale);
	virtual void  SetRenderable(bool i_Renderable);

	//--------------------------------------------------------------------
	//	Get functions just return the data internally based on
	//	the "Set" calls earlier.
	//--------------------------------------------------------------------
	maPoint3d GetLayerScale(int i_IconLayerIndex) const;
	bool GetLayerRenderable(int i_IconLayerIndex) const;

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	icnIconSet &m_Object;

#if USE_PROXIES
	std::vector<maVector3d> m_LayerScale;
	std::vector<bool> m_bLayerRenderable;
#endif
};
