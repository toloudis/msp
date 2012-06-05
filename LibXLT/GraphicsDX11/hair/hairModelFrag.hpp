/****************************************************************************\
**	hairModelFrag.hpp
**
**	
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef HAIR_MODEL_FRAG_HPP
#error hairModelFrag.hpp multiply included
#endif
#define HAIR_MODEL_FRAG_HPP

#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif
#ifndef MDL_HAIRINFO_HPP
#include "Graphics/mdl/mdlHairInfo.hpp"
#endif 
#ifndef MAT_TEXTURE_HPP
#include "Graphics/Mat/matTexture.hpp"
#endif 

#include <vector>

//============================================================================
//============================================================================
class matMaterial;

//structure used for data tessellation texture
struct StrandMaterial
{
	StrandMaterial( mdlHairMaterial& i_mtl );
	maVector4d RootColorRadius;	/// Root color (RGB), radius
	maVector4d TipColorRadius;	/// Tip Color (RGB), radius
	maVector4d Mtl;				/// (Opacity, Specular, m_Gloss, AmbientDiffuse)
};

//============================================================================
//============================================================================
class hairModelFrag : public g3dFragment
{
	public:
		//--------------------------------------------------------------------
		// Set id for fragments in order to choose renderer
		//--------------------------------------------------------------------
		static void SetRendererId(int i_RenderMode);

		//--------------------------------------------------------------------
		// Constructor
		//--------------------------------------------------------------------
		hairModelFrag( const mdlHairInfo &i_HairInfo, matMaterial* i_pMaterial );

		//--------------------------------------------------------------------
		// Destructor
		//--------------------------------------------------------------------
		virtual ~hairModelFrag();

		//------------------------------------------------------------------------
		//	Deallocate - called when all device dependent resources should be
		//	released.
		//------------------------------------------------------------------------
		virtual void Deallocate();

		//------------------------------------------------------------------------
		//	Reallocate - called when the device has been Reset and resources can
		//	be reloaded again.
		//------------------------------------------------------------------------
		virtual void Reallocate();

		//--------------------------------------------------------------------
		// UpdateVertices - alter the position of the vertices in the
		// given fragment. i_pNormals may be NULL, in which case the
		// normals should remain as before. i_NumVertices should
		// represent the number of positions given and should match the
		// number of vertices in the fragment.
		// This method can only be called on a fragment that was created
		// with the "morphable" flag set to true.
		//--------------------------------------------------------------------
		virtual void UpdateVertices(int i_NumVertices, 
									const maPoint3d* i_pVertices, 
									const maVector3d* i_pNormals );

		//----------------------------------------------------------------------------
		//	GetNumVertices - the number of vertices in the vertex buffer
		//----------------------------------------------------------------------------
		virtual int GetNumVertices() const;

		//----------------------------------------------------------------------------
		//	GetNumIndices - the number of indices in the index buffer
		//----------------------------------------------------------------------------
		virtual int GetNumIndices() const;

		//---------------------------------------------------------------------------
		// GetVertexFormat - returns vertex format of vertex buffer using the
		//	enumeration in g3dType.
		//---------------------------------------------------------------------------
		virtual g3dType::VertexFormat GetVertexFormat() const;

		//--------------------------------------------------------------------
		//  Lock - returns the pointer to the vertex buffer copy in system memory.
		//  This should be called when you need to modify the vertices
		//  ONLY should be called on morphable fragments
		//--------------------------------------------------------------------
		virtual unsigned char* Lock();

		//--------------------------------------------------------------------
		//  Unlock - updates the vertex buffer in VRAM. This should be called
		//  when you are done modifying the vertices.
		//	ONLY should be called on morphable fragments
		//--------------------------------------------------------------------
		virtual void Unlock();

		//--------------------------------------------------------------------
		//  LockIndices
		//--------------------------------------------------------------------
		virtual unsigned char* LockIndices();

		//--------------------------------------------------------------------
		//  UnlockIndices
		//--------------------------------------------------------------------
		virtual void UnlockIndices();

		//--------------------------------------------------------------------
		//  ReadOnlyLock
		//--------------------------------------------------------------------
		virtual unsigned char* ReadOnlyLock();

		//--------------------------------------------------------------------
		//  ReadOnlyUnlock
		//--------------------------------------------------------------------
		virtual void ReadOnlyUnlock();

		//--------------------------------------------------------------------
		// ReadOnlyLockIndices
		//--------------------------------------------------------------------
		virtual unsigned char* ReadOnlyLockIndices();

		//--------------------------------------------------------------------
		//  ReadOnlyUnlockIndices
		//--------------------------------------------------------------------
		virtual void ReadOnlyUnlockIndices();

		virtual void ComponentSort(const maMatrix4x4& i_Transorm,
			const maPoint3d& i_CameraPos);

		//--------------------------------------------------------------------
		//  PreBatch - Prepares internal variables for batch rendering
		//  Must call before other batching calls
		//--------------------------------------------------------------------
		void PreBatch();

		D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType();

		///-------------Tessellation members-----------------
		bool IsHardwareTessellated() const;
		float GetTessellation();
		int GetVertsPerStrand();
		ID3D11Buffer* GetHairMaterials(){ return m_pHairMaterials; }
		ID3D11ShaderResourceView* GetHairMaterialsView(){ return m_pHairMaterialsView; }
		ID3D11Buffer* GetHairGeometry(){ return m_pHairGeometry; }
		ID3D11ShaderResourceView* GetHairGeometryView(){ return m_pHairGeometryView; }

		//--------------------------------------------------------------------
		//  GetNumHairTriangles return the total number if triangles rendered
		//  This accounts for the tessellation, interpolation, and GS expansion
		//--------------------------------------------------------------------
		int GetNumHairTriangles();

	private:
		static int sm_RendererId;

		mdlHairInfo m_HairInfo;

		DWORD m_nNumVertices;		//number of vertices in the batch

		//these are here to compare to a change (requiring a reallocate)
		float m_fTessellation;	
		bool m_bHardwareTessellate;

///-------------Hardware Tessellation members-----------------
		ID3D11Buffer* m_pHairMaterials;
		ID3D11ShaderResourceView* m_pHairMaterialsView;
		ID3D11Buffer* m_pHairGeometry;
		ID3D11ShaderResourceView* m_pHairGeometryView;

		//--------------------------------------------------------------------
		//  CreateHairBuffer allocates the buffers to store the hair data.
		//  Four buffers are creates, one for materials and the other for the geometry
		//  Each has a StructuredBuffer to store the data and a shader resource view for
		//  shader access
		//--------------------------------------------------------------------
		void CreateHairBuffer();
};
