/****************************************************************************\
**	smdlSubdivNetwork.hpp
**
**	A smdlSubdivNetwork subdivides a mesh into a certain number of levels
**	and then maintains information so that it can update the mesh when the
**	base mesh's vertices are animated. 
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_SUBDIVNETWORK_HPP
#error smdlSubdivNetwork.hpp multiply included
#endif
#define SMDL_SUBDIVNETWORK_HPP

#ifndef ENV_THREAD_HPP
#include "Core/Env/envThread.hpp"
#endif 
#ifndef MDL_FRAGINFO_HPP
#include "Graphics/mdl/mdlFragInfo.hpp"
#endif
#ifndef MDL_SUBDIVINFO_HPP
#include "Graphics/mdl/mdlSubdivInfo.hpp"
#endif
#ifndef SMDL_SUBDIVUTIL_HPP
#include "Graphics/smdl/private/smdlSubdivUtil.hpp"
#endif


//============================================================================
//============================================================================
class smdlSubdivNetwork
{
	public:
		//--------------------------------------------------------------------
		// Set flag to turn off generation of basis vectors (for faster loads)
		//--------------------------------------------------------------------
		static void SetComputeBasisVectors(bool i_bCompute);

		//--------------------------------------------------------------------
		// Constructor takes info about the mesh and subdivides so that it 
		// is initially set to i_InitialSubdivLevel.
		// By setting the i_MaxSubdivLevel, the network can make some
		// memory optimizations knowing that it does not need to prepare
		// for deeper subdivisions.
		// If i_bMergeVertices is true, then the positions are compared
		// within a tolerance in order to close up seams.
		//--------------------------------------------------------------------
		smdlSubdivNetwork(const mdlSubdivInfo& i_FragInfo, 
						  int i_InitialSubdivLevel,
						  int i_MaxSubdivLevel,
						  bool i_bMergeVertices = false);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~smdlSubdivNetwork();

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
		const mdlFragInfo& GetSubdivFragInfo() const;

		//--------------------------------------------------------------------
		// Return number of faces in the subdivided model at the
		// given level. Used for estimating cost of viewing.
		//--------------------------------------------------------------------
		int GetNumFacesAtLevel(int i_SubdivLevel) const;

		//--------------------------------------------------------------------
		// Return number of vertices in base mesh
		//--------------------------------------------------------------------
		int GetNumBaseMeshVertices();

		//--------------------------------------------------------------------
		// Return number of vertices in the subdivided model
		// at the current subdivision level, used for sizing
		// the array for GetSubdivVertices()
		//--------------------------------------------------------------------
		int GetNumSubdivVertices() const;

		//--------------------------------------------------------------------
		// Return positions of vertices from the subdivided mesh into the 
		//	given array. Make sure the array is big enough to hold 
		//	GetNumSubdivVertices() number of points.
		//--------------------------------------------------------------------
		void GetSubdivVertices( maPoint3d* o_pVertices,
								maVector3d* o_pNormals,
								maVector3d* o_pSs,
								maVector3d* o_pTs,
								int i_ArraySize) const;

		//--------------------------------------------------------------------
		// Return direct access to the vertex list in the current
		//	subdivision level.
		//--------------------------------------------------------------------
		const smdlSubdivUtil::sVert* GetSubdivVertices() const;

		//--------------------------------------------------------------------
		// Given the new array of mesh vertices, compute the new positions
		//	of the subdivided mesh. The number of vertices passed in
		//	should match the number of vertices in the original mesh.
		//--------------------------------------------------------------------
		void AlterBaseMesh(const maPoint3d* i_pVertices,
						   int i_NumVertices);

		//--------------------------------------------------------------------
		// Subdiv networks can be shared, but only one can be actively
		// updating positions at a time. So, you need to wrap
		// calls to "AlterBaseMesh" and "GetSubdivVertices" with
		// a envScopedLock using this envMutex.
		//--------------------------------------------------------------------
		envMutex& GetMutex();

//	private:
	public:	// public, really?
		static bool sm_bComputeBasisVectors;

		// Subdiv networks can be shared
		envMutex m_Mutex;

		struct SubdivLevel
		{
			smdlSubdivUtil::sVert* m_pVList;
			smdlSubdivUtil::sFace* m_pTList;
			smdlSubdivUtil::sEdge* m_pEList;
			int m_nVerts, m_nEdges, m_nFaces;
			std::vector<int> m_MaterialChanges;

			SubdivLevel(int i_nVerts, int i_nFaces, int i_nEdges = 0);
			~SubdivLevel();
		};

	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void GenNewVertLocs(SubdivLevel* i_pLevel);

#ifdef QUAD_PATCHES
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void GenSharedLists(SubdivLevel* i_pLevel);
#endif//QUAD_PATCHES

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Update(SubdivLevel* i_pBase, SubdivLevel* i_pLevel);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Subdivide(int i_SubdivLevel,
						const std::vector<mayCreaseInfo> &i_EdgeCreases, 
						int i_MaxEdgeCreaseLevel);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		SubdivLevel* Subdivide(SubdivLevel* i_pBase, 
						bool i_bCreateEdges, 
						bool i_bPromoteCreases);


		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void GatherFragInfo(SubdivLevel* i_pLevel,
							mdlFragInfo& o_SubdivFragInfo);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		int count_new_faces(SubdivLevel* i_pBase);

		//--------------------------------------------------------------------
		// mark edges as creased 
		//--------------------------------------------------------------------
		void assign_creases(smdlSubdivNetwork::SubdivLevel *i_pLevel, 
							const std::vector<mayCreaseInfo> &i_EdgeCreases, 
							int i_LevelIndex);


	public:
//-------------Direct evaluation functions------------------------------------
		typedef struct  
		{
			double *L;		//L[k]
			double *iV;		//iV[k][k]
			double **x;		//x[k][3][16]
		} EIGENSTRUCT;

		EIGENSTRUCT *m_Eigenvals;	//EIGENSTRUCT[ nmax ];
		int	m_nMax;

		void ProjectPoints( maPoint3d* Cp, maPoint3d* C, int N );
		void EvalSurf( maPoint3d P, double u, double v, maPoint3d* Cp, int N );
		double EvalSpline( double* Val, double u, double v );

//----------------------------------------------------------------------------

		//bga - Changed to mdlFragInfo in order to express multiple materials
		//mdlFragInfo m_SubdivFragInfo;
		mdlFragInfo m_SubdivFragInfo;

		std::vector<SubdivLevel*> m_Levels;
		//std::vector<int> m_Remap;
		int m_CurSubdivLevel;
		bool m_bHasTextureCoords;
		
		std::vector<mayCreaseInfo> m_EdgeCreases;
		int m_MaxEdgeCreaseLevel;
		int m_MaxSubdivLevel;
};
