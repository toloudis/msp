/****************************************************************************\
**	hairModelFragVk.cpp
**
**	GraphicsVulkan port of GraphicsDX11/hair/private/hairModelFrag.cpp
**	(namespace gvk).
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_HAIR_MODELFRAGVK_HPP
#include "GraphicsVulkan/hair/hairModelFragVk.hpp"
#endif
#ifndef GVK_VKDBUFFER_HPP
#include "GraphicsVulkan/vkd/vkdBuffer.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/G3d/g3dPrefs.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif

#include <cmath>
#include <vector>

namespace gvk
{

//============================================================================
//============================================================================
int hairModelFragVk::sm_RendererId = 0;

//--------------------------------------------------------------------
// Set id for fragments in order to choose renderer
//--------------------------------------------------------------------
//static
void hairModelFragVk::SetRendererId(int i_RenderMode)
{
	hairModelFragVk::sm_RendererId = i_RenderMode;
}

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
hairModelFragVk::hairModelFragVk( const mdlHairInfo &i_HairInfo, matMaterial* i_pMaterial )
: g3dFragment( i_pMaterial, hairModelFragVk::sm_RendererId ),
  m_HairInfo( i_HairInfo ),
  m_nNumVertices( 0 ),
  m_fTessellation( 0 ),
  m_bHardwareTessellate( false )
{
	this->SetHair( true );

	// Approximate the bounding box
	maAxisBox bounding_box;
	for( int i = 0; i < m_HairInfo.m_Strands.size(); ++i )
	{
		for( int j = 0; j < m_HairInfo.m_nVerticesPerStrand; j++ )
		{
			bounding_box.Union( m_HairInfo.m_Strands[i].m_ControlPoints[j].Position );
		}
	}

	SetBoundingBox( bounding_box );

	m_bHardwareTessellate = g3dPrefs::CurrentPrefs().m_bUseHardwareTessellation;

	Reallocate();
}

//--------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------
hairModelFragVk::~hairModelFragVk()
{
	Deallocate();
}

//------------------------------------------------------------------------
//	Deallocate - called when all device dependent resources should be
//	released.
//------------------------------------------------------------------------
void hairModelFragVk::Deallocate()
{
	m_pHairMaterials.reset();
	m_pHairGeometry.reset();
}

//------------------------------------------------------------------------
//	Reallocate - called when the device has been Reset and resources can
//	be reloaded again.
//------------------------------------------------------------------------
void hairModelFragVk::Reallocate()
{
	Deallocate();

	CreateHairBuffer();
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
void hairModelFragVk::UpdateVertices( int i_NumVertices,
						const maPoint3d* i_pVertices,
						const maVector3d* i_pNormals )
{
	int num_verts = m_HairInfo.m_nVerticesPerStrand * m_HairInfo.m_Strands.size();
	if (num_verts == i_NumVertices)
	{
		const maPoint3d *pVert = i_pVertices;
		for (int s=0; s<m_HairInfo.m_Strands.size(); s++)
		{
			mdlHairStrand &strand = m_HairInfo.m_Strands[s];
			for (int v=0; v<m_HairInfo.m_nVerticesPerStrand; v++)
			{
				strand.m_ControlPoints[v].Position = (*pVert++);
			}
		}
	}
}

//----------------------------------------------------------------------------
//	GetNumVertices - the number of vertices in the vertex buffer
//----------------------------------------------------------------------------
//virtual
int hairModelFragVk::GetNumVertices() const
{
	return m_nNumVertices;
}

//----------------------------------------------------------------------------
//	GetNumIndices - the number of indices in the index buffer
//----------------------------------------------------------------------------
int hairModelFragVk::GetNumIndices() const
{
	DBG_ASSERT(false, "UpdateVertices not implemented for hairModelFragVk");
	return 0;
}

//---------------------------------------------------------------------------
// GetVertexFormat - returns vertex format of vertex buffer using the
//	enumeration in g3dType.
//---------------------------------------------------------------------------
//virtual
g3dType::VertexFormat hairModelFragVk::GetVertexFormat() const
{
	DBG_ASSERT(false, "GetVertexFormat not implemented for hairModelFragVk");
	return g3dType::e_Undefined;
}

//--------------------------------------------------------------------
//  Lock - returns the pointer to the vertex buffer copy in system memory.
//  This should be called when you need to modify the vertices
//  ONLY should be called on morphable fragments
//--------------------------------------------------------------------
//virtual
unsigned char* hairModelFragVk::Lock()
{
	DBG_ASSERT(false, "Lock not implemented for hairModelFragVk");
	return NULL;
}

//--------------------------------------------------------------------
//  Unlock - updates the vertex buffer in VRAM. This should be called
//  when you are done modifying the vertices.
//	ONLY should be called on morphable fragments
//--------------------------------------------------------------------
//virtual
void hairModelFragVk::Unlock()
{
	DBG_ASSERT(false, "Unlock not implemented for hairModelFragVk");
}

//--------------------------------------------------------------------
//  LockIndices
//--------------------------------------------------------------------
//virtual
unsigned char* hairModelFragVk::LockIndices()
{
	return NULL;
}

//--------------------------------------------------------------------
//  UnlockIndices
//--------------------------------------------------------------------
//virtual
void hairModelFragVk::UnlockIndices()
{
}

//--------------------------------------------------------------------
//  ReadOnlyLock - DO NOT CALL THIS EVER. It's temporary for Renderman
//--------------------------------------------------------------------
unsigned char* hairModelFragVk::ReadOnlyLock()
{
	DBG_ASSERT(false, "ReadOnlyLock not implemented for hairModelFragVk");
	return 0;
}

//--------------------------------------------------------------------
//  ReadOnlyUnlock - DO NOT CALL THIS EVER. It's temporary for Renderman
//--------------------------------------------------------------------
void hairModelFragVk::ReadOnlyUnlock()
{
	DBG_ASSERT(false, "ReadOnlyUnlock not implemented for hairModelFragVk");
}

//--------------------------------------------------------------------
// ReadOnlyLockIndices - DO NOT CALL THIS EVER. It's temporary for Renderman
//--------------------------------------------------------------------
unsigned char* hairModelFragVk::ReadOnlyLockIndices()
{
	return NULL;
}

//--------------------------------------------------------------------
//  ReadOnlyUnlockIndices - DO NOT CALL THIS EVER. It's temporary for Renderman
//--------------------------------------------------------------------
void hairModelFragVk::ReadOnlyUnlockIndices()
{

}

//virtual
void hairModelFragVk::ComponentSort(const maMatrix4x4& i_Transorm, const maPoint3d& i_CameraPos)
{
	DBG_ASSERT(false, "ComponentSort not implemented for hairModelFragVk");
}

//prepares internal variables for batch rendering
//needs to be called if the vertex buffer limit or tessellation changes
void hairModelFragVk::PreBatch()
{
	//check for buffer changes
	if( m_bHardwareTessellate != g3dPrefs::CurrentPrefs().m_bUseHardwareTessellation )
	{
		m_bHardwareTessellate = g3dPrefs::CurrentPrefs().m_bUseHardwareTessellation;
	}
	if( m_fTessellation != g3dPrefs::CurrentPrefs().m_HairTessellation )	//tessellation level has changed
	{
		m_fTessellation = g3dPrefs::CurrentPrefs().m_HairTessellation;
	}
}

// Hair is treated as a broken line strip
// The shader handles the partition between hair strands
// Tessellation assumes just 1 point per patch so the same data
// can be used.
// Note that this only controls the tessellation and not the
// vertex processing as the tessellation bypasses the Vertex Shader
VkPrimitiveTopology hairModelFragVk::GetPrimitiveType()
{
	if( IsHardwareTessellated() )
	{
		return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
	}
	return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
}

bool hairModelFragVk::IsHardwareTessellated() const
{
	return m_bHardwareTessellate;
}

float hairModelFragVk::GetTessellation()
{
	return m_fTessellation;
}

int hairModelFragVk::GetVertsPerStrand()
{
	return m_HairInfo.m_nVerticesPerStrand;
}

void hairModelFragVk::CreateHairBuffer()
{
	int nStrand = m_HairInfo.m_Strands.size();
	int nVPS = m_HairInfo.m_nVerticesPerStrand;

	// Vulkan has no empty buffers (DX11 asserts making them): a hair object
	// without strands has none, and draws nothing
	if( nStrand == 0 || nVPS <= 0 )
	{
		m_nNumVertices = 0;
		return;
	}

//-----Create Material buffer----------------
	std::vector<mdlHairMaterial> materials( nStrand );
	for( int s = 0; s < nStrand; s++ )
	{
		materials[s] = m_HairInfo.m_Strands[s].Material;
	}

	//create the structured buffer (immutable on DX11: written once here)
	const VkDeviceSize materials_size = sizeof(mdlHairMaterial) * nStrand;
	m_pHairMaterials = vkdBuffer::Create( vkdGlobal::Device(), materials_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, "g_HairMaterials" );
	m_pHairMaterials->Upload( vkdGlobal::Context(), 0, materials.data(), materials_size );

//------------Create Geometry buffer-----------------------
	std::vector<maVector3d> geometry( nStrand * nVPS );
	maVector3d* pG = geometry.data();

	for( int s = 0; s < nStrand; s++ )
	{
		mdlHairStrand& Strand = m_HairInfo.m_Strands[s];
		for( int v = 0; v < nVPS; v++ )
		{
			*pG++ = Strand.m_ControlPoints[v].Position;
		}
	}

	//create the structured buffer
	const VkDeviceSize geometry_size = sizeof(maVector3d) * nStrand * nVPS;
	m_pHairGeometry = vkdBuffer::Create( vkdGlobal::Device(), geometry_size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, "g_HairGeometry" );
	m_pHairGeometry->Upload( vkdGlobal::Context(), 0, geometry.data(), geometry_size );

	m_nNumVertices = m_HairInfo.m_nHairVertices;
}

//--------------------------------------------------------------------------
// GetNumHairTriangles - Since hair are lines expanded to triangles
//  This performs the calculations based on tessellation, interpolation and
//  geometric info to return the actual number of rendered triangles
//--------------------------------------------------------------------------
int hairModelFragVk::GetNumHairTriangles()
{
	int nStrand = m_HairInfo.m_Strands.size();
	int Segments = m_HairInfo.m_nVerticesPerStrand - 1;
	int Tess = 1;
	int Inst = 1;

//	int Tess = (int)floor(m_fTessellation);	//Integer
	if( IsHardwareTessellated() )
	{
		Tess = (int)ceil((m_fTessellation-1)/2.0f) * 2 + 1;	//fractional odd
		Inst = g3dPrefs::CurrentPrefs().m_HairInterpolationCount;
		if( Inst < 1 ) Inst = 1;
	}

	return Segments * Tess * 2 * nStrand * Inst;
}

}
