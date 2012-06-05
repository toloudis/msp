/*****************************************************************************
**	gpxFragment.cpp
**
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Tool/gpx/gpxFragment.hpp"

#include "Graphics/G3d/g3dFragment.hpp"
#include "Tool/gpx/gpxProxyUtil.hpp"


//--------------------------------------------------------------------
// Constructor takes reference to fragment it will control
//--------------------------------------------------------------------
gpxFragment::gpxFragment(g3dFragment &i_Fragment)
:	m_Fragment(i_Fragment)
{
#if USE_PROXIES
	m_bUpdateVertices = false;

	m_bCastsShadows = i_Fragment.GetCastsShadow();
	m_bReceivesShadow = i_Fragment.GetReceivesShadow();
	m_bShadowHull = i_Fragment.IsShadowHull();
	m_bDoubleSided = i_Fragment.GetDoubleSided();
	m_bReceivesOcclusion = i_Fragment.GetReceivesOcclusion();
	m_bReceivesGI = i_Fragment.GetReceivesGI();
	m_bUseBakedTexture = i_Fragment.GetUseBakedTexture();
#endif
	PROXY_ADD();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
gpxFragment::~gpxFragment()
{
	PROXY_REMOVE();
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void gpxFragment::UpdateVertices( int i_NumVertices, 
								  const maPoint3d* i_pVertices, 
								  const maVector3d* i_pNormals )
{
	DBG_ASSERT(i_pVertices, "Cannot have a NULL pointer for vertex data.");

#if USE_PROXIES
	m_bUpdateVertices = true;
	m_Vertices.resize(i_NumVertices);	
	memcpy(&m_Vertices[0], i_pVertices, i_NumVertices*sizeof(maPoint3d));
	if (i_pNormals)
	{
		m_Normals.resize(i_NumVertices);	
		memcpy(&m_Normals[0], i_pNormals, i_NumVertices*sizeof(maVector3d));
	}
	else m_Normals.clear();
	this->SetNeedsUpdate(true);
#else
	m_Fragment.UpdateVertices( i_NumVertices, i_pVertices, i_pNormals );
#endif
}
void gpxFragment::SetCastsShadow(bool i_bShadow)
{
	PROXY_SET_OR_STORE(m_Fragment, SetCastsShadow, m_bCastsShadows, i_bShadow);
}
void gpxFragment::SetReceivesShadow(bool i_bShadow)
{
	PROXY_SET_OR_STORE(m_Fragment, SetReceivesShadow, m_bReceivesShadow, i_bShadow);
}
void gpxFragment::SetShadowHull(bool i_bShadow)
{
	PROXY_SET_OR_STORE(m_Fragment, SetShadowHull, m_bShadowHull, i_bShadow);
}
void gpxFragment::SetDoubleSided(bool i_bVal)
{
	PROXY_SET_OR_STORE(m_Fragment, SetDoubleSided, m_bDoubleSided, i_bVal);
}
void gpxFragment::SetUseBakedTexture(bool i_bVal)
{
	PROXY_SET_OR_STORE(m_Fragment, SetUseBakedTexture, m_bUseBakedTexture, i_bVal);
}
void gpxFragment::SetReceivesOcclusion(bool i_bOcclusion)
{
	PROXY_SET_OR_STORE(m_Fragment, SetReceivesOcclusion, m_bReceivesOcclusion, i_bOcclusion);
}
void gpxFragment::SetReceivesGI(bool i_bGI)
{
	PROXY_SET_OR_STORE(m_Fragment, SetReceivesGI, m_bReceivesGI, i_bGI);
}

//----------------------------------------------------------------------------
//	GetNumVertices - the number of vertices in the vertex buffer
//----------------------------------------------------------------------------
int gpxFragment::GetNumVertices() const
{
	return m_Fragment.GetNumVertices();
}

//--------------------------------------------------------------------
// Update() pushes changes from proxy object into the graphics
//	library object. Should return true if changes were made.
//	This function will only be called if "NeedsUpdate" is true
//	so it is not necessary to check this flag again.
//--------------------------------------------------------------------
//virtual 
bool gpxFragment::Update()
{
#if USE_PROXIES
	//envScopedLock proxy_lock(this->GetMutex());

	if (m_bUpdateVertices)
	{
		const maVector3d* pNormals = (m_Normals.empty()) ? NULL : &m_Normals[0];
		m_Fragment.UpdateVertices( m_Vertices.size(), &m_Vertices[0], pNormals );
		m_bUpdateVertices = false;
	}

	m_Fragment.SetCastsShadow(m_bCastsShadows);
	m_Fragment.SetReceivesShadow(m_bReceivesShadow);
	m_Fragment.SetDoubleSided(m_bDoubleSided);
	m_Fragment.SetUseBakedTexture(m_bUseBakedTexture);
	m_Fragment.SetShadowHull(m_bShadowHull);
	m_Fragment.SetReceivesOcclusion(m_bReceivesOcclusion);
	m_Fragment.SetReceivesGI(m_bReceivesGI);

	this->SetNeedsUpdate(false);
#endif

	return true;
}
