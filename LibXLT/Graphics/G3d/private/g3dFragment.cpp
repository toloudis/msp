/****************************************************************************\
**	g3dFragment.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dFragment.hpp"

#include "Graphics/eff/effOcclusionData.hpp"
#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/g2d/g2dResetHandler.hpp"

#include <algorithm>
#include <list>


//============================================================================
//============================================================================
namespace
{
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::list<g3dFragment*> l_Frags;

//============================================================================
//============================================================================
class g3dFragmentReloader : public g2dResetHandler
{
	public:
		//------------------------------------------------------------------------
		//	Deallocate is called when all device dependent resources should be
		//	released.
		//------------------------------------------------------------------------
		virtual void Deallocate();

		//------------------------------------------------------------------------
		//	Allocate is called when the device has been Reset and resources can
		//	be reloaded again.
		//------------------------------------------------------------------------
		virtual void Reallocate();
};

//------------------------------------------------------------------------
//	Deallocate is called when all device dependent resources should be
//	released.
//------------------------------------------------------------------------
void g3dFragmentReloader::Deallocate()
{
	std::list<g3dFragment*>::iterator i = l_Frags.begin(), end = l_Frags.end();

	while (i != end)
	{
		(*i)->Deallocate();
		++i;
	}
}

//------------------------------------------------------------------------
//	Allocate is called when the device has been Reset and resources can
//	be reloaded again.
//------------------------------------------------------------------------
void g3dFragmentReloader::Reallocate()
{
	std::list<g3dFragment*>::iterator i = l_Frags.begin(), end = l_Frags.end();

	while (i != end)
	{
		(*i)->Reallocate();
		++i;
	}
}

bool l_bNeedToAddResetHandler = true;

}

//--------------------------------------------------------------------
//  Constructor
//--------------------------------------------------------------------
g3dFragment::g3dFragment( matMaterial* i_pMaterial, 
						  int i_nRenderMode,
						  bool i_bMorphable,
						  bool i_bComponentSort )
:	m_pMaterial( i_pMaterial ),
	m_nRenderMode( i_nRenderMode ),
	m_bIsOccluder( false ),
	m_bReceivesOcclusion( true ),
	m_bReceivesGI( true ),
	m_bDeferTransparency( false ),
	m_bUseDitheredShadow(false),
	m_DitherAlphaBias(0.25f),
	m_bModelSpaceVertices(true),
	m_bReceivesBake(false),
	m_bHardwareTesselate(false),
	m_fHardwareTessellateVal( 1.0f ),
	m_bHasSkinning(false),
	m_bHasVelocityBuffer(false),
	m_pTessellateMeshTexture( NULL )
{
	m_Flags.m_bMorphable = i_bMorphable;
	m_Flags.m_bComponentSort = i_bComponentSort;
	m_Flags.m_bModelSpaceBox = true;
	m_Flags.m_bCastsShadow = false;
	m_Flags.m_bReceivesShadow = false;
	m_Flags.m_bShadowHull = false;
	m_Flags.m_bDoubleSided = false;
	m_Flags.m_bUseBakedTexture = false;
//	m_Flags.m_bDrawWireframe = false;
//	m_pOcclusionData = new effOcclusionData;
	m_Flags.m_bHair = false;

	if (l_bNeedToAddResetHandler)
	{
		g2dResetHandler::AddResetHandler(new g3dFragmentReloader);
		l_bNeedToAddResetHandler = false;
	}

	l_Frags.push_back(this);
}

//--------------------------------------------------------------------
//  Copy Constructor
//--------------------------------------------------------------------
g3dFragment::g3dFragment(const g3dFragment& i_Frag)
:	m_pMaterial( i_Frag.m_pMaterial ),
	m_nRenderMode( i_Frag.m_nRenderMode ),
	m_BoundingBox(i_Frag.m_BoundingBox),
	m_bIsOccluder( i_Frag.m_bIsOccluder ),
	m_bReceivesOcclusion(  i_Frag.m_bReceivesOcclusion ),
	m_bReceivesGI( i_Frag.m_bReceivesGI ),
	m_bDeferTransparency(  i_Frag.m_bDeferTransparency ),
	m_bUseDitheredShadow( i_Frag.m_bUseDitheredShadow),
	m_DitherAlphaBias( i_Frag.m_DitherAlphaBias),
	m_bModelSpaceVertices( i_Frag.m_bModelSpaceVertices),
	m_bReceivesBake( i_Frag.m_bReceivesBake),
	m_bHardwareTesselate( i_Frag.m_bHardwareTesselate),
	m_bHasSkinning( false ),
	m_bHasVelocityBuffer( false ),
	m_Flags( i_Frag.m_Flags ),
	m_pTessellateMeshTexture( i_Frag.m_pTessellateMeshTexture )
{
//	m_pOcclusionData = new effOcclusionData;
	l_Frags.push_back(this);
}

//--------------------------------------------------------------------
//  Destructor
//--------------------------------------------------------------------
g3dFragment::~g3dFragment()
{
	//delete m_pOcclusionData;

	std::list<g3dFragment*>::iterator i = std::find(l_Frags.begin(), l_Frags.end(), this);
	DBG_ASSERT(i != l_Frags.end(), "fragment not found in list of fragments in ~g3dFragment()");
	
	if (i != l_Frags.end())
		l_Frags.erase(i);
}

//--------------------------------------------------------------------
//	SetRenderMode sets the rendering mode for the fragment.
//--------------------------------------------------------------------
void g3dFragment::SetRenderMode( int i_nRenderMode )
{
	m_nRenderMode = i_nRenderMode;
}

//--------------------------------------------------------------------
//  SetMaterial
//--------------------------------------------------------------------
void g3dFragment::SetMaterial( matMaterial* i_pMaterial )
{
	m_pMaterial = i_pMaterial;
}

//--------------------------------------------------------------------
//	SetOriginalMaterial sets the material that existed before the highlight
//--------------------------------------------------------------------
void g3dFragment::SetOriginalMaterial( matMaterial* i_pOriginalMaterial )
{
	m_pOriginalMaterial = i_pOriginalMaterial;
}

//----------------------------------------------------------------------------
//	SetModelSpaceBox - set true if bounding box is in model space (default) or
//	false if it is in world space
//----------------------------------------------------------------------------
void g3dFragment::SetModelSpaceBox( bool i_bModelSpaceBox )
{
	m_Flags.m_bModelSpaceBox = i_bModelSpaceBox;
}

//----------------------------------------------------------------------------
//	GetCount returns the number of g3dFragments in existence, primarily for
//	leak checking.
//----------------------------------------------------------------------------
int g3dFragment::GetCount()
{
	return l_Frags.size();
}

//effOcclusionData* g3dFragment::GetOcclusionData() const
//{
//	return m_pOcclusionData;
//}

//----------------------------------------------------------------------------
//	Return uv scaling factors to keep uvs within 0..1
//----------------------------------------------------------------------------
void g3dFragment::GetUVBakeFactors(maPoint2d& o_Scale, maPoint2d& o_Translate) const
{
	// default impl
	o_Scale = maPoint2d(1,1);
	o_Translate = maPoint2d(0,0);
}

//----------------------------------------------------------------------------
//	Return uv overlap factor
//----------------------------------------------------------------------------
float g3dFragment::GetUVOverlapFactor() const
{
	// default impl
	return 0.0f;
}

//----------------------------------------------------------------------------
//	GetSize returns approximate amount of memory (in bytes) being
// used by this fragment
//----------------------------------------------------------------------------
unsigned int g3dFragment::GetSize() const
{
	return 0;
}

//--------------------------------------------------------------------
// DitherAlphaBias is value from 0-1 to shift the shadows toward more
// or less translucency (using random dither pattern in the shader)
//--------------------------------------------------------------------
void g3dFragment::SetShadowDithering(bool i_bUseDither, float i_DitherAlphaBias)
{
	m_bUseDitheredShadow = i_bUseDither;
	m_DitherAlphaBias = i_DitherAlphaBias;
}
void g3dFragment::GetShadowDithering(bool& o_bUseDither, float& o_DitherAlphaBias) const
{
	o_bUseDither = m_bUseDitheredShadow;
	o_DitherAlphaBias = m_DitherAlphaBias;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g3dFragment::SetHasSkinning(bool i_bSkinning)
{
	m_bHasSkinning = i_bSkinning;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void g3dFragment::SetHasVelocityBuffer(bool i_bVelocityBuffer)
{
	m_bHasVelocityBuffer = i_bVelocityBuffer;
}

//--------------------------------------------------------------------
// GetFragmentName()
//--------------------------------------------------------------------
std::string g3dFragment::GetFragmentName()
{
	return m_FragmentName;
}

//--------------------------------------------------------------------
// SetFragmentName()
//--------------------------------------------------------------------
void g3dFragment::SetFragmentName( std::string i_Name )
{
	m_FragmentName = i_Name;
}
