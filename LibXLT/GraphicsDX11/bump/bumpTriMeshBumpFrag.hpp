/****************************************************************************\
**	bumpTriMeshBumpFrag.hpp
**
**	A bumpTriMeshBumpFrag represents a triangle mesh geometry
**	with extra texture space info at each vertex in order to render
**  per-pixel effects like bump mapping.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef BUMP_TRIMESHBUMPFRAG_HPP
#error bumpTriMeshBumpFrag.hpp multiply included
#endif
#define BUMP_TRIMESHBUMPFRAG_HPP

#ifndef TMESH_FRAG_HPP
#include "GraphicsDX11/tmesh/tmeshFrag.hpp"
#endif
#ifndef MA_POINT2D_HPP
#include "Core/ma/maPoint2d.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#ifndef TMESH_OPTIMIZEDBVH_HPP
#include "GraphicsDX11/tmesh/tmeshOptimizedBVH.hpp"
#endif

#include <vector>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class g3dOcclusionTree;
class matMaterial;

class bumpTriMeshBumpFrag : public tmeshFrag
{
	public:
		//--------------------------------------------------------------------
		// Set id for fragments in order to choose renderer
		//--------------------------------------------------------------------
		static void SetRendererId(int i_RenderMode);

		//--------------------------------------------------------------------
		// Set flag to turn off generation of basis vectors (for faster loads)
		//--------------------------------------------------------------------
		static void SetComputeBasisVectors(bool i_bCompute);

		//--------------------------------------------------------------------
		//	Constructor - One set of texture coordinates, plus prelit color
		//		p[er vertex for ambient occlusion
		//--------------------------------------------------------------------
		bumpTriMeshBumpFrag(const maPoint3d* i_Vertices,
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
		bumpTriMeshBumpFrag(shared_ptr<tmeshVertexBuffer> &i_VertexBuffer,
							shared_ptr<tmeshIndexBuffer> &i_IndexBuffer,
							matMaterial* i_pMaterial);

		//--------------------------------------------------------------------
		//	Constructor taking shared buffers, including velocity buffer.
		//--------------------------------------------------------------------
		bumpTriMeshBumpFrag(shared_ptr<tmeshVertexBuffer> &i_VertexBuffer,
										 shared_ptr<tmeshVertexBuffer> &i_VertexBufferOld,
										 shared_ptr<tmeshIndexBuffer> & i_IndexBuffer,
										 matMaterial* i_pMaterial);

		//--------------------------------------------------------------------
		// Copy constructor
		//--------------------------------------------------------------------
		bumpTriMeshBumpFrag(const bumpTriMeshBumpFrag &i_Frag);

		//--------------------------------------------------------------------
		//  Destructor
		//--------------------------------------------------------------------
		virtual ~bumpTriMeshBumpFrag();

		//---------------------------------------------------------------------------
		// Update buffer functions provide a way to alter the topology of the
		//	geometry without deleting and creating a new fragment. The assumption
		//	is that the number of vertices or indices is changing. Otherwise,
		//	use the UpdateVertices() or Lock/Unlock() functions.
		//---------------------------------------------------------------------------
		void UpdateVertexBuffer(shared_ptr<tmeshVertexBuffer> &i_VertexBuffer,
								shared_ptr<tmeshVertexBuffer> &i_VertexBufferOld);
		void UpdateIndexBuffer(shared_ptr<tmeshIndexBuffer> &i_IndexBuffer);

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


		//virtual bool GetUseVertexColors() const;

		ID3D11Buffer* constructBvh(int i_nVertices, const maPoint3d* i_Vertices,
								   int i_nIndices, const g3dIndexPtr i_Indices);

		BYTE * PrepareBvhForGPU(OptimizedBvhNode * root);

		const g3dOcclusionTree* GetAOTree() const;
//		void DeleteAOTree() {delete m_pAOTree; m_pAOTree = NULL;}

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

		ID3D11ShaderResourceView* m_pVBSRV;
		ID3D11ShaderResourceView* m_pIBSRV;
		ID3D11Buffer* m_pBVH;	
		ID3D11ShaderResourceView* m_pBVHSRV;	

private:
		static int sm_RendererId;
		static bool sm_bComputeBasisVectors;

		// triangle struct for sorting
		struct sTri
		{
			envType::UInt32 m_I1, m_I2, m_I3;
			float m_Dist;
			bool operator < ( const sTri& i_Tri2 );
		};
		std::vector<sTri> m_SortList;

		mutable g3dOcclusionTree* m_pAOTree;

		//maPoint2d m_BakeUVScale, m_BakeUVTranslate;

};
