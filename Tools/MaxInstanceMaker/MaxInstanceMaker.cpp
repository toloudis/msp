/*****************************************************************************
**  MaxInstanceMaker.cpp
**  The main plugin file
**
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "MaxInstanceMaker.hpp"
#include <vector>
#include <list>
#include "decomp.h"


//========================================================================
//	defintions of static variables used by the plugin
//	
//========================================================================

SgpuInstanceMakerClassDesc SgpuInstanceMakerClassDesc::theSgpuInstanceMakerClassDesc ;
SgpuInstanceMaker SgpuInstanceMaker::theSgpuInstanceMaker;
//========================================================================
//	Main dialog callback as a member function
//	
//========================================================================

INT_PTR CALLBACK SgpuInstanceMaker::DlgProc(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_INITDIALOG:
			EnableWindow(GetDlgItem(hWnd,IDOK),TRUE);
			break;

		case WM_DESTROY:	
			//EndDialog( hWnd, 0);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam)) 
			{
			case IDC_OK:
				Do();
				break;
			default:
				break;							
			}
			break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
			m_pInterface->RollupMouseMessage(hWnd,msg,wParam,lParam); 
			break;

		default:
			return FALSE;
	}
	return TRUE;
}	

//========================================================================
//	Main dialog callback as a static function
//	
//========================================================================

INT_PTR CALLBACK SgpuInstanceMaker::DlgProcS(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	return SgpuInstanceMaker::theSgpuInstanceMaker.DlgProc( hWnd, message, wParam, lParam );
}

//========================================================================
//	Adding the UI elements when the plugin is invoked.
//	
//========================================================================

void SgpuInstanceMaker::BeginEditParams(Interface *i_pInterface, IUtil *i_pIUtil )
{
	m_pInterface = i_pInterface;
	m_pIUtil = i_pIUtil;
	m_hWnd = i_pInterface->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE( IDD_DIALOGBAR ),
		DlgProcS,
		GetString( IDS_SGPU_INSTANCEMAKER ),
		0);
}
//========================================================================
//	Closing the UI elements when the plugin is invoked
//	
//========================================================================

void SgpuInstanceMaker::EndEditParams(Interface *ip,IUtil *iu)
{
	m_pIUtil = NULL;
	m_pInterface = NULL;
	ip->DeleteRollupPage( m_hWnd );
}

//========================================================================
// Get the pivot transform corresponding to a max node
//========================================================================
void GetPivotTransform( INode *i_pCurNode, Matrix3 &o_Tm )
{
	Point3 opos = i_pCurNode->GetObjOffsetPos();
	Quat orot = i_pCurNode->GetObjOffsetRot();
	ScaleValue oscale = i_pCurNode->GetObjOffsetScale();

	ApplyScaling(o_Tm, oscale);
	RotateMatrix(o_Tm, orot);
	o_Tm.Translate(opos);

	o_Tm.ValidateFlags();
}
//========================================================================
//	An interactive version of the plugin
//	
// Given two selected objects,
// guess the transformation that transforms the second object into the first object
// THis doesnt do much other than we can do dbugging.
//========================================================================

bool SgpuInstanceMaker::Do()
{
	int nSelectedNodes = GetCOREInterface()->GetSelNodeCount();
	if( nSelectedNodes >= 2 )
	{
		INode *pNode1 = GetCOREInterface()->GetSelNode(0);
		INode *pNode2 = GetCOREInterface()->GetSelNode(1); 	
		Matrix3 ret;
		float error;
		Matrix3 *pret= NULL;
		bool bRet = InstanceResolve( pNode1, pNode2, pret, error );
		if( pret )
		{
			ret = *pret;
		}
		return bRet;
	}
	return false;
}

//========================================================================
// does the triange objects i_pTriObj1 and i_pTriObj2 has the same geometry and topology
//========================================================================

bool SgpuInstanceMaker::IsConsistentGeomAndTop( TriObject *i_pTriObj1, TriObject *i_pTriObj2 )
{
	::Mesh & mesh1 = i_pTriObj1->GetMesh();
	int nVert1 = mesh1.getNumVerts();
	::Mesh & mesh2 = i_pTriObj2->GetMesh();
	int nVert2 = mesh2.getNumVerts();		
	int nFaces1 = mesh1.getNumFaces();
	int nFaces2 = mesh2.getNumFaces();
	if( nVert1 != nVert2  || nFaces1 != nFaces2 )
	{
		return false;
	}
	bool sameTopology = true;
	for( int i=0; i < nFaces1 && sameTopology ; ++i )
	{
		Face &face1 = mesh1.faces[ i ];
		Face &face2 = mesh2.faces[ i ];
		for( int j=0; j <3 && sameTopology ; ++j )
		{
			sameTopology = (face1.v[j] == face2.v[j] );
		}
	}
	if( !sameTopology )
	{
		return false;
	}
	return true;
}

//========================================================================
// is the submaterial assignmment for the triangle objects i_pTriObj1, and i_pTriObj2 are the same.
//========================================================================
bool SgpuInstanceMaker::IsConsistentMtl( TriObject *i_pTriObj1, TriObject *i_pTriObj2, Mtl *i_pMtl1, Mtl * i_pMtl2  )
{
	if( i_pMtl1 != i_pMtl2 )
		return false;
	::Mesh & mesh1 = i_pTriObj1->GetMesh();
	int nVert1 = mesh1.getNumVerts();
	::Mesh & mesh2 = i_pTriObj2->GetMesh();
	int nVert2 = mesh2.getNumVerts();		
	int nFaces1 = mesh1.getNumFaces();
	int nFaces2 = mesh2.getNumFaces();
	typedef std::list< int > TFaceList;
	typedef std::map< int, TFaceList > TMtlFaceMap;
	TMtlFaceMap mtlFaceMap1, mtlFaceMap2;
	for( int i=0; i < nFaces1 ; ++i)
	{	
		Face &f = mesh1.faces[i];
		int mid = f.getMatID();
		if (mtlFaceMap1.find( mid ) == mtlFaceMap1.end() )
		{   
			TFaceList faceList_;
			mtlFaceMap1.insert( std::pair<int, TFaceList>(mid, faceList_) ); 
		}
		TMtlFaceMap::iterator fit = mtlFaceMap1.find( mid );
		TFaceList &faceList = fit->second;
		faceList.push_back( i );
	}

	for( int i=0; i < nFaces2 ; ++i)
	{	
		Face &f = mesh2.faces[i];
		int mid = f.getMatID();
		if (mtlFaceMap2.find( mid ) == mtlFaceMap2.end() )
		{   
			TFaceList faceList_;
			mtlFaceMap2.insert( std::pair<int, TFaceList>(mid, faceList_) ); 
		}
		TMtlFaceMap::iterator fit = mtlFaceMap2.find( mid );
		TFaceList &faceList = fit->second;
		faceList.push_back( i );
	}
	if(  mtlFaceMap1 == mtlFaceMap2 )
	{
		return true;
	} else
	{
		return false;
	}
}

//========================================================================
// guess the matrix that transforms the vertices of i_pNode2 to that of i_pNode1
//========================================================================

bool SgpuInstanceMaker::InstanceResolve( INode *i_pNode1, INode *i_pNode2, Matrix3 *&pret, float &error)
{
	const MCHAR * szNodeName1 = i_pNode1->GetName();
	const MCHAR * szNodeName2 = i_pNode2->GetName();
	//Get the world transform of the nodes
	Matrix3 mat1 = i_pNode1->GetNodeTM( TIME_EXPORT_START );
	Matrix3 mat2 = i_pNode2->GetNodeTM( TIME_EXPORT_START );
	//evaluate the nodes at the initial time
	ObjectState state_1 = i_pNode1->EvalWorldState( TIME_EXPORT_START );
	ObjectState state_2 = i_pNode2->EvalWorldState( TIME_EXPORT_START );

	Object * pObject_1  = state_1.obj;
	Object * pObject_2 = state_2.obj;

	//convrt the geomtry to a triangule object
	TriObject *pTriObj_1 = (TriObject*) pObject_1->ConvertToType(TIME_EXPORT_START, Class_ID(TRIOBJ_CLASS_ID, 0));
	TriObject *pTriObj_2 = (TriObject*) pObject_2->ConvertToType(TIME_EXPORT_START, Class_ID(TRIOBJ_CLASS_ID, 0));
	//If cant convert any of the two
	//return false
	if( pTriObj_1 == NULL || pTriObj_2 == NULL)
	{
		return false;
	}
	//if the objects cant be instanced return false
	bool bConsistent = IsConsistentGeomAndTop( pTriObj_1, pTriObj_2 );
	Mtl *pMtl1 = i_pNode1->GetMtl();
	Mtl *pMtl2 = i_pNode2->GetMtl();
	bConsistent = bConsistent && IsConsistentMtl(  pTriObj_1, pTriObj_2, pMtl1, pMtl2 );
	if( !bConsistent )
	{
		return false;
	}


	//objects can be instanced
	//proceed
	::Mesh & mesh1 = pTriObj_1->GetMesh();
	int nVert1 = mesh1.getNumVerts();
	::Mesh & mesh2 = pTriObj_2->GetMesh();
	int nVert2 = mesh2.getNumVerts();	
	Matrix3 mat2Inv = Inverse(mat2 );
	Matrix3 ptm1;
	ptm1.IdentityMatrix();
	Matrix3 ptm2 ;
	ptm2.IdentityMatrix();
	GetPivotTransform( i_pNode1, ptm1 );
	GetPivotTransform( i_pNode2, ptm2 );
	std::vector< float > meshVerts1( 4 * nVert1 );
	std::vector< float > meshVerts2( 4 * nVert2 );
	std::vector< float > xform ( 16 );
	//marshall  the vertex info for the first object
	for( int i=0; i < nVert1; ++i )
	{
		Point3 pos =  mesh1.verts[ i ];
		//compute effective world transform, transformed by the world transform of the
		//second object
		pos = pos * ptm1 * mat1 * mat2Inv;
		meshVerts1[4*i] = pos.x;
		meshVerts1[4*i + 1] = pos.y;
		meshVerts1[4*i +2 ] = pos.z;
		meshVerts1[4*i + 3] = 1.0f;
	}
	//marshall the vertex info for the second object
	for( int i=0; i < nVert1; ++i )
	{
		Point3 pos =  mesh2.verts[ i ];
		pos = pos * ptm2;
		meshVerts2[4*i] = pos.x;
		meshVerts2[4*i + 1] = pos.y;
		meshVerts2[4*i + 2 ] = pos.z;
		meshVerts2[4*i + 3] = 1.0f;
	}

	//was this node-pair processed before
	//if so just retreive te previous result
	INodePair p( i_pNode1, i_pNode2 );
	TCache::iterator cit = m_Cache.find( p );
	if( cit == m_Cache.end() )
	{
		//If this is th first time,
		//do the core computation
		double maxRelError = m_InstanceWork.Do( nVert1,
			meshVerts1,
			meshVerts2,
			xform
			);		
		//marshall back the result transform
		Matrix3 ret;
		for( int i=0; i < 4; ++i )
		{
			Point3 p;
			for( int j=0; j < 3; ++j )
			{
				p[j] = xform[ i*4+ j ];
			}
			float f = xform[i*4 +  3 ];
			ret.SetRow( i, p );
		}			
		error = maxRelError;
		shared_ptr<Matrix3> shMat( new  Matrix3(ret.GetAddr()) );
		TData data( shMat, maxRelError );
		TCache::value_type v( p, data );
		//for debugging purposes
		//check the affine components of the result matrix
		AffineParts comps;		// Requires header decomp.h
		decomp_affine(ret, &comps);
		//cache the result
		m_Cache.insert( v );
		pret = shMat.get();
	} else
	{
		TCache::value_type &v = *cit;
		INodePair p = v.first;
		TData &data = v.second;
		shared_ptr<Matrix3> shMat = data.first;
		pret = shMat.get();
		error = data.second;
	}
	return true;
}

//========================================================================
// Does the geomtry, topology and material assignments of each of the two objects
// allow us to represent the first one as an instance of the second
//========================================================================	
bool SgpuInstanceMaker::CanExpressAsInstances( INode *i_pNode1, INode *i_pNode2 )
{
	Matrix3 ret;
	float error;
	Matrix3 *pret = NULL;
	const TCHAR *pzName1= i_pNode1->GetName();
	const TCHAR *pzName2= i_pNode2->GetName();
	bool bVal = InstanceResolve( i_pNode1, i_pNode2, pret, error );
	if( pret )
	{
		ret = *pret;
	}
	return bVal;
}
//========================================================================	
// get the transformation that transforms the second object
//to the first object
//========================================================================

Matrix3 SgpuInstanceMaker::GetTransform( INode *i_pNode1, INode *i_pNode2 )
{
	Matrix3 ret;
	ret.IdentityMatrix();
	float error;
	Matrix3 *pret= NULL;
	const TCHAR *pszName1 = i_pNode1->GetName();
	const TCHAR *pszName2 = i_pNode2->GetName();
	bool bCanResolve =  InstanceResolve( i_pNode1, i_pNode2, pret, error );
	if( pret )
	{
		ret = *pret;
	}
	return ret;
}


//========================================================================
//get the maximum error of any component difference from the first
//vertex to the transformed second vertex
//========================================================================
float SgpuInstanceMaker::GetError( INode *i_pNode1, INode *i_pNode2 )
{
	Matrix3 ret;
	ret.IdentityMatrix();
	float error;
	Matrix3 *pret =NULL;
	bool bCanResolve =  InstanceResolve( i_pNode1, i_pNode2, pret, error );
	return error;
}