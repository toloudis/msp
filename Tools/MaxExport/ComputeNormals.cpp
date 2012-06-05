/*****************************************************************************
**  PolyOrTriMesh.cpp
**
**	Contains classes and functions that contain the logic of 
**	how to compute the normal for the mesh
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "ComputeNormals.hpp"


#include "MtlExporter.hpp"
#include "ExportDoc.hpp"
#include "HashSetPoint3.hpp"
#ifndef MAXEXP_MAXEXPORTERUTILS_HPP
#include "MaxExportUtils.hpp"
#endif
#ifndef MAXEXP_EXPORTINTENT_HPP
#include "ExportIntent.hpp"
#endif
#ifndef MAXEXP_POLYORTRIMESH_HPP_HPP
#include "PolyOrTriMesh.hpp"
#endif
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
//max include files
#include "MaxCommon.hpp"
#include "stdmat.h"
#include "mnMesh.h"

#include <cstdio>
#include <string>
#include <algorithm>
#include <hash_set>
#include <crtdbg.h>
#include <set>

using namespace std;
using namespace stdext;

namespace MaxExp
{ 


	//If the mesh has valid user specific normals
	//then returns true
	bool ComputeNormalsBase::CheckUserSpecifiedNormals( PolyOrTriMesh &i_POrT )
	{
		bool bRetVal = false;
		if( NULL != i_POrT.m_pTri )
		{
			::Mesh & mesh = i_POrT.m_pTri->GetMesh();
			MeshNormalSpec *meshNormalSpec = mesh.GetSpecifiedNormals();
			bRetVal = ( meshNormalSpec != NULL && meshNormalSpec->GetNumNormals() > 0 );
		} else if ( NULL != i_POrT.m_pPoly )
		{

			MNMesh &mnMesh = i_POrT.m_pPoly->GetMesh();
			MNNormalSpec *meshNormalSpec = mnMesh.GetSpecifiedNormals();
			bRetVal = ( meshNormalSpec != NULL && meshNormalSpec->GetNumNormals() > 0 );
		}
		return bRetVal;
	}

	//If this is caled for the first time, it builds the RVertex/RNormals
	//of the mesh. Subsequebt calls are harmless
	void ComputeNormalsBase::CheckNormals( PolyOrTriMesh &i_POrT  )
	{
		bool bRetVal = false;
		if( NULL != i_POrT.m_pTri )
		{
			::Mesh & mesh = i_POrT.m_pTri->GetMesh();
			mesh.checkNormals( TRUE );
		} else if ( NULL != i_POrT.m_pPoly )
		{

			MNMesh &mnMesh = i_POrT.m_pPoly->GetMesh();
			mnMesh.checkNormals( TRUE );
		}
	}


	//For deteremining the index of a normal corresponding
	//to a face corner, we need to keep track of the
	//total number of RNormals which comes before this RVertex.

	//How many normals are there in all RVertices upto the i'th RVertex,
	//(not incuding the i'th RVertex)
	//The TVertNormalsOIndexOffsetMap is a map that is initialized
	//by the constructor and it helps in determinibg the required offset.
	//Please note that  for checking whether the smoothing grioup of a face 
	//is consistent with any normals of  RVertex we use 'eequality' check
	//Need to verify whether this is fine, (or whether we should use '&'-ing)
	bool ComputeNormalsBase::HaveConsistentRVertices( ::Mesh & i_Mesh )
	{
		bool bIsRVertsConsistent = true;
		int nFaces = i_Mesh.numFaces;
		//for each face
		for( int fIdx =0; fIdx < nFaces; ++fIdx )
		{
			Face &face = i_Mesh.faces[ fIdx ];
			//for each corner
			for( int vertIdxInFace=0; vertIdxInFace < 3; ++vertIdxInFace )
			{
				int vertIdxInMesh = face.v[ vertIdxInFace ];		
				//get the RVertex
				RVertex *pRVert = i_Mesh.getRVertPtr( vertIdxInMesh );
				if( pRVert == NULL )
				{
					return false;
				}
				DBG_ASSERT( pRVert, "should have built RVerts" );
				const int nRNormalsForVertex = pRVert->rFlags & NORCT_MASK;
				//If there are more than one RNormals,
				if( nRNormalsForVertex > 1 )
				{
					bool bFoundNormal = false;
					for(  int k=0; k < nRNormalsForVertex ; ++k)
					{
						RNormal &rn  = pRVert->ern[k];
						//Is this rm.getSmGroup() & face.smGroup ?
						if( rn.getSmGroup() == face.smGroup )
						{
							bFoundNormal = true;
							break;
						}
					}
					if( !bFoundNormal )
					{
						//inconsistent
						return false;
					}
				}
			}
		}
		return true;
	}

	int ComputeUserSpecifiedNormals::GetNNormals()
	{
		DBG_ASSERT( CheckUserSpecifiedNormals( m_Parent ), "mesh should be having user specified normals" );
		int nRetVal = 0;
		if( NULL != m_Parent.m_pTri )
		{
			::Mesh & mesh = m_Parent.m_pTri->GetMesh();
			//if there are user specified normals
			//returns that number

			MeshNormalSpec *meshNormalSpec = mesh.GetSpecifiedNormals();
			nRetVal = meshNormalSpec->GetNumNormals();
		} else  if ( NULL != m_Parent.m_pPoly )
		{	
			//else if it is poly mesh
			MNMesh &mnMesh = m_Parent.m_pPoly->GetMesh();
			//retirn the number of user specified normals
			MNNormalSpec *meshNormalSpec = mnMesh.GetSpecifiedNormals();
			nRetVal = meshNormalSpec->GetNumNormals();
		}
		return nRetVal;
	}


	int  ComputeUserSpecifiedNormals::GetNormal( int i_nFaceIdx, int i_nVertIdxInFace, Point3 &o_Normal )
	{
		DBG_ASSERT( CheckUserSpecifiedNormals( m_Parent ), "mesh should be having user specified normals" );
		DBG_ASSERT( ( i_nFaceIdx <  m_Parent.GetNFaces() ),\
			"inFaceIdx: " << i_nFaceIdx << " should be less than nFaces: " << m_Parent.GetNFaces() );
		DBG_ASSERT( ( i_nVertIdxInFace < m_Parent.GetNVertsInFace( i_nFaceIdx ) ), \
			"inFaceIdx: " << i_nVertIdxInFace << " should be less than nFaces: " << m_Parent.GetNVertsInFace( i_nFaceIdx ) );

		int normalIdx = -1;
		if( NULL != m_Parent.m_pTri )
		{
			::Mesh & mesh = m_Parent.m_pTri->GetMesh();
			MeshNormalSpec *meshNormalSpec = mesh.GetSpecifiedNormals();
			DBG_ASSERT ( ( meshNormalSpec != NULL && meshNormalSpec->GetNumNormals() > 0 ), "mesh should have user specided normals" );
			o_Normal = meshNormalSpec->GetNormal(i_nFaceIdx, i_nVertIdxInFace);
			normalIdx = meshNormalSpec->GetNormalIndex( i_nFaceIdx, i_nVertIdxInFace );
		} else if ( NULL != m_Parent.m_pPoly )
		{

			MNMesh &mnMesh = m_Parent.m_pPoly->GetMesh();
			MNNormalSpec *meshNormalSpec = mnMesh.GetSpecifiedNormals();
			DBG_ASSERT( ( meshNormalSpec != NULL && meshNormalSpec->GetNumNormals() >= 0 ), "mesh should have user specified normals");
			o_Normal = meshNormalSpec->GetNormal( i_nFaceIdx, i_nVertIdxInFace );
			normalIdx = meshNormalSpec->GetNormalIndex( i_nFaceIdx, i_nVertIdxInFace );
		} 
		return normalIdx;
	}

	Point3  ComputeUserSpecifiedNormals::GetIthNormal( int i_NormalIdx )
	{
		Point3 retVal(0,0,0);
		DBG_ASSERT( CheckUserSpecifiedNormals( m_Parent ), "mesh should be having user specified normals" );
		int nRetVal = 0;
		if( NULL != m_Parent.m_pTri )
		{
			::Mesh & mesh = m_Parent.m_pTri->GetMesh();
			//if there are user specified normals
			//returns that number

			MeshNormalSpec *meshNormalSpec = mesh.GetSpecifiedNormals();
			int nNumNormals = meshNormalSpec->GetNumNormals();
			Point3 *pNormals = meshNormalSpec->GetNormalArray();
			DBG_ASSERT( (i_NormalIdx < nNumNormals ), \
				"i_NormalIdx: " << i_NormalIdx <<  " should be less than :" << nNumNormals );
			retVal = pNormals[ i_NormalIdx ];
		} else  if ( NULL != m_Parent.m_pPoly )
		{	
			//else if it is poly mesh
			MNMesh &mnMesh = m_Parent.m_pPoly->GetMesh();
			//retirn the number of user specified normals
			MNNormalSpec *meshNormalSpec = mnMesh.GetSpecifiedNormals();
			int nNumNormals = meshNormalSpec->GetNumNormals();
			DBG_ASSERT( (i_NormalIdx < nNumNormals ), \
				"i_NormalIdx: " << i_NormalIdx <<  " should be less than :" << nNumNormals );
			retVal = meshNormalSpec->GetNormalArray()[ i_NormalIdx ];
		}
		return retVal;
	}

	ComputeRNormals::ComputeRNormals( PolyOrTriMesh & i_Parent ):
	ComputeNormalsBase( i_Parent ),
		m_nNormals(0)
	{
		Init();
	}

	ComputeRNormals::~ComputeRNormals()
	{

		if( NULL != m_Parent.m_pTri )
		{
			::Mesh & mesh = m_Parent.m_pTri->GetMesh();
		}
	}

	int ComputeRNormals::GetNNormals( RVertex *i_pRVert )
	{
		DBG_ASSERT( ( NULL != i_pRVert ), "RVertices should not b NULL" );
		const int nRNormalsForVertex = i_pRVert->rFlags & NORCT_MASK;
		DBG_ASSERT( ( nRNormalsForVertex > 0 ), "RNormals should have atleast one valid normal" );
		return nRNormalsForVertex;
	}

	int ComputeRNormals::GetNNormals()
	{
		return m_nNormals;
	}

	int ComputeRNormals::ComputeNNormals()
	{
		//number of mesh verts
		int nVerts = m_Parent.GetNVerts();
		int nNormals = 0;
		if( NULL != m_Parent.m_pTri )
		{
			::Mesh & mesh = m_Parent.m_pTri->GetMesh();
			for( int rVertIdxInMesh =0; rVertIdxInMesh < nVerts; ++ rVertIdxInMesh )
			{
				//get the RVertex corresponding to ith vert
				RVertex *pRVert = mesh.getRVertPtr( rVertIdxInMesh );
				nNormals += GetNNormals( pRVert );
			}
		} else if ( m_Parent.m_pPoly )
		{
			DBG_ASSERT( false, "do not support RVerts for poly mesh" );
		}
		return nNormals;
	}

	void ComputeRNormals::Init( )
	{
		DBG_ASSERT( ( m_Parent.GetNVerts() > 0), "mesh should have some  verts" );
		int nVerts = m_Parent.GetNVerts();

		//compute the number of RNormals
		m_nNormals=ComputeNNormals();
		//build the m_RVertNormalsIndexOffsetMap
		//also, build the copy of RNormals, (ie: m_Normals) 
		if( NULL != m_Parent.m_pTri )
		{
			::Mesh & mesh = m_Parent.m_pTri->GetMesh();
			m_RVertNormalsIndexOffsetMap[ 0 ] = 0;
			int nNormals = GetNNormals();
			m_Normals.resize( nNormals );
			for( int rVertIdxInMesh =1, curNormalIdx=0; rVertIdxInMesh < nVerts; ++ rVertIdxInMesh )
			{
				RVertex *pRVert = mesh.getRVertPtr( rVertIdxInMesh-1 );				
				DBG_ASSERT( (NULL != pRVert ), ("mesh should have RVerts built") );
				const int numNormalsInRVert = pRVert->rFlags & NORCT_MASK;
				m_RVertNormalsIndexOffsetMap[ rVertIdxInMesh ] = m_RVertNormalsIndexOffsetMap[ rVertIdxInMesh -1 ] + numNormalsInRVert;				
				if( numNormalsInRVert == 1 )
				{
					DBG_ASSERT( curNormalIdx < nNormals , \
						"curNormalIdx: " << curNormalIdx << " should be less than " << nNormals );
					m_Normals[ curNormalIdx++ ] =  pRVert->rn.getNormal();
				} else
				{
					//otherwise extract the normal, which has the 
					//same smoothing group association as the face
					bool bFoundNormal = false;
					for(  int k=0; k < numNormalsInRVert ; ++k)
					{
						RNormal &rn  = pRVert->ern[k];
						DBG_ASSERT( curNormalIdx < nNormals , \
							"curNormalIdx: " << curNormalIdx << " should be less than " << nNormals );
						m_Normals[ curNormalIdx++ ] = rn.getNormal();
					}
				}
			}
		} else if ( NULL != m_Parent.m_pPoly )
		{
			DBG_ASSERT( false, "do not support RVerts for poly mesh" );
		}
	}

	int ComputeRNormals::GetRVertNormalsIndexOffset( int i_RVertIdxInMesh )
	{
		DBG_ASSERT( (m_RVertNormalsIndexOffsetMap.size() == m_Parent.GetNVerts() ),\
			"RVertNormalsIndexOffsetMap should be initialized properly" );		
		DBG_ASSERT( (i_RVertIdxInMesh < m_Parent.GetNVerts() ),\
			"i_RVertIdxInMesh: " << i_RVertIdxInMesh << "  should be < " << m_Parent.GetNVerts() );
		TRVertNormalsIndexOffsetMap::const_iterator rit = m_RVertNormalsIndexOffsetMap.find( i_RVertIdxInMesh );
		DBG_ASSERT( (rit != m_RVertNormalsIndexOffsetMap.end() ),\
			"m_RVertNormalsIndexOffset should have an entry for i_RVertIdxInMEsh: " << i_RVertIdxInMesh );
		return m_RVertNormalsIndexOffsetMap[ i_RVertIdxInMesh ];
	}

	int  ComputeRNormals::GetNormal( int i_nFaceIdx, int i_nVertIdxInFace, Point3 &o_Normal )
	{

		DBG_ASSERT( ( i_nFaceIdx <  m_Parent.GetNFaces() ),\
			"inFaceIdx: " << i_nFaceIdx << " should be less than nFaces: " << m_Parent.GetNFaces() );
		DBG_ASSERT( ( i_nVertIdxInFace < m_Parent.GetNVertsInFace( i_nFaceIdx ) ), \
			"inFaceIdx: " << i_nVertIdxInFace << " should be less than nFaces: " << m_Parent.GetNVertsInFace( i_nFaceIdx ) );

		int normalIdx = -1;
		if( NULL != m_Parent.m_pTri )
		{
			::Mesh & mesh = m_Parent.m_pTri->GetMesh();
			Face &face = mesh.faces[i_nFaceIdx];
			int vertIdxInMesh = face.v[i_nVertIdxInFace];
			RVertex* pRVert = mesh.getRVertPtr( vertIdxInMesh );
			DBG_ASSERT( (NULL != pRVert ), ("mesh should have RVerts built") );
			const int nRNormalsForVertex = pRVert->rFlags & NORCT_MASK;
			DBG_ASSERT( nRNormalsForVertex > 0, "RVerts shoulod have more than one normals" );
			//If the mesh has only normal for this vertex,
			//irrespective of the face currently associated with
			//this vertex, then return that normal
			int rVertOffset = GetRVertNormalsIndexOffset( vertIdxInMesh );
			if( nRNormalsForVertex == 1 )
			{
				o_Normal = pRVert->rn.getNormal();
				normalIdx = rVertOffset;
			} else
			{
				//otherwise extract the normal, which has the 
				//same smoothing group association as the face
				bool bFoundNormal = false;
				for(  int k=0; k < nRNormalsForVertex ; ++k)
				{
					RNormal &rn  = pRVert->ern[k];
					if( rn.getSmGroup() == face.smGroup )
					{
						bFoundNormal = true;
						o_Normal = rn.getNormal();
						normalIdx = rVertOffset + k;
						break;
					}
				}
				DBG_ASSERT( bFoundNormal, \
					"should have found a normal consistent with the face's(" << i_nFaceIdx << ") smooth group: "<< face.smGroup );
			}
		} else if ( NULL !=  m_Parent.m_pPoly )
		{				
			DBG_ASSERT( false, "do not support RVerts for poly mesh" );
		} else
		{
			DBG_ASSERT( false, "do not support RVerts" );
		}
		return normalIdx;
	}


	Point3  ComputeRNormals::GetIthNormal( int i_NormalIdx )
	{
		DBG_ASSERT ( ( i_NormalIdx < m_nNormals ), "i_NormalIdx: " << i_NormalIdx << " should be less than " << m_nNormals );
		DBG_ASSERT( ( m_Normals.size() == m_nNormals ), "m_Normals should be initialized to the normals " );
		return m_Normals[ i_NormalIdx ];
	}



	int ComputeFaceNormals::GetNNormals()
	{
		int nFaces = m_Parent.GetNFaces();
		return nFaces;
	}


	int  ComputeFaceNormals::GetNormal( int i_nFaceIdx, int i_nVertIdxInFace, Point3 &o_Normal )
	{

		DBG_ASSERT( ( i_nFaceIdx <  m_Parent.GetNFaces() ),\
			"inFaceIdx: " << i_nFaceIdx << " should be less than nFaces: " << m_Parent.GetNFaces() );
		DBG_ASSERT( ( i_nVertIdxInFace < m_Parent.GetNVertsInFace( i_nFaceIdx ) ), \
			"inFaceIdx: " << i_nVertIdxInFace << " should be less than nFaces: " << m_Parent.GetNVertsInFace( i_nFaceIdx ) );

		int normalIdx = -1;
		if( NULL != m_Parent.m_pTri )
		{

			::Mesh & mesh = m_Parent.m_pTri->GetMesh();
			o_Normal = mesh.getFaceNormal( i_nFaceIdx );
			normalIdx = i_nFaceIdx;
		} else if ( NULL != m_Parent.m_pPoly )
		{
			MNMesh &mnMesh = m_Parent.m_pPoly->GetMesh();
			o_Normal = mnMesh.GetFaceNormal( i_nFaceIdx, TRUE /*normalize*/ );
			normalIdx = i_nFaceIdx;
		} 
		return normalIdx;
	}

	Point3  ComputeFaceNormals::GetIthNormal( int i_NormalIdx )
	{
		int nFaces = m_Parent.GetNFaces();
		DBG_ASSERT ( ( i_NormalIdx < nFaces ), "i_NormalIdx: " << i_NormalIdx << " should be less than " << nFaces );
		Point3 retVal(0,0,0);
		GetNormal( i_NormalIdx, 0, retVal );
		return retVal;
	}

	//=============================================================================

	//Factory method for creating the appropriate derived class of ComputeNormalBase-s

	// First check for any user specified set of normals in the  tri or poly mesh
	// If so, construct a ComputeUserSpecifiedNormals

	// If not found, make a ComputeRNormals class.
	// THis will ask use the max to build RVerts-RNormals which takes into account 
	// the smoothing group to which a face belongs to when computeing the normal of a vertex.
	//. THis will be supported only for a tri mesh
	// Also, the RVerts-RNormal construction has to be consistent., for eny given face
	// for any given vertex in the face, the corresponding RVert should contain an RNormal 
	//of the same smoothing group.

	// If none of these arrangements fits
	// let us use a ComputeFaceNormals class, which computes the flat shaded normals
	// of a face
	//=============================================================================

	ComputeNormalsBase* ComputeNormalsBase ::MakeComputeNormals( PolyOrTriMesh & i_POrT )
	{
		ComputeNormalsBase *pRetVal = NULL;
		if( i_POrT.GetNVerts() <= 0 )
		{
			pRetVal = new ComputeNormals_Dummy( i_POrT );
			EXPLOG.WriteInfo( "num Verts to export == 0, so using ComputeNormals_Dummy" );

		}
		if ( ComputeNormalsBase::CheckUserSpecifiedNormals( i_POrT ) )
		{
			pRetVal = new ComputeUserSpecifiedNormals( i_POrT );

			EXPLOG.WriteInfo( "using user specified normals to compute normals" );
		} else  if ( i_POrT.m_pTri )
		{
			::Mesh & mesh = i_POrT.m_pTri->GetMesh();
			ComputeNormalsBase::CheckNormals( i_POrT );
			bool bConsistentRVerts = ComputeNormalsBase::HaveConsistentRVertices( mesh );
			if ( bConsistentRVerts )
			{
				pRetVal = new ComputeRNormals( i_POrT );
			}
			else 
			{

				pRetVal = new ComputeFaceNormals( i_POrT );
				EXPLOG.WriteWarning( "mesh cannot use RVerts/RNormals, due to inconstency, using FaceNormals instead");
			}
		} else if ( i_POrT.m_pPoly )
		{
			pRetVal = new ComputeFaceNormals( i_POrT );
			EXPLOG.WriteInfo( "mesh using FAceNormals" );
		} else
		{

			pRetVal = new ComputeNormals_Dummy( i_POrT );			
			EXPLOG.WriteInfo( "no m_pTri and no m_pPoly, so using ComputeNormals_Dummy" );
		}

		return pRetVal;
	}




} //namespace MaxExp