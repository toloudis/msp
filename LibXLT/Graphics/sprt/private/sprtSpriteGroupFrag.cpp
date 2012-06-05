/****************************************************************************\
**	sprtSpriteGroupFrag.hpp
**
**	
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/sprt/sprtSpriteGroupFrag.hpp"


//============================================================================
//============================================================================
int sprtSpriteGroupFrag::sm_RendererId = 0;
int sprtSpriteGroupFrag::sm_StreakRendererId = 0;


//--------------------------------------------------------------------
// Set id for fragments in order to choose renderer
//--------------------------------------------------------------------
//static 
void sprtSpriteGroupFrag::SetRendererId(int i_RenderMode)
{
	sprtSpriteGroupFrag::sm_RendererId = i_RenderMode;
}
//static 
void sprtSpriteGroupFrag::SetStreakRendererId(int i_RenderMode)
{
	sprtSpriteGroupFrag::sm_StreakRendererId = i_RenderMode;
}

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
sprtSpriteGroupFrag::sprtSpriteGroupFrag( matMaterial* i_pMaterial ) 
:	g3dFragment( i_pMaterial, sprtSpriteGroupFrag::sm_RendererId ),
	m_pHead( NULL ),
	m_StreakFade(1.0),
	m_StreakTaper(1.0)
{
	SetModelSpaceBox( false );
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
sprtSpriteGroupFrag::~sprtSpriteGroupFrag() 
{

}

//------------------------------------------------------------------------
//	Deallocate - called when all device dependent resources should be
//	released.
//------------------------------------------------------------------------
void sprtSpriteGroupFrag::Deallocate()
{
}

//------------------------------------------------------------------------
//	Reallocate - called when the device has been Reset and resources can
//	be reloaded again.
//------------------------------------------------------------------------
void sprtSpriteGroupFrag::Reallocate()
{
}

//--------------------------------------------------------------------
//	SetHead - sets the head of the sprite group.
//	Does not own the sprite group.
//--------------------------------------------------------------------
void sprtSpriteGroupFrag::SetHead( sprtSpriteData* i_pHead )
{
	m_pHead = i_pHead;
}

//--------------------------------------------------------------------
// UpdateVertices - alter the position of the vertices in the
// given fragment. i_pNormals may be NULL, in which case the
// normals should remain as before. i_NumVertices should
// represent the number of positions given and should match the
// number of vertices in the fragment.
// This method can only be called on a fragment that was created
// with the "morphable" flag set to true.
//--------------------------------------------------------------------
//virtual 
void sprtSpriteGroupFrag::UpdateVertices( int i_NumVertices, 
						const maPoint3d* i_pVertices, 
						const maVector3d* i_pNormals )
{
	DBG_ASSERT(false, "UpdateVertices not implemented for sprtSpriteGroupFrag");
}

//----------------------------------------------------------------------------
//	GetNumVertices - the number of vertices in the vertex buffer
//----------------------------------------------------------------------------
//virtual 
int sprtSpriteGroupFrag::GetNumVertices() const
{
	DBG_ASSERT(false, "UpdateVertices not implemented for sprtSpriteGroupFrag");
	return 0;
}

//----------------------------------------------------------------------------
//	GetNumIndices - the number of indices in the index buffer
//----------------------------------------------------------------------------
int sprtSpriteGroupFrag::GetNumIndices() const
{
	DBG_ASSERT(false, "UpdateVertices not implemented for sprtSpriteGroupFrag");
	return 0;
}

//---------------------------------------------------------------------------
// GetVertexFormat - returns vertex format of vertex buffer using the
//	enumeration in g3dType.
//---------------------------------------------------------------------------
//virtual 
g3dType::VertexFormat sprtSpriteGroupFrag::GetVertexFormat() const
{
	DBG_ASSERT(false, "GetVertexFormat not implemented for sprtSpriteGroupFrag");
	return g3dType::e_Undefined;
}

//--------------------------------------------------------------------
//  Lock - returns the pointer to the vertex buffer copy in system memory.
//  This should be called when you need to modify the vertices
//  ONLY should be called on morphable fragments
//--------------------------------------------------------------------
//virtual 
unsigned char* sprtSpriteGroupFrag::Lock()
{
	DBG_ASSERT(false, "UpdateVertices not implemented for sprtSpriteGroupFrag");
	return NULL;
}

//--------------------------------------------------------------------
//  Unlock - updates the vertex buffer in VRAM. This should be called
//  when you are done modifying the vertices.
//	ONLY should be called on morphable fragments
//--------------------------------------------------------------------
//virtual 
void sprtSpriteGroupFrag::Unlock()
{
	DBG_ASSERT(false, "UpdateVertices not implemented for sprtSpriteGroupFrag");
}

//--------------------------------------------------------------------
//  LockIndices
//--------------------------------------------------------------------
//virtual 
unsigned char* sprtSpriteGroupFrag::LockIndices()
{
	DBG_ASSERT(false, "UpdateIndices not implemented for sprtSpriteGroupFrag");
	return NULL;
}

//--------------------------------------------------------------------
//  LockIndices
//--------------------------------------------------------------------
//virtual 
void sprtSpriteGroupFrag::UnlockIndices()
{
	DBG_ASSERT(false, "UpdateIndices not implemented for sprtSpriteGroupFrag");
}

//--------------------------------------------------------------------
//  ReadOnlyLock - DO NOT CALL THIS EVER. It's temporary for Renderman
//--------------------------------------------------------------------
unsigned char* sprtSpriteGroupFrag::ReadOnlyLock()
{
	DBG_ASSERT(false, "UpdateVertices not implemented for sprtSpriteGroupFrag");
	return NULL;
}

//--------------------------------------------------------------------
//  ReadOnlyUnlock - DO NOT CALL THIS EVER. It's temporary for Renderman
//--------------------------------------------------------------------
void sprtSpriteGroupFrag::ReadOnlyUnlock()
{
	DBG_ASSERT(false, "UpdateVertices not implemented for sprtSpriteGroupFrag");
}

//----------------------------------------------------------------------------
// ReadOnlyLockIndices() - DO NOT CALL THIS EVER. It's temporary for Renderman
//----------------------------------------------------------------------------
unsigned char* sprtSpriteGroupFrag::ReadOnlyLockIndices()
{
	DBG_ASSERT(false, "UpdateIndices not implemented for sprtSpriteGroupFrag");
	return NULL;
}

//----------------------------------------------------------------------------
// ReadOnlyUnlockIndices() - DO NOT CALL THIS EVER. It's temporary for Renderman
//----------------------------------------------------------------------------
void sprtSpriteGroupFrag::ReadOnlyUnlockIndices()
{
	DBG_ASSERT(false, "UpdateIndices not implemented for sprtSpriteGroupFrag");
}

//--------------------------------------------------------------------
// ComponentSort - let the fragment sort its internal components before
// rendering. The current model to world transformation and the camera 
// position are passed as arguments in order to do the sorting.
//--------------------------------------------------------------------
//virtual 
void sprtSpriteGroupFrag::ComponentSort(const maMatrix4x4& i_Transorm,
							const maPoint3d& i_CameraPos)
{

}

//--------------------------------------------------------------------
// Streak rendering renders a stretched polygon to represent the
// motion of a particle over time.
//--------------------------------------------------------------------
void sprtSpriteGroupFrag::SetRenderStreaks(bool i_bStreaks)
{
	m_bRenderStreaks = i_bStreaks;

	// Set the renderer id to control whether particles or streaks are rendered
	this->SetRenderMode( i_bStreaks ? sm_StreakRendererId : sm_RendererId );
}
bool sprtSpriteGroupFrag::GetRenderStreaks() const
{
	return m_bRenderStreaks;
}

//--------------------------------------------------------------------
// Streak Fade is value from 0-1 to modify the alpha
// value of the stretched vertices.
//--------------------------------------------------------------------
float sprtSpriteGroupFrag::GetStreakFade() const
{
	return m_StreakFade;
}
void sprtSpriteGroupFrag::SetStreakFade(float i_Fade)
{
	m_StreakFade = i_Fade;
}

//--------------------------------------------------------------------
// Streak Taper is value from 0-1 to modify the scale
// of the stretched vertices.
//--------------------------------------------------------------------
float sprtSpriteGroupFrag::GetStreakTaper() const
{
	return m_StreakTaper;
}
void sprtSpriteGroupFrag::SetStreakTaper(float i_Taper)
{
	m_StreakTaper = i_Taper;
}

