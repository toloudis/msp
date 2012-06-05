/*****************************************************************************
**  PolyOrTriMesh.cpp
**
**	Wrapper class that encompasses common functionalities of Mesh and MNMesh
**	of 3ds Max SDK
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "PolyOrTriMesh.hpp"


#include "MtlExporter.hpp"
#include "ExportDoc.hpp"
#include "HashSetPoint3.hpp"
#ifndef MAXEXP_MAXEXPORTERUTILS_HPP
#include "MaxExportUtils.hpp"
#endif
#ifndef MAXEXP_EXPORTINTENT_HPP
#include "ExportIntent.hpp"
#endif
#ifndef MAXEXP_COMPUTENORMALS_HPP
#include "ComputeNormals.hpp"
#endif
#include "MeshExporter.hpp"
#include "SkinExporter.hpp"

#ifndef ENV_STRING_HPP
#include "Core/Env/envString.hpp"
#endif 
#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/fs/fsXMLWriter.hpp"
#include "Graphics/vtx/vtxVertexFrame.hpp"
#include "Graphics/mdl/mdlNodeInfo.hpp"
#include "Graphics/mdl/mdlFragInfo.hpp"
#include "Graphics/mdl/mdlFragUtil.hpp"
#include "Graphics/mdl/mdlSubdivInfo.hpp"
//max include files
#include "MaxCommon.hpp"
#include "stdmat.h"
#include "buildver.h"
#include "shaders.h"
#include "XRef/iXrefObj.h"
#include "XRef/iXrefMaterial.h"
#include "CS/Bipexp.h"
#include "mnMesh.h"
#include "ipointcache.h"

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


	//========================================================================
	// A utility class which hides the logic of detrmining,
	// whether the current node has a poly-mesh or  triangular representation.
	// At the end of  the construction 
	// one of the m_pPoly or m_ptri would be non-null	

	// Even though this class has the support for accomodating
	// either a polygonal  mesh representation or a triangular mesh representation,
	// right now the exporter data expects the mesh to be triangular.
	// So in the constructor of this class(towards the end), 
	// if we endup with a poly-mesh, an attempt is made to triangulate a poly-mesh
	// to a tiangle mesh. Finally if such a conversion doesnt work and we still
	// endup nwith a poly-mesh, then the mesh exporter will trigger an error
	//========================================================================

	PolyOrTriMesh::PolyOrTriMesh( 
		INode &i_CurNode,  
		ExportDoc *i_pExportDoc,
		Object &i_Obj,
		bool i_bTriangulate):
	m_pCurNode(&i_CurNode),
		m_pExportDoc( i_pExportDoc ),
		m_pObj(&i_Obj),
		m_pPoly(NULL),
		m_pTri(NULL),
		m_bDeleteObj(false)
	{	
		if( i_bTriangulate )
		{
			InitWithConversionToTri();
			if( NULL == m_pTri )
			{
				EXPLOG.WriteError( "cannot triangulate node : %s", i_CurNode.GetName() );
				throw export_failure();
			}
		} else
		{
			InitWithConversionToPoly();
			if( NULL == m_pPoly && NULL == m_pTri )
			{				
				EXPLOG.WriteError( "cannot get either a polymesh or a trimesh from the  node : %s", i_CurNode.GetName() );
				throw export_failure();
			}
		}
	}	



	/**
	Try to convert the geometry into a tri-mesh
	If not throw an exception.
	*/
	void PolyOrTriMesh::InitWithConversionToTri()
	{
		assert( m_pObj != NULL);
		MCHAR *szNodename = m_pCurNode->GetName();
		Class_ID id = m_pObj->ClassID();
		SClass_ID sid = m_pObj->SuperClassID();


		//is this an editable poly object?
		if( sid == GEOMOBJECT_CLASS_ID && ( id == EPOLYOBJ_CLASS_ID || id.PartA() == POLYOBJ_CLASS_ID) )
		{
			m_pPoly = (PolyObject *) m_pObj;
		}
		if( m_pPoly == NULL )
		{
			//is this an editable  mesh?
			if (sid == GEOMOBJECT_CLASS_ID && (id.PartA() == EDITTRIOBJ_CLASS_ID || id.PartA() == TRIOBJ_CLASS_ID))
			{
				m_pTri = (TriObject*) m_pObj;
			} else
			{ // not an editable mesh
				//is the object a geomtric primitive?
				//detail: some bad stuff going here regarding controllers

				if (id == Class_ID(BOXOBJ_CLASS_ID, 0) ||
					id == Class_ID(SPHERE_CLASS_ID, 0) ||
					id == Class_ID(CYLINDER_CLASS_ID, 0) ||
#if (MAX_VERSION_MAJOR >= 8)
#ifndef NO_OBJECT_STANDARD_PRIMITIVES
					id == PLANE_CLASS_ID ||
					id == PYRAMID_CLASS_ID ||
					id == GSPHERE_CLASS_ID ||
#endif // NO_OBJECT_STANDARD_PRIMITIVES
#endif // USE_MAX_8
					id == Class_ID(CONE_CLASS_ID, 0) ||
					id == Class_ID(TORUS_CLASS_ID, 0) ||
					id == Class_ID(TUBE_CLASS_ID, 0) ||
					id == Class_ID(HEDRA_CLASS_ID, 0) ||
					id == Class_ID(BOOLOBJ_CLASS_ID, 0)
					)
				{
					//if so can it be converted to a triobject
					if (m_pObj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
					{
						m_pTri = (TriObject*) m_pObj->ConvertToType(static_cast<TimeValue>(OPTS.m_StartTime), Class_ID(TRIOBJ_CLASS_ID, 0));
						m_bDeleteObj = true;
					}
				}
				//if still tri == NULL,
				//can this object be converted to a poly object
				if( NULL == m_pTri  && ( m_pObj->CanConvertToType(Class_ID(POLYOBJ_CLASS_ID, 0))) )
				{
					m_pPoly = (PolyObject*) m_pObj->ConvertToType(static_cast<TimeValue>(OPTS.m_StartTime), Class_ID(POLYOBJ_CLASS_ID, 0));
					m_bDeleteObj = true;
				}
			}
		} 

		//If object is representable as a poly-object
		if( m_pPoly )
		{
			assert( m_pTri == NULL );
			//try to convert it to a triObject
			Object *objWhichIsAPoly = (Object *)m_pPoly;
			if (objWhichIsAPoly->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
			{
				//if we can convert the poly object to a triObject
				m_pTri = (TriObject*) m_pObj->ConvertToType( static_cast<TimeValue>(OPTS.m_StartTime), Class_ID(TRIOBJ_CLASS_ID, 0));
				//if the object  corresponding to 'poly' is the same as the original 'obj'
				if( objWhichIsAPoly == m_pObj )
				{
					//delete obj and set poly = NULL, because we have a 'tri'
					m_bDeleteObj = true;
					m_pPoly =  NULL;
				} else
				{  //if the object  corresponding to 'poly' is not the same as the original 'obj'
					assert( m_bDeleteObj == true );
					m_pPoly->DeleteMe();
					m_pPoly = NULL;
				}
				int foo = m_pTri->GetMesh().getNumVerts();
				foo =0;
			} 

		}
		//if m_pPoly is not null, then m_pTri is null
		//if m_pTri is not null, then m_pPoly is null
		assert (  ( NULL == m_pPoly ) ||  ( NULL == m_pTri ) );
	}



	/**
	Try to convert the geometry into a poly-mesh,
	if not, a tri mesh
	*/
	void PolyOrTriMesh::InitWithConversionToPoly()
	{
		assert( m_pObj != NULL);
		MCHAR *szNodename = m_pCurNode->GetName();
		Class_ID id = m_pObj->ClassID();
		SClass_ID sid = m_pObj->SuperClassID();

		//is this a tri object?
		if (sid == GEOMOBJECT_CLASS_ID && (id.PartA() == EDITTRIOBJ_CLASS_ID || id.PartA() == TRIOBJ_CLASS_ID))
		{
			m_pTri = (TriObject*) m_pObj;
		}

		if( m_pTri == NULL )
		{
			//is this an editable poly object?
			if( sid == GEOMOBJECT_CLASS_ID && ( id == EPOLYOBJ_CLASS_ID || id.PartA() == POLYOBJ_CLASS_ID) )
			{
				m_pPoly = (PolyObject *) m_pObj;
			}
			else
			{   // not an editablenpoly
				//is the object a geomtric primitive?
				//detail: some bad stuff going here regarding controllers

				if (id == Class_ID(BOXOBJ_CLASS_ID, 0) ||
					id == Class_ID(SPHERE_CLASS_ID, 0) ||
					id == Class_ID(CYLINDER_CLASS_ID, 0) ||
#if (MAX_VERSION_MAJOR >= 8)
#ifndef NO_OBJECT_STANDARD_PRIMITIVES
					id == PLANE_CLASS_ID ||
					id == PYRAMID_CLASS_ID ||
					id == GSPHERE_CLASS_ID ||
#endif // NO_OBJECT_STANDARD_PRIMITIVES
#endif // USE_MAX_8
					id == Class_ID(CONE_CLASS_ID, 0) ||
					id == Class_ID(TORUS_CLASS_ID, 0) ||
					id == Class_ID(TUBE_CLASS_ID, 0) ||
					id == Class_ID(HEDRA_CLASS_ID, 0) ||
					id == Class_ID(BOOLOBJ_CLASS_ID, 0)
					)
				{
					//if so can it be converted to a polyobject
					if (m_pObj->CanConvertToType(Class_ID(POLYOBJ_CLASS_ID, 0)))
					{
						m_pPoly = (PolyObject*) m_pObj->ConvertToType( static_cast<TimeValue>(OPTS.m_StartTime), Class_ID( POLYOBJ_CLASS_ID, 0));
						m_bDeleteObj = true;
					}
				}

				//if still poly == NULL,
				//can this object be converted to a tri
				if( NULL == m_pPoly  && ( m_pObj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) )
				{
					m_pTri = ( TriObject* ) m_pObj->ConvertToType( static_cast<TimeValue>(OPTS.m_StartTime), Class_ID( TRIOBJ_CLASS_ID, 0));
					m_bDeleteObj = true;
				}
			}
		} 

	}


	PolyOrTriMesh::~PolyOrTriMesh()
	{
		//If the tri  or poly representation were
		//expressly generated for export
		//as opposed to being a part of the object state,
		//delete them
		if (m_bDeleteObj)
		{
			if ( m_pTri != NULL) m_pTri->DeleteMe();
			if ( m_pPoly != NULL) m_pPoly->DeleteMe();
		}
	}

	bool PolyOrTriMesh::IsValid() const
	{
		//either poly is not NULL or _tri is not NULL;
		return ( (m_pPoly != NULL) &&  (m_pTri == NULL) || (m_pPoly == NULL) &&  (m_pTri != NULL) ) ;
	}

	bool PolyOrTriMesh::IsPoly() const
	{
		return IsValid() && m_pTri == NULL;
	}
	bool PolyOrTriMesh::IsTri() const
	{
		return IsValid() &&  m_pTri != NULL;
	}
	//Each mesh, in addition to the position and normal info
	//can have additional channels which are in meshmap-s.
	//This gets all the  texture channel ids of the mesh.

	void PolyOrTriMesh::GetTextureChannels( vector<int> &o_Channels )
	{
		if( NULL != m_pTri)
		{
			::Mesh  &triMesh = m_pTri->GetMesh();

			//for (int i = -NUM_HIDDENMAPS; i < MAX_MESHMAPS; ++i)
			//For now disregard the hidden maps
			//Also since map with index 0 is the color map,
			//dont bother, start from 1
			for (int i = 1; i < MAX_MESHMAPS; ++i)
			{
				if (triMesh.mapSupport(i) == TRUE)
					o_Channels.push_back(i);
			}
		} else if (NULL != m_pPoly)
		{
			MNMesh &mnMesh = m_pPoly->GetMesh();
			int chanNum = mnMesh.MNum();

			//for (int i = -NUM_HIDDENMAPS; i < MAX_MESHMAPS; ++i)
			//For now disregard the hidden maps
			//Also since map with index 0 is the color map,
			//dont bother, start from 1
			for (int i = 1; i < chanNum; ++i)
			{
				MNMap *map = mnMesh.M(i);
				if (map == NULL) continue;
				if (!map->GetFlag(MN_DEAD))
					o_Channels.push_back(i);
			}
		}
	}

	int PolyOrTriMesh::GetNVerts()const
	{			
		int nVerts = 0;
		if( NULL != m_pTri )
		{
			::Mesh & mesh = m_pTri->GetMesh();
			nVerts = mesh.getNumVerts();
		} else if (NULL != m_pPoly )
		{
			MNMesh &mnMesh = m_pPoly->GetMesh();
			nVerts = mnMesh.VNum();
		}
		return nVerts;
	}


	int PolyOrTriMesh::GetNFaces() const
	{
		int nFaces = 0;
		if( NULL != m_pTri )
		{
			::Mesh & mesh = m_pTri->GetMesh();
			nFaces = mesh.getNumFaces();
		} else if (NULL != m_pPoly )
		{
			MNMesh &mnMesh = m_pPoly->GetMesh();
			nFaces = mnMesh.FNum();
		}
		return nFaces;

	}


	int PolyOrTriMesh::GetNVertsInFace( int fIdx )
	{
		int nVertsInFace = 0;
		if( NULL != m_pTri )
		{
			nVertsInFace = 3;
		} else if (NULL != m_pPoly )
		{
			MNMesh &mnMesh = m_pPoly->GetMesh();
			MNFace *pFace = mnMesh.F( fIdx );
			assert( pFace );
			nVertsInFace = pFace->deg;
		}
		return nVertsInFace;
	}

	int PolyOrTriMesh::GetNTVerts( int tchIdx)
	{				
		int nTVerts = 0;
		if( NULL != m_pTri )
		{
			::Mesh & mesh = m_pTri->GetMesh();
			MeshMap &tmap = mesh.Map( tchIdx );
			nTVerts = tmap.getNumVerts();

		} else if (NULL != m_pPoly )
		{				
			MNMesh &mnMesh = m_pPoly->GetMesh();
			nTVerts = mnMesh.M( tchIdx )->numv;
		}		
		return nTVerts;
	}

	int PolyOrTriMesh::GetVertIdxInMesh( int faceIdx, int vertIdxInFace )
	{				
		int vertIdxInMesh = 0;
		if( NULL != m_pTri )
		{
			assert( vertIdxInFace < 3 );
			::Mesh & mesh = m_pTri->GetMesh();
			assert( faceIdx < mesh.getNumFaces() );
			Face &face = mesh.faces[ faceIdx ];
			vertIdxInMesh = face.v[ vertIdxInFace ];

		} else if (NULL != m_pPoly )
		{				
			MNMesh &mnMesh = m_pPoly->GetMesh();
			assert( faceIdx < mnMesh.FNum() );
			MNFace *pFace = mnMesh.F( faceIdx );
			assert( pFace );
			vertIdxInMesh = pFace->vtx[ vertIdxInFace ];
		}		
		return vertIdxInMesh ;
	}


	Point3  PolyOrTriMesh::GetVert( int vertIdxInMesh )
	{				

		if( NULL != m_pTri )
		{
			::Mesh & mesh = m_pTri->GetMesh();				
			assert( vertIdxInMesh < mesh.getNumVerts() );
			return mesh.verts[ vertIdxInMesh ];

		} else  
		{	assert (NULL != m_pPoly );		
		MNMesh &mnMesh = m_pPoly->GetMesh();
		assert( vertIdxInMesh < mnMesh.VNum() );
		return mnMesh.V( vertIdxInMesh )->p;
		}		
	}

	int PolyOrTriMesh::GetTVertIdx( int tchIdx, int faceIdx, int vertIdxInFace )
	{
		int nTVertIdx = 0;
		if( NULL != m_pTri )
		{
			::Mesh & mesh = m_pTri->GetMesh();						
			MeshMap &tmap = mesh.Map( tchIdx );
			assert( faceIdx < mesh.getNumFaces() );
			TVFace &texFace = tmap.tf[ faceIdx ];
			DWORD dTextureIdx = texFace.getTVert( vertIdxInFace );
			nTVertIdx = static_cast< int > ( dTextureIdx );
			assert( nTVertIdx < tmap.getNumVerts() );
		} else if (NULL != m_pPoly )
		{				
			MNMesh &mnMesh = m_pPoly->GetMesh();
			assert( faceIdx < mnMesh.FNum() );				
			MNMapFace * pMapFace = mnMesh.MF( tchIdx, faceIdx );
			assert( vertIdxInFace < pMapFace->deg );
			nTVertIdx = pMapFace->tv[ vertIdxInFace ];
			int nTVerts = mnMesh.M( tchIdx )->numv;
			assert( nTVertIdx < nTVerts );
		}	
		return nTVertIdx;
	}

	UVVert  PolyOrTriMesh::GetTVert( int tchIdx, int tVertIdx )
	{		
		if( NULL != m_pTri )
		{
			::Mesh & mesh = m_pTri->GetMesh();						
			MeshMap &tmap = mesh.Map( tchIdx );					
			assert( tVertIdx < tmap.getNumVerts() );
			UVVert uvVert = tmap.tv[ tVertIdx ];
			return uvVert;
		} else 
		{	assert( NULL != m_pPoly );	
		MNMesh &mnMesh = m_pPoly->GetMesh();
		assert( tVertIdx < ( mnMesh.M( tchIdx )->numv ) );
		UVVert uvVert = mnMesh.M( tchIdx )->V( tVertIdx );
		return uvVert;
		}
	}

	//Get the number of normals  used by the underlying computeNormals- class
	int  PolyOrTriMesh::GetNNormals( )
	{	
		if( !m_ComputeNormals )
		{
			m_ComputeNormals.reset ( ComputeNormalsBase::MakeComputeNormals( *this ) );
			DBG_ASSERT( ( NULL != m_ComputeNormals ), "should have a computeNormals" );
		}
		return m_ComputeNormals->GetNNormals();
	}

	//get the normal gven the face corresponding and the index of the vertex in the face
	//return the index of the normal computed
	int  PolyOrTriMesh::GetNormal( int i_nFaceIdx, int i_nVertIdxInFace, Point3 &o_Normal )
	{

		if( !m_ComputeNormals )
		{
			m_ComputeNormals.reset ( ComputeNormalsBase::MakeComputeNormals( *this ) );
			DBG_ASSERT( ( NULL != m_ComputeNormals ), "should have a computeNormals" );
		}
		return m_ComputeNormals->GetNormal( i_nFaceIdx, i_nVertIdxInFace, o_Normal );
	}

	//get i-th normal acording to the normal computation
	//This could be i'th user specified normal
	//or the ith RNNormal or the i-th face normal
	Point3 PolyOrTriMesh::GetIthNormal( int i_NormalIdx )
	{
		if( !m_ComputeNormals )
		{
			m_ComputeNormals.reset ( ComputeNormalsBase::MakeComputeNormals( *this ) );
			DBG_ASSERT( ( NULL != m_ComputeNormals ), "should have a computeNormals" );
		}
		return m_ComputeNormals->GetIthNormal( i_NormalIdx );
	}

} //namespace MaxExp