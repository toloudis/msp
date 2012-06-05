/****************************************************************************\
**	smdlPNTessellator.hpp
**
**	A smdlPNTessellator subdivides a mesh into a certain number of levels
**	and then maintains information so that it can update the mesh when the
**	base mesh's vertices are animated. 
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_SMDLPNTESSELLATOR_HPP
#error smdlPNTessellator.hpp multiply included
#endif
#define SMDL_SMDLPNTESSELLATOR_HPP

#ifndef ENV_THREAD_HPP
#include "Core/Env/envThread.hpp"
#endif 
#ifndef MDL_SPLITFRAGINFO_HPP
#include "Graphics/mdl/mdlSplitFragInfo.hpp"
#endif
#ifndef MDL_SUBDIVINFO_HPP
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#endif
#ifndef SMDL_SMDLTESSELLATORBASE_HPP
#include "Graphics/smdl/private/smdlTessellatorBase.hpp"
#endif
#ifndef SMDL_SUBDIVUTIL_HPP
#include "Graphics/smdl/private/smdlSubdivUtil.hpp"
#endif


//============================================================================
//============================================================================
class smdlSubdivNetwork;


//============================================================================
//============================================================================
class smdlPNTessellator : public smdlTessellatorBase
{
	public:
		//--------------------------------------------------------------------
		// Set flag to turn off generation of basis vectors (for faster loads)
		//--------------------------------------------------------------------
		static void SetComputeBasisVectors(bool i_bCompute);

		smdlPNTessellator( shared_ptr<smdlSubdivNetwork> i_pSubdivNetwork );

		//--------------------------------------------------------------------
		// Constructor takes info about the mesh and subdivides so that it 
		// is initially set to i_InitialSubdivLevel.
		// By setting the i_MaxSubdivLevel, the network can make some
		// memory optimizations knowing that it does not need to prepare
		// for deeper subdivisions.
		//--------------------------------------------------------------------
		smdlPNTessellator(const mdlSubdivInfo& i_FragInfo, 
						  int i_InitialSubdivLevel,
						  int i_MaxSubdivLevel);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~smdlPNTessellator();

		//--------------------------------------------------------------------
		//Assigns original vertices to tessellation array
		//returns starting vertex index
		//--------------------------------------------------------------------
		int OriginalVerts( const nPatch& patch );

		//--------------------------------------------------------------------
		//Creates first level tessellation of base mesh into tessellation array
		//returns starting vertex index
		//--------------------------------------------------------------------
		int PreTessellate( const nPatch& patch );

		//--------------------------------------------------------------------
		//Creates the proper triangle indices from the starting index and current subdiv level
		//--------------------------------------------------------------------
		void CreatePatchIndices( int StartIndex, float tessellate );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		int AddTessellatedVertex( const nPatch& patch, const float3& barycenter );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		int ComputeSubVertices( const nPatch& patch, float tessellate  );

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

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		int GetTessTriangles( int level, int prims ) const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		int GetTessVertices( int level, int prims ) const;

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

		//--------------------------------------------------------------------
		// Given the new array of mesh vertices and normals, compute the new positions
		//	of the subdivided mesh. The number of vertices passed in
		//	should match the number of vertices in the original mesh.
		//--------------------------------------------------------------------
		void AlterBaseMesh(const std::vector<maPoint3d>& i_Vertices,
							const std::vector<maVector3d>& i_Normals );

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
		//envMutex m_Mutex;

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

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void AverageNormals();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void ComputeBasisVectors();

	private:
		mdlSplitFragInfo m_SubdivFragInfo;

		std::vector<nPatch> m_PatchList;		//Triangulated N Patches of Base Mesh
		std::vector<tTri> m_TList;				//triangles of base mesh (dereferenced)
		std::vector<tVert> m_VList;				//vertices of base mesh
		std::vector<tVert> m_VList_Tess;		//Software Tessellated vertices
		std::vector<envType::UInt32> m_IList_Tess;		//Software Tessellated indices

		//std::vector<int> m_Remap;
		int m_CurSubdivLevel;
		bool m_bHasTextureCoords;
		
		int m_MaxSubdivLevel;

		bool m_bDisplayControlMesh;
};
