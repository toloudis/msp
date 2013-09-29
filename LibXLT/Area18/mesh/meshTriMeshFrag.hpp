/****************************************************************************\
**	meshTriMeshFrag.hpp
**
**	A meshTriMeshFrag represents a triangle mesh geometry
**	with extra texture space info at each vertex in order to render
**  per-pixel effects like bump mapping.
**
** Area17
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MESH_TRIMESHFRAG_HPP
#error meshTriMeshFrag.hpp multiply included
#endif
#define MESH_TRIMESHFRAG_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#include "Area18/ogl/oglTypes.hpp"
#ifndef MESH_VERTEXBUFFER_HPP
#include "Area18/mesh/meshVertexBuffer.hpp"
#endif 
#ifndef MESH_INDEXBUFFER_HPP
#include "Area18/mesh/meshIndexBuffer.hpp"
#endif 
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif

#include <vector>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class matMaterial;
class meshRenderer;

class meshTriMeshFrag : public g3dFragment
{
	public:
		//--------------------------------------------------------------------
		// Set id for fragments in order to choose renderer
		//--------------------------------------------------------------------
		static void SetRendererId(int i_RenderMode);
		static void SetRenderer(meshRenderer* i_Renderer);
		static meshRenderer* GetRenderer();

		//--------------------------------------------------------------------
		// Set flag to turn off generation of basis vectors (for faster loads)
		//--------------------------------------------------------------------
		static void SetComputeBasisVectors(bool i_bCompute);

		enum PrimitiveType
		{
			e_TriangleList = GL_TRIANGLES,
			e_LineList = GL_LINES,
			e_TriangleStrip = GL_TRIANGLE_STRIP
		};

		//--------------------------------------------------------------------
		//	Constructor - One set of texture coordinates, plus prelit color
		//		p[er vertex for ambient occlusion
		//--------------------------------------------------------------------
		meshTriMeshFrag(oglDevice* i_pDevice,
			const maPoint3d* i_Vertices,
			const maPoint3d* i_Normals,
			const maPoint2d* i_UVs,
			const maPoint3d* i_Ss,
			const maPoint3d* i_Ts,
			int	i_nVertices,
			const g3dIndexPtr i_Indices,
			int	i_nIndices,
			int i_nNonShadowIndices,
			matMaterial* i_pMaterial,
			bool i_bMorphable = false,
			bool i_bComponentSort = false,
			PrimitiveType i_PrimitiveType = e_TriangleList);

		//--------------------------------------------------------------------
		//	Constructor taking shared buffers
		//--------------------------------------------------------------------
		meshTriMeshFrag(shared_ptr<meshVertexBuffer> &i_VertexBuffer,
							shared_ptr<meshIndexBuffer> &i_IndexBuffer,
							matMaterial* i_pMaterial);

		//--------------------------------------------------------------------
		//	Constructor taking shared buffers, including velocity buffer.
		//--------------------------------------------------------------------
		meshTriMeshFrag(shared_ptr<meshVertexBuffer> &i_VertexBuffer,
										 shared_ptr<meshVertexBuffer> &i_VertexBufferOld,
										 shared_ptr<meshIndexBuffer> & i_IndexBuffer,
										 matMaterial* i_pMaterial);

		//--------------------------------------------------------------------
		// Copy constructor
		//--------------------------------------------------------------------
		meshTriMeshFrag(const meshTriMeshFrag &i_Frag);

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		virtual ~meshTriMeshFrag();

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

		//---------------------------------------------------------------------------
		// GetVertexFormat - returns vertex format of vertex buffer using the
		//	enumeration in g3dType.
		//---------------------------------------------------------------------------
		virtual g3dType::VertexFormat GetVertexFormat() const;
		virtual g3dType::VertexFormat GetVertexFormat_Old() const;

		//--------------------------------------------------------------------
		//  Lock - returns the pointer to the vertex buffer copy in system memory.
		//  This should be called when you need to modify the vertices
		//  ONLY should be called on morphable fragments
		//--------------------------------------------------------------------
		unsigned char* Lock();

		//--------------------------------------------------------------------
		//  Unlock - updates the vertex buffer in VRAM. This should be called
		//  when you are done modifying the vertices.
		//	ONLY should be called on morphable fragments
		//--------------------------------------------------------------------
		void Unlock();

		//--------------------------------------------------------------------
		//  Lock - returns the pointer to the index buffer copy in system memory.
		//  This should be called when you need to modify the indices
		//  ONLY should be called on component-sort fragments
		//--------------------------------------------------------------------
		unsigned char* LockIndices();

		//--------------------------------------------------------------------
		//  Unlock - updates the index buffer in VRAM. This should be called
		//  when you are done modifying the indices.
		//	ONLY should be called on component-sort fragments
		//--------------------------------------------------------------------
		void UnlockIndices();

		//--------------------------------------------------------------------
		//  ReadOnlyLock
		//--------------------------------------------------------------------
		unsigned char* ReadOnlyLock();

		//--------------------------------------------------------------------
		//  ReadOnlyUnlock
		//--------------------------------------------------------------------
		void ReadOnlyUnlock();

		//--------------------------------------------------------------------
		//  ReadOnlyLockIndices
		//--------------------------------------------------------------------
		unsigned char* ReadOnlyLockIndices();

		//--------------------------------------------------------------------
		// ReadOnlyUnlockIndices
		//--------------------------------------------------------------------
		void ReadOnlyUnlockIndices();

		//----------------------------------------------------------------------------
		//	GetPrimitiveType - return the primitve type of the fragment
		//----------------------------------------------------------------------------
		inline PrimitiveType GetPrimitiveType() const;

		//----------------------------------------------------------------------------
		//	SetPrimitiveType - sets the primitive type of the fragment
		//----------------------------------------------------------------------------
		void SetPrimitiveType( PrimitiveType i_eType );

		//---------------------------------------------------------------------------
		// Update buffer functions provide a way to alter the topology of the
		//	geometry without deleting and creating a new fragment. The assumption
		//	is that the number of vertices or indices is changing. Otherwise,
		//	use the UpdateVertices() or Lock/Unlock() functions.
		//---------------------------------------------------------------------------
		void UpdateVertexBuffer(shared_ptr<meshVertexBuffer> &i_VertexBuffer,
								shared_ptr<meshVertexBuffer> &i_VertexBufferOld);
		void UpdateIndexBuffer(shared_ptr<meshIndexBuffer> &i_IndexBuffer);

		//---------------------------------------------------------------------------
		// UpdateVertices - alter the position of the vertices in the
		// given fragment. i_pNormals may be NULL, in which case the
		// normals should remain as before. i_NumVertices should
		// represent the number of positions given and should match the
		// number of vertices in the fragment.
		// This method can only be called on a fragment that was created
		// with the "morphable" flag set to true.
		//---------------------------------------------------------------------------
		virtual void UpdateVertices( int i_NumVertices, 
									 const maPoint3d* i_pVertices, 
									 const maVector3d* i_pNormals );

		//---------------------------------------------------------------------------
		// ComponentSort - let the fragment sort its internal components before
		// rendering. The current model to world transformation and the camera 
		// position are passed as arguments in order to do the sorting.
		//---------------------------------------------------------------------------
		virtual void ComponentSort(const maMatrix4x4& i_Transorm,
								   const maPoint3d& i_CameraPos);
		virtual void Split(const maAxisBox& i_camSpaceBox);

		//---------------------------------------------------------------------------
		// GetFaceInfo - return a triangle from the underlying mesh, if one exists.
		//	If no such triangle can be found, return false. This call can be expensive
		//	as it may involve locking a vertex buffer.
		//---------------------------------------------------------------------------
		virtual bool GetFaceInfo(int i_Face, maPoint3d o_Points[3], maVector3d o_Normals[3]);

		//----------------------------------------------------------------------------
		//	Return uv scaling factors to keep uvs within 0..1
		//----------------------------------------------------------------------------
		virtual void GetUVBakeFactors(maPoint2d& o_Scale, maPoint2d& o_Translate) const;

		//----------------------------------------------------------------------------
		//	Return uv overlap factor
		//----------------------------------------------------------------------------
		virtual float GetUVOverlapFactor() const;

		//----------------------------------------------------------------------------
		// Should the fragment be animated with hardware skinning (if possible)?
		//----------------------------------------------------------------------------
		virtual void SetHasSkinning(bool i_bSkinning);

		//----------------------------------------------------------------------------
		// Should the fragment be animated with velocity maps (if possible)?
		//----------------------------------------------------------------------------
		void CreateVelocityBuffer(bool i_bVelocityBuffer);

		//--------------------------------------------------------------------
		//  GetVertexBuffer
		//--------------------------------------------------------------------
		inline const shared_ptr<meshVertexBuffer>& GetVertexBuffer() const;
		inline shared_ptr<meshVertexBuffer>& GetVertexBuffer();

		//--------------------------------------------------------------------
		//  GetVertexBuffer_Old
		//--------------------------------------------------------------------
		inline const shared_ptr<meshVertexBuffer>& GetVertexBuffer_Old() const;
		inline shared_ptr<meshVertexBuffer>& GetVertexBuffer_Old();

		//--------------------------------------------------------------------
		//  GetIndexBuffer
		//--------------------------------------------------------------------
		inline const shared_ptr<meshIndexBuffer>& GetIndexBuffer() const;
		inline shared_ptr<meshIndexBuffer>& GetIndexBuffer();

		//--------------------------------------------------------------------
		//  GetSkinningBuffer
		//--------------------------------------------------------------------
		inline const shared_ptr<meshVertexBuffer>& GetSkinningBuffer() const;
		inline shared_ptr<meshVertexBuffer>& GetSkinningBuffer();

		//----------------------------------------------------------------------------
		//	GetNumVertices - the number of vertices in the vertex buffer
		//----------------------------------------------------------------------------
		inline int GetNumVertices() const;

		//----------------------------------------------------------------------------
		//	GetNumIndices - the number of indices in the index buffer
		//----------------------------------------------------------------------------
		inline int GetNumIndices() const;

		//----------------------------------------------------------------------------
		//	GetNumNonShadowIndices - the number of indices in the index buffer
		//		that are not used for shadows welding (always first in the index).
		//----------------------------------------------------------------------------
		inline int GetNumNonShadowIndices() const;

		//----------------------------------------------------------------------------
		// Get size of an index in bytes (16 -> 2, 32 -> 4)
		//----------------------------------------------------------------------------
		inline int GetSizeOfIndex() const;

		//----------------------------------------------------------------------------
		//	GetVertexStride - the size of a vertex in the vertex buffer
		//----------------------------------------------------------------------------
		inline int GetVertexStride() const;

private:
		shared_ptr<meshVertexBuffer> m_pVertexBuffer;
		shared_ptr<meshVertexBuffer> m_pVertexBuffer_Old;
		shared_ptr<meshVertexBuffer> m_pSkinningBuffer;
		shared_ptr<meshIndexBuffer> m_pIndexBuffer;
		
		DWORD m_VertexShader;
		int m_nZBias;
		PrimitiveType m_ePrimitiveType;
		static int sm_RendererId;
		static bool sm_bComputeBasisVectors;
		static meshRenderer* sm_Renderer;

		// triangle struct for sorting
		struct sTri
		{
			envType::UInt32 m_I1, m_I2, m_I3;
			float m_Dist;
			bool operator < ( const sTri& i_Tri2 );
		};
		std::vector<sTri> m_SortList;

		//--------------------------------------------------------------------
		//  SetVertexBuffer
		//--------------------------------------------------------------------
		void SetVertexBuffer( const shared_ptr<meshVertexBuffer>& i_pVertexBuffer);
		void SetVertexBuffer( oglDevice* i_pDevice,
			oglBufferHandle i_pVertexBuffer,
			BYTE* i_pVertexBufferCopy,
			int i_nVertices );
		void SetVertexBuffer_Old( oglDevice* i_pDevice,
			oglBufferHandle i_pVertexBuffer,
			BYTE* i_pVertexBufferCopy,
			int inVertices );

		//--------------------------------------------------------------------
		//  SetIndexBuffer
		//--------------------------------------------------------------------
		void SetIndexBuffer( const shared_ptr<meshIndexBuffer>& i_pIndexBuffer);
		void SetIndexBuffer( oglDevice* i_pDevice,
			oglBufferHandle i_pIndexBuffer,
			BYTE* i_pIndexBufferCopy,
			int i_nIndices,
			int i_SizeOfIndex,
			int i_nNonShadowIndices = -1 );

		//--------------------------------------------------------------------
		//  SetSkinningBuffer
		//--------------------------------------------------------------------
		void SetSkinningBuffer( oglDevice* i_pDevice,
			oglBufferHandle i_pSkinningBuffer,
			BYTE* i_pSkinningBufferCopy,
			int i_nVertices );

		void SetVelocityBuffer( oglDevice* i_pDevice,
			oglBufferHandle i_pVertexBuffer_Old,
			BYTE* i_pVertexBuffer_OldCopy,
			int i_nVertices );

};

//--------------------------------------------------------------------
//  GetVertexBuffer
//--------------------------------------------------------------------
inline const shared_ptr<meshVertexBuffer>& meshTriMeshFrag::GetVertexBuffer() const
{
	return m_pVertexBuffer;
}
inline shared_ptr<meshVertexBuffer>& meshTriMeshFrag::GetVertexBuffer()
{
	return m_pVertexBuffer;
}

//--------------------------------------------------------------------
//  GetIndexBuffer
//--------------------------------------------------------------------
inline const shared_ptr<meshIndexBuffer>& meshTriMeshFrag::GetIndexBuffer() const
{
	return m_pIndexBuffer;
}
inline shared_ptr<meshIndexBuffer>& meshTriMeshFrag::GetIndexBuffer()
{
	return m_pIndexBuffer;
}

//--------------------------------------------------------------------
//  GetSkinningBuffer
//--------------------------------------------------------------------
inline const shared_ptr<meshVertexBuffer>& meshTriMeshFrag::GetSkinningBuffer() const
{
	return m_pSkinningBuffer;
}
inline shared_ptr<meshVertexBuffer>& meshTriMeshFrag::GetSkinningBuffer()
{
	return m_pSkinningBuffer;
}

//--------------------------------------------------------------------
//  GetVertexBuffer_Old
//--------------------------------------------------------------------
inline const shared_ptr<meshVertexBuffer>& meshTriMeshFrag::GetVertexBuffer_Old() const
{
	return m_pVertexBuffer_Old;
}
inline shared_ptr<meshVertexBuffer>& meshTriMeshFrag::GetVertexBuffer_Old()
{
	return m_pVertexBuffer_Old;
}

//----------------------------------------------------------------------------
//	GetNumVertices - the number of vertices in the vertex buffer
//----------------------------------------------------------------------------
inline int meshTriMeshFrag::GetNumVertices() const
{
	return m_pVertexBuffer->GetNumVertices();
}

//----------------------------------------------------------------------------
//	GetNumIndices - the number of indices in the index buffer
//----------------------------------------------------------------------------
inline int meshTriMeshFrag::GetNumIndices() const
{
	return m_pIndexBuffer->GetNumIndices();
}

//----------------------------------------------------------------------------
//	GetNumNonShadowIndices - the number of indices in the index buffer
//		that are not used for shadows welding (always first in the index).
//----------------------------------------------------------------------------
inline int meshTriMeshFrag::GetNumNonShadowIndices() const
{
	return m_pIndexBuffer->GetNumNonShadowIndices();
}

//----------------------------------------------------------------------------
// Get size of an index in bytes (16 -> 2, 32 -> 4)
//----------------------------------------------------------------------------
inline int meshTriMeshFrag::GetSizeOfIndex() const
{
	return m_pIndexBuffer->GetSizeOfIndex();
}

//----------------------------------------------------------------------------
//	GetVertexStride
//----------------------------------------------------------------------------
inline int meshTriMeshFrag::GetVertexStride() const
{
	return m_pVertexBuffer->GetVertexStride();
}

//----------------------------------------------------------------------------
//	GetPrimitiveType - return the primitve type of the fragment
//----------------------------------------------------------------------------
inline meshTriMeshFrag::PrimitiveType meshTriMeshFrag::GetPrimitiveType() const
{
	return m_ePrimitiveType;
}
