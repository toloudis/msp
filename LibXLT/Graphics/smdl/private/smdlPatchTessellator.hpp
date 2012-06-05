/****************************************************************************\
**	smdlPatchTessellator.hpp
**
**	A smdlPatchTessellator subdivides a mesh into a certain number of levels
**	and then maintains information so that it can update the mesh when the
**	base mesh's vertices are animated. 
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_SMDLPATCHTESSELLATOR_HPP
#error smdlPatchTessellator.hpp multiply included
#endif
#define SMDL_SMDLPATCHTESSELLATOR_HPP

#ifndef ENV_THREAD_HPP
#include "Core/Env/envThread.hpp"
#endif 
#ifndef MDL_SPLITFRAGINFO_HPP
#include "Graphics/mdl/mdlSplitFragInfo.hpp"
#endif
#ifndef MDL_SUBDIVINFO_HPP
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#endif
#ifndef SMDL_SMDLBEZIERPATCH_HPP
#include "Graphics/smdl/private/smdlBezierPatch.hpp"
#endif
#ifndef SMDL_SMDLTESSELLATORBASE_HPP
#include "Graphics/smdl/private/smdlTessellatorBase.hpp"
#endif
#ifndef SMDL_SUBDIVUTIL_HPP
#include "Graphics/smdl/private/smdlSubdivUtil.hpp"
#endif


//============================================================================
//use optimized ACC algorithm and hardware patches (no software tessellation)
//============================================================================
#define NEWPATCHES


//============================================================================
//============================================================================
class smdlSubdivNetwork;


//============================================================================
//============================================================================
class smdlPatchTessellator : public smdlTessellatorBase
{
	public:

		//--------------------------------------------------------------------
		// Set flag to turn off generation of basis vectors (for faster loads)
		//--------------------------------------------------------------------
		static void SetComputeBasisVectors(bool i_bCompute);

		smdlPatchTessellator( shared_ptr<smdlSubdivNetwork> i_pSubdivNetwork );

		//--------------------------------------------------------------------
		// Constructor takes info about the mesh and subdivides so that it 
		// is initially set to i_InitialSubdivLevel.
		// By setting the i_MaxSubdivLevel, the network can make some
		// memory optimizations knowing that it does not need to prepare
		// for deeper subdivisions.
		//--------------------------------------------------------------------
		//smdlPatchTessellator(const mdlSubdivInfo& i_FragInfo, 
		//				  int i_InitialSubdivLevel,
		//				  int i_MaxSubdivLevel);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~smdlPatchTessellator();

		//--------------------------------------------------------------------
		//Assigns original vertices to tessellation array
		//returns starting vertex index
		//--------------------------------------------------------------------
		int OriginalVerts( const qPatch& patch );

		//--------------------------------------------------------------------
		//Creates first level tessellation of base mesh into tessellation array
		//returns starting vertex index
		//--------------------------------------------------------------------
		int PreTessellate( const qPatch& patch );

		//--------------------------------------------------------------------
		//Creates the proper triangle indices from the starting index and current subdiv level
		//--------------------------------------------------------------------
		void CreatePatchIndices( int StartIndex, float tessellate );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		int AddTessellatedVertex( const qPatch& patch, const float4& barycenter );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		int ComputeSubVertices( const qPatch& patch, float tessellate  );

		//--------------------------------------------------------------------
		// Return current subdivision level being used.
		//--------------------------------------------------------------------
		int GetCurrentSubdivLevel() const;

		//--------------------------------------------------------------------
		// Return maximum level of subdivision for which the network 
		//	has been created
		//--------------------------------------------------------------------
		int GetMaxSubdivLevel() const;

		//--------------------------------------------------------------------
		// Set the current subdivision level being used, this should be 
		// a level less than or equal to the return value of 
		// GetMaxSubdivLevel()
		//--------------------------------------------------------------------
		void SetCurrentSubdivLevel(int i_SubdivLevel);

		//--------------------------------------------------------------------
		// Return info about the subdivided mesh in FragInfo format.
		//	This should be called once after subdividing in order to
		//	create the fragment, but then you should be able to just
		//	alter the fragment's vertices when animating using the
		//	AlterBaseMesh() function and GetSubdivVertices().
		//--------------------------------------------------------------------
		const mdlSplitFragInfo& GetSubdivFragInfo() const;

		//--------------------------------------------------------------------
		// Return number of faces in the subdivided model at the
		// given level. Used for estimating cost of viewing.
		//--------------------------------------------------------------------
		int GetNumFacesAtLevel(int i_SubdivLevel) const;

#ifndef NEWPATCHES
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		int GetTessTriangles( int level, int prims ) const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		int GetTessVertices( int level, int prims ) const;
#endif

		//--------------------------------------------------------------------
		// Return number of vertices in base mesh
		//--------------------------------------------------------------------
		int GetNumBaseMeshVertices();

		//--------------------------------------------------------------------
		// Return number of vertices in the subdivided model
		// at the current subdivision level, used for sizing
		// the array for GetSubdivVertices()
		//--------------------------------------------------------------------
		int GetNumTessellatedVertices() const;

		//--------------------------------------------------------------------
		// Return positions of vertices from the subdivided mesh into the 
		//	given array. Make sure the array is big enough to hold 
		//	GetNumSubdivVertices() number of points.
		//--------------------------------------------------------------------
		void GetTessellatedVertices( maPoint3d* o_pVertices,
								maVector3d* o_pNormals,
								maVector3d* o_pSs,
								maVector3d* o_pTs,
								int i_ArraySize) const;

		//--------------------------------------------------------------------
		// Return direct access to the vertex list in the current
		//	subdivision level.
		//--------------------------------------------------------------------
		const std::vector<tVert>& GetTessellatedVertices() const;

		matTexture* GetTessellatorMeshTexture();

		void UpdateMeshTexture();

		//--------------------------------------------------------------------
		// Given the new array of mesh vertices and normals, compute the new positions
		//	of the subdivided mesh. The number of vertices passed in
		//	should match the number of vertices in the original mesh.
		//--------------------------------------------------------------------
		void AlterBaseMesh( const std::vector<maPoint3d>& i_Vertices );

		//--------------------------------------------------------------------
		// Subdiv networks can be shared, but only one can be actively
		// updating positions at a time. So, you need to wrap
		// calls to "AlterBaseMesh" and "GetSubdivVertices" with
		// a envScopedLock using this envMutex.
		//--------------------------------------------------------------------
//		envMutex& GetMutex();

	private:
		static bool sm_bComputeBasisVectors;

		// Subdiv networks can be shared
//		envMutex m_Mutex;

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Update();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Tessellate();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void GatherFragInfo();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void ComputeSubVertices();

#ifndef NEWPATCHES
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void AverageNormals();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void ComputeBasisVectors();
#endif//!NEWPATCHES

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void ConvertSubdivs( smdlSubdivUtil::sFace* i_pF, int i_nFaces );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void LoadMeshTexture();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		smdlSubdivUtil::sFace* FindEdgeFace( smdlSubdivUtil::sFace* i_NotFace, smdlSubdivUtil::sVert** io_V1, smdlSubdivUtil::sVert** io_V2 );

	private:
		mdlSplitFragInfo m_SubdivFragInfo;

//		std::vector<tQuad> m_QList;				//Quadrilaterals of base mesh (dereferenced)
		std::vector<tVert> m_VList;				//vertices of base mesh
		std::vector<tVert> m_VList_Tess;		//Software Tessellated vertices
		std::vector<envType::UInt32> m_IList_Tess;		//Software Tessellated indices

		//std::vector<int> m_Remap;
		int m_CurSubdivLevel;
		bool m_bHasTextureCoords;
		
		int m_MaxSubdivLevel;

		bool m_bDisplayControlMesh;

		matTexture*	m_pMeshTexture;

		shared_ptr<smdlSubdivNetwork> m_pSubdivNetwork;

#ifdef NEWPATCHES
		std::vector<BezierPatch> m_BPatchList;		//Bezier Patches
#else
		std::vector<qPatch> m_PatchList;		//Quadrilateral Patches of Base Mesh
		std::vector<bPatch> m_SubPatches;
#endif

		bool m_bSubRequired;	//set if Catmull-Clark subdivision is required to create quad mesh
};

