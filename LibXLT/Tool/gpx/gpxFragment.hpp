/*****************************************************************************
**	gpxFragment.hpp
**
**	This class is a thread-safe proxy for a g3dFragment.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef GPX_FRAGMENT_HPP
#error gpxFragment.hpp multiply included
#endif
#define GPX_FRAGMENT_HPP

#ifndef GPX_PROXYOBJECT_HPP
#include "Tool/gpx/gpxProxyObject.hpp"
#endif 
#ifndef MA_POINT3D_HPP
#include "Core/Ma/maPoint3d.hpp"
#endif 

#include <vector>


//============================================================================
//	forward references
//============================================================================
class g3dFragment;


//============================================================================
//============================================================================
class gpxFragment : public gpxProxyObject
{
public:
	//--------------------------------------------------------------------
	// Constructor takes reference to fragment it will control
	//--------------------------------------------------------------------
	explicit gpxFragment(g3dFragment &i_Fragment);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~gpxFragment();

	//--------------------------------------------------------------------
	//	Proxies functions just set the data internally. The changes are
	//	only pushed through to the fragment when "Update()" is called.
	//--------------------------------------------------------------------
	void UpdateVertices( int i_NumVertices, 
						 const maPoint3d* i_pVertices, 
						 const maVector3d* i_pNormals = NULL );
	void SetCastsShadow(bool i_bShadow);
	void SetReceivesShadow(bool i_bShadow);
	void SetShadowHull(bool i_bShadow);
	void SetDoubleSided(bool i_bVal);
	void SetUseBakedTexture(bool i_bVal);
	void SetReceivesOcclusion(bool i_bOcclusion);
	void SetReceivesGI(bool i_bGI);

	//----------------------------------------------------------------------------
	//	GetNumVertices - the number of vertices in the vertex buffer
	//----------------------------------------------------------------------------
	int GetNumVertices() const;

	//--------------------------------------------------------------------
	// Update() pushes changes from proxy object into the graphics
	//	library object. Should return true if changes were made.
	//	This function will only be called if "NeedsUpdate" is true
	//	so it is not necessary to check this flag again.
	//--------------------------------------------------------------------
	virtual bool Update();

private:
	g3dFragment &m_Fragment;

// Extra copy of data if using proxy info
#if USE_PROXIES
	bool m_bUpdateVertices;
	std::vector<maPoint3d> m_Vertices;
	std::vector<maVector3d> m_Normals;

	bool m_bCastsShadows;
	bool m_bReceivesShadow;
	bool m_bShadowHull;
	bool m_bDoubleSided;
	bool m_bUseBakedTexture;
	bool m_bReceivesOcclusion;
	bool m_bReceivesGI;
#endif
};
