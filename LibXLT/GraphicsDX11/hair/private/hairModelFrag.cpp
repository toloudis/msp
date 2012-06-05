/****************************************************************************\
**	hairModelFrag.hpp
**
**	
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/hair/hairModelFrag.hpp"
#include "GraphicsDX11/g3d/g3dDX11BufferUtil.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "Graphics/G3d/g3dPrefs.hpp"
#include "Graphics/Mat/matTextureMgr.hpp"
#include "Graphics/Mat/matShaderMgr.hpp"
#include "GraphicsDX11/Eff/effShaderBaseDX11.hpp"

//============================================================================
//============================================================================
int hairModelFrag::sm_RendererId = 0;

namespace
{
}

//--------------------------------------------------------------------
// Set id for fragments in order to choose renderer
//--------------------------------------------------------------------
//static 
void hairModelFrag::SetRendererId(int i_RenderMode)
{
	hairModelFrag::sm_RendererId = i_RenderMode;
}

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
hairModelFrag::hairModelFrag( const mdlHairInfo &i_HairInfo, matMaterial* i_pMaterial ) 
: g3dFragment( i_pMaterial, hairModelFrag::sm_RendererId ),
  m_HairInfo( i_HairInfo ),
  m_nNumVertices( 0 ),
  m_pHairMaterials( NULL ),
  m_pHairMaterialsView( NULL ),
  m_pHairGeometry( NULL ),
  m_pHairGeometryView( NULL ),
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
hairModelFrag::~hairModelFrag() 
{
	Deallocate();
}

//------------------------------------------------------------------------
//	Deallocate - called when all device dependent resources should be
//	released.
//------------------------------------------------------------------------
void hairModelFrag::Deallocate()
{
	SAFE_RELEASE( m_pHairMaterials );
	SAFE_RELEASE( m_pHairMaterialsView );
	SAFE_RELEASE( m_pHairGeometry );
	SAFE_RELEASE( m_pHairGeometryView );
}

//------------------------------------------------------------------------
//	Reallocate - called when the device has been Reset and resources can
//	be reloaded again.
//------------------------------------------------------------------------
void hairModelFrag::Reallocate()
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
void hairModelFrag::UpdateVertices( int i_NumVertices, 
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
int hairModelFrag::GetNumVertices() const
{
	return m_nNumVertices;
}

//----------------------------------------------------------------------------
//	GetNumIndices - the number of indices in the index buffer
//----------------------------------------------------------------------------
int hairModelFrag::GetNumIndices() const
{
	DBG_ASSERT(false, "UpdateVertices not implemented for hairModelFrag");
	return 0;
}

//---------------------------------------------------------------------------
// GetVertexFormat - returns vertex format of vertex buffer using the
//	enumeration in g3dType.
//---------------------------------------------------------------------------
//virtual 
g3dType::VertexFormat hairModelFrag::GetVertexFormat() const
{
	DBG_ASSERT(false, "GetVertexFormat not implemented for hairModelFrag");
	return g3dType::e_Undefined;
}

//--------------------------------------------------------------------
//  Lock - returns the pointer to the vertex buffer copy in system memory.
//  This should be called when you need to modify the vertices
//  ONLY should be called on morphable fragments
//--------------------------------------------------------------------
//virtual 
unsigned char* hairModelFrag::Lock()
{
	DBG_ASSERT(false, "Lock not implemented for hairModelFrag");
	return NULL;
}

//--------------------------------------------------------------------
//  Unlock - updates the vertex buffer in VRAM. This should be called
//  when you are done modifying the vertices.
//	ONLY should be called on morphable fragments
//--------------------------------------------------------------------
//virtual 
void hairModelFrag::Unlock()
{
	DBG_ASSERT(false, "Unlock not implemented for hairModelFrag");
}

//--------------------------------------------------------------------
//  LockIndices
//--------------------------------------------------------------------
//virtual 
unsigned char* hairModelFrag::LockIndices()
{
	return NULL;
}

//--------------------------------------------------------------------
//  UnlockIndices
//--------------------------------------------------------------------
//virtual 
void hairModelFrag::UnlockIndices()
{
}

//--------------------------------------------------------------------
//  ReadOnlyLock - DO NOT CALL THIS EVER. It's temporary for Renderman
//--------------------------------------------------------------------
unsigned char* hairModelFrag::ReadOnlyLock()
{
	DBG_ASSERT(false, "ReadOnlyLock not implemented for hairModelFrag");
	return 0;
}

//--------------------------------------------------------------------
//  ReadOnlyUnlock - DO NOT CALL THIS EVER. It's temporary for Renderman
//--------------------------------------------------------------------
void hairModelFrag::ReadOnlyUnlock()
{
	DBG_ASSERT(false, "ReadOnlyUnlock not implemented for hairModelFrag");
}

//--------------------------------------------------------------------
// ReadOnlyLockIndices - DO NOT CALL THIS EVER. It's temporary for Renderman
//--------------------------------------------------------------------
unsigned char* hairModelFrag::ReadOnlyLockIndices()
{
	return NULL;
}

//--------------------------------------------------------------------
//  ReadOnlyUnlockIndices - DO NOT CALL THIS EVER. It's temporary for Renderman
//--------------------------------------------------------------------
void hairModelFrag::ReadOnlyUnlockIndices()
{

}

//virtual
void hairModelFrag::ComponentSort(const maMatrix4x4& i_Transorm, const maPoint3d& i_CameraPos)
{
	DBG_ASSERT(false, "ComponentSort not implemented for hairModelFrag");
}

//prepares internal variables for batch rendering
//needs to be called if the vertex buffer limit or tessellation changes
void hairModelFrag::PreBatch()
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
D3D11_PRIMITIVE_TOPOLOGY hairModelFrag::GetPrimitiveType()
{
	if( IsHardwareTessellated() )
	{
		return D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST;
	}
	return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
}

bool hairModelFrag::IsHardwareTessellated() const
{
	return m_bHardwareTessellate;
}

float hairModelFrag::GetTessellation()
{
	return m_fTessellation;
}

int hairModelFrag::GetVertsPerStrand()
{
	return m_HairInfo.m_nVerticesPerStrand;
}

void hairModelFrag::CreateHairBuffer()
{
	int nStrand = m_HairInfo.m_Strands.size();

//-----Create Material buffer----------------
	mdlHairMaterial* pData = new mdlHairMaterial[ nStrand ];
	mdlHairMaterial* pV = pData;

	for( int s = 0; s < nStrand; s++ )
	{
		mdlHairStrand& Strand = m_HairInfo.m_Strands[s];
		*pV++ = Strand.Material;
	}

	//create the structured buffer
	CD3D11_BUFFER_DESC bufDesc( sizeof(mdlHairMaterial) * nStrand, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_IMMUTABLE, 0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(mdlHairMaterial));
	D3D11_SUBRESOURCE_DATA sdata;

	sdata.pSysMem = pData;
	sdata.SysMemPitch = 0;
	sdata.SysMemSlicePitch = 0;
	// Create the vertex buffer
	HRESULT op_result = g2dDX11Global::g_pDevice->CreateBuffer( &bufDesc, &sdata, &m_pHairMaterials );
	if( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError( op_result );
		DBG_ASSERT( false, "Error creating Hair materials buffer" );
	}

	delete[] pData;

	//create the shader resource view
	op_result = g2dDX11Global::g_pDevice->CreateShaderResourceView( m_pHairMaterials, NULL, &m_pHairMaterialsView );
	if( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError( op_result );
		DBG_ASSERT( false, "Error creating Hair materials view" );
	}

//------------Create Geometry buffer-----------------------
	int nVPS = m_HairInfo.m_nVerticesPerStrand;
	maVector3d* pGeo = new maVector3d[ nStrand * nVPS ];
	maVector3d* pG = pGeo;

	for( int s = 0; s < nStrand; s++ )
	{
		mdlHairStrand& Strand = m_HairInfo.m_Strands[s];
		for( int v = 0; v < nVPS; v++ )
		{
			*pG++ = Strand.m_ControlPoints[v].Position;
		}
	}

	//create the structured buffer
	CD3D11_BUFFER_DESC geoDesc( sizeof(maVector3d) * nStrand * nVPS, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_IMMUTABLE, 0, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(maVector3d));

	sdata.pSysMem = pGeo;
	sdata.SysMemPitch = 0;
	sdata.SysMemSlicePitch = 0;
	// Create the vertex buffer
	op_result = g2dDX11Global::g_pDevice->CreateBuffer( &geoDesc, &sdata, &m_pHairGeometry );
	if( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError( op_result );
		DBG_ASSERT( false, "Error creating Hair geometry buffer" );
	}

	delete[] pGeo;

	//create the shader resource view
	op_result = g2dDX11Global::g_pDevice->CreateShaderResourceView( m_pHairGeometry, NULL, &m_pHairGeometryView );
	if( !SUCCEEDED(op_result) )
	{
		g2dDX11Global::PrintDXError( op_result );
		DBG_ASSERT( false, "Error creating Hair geometry view" );
	}

	m_nNumVertices = m_HairInfo.m_nHairVertices;
}

//--------------------------------------------------------------------------
// GetNumHairTriangles - Since hair are lines expanded to triangles
//  This performs the calculations based on tessellation, interpolation and
//  geometric info to return the actual number of rendered triangles
//--------------------------------------------------------------------------
int hairModelFrag::GetNumHairTriangles()
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