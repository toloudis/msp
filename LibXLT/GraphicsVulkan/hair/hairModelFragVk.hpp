/****************************************************************************\
**	hairModelFragVk.hpp
**
**	GraphicsVulkan port of GraphicsDX11/hair/hairModelFrag.hpp (namespace
**	gvk). The strands' materials and control points are in two structured
**	buffers (vkdBuffer, STORAGE_BUFFER), uploaded once as DX11's immutable
**	ones are; Hair_SH's shaders read them as g_HairMaterials and
**	g_HairGeometry. A hair object without strands has no buffers (D3D11
**	can't make empty ones; DX11 asserts), and HairRendererVk draws nothing
**	for it.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GVK_HAIR_MODELFRAGVK_HPP
#error hairModelFragVk.hpp multiply included
#endif
#define GVK_HAIR_MODELFRAGVK_HPP

#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef MDL_HAIRINFO_HPP
#include "Graphics/mdl/mdlHairInfo.hpp"
#endif

#include <volk.h>

#include <memory>

//============================================================================
//============================================================================
class matMaterial;

namespace gvk
{

class vkdBuffer;

//============================================================================
//============================================================================
class hairModelFragVk : public g3dFragment
{
	public:
		//--------------------------------------------------------------------
		// Set id for fragments in order to choose renderer
		//--------------------------------------------------------------------
		static void SetRendererId(int i_RenderMode);

		//--------------------------------------------------------------------
		// Constructor
		//--------------------------------------------------------------------
		hairModelFragVk( const mdlHairInfo &i_HairInfo, matMaterial* i_pMaterial );

		//--------------------------------------------------------------------
		// Destructor
		//--------------------------------------------------------------------
		virtual ~hairModelFragVk();

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
		// (As on DX11, the buffers aren't uploaded again: the strands drawn
		// change only after Reallocate.)
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

		//	The topology (a patch list of one control point when tessellated;
		//	SetTopology's control point count is 1 then)
		VkPrimitiveTopology GetPrimitiveType();

		///-------------Tessellation members-----------------
		bool IsHardwareTessellated() const;
		float GetTessellation();
		int GetVertsPerStrand();
		//	NULL for a hair object without strands
		vkdBuffer* GetHairMaterials(){ return m_pHairMaterials.get(); }
		vkdBuffer* GetHairGeometry(){ return m_pHairGeometry.get(); }

		//--------------------------------------------------------------------
		//  GetNumHairTriangles return the total number if triangles rendered
		//  This accounts for the tessellation, interpolation, and GS expansion
		//--------------------------------------------------------------------
		int GetNumHairTriangles();

	private:
		static int sm_RendererId;

		mdlHairInfo m_HairInfo;

		uint32_t m_nNumVertices;		//number of vertices in the batch

		//these are here to compare to a change (requiring a reallocate)
		float m_fTessellation;
		bool m_bHardwareTessellate;

///-------------Hardware Tessellation members-----------------
		std::unique_ptr<vkdBuffer> m_pHairMaterials;
		std::unique_ptr<vkdBuffer> m_pHairGeometry;

		//--------------------------------------------------------------------
		//  CreateHairBuffer allocates the buffers to store the hair data.
		//  Two structured buffers are created, one for materials and the
		//  other for the geometry (D3D11's shader resource views are their
		//  resource heap indices on Vulkan, made when they are bound)
		//--------------------------------------------------------------------
		void CreateHairBuffer();
};

}
