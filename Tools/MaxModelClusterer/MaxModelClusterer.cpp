
/*****************************************************************************
**  MaxModelClusterer.cpp
** 
**  Does the clustering of objects in a max scene
**  
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "MaxModelClusterer.hpp"
#include "KMeansCluster.hpp"

#include <vector>
#include <list>
#include "decomp.h"
#include "inode.h"
#include "triobj.h"
#include "meshadj.h"
#include "shape.h"
#include "MeshDelta.h"
#include <vector>
#include <ctime>
#include <list>
#include <hash_set>

using namespace std;
using namespace stdext;
#define TIME_EXPORT_START 0
#define NUM_CLUSTERS 5000
#define MAX_NUM_MAPS_SUPPORTED  100


//========================================================================
// A profile utility class,
// upo  destruction prints out the time elapsed since its construction
//========================================================================
class SimpleProfile
{
public:
	SimpleProfile( const std::string &name );
	~SimpleProfile();
	const std::string m_Name;
	clock_t m_Start;
	clock_t m_Stop;
};
SimpleProfile::SimpleProfile( const std::string &name ):m_Name(name), m_Start(0), m_Stop(0)
{
	m_Start = clock();	
}
SimpleProfile::~SimpleProfile()
{
	m_Stop = clock();
	LONG time_elapsed = m_Stop - m_Start;
	_RPT2( _CRT_WARN, "action '%s'  took '%d' millisecs", m_Name.c_str(), (m_Stop - m_Start));
}


//========================================================================
// Static definitions
//========================================================================

SgpuModelClustererClassDesc SgpuModelClustererClassDesc::theSgpuModelClustererClassDesc ;
SgpuModelClusterer SgpuModelClusterer::theSgpuModelClusterer;


//========================================================================
// Main plugin dialog callback as a member function
//========================================================================
INT_PTR CALLBACK SgpuModelClusterer::DlgProc(
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
			case IDOK:
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
// Main plugin dialog callback as a static function
//========================================================================
INT_PTR CALLBACK SgpuModelClusterer::DlgProcS(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	return SgpuModelClusterer::theSgpuModelClusterer.DlgProc( hWnd, message, wParam, lParam );
}


//========================================================================
// Creates the UI elements associated with the plugin
//========================================================================
void SgpuModelClusterer::BeginEditParams(Interface *i_pInterface, IUtil *i_pIUtil )
{
    m_pInterface = i_pInterface;
	m_pIUtil = i_pIUtil;
	m_hWnd = i_pInterface->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE( IDD_SGPU_MODELCLUSTERER ),
		DlgProcS,
		GetString( IDS_SGPU_MODELCLUSTERER ),
		0);
}

//========================================================================
// Destroys the UI elements associated with the plugin
//========================================================================
void SgpuModelClusterer::EndEditParams(Interface *ip,IUtil *iu)
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
// Creates and caches the tri-object associated with the node
//========================================================================
	TriObject *SgpuModelClusterer::GetTriObject ( INode *i_pNode )
	{
		TriObject *pRet = NULL;
		TNodeCache::const_iterator tit = m_NodeCache.find( i_pNode );
		if( tit != m_NodeCache.end() )
		{
			pRet = tit->second;
			return pRet;
		}
		ObjectState state = i_pNode->EvalWorldState( TIME_EXPORT_START );
		Object * pObject  = state.obj;
		Class_ID id = pObject->ClassID();
		SClass_ID sid = pObject->SuperClassID();
		if (sid == GEOMOBJECT_CLASS_ID && (id.PartA() == EDITTRIOBJ_CLASS_ID || id.PartA() == TRIOBJ_CLASS_ID))
		{
			pRet = (TriObject*) pObject;
			TNodeCache::value_type v( i_pNode, pRet );
			m_NodeCache.insert( v );
		}
		return pRet;
	}


//========================================================================
// Main function
//========================================================================
	bool SgpuModelClusterer::Do()
	{
		SimpleProfile("modelbreaker");
		INode *pRootNode = GetCOREInterface()->GetRootNode();
		//all objects, to be clustered  should be direct  children of the root node
		int nChildren = pRootNode->NumberOfChildren();
		vector< INode *> nodeVector;
		nodeVector.reserve( nChildren );
		//center of the candidate objects
		vector< Point3 > centroidsOfObjects;
		centroidsOfObjects.reserve( nChildren );
		Box3 box;
		box.Init();
		try
		{
		    
			
			//Find the bounding box of the scene
			//Also calculate the centroid of each object.
			for( int i=0; i < nChildren; ++i )
			{
				INode *pNode = pRootNode->GetChildNode( i );
				TriObject *pTriObj = GetTriObject( pNode );
				if( pTriObj == NULL )
				{
					continue;
				}
				nodeVector.push_back( pNode );
				::Mesh &mesh = pTriObj->mesh;
				int numVerts = mesh.numVerts;
				int numFaces = mesh.numFaces;
				Matrix3 nodeTM = pNode->GetNodeTM( TIME_EXPORT_START );
				Matrix3 pTM;
				pTM.IdentityMatrix();
				GetPivotTransform( pNode, pTM );
				Point3 posCenter(0.0f, 0.0f, 0.0f);
				if( numVerts > 0 )
				{
					for( int j=0; j < numVerts; ++ j)
					{
						Point3 pos =  mesh.verts[ j ];
						pos = pos * pTM * nodeTM;
						posCenter += pos;
						box.IncludePoints( &pos, 1 );
					}
					posCenter /= static_cast< float > ( numVerts );
				}
				centroidsOfObjects.push_back( posCenter );
			}

			
			//Do the clustering

			assert( nodeVector.size() == centroidsOfObjects.size() );
			//Construct the KMeansCluster object
			//input = centroids of the objects
			// and bounding box of the scene
			KMeansCluster< Point3, vector<Point3>::const_iterator, hash_set< int >  > kCluster( NUM_CLUSTERS,  centroidsOfObjects.begin(), centroidsOfObjects.end() , box.Max(), box.Min() );
			//output = a vector of hash_sets.
			//Each hash_set is a cluster/
			//And it contains the indices of objects in the cluster
			vector< shared_ptr< hash_set< int >  > > output( NUM_CLUSTERS );
			for (int j =0; j < NUM_CLUSTERS; ++j )
			{
				output[j].reset( new hash_set< int> () );
			}
			//Do the clustering
			kCluster.Do( output );
			




			//Form cluster meshes
			assert( output.size() == NUM_CLUSTERS );
			vector< shared_ptr< hash_set< int > > >::const_iterator lit;
			int k=0;
			//for each cluster
			for( lit = output.begin(); lit != output.end(); ++lit , ++k )
			{
				//get the objects associated with this cluster
				const shared_ptr< hash_set< int > > nodesInThisCluster = *lit;
				if( nodesInThisCluster->size() <= 0 )
				{
					//if the cluster is empty,
					//continue
					continue;
				}
				hash_set< int >::const_iterator hit;
				//number of vertices in this cluster mesh
				int nVertsInThisCuster =0;				
				//number of faces in this cluster mesh
				int nFacesInThisCluster = 0;				
				//number of maps in this cluster mesh
				int numMapsInThisCluster = -1;				
				//maximum number of maps = 100
				//whether i-th map is supported in the cluster mesh.
				//A map will be supported in the cluster mesh if it is supported
				//by all the original meshes of the nodes assigned to the  cluster
				BitArray clusterMapSupport( MAX_NUM_MAPS_SUPPORTED );
				//initially support all maps
				clusterMapSupport.SetAll();
				//number of TVerts in each map
				//intitialize that to 0
				vector< int > numClusterTVertsInEachMap( MAX_NUM_MAPS_SUPPORTED, 0 );
				//for each node in the cluster
				for( hit = nodesInThisCluster->begin(); hit != nodesInThisCluster->end(); ++hit )
				{
					int iNodeNumber = *hit;
					//get the node
					INode *pNode = nodeVector[ iNodeNumber];
					//get the triObject corresponding to the node
					TriObject *pTriObj = GetTriObject( pNode );
					::Mesh &nodeMesh = pTriObj->mesh;
					int nNodeMeshVerts = nodeMesh.numVerts;
					int nNodeMeshFaces = nodeMesh.numFaces;
					//update the number of verts and faces in the cluster
					nVertsInThisCuster += nNodeMeshVerts;
					nFacesInThisCluster  += nNodeMeshFaces;
					//number of maps in the current node
					int nNodeMeshNumMaps = nodeMesh.getNumMaps();
					if (  numMapsInThisCluster >= 0  )
					{
						//oif the number of maps in the current node
						//is not the same as the number of maps so far guessed
						//abort
						if( nNodeMeshNumMaps != numMapsInThisCluster )
						{
							throw std::runtime_error( std::string( "map support is different for meshes in cluster" ) );
						}
					} else
					{
						//set the number of maps in the cluster
						//from the nu,mber of maps in the first node
						numMapsInThisCluster = nNodeMeshNumMaps;
					}
					assert( nNodeMeshNumMaps < MAX_NUM_MAPS_SUPPORTED );
					int nNumMapVerts = 0;
					//get the maximum number of mapverts in all
					//the maps
					for (int mp=0; mp < nodeMesh.getNumMaps() && mp < 2; mp++ ) 
					{
						nNumMapVerts = ( nNumMapVerts <  nodeMesh.getNumMapVerts(mp) ) ?  nodeMesh.getNumMapVerts(mp) : nNumMapVerts;
					}
					//form a bit array
					BitArray nodeMeshTVerts( nNumMapVerts );
					//for each map, (we consier maps only upto 2)
					for (int mp=0; mp < nodeMesh.getNumMaps() && mp < 2; mp++ ) 
					{
						//if that map is not supported,
						//clear the map support for the cluster
						if ( !nodeMesh.mapSupport(mp) ) 
						{
							clusterMapSupport.Clear( mp );
							continue;
						}
						//find which of the TVerts is used
						nodeMeshTVerts.ClearAll();
						TVFace *mapf = nodeMesh.mapFaces(mp);
						UVVert *mapv = nodeMesh.mapVerts(mp);
						for( int j=0; j < nNodeMeshFaces; ++j )
						{
							DWORD *vv = mapf[j].t;
							nodeMeshTVerts.Set( vv[0] );
							nodeMeshTVerts.Set( vv[1] );
							nodeMeshTVerts.Set( vv[2] );
						}
						//number of TVerts for this map for the cluster
						numClusterTVertsInEachMap[ mp ] += (int)( nodeMeshTVerts.NumberSet() );
					}
				}
				for (int mp=0; mp <  numMapsInThisCluster;  mp++ ) 
				{
					numClusterTVertsInEachMap[ mp ]  = ( clusterMapSupport[ mp ] ) ?  numClusterTVertsInEachMap[ mp ]  : 0;
				}						
				//new cluster object mesh
				TriObject *pNewClusterObj = new  TriObject;
				::Mesh &newMesh = pNewClusterObj->mesh;			
				newMesh.setNumVerts( nVertsInThisCuster );
				newMesh.setNumFaces( nFacesInThisCluster  );
				newMesh.setNumMaps ( numMapsInThisCluster );
				for( int mp=0; mp < numMapsInThisCluster; ++mp )
				{
					//set true to all maps by default
					if( clusterMapSupport[ mp ]  )
					{
						newMesh.setMapSupport ( mp, TRUE);
						newMesh.setNumMapFaces( mp, nFacesInThisCluster );
						newMesh.setNumMapVerts( mp, numClusterTVertsInEachMap[ mp ] );
						
					} else
					{
						newMesh.setMapSupport ( mp, FALSE);
					}

				}
				int nextClusterVertIndex = 0;
				int nextClusterFaceIndex = 0;
				vector< int > nextClusterTVertIndex( MAX_NUM_MAPS_SUPPORTED, 0 );
				//for each node in the cluster
				for( hit = nodesInThisCluster->begin(); hit != nodesInThisCluster->end(); ++hit )
				{
					int iNodeNumber = *hit;
					INode *pNode = nodeVector[ iNodeNumber];
					TriObject *pTriObj = GetTriObject( pNode );
					Matrix3 nodeTM = pNode->GetNodeTM( TIME_EXPORT_START );
					Matrix3 pTM;
					pTM.IdentityMatrix();
					GetPivotTransform( pNode, pTM );
					::Mesh nodeMesh = pTriObj->mesh; 
					int nNodeMeshVerts = nodeMesh.numVerts;
					int nNodeMeshFaces = nodeMesh.numFaces;
					vector< int > vertLut( nNodeMeshVerts, -1 );
					vector< int > faceLut( nNodeMeshFaces, -1 );
					if( nNodeMeshVerts <= 0 )
					{
						continue;
					}
					//get all the mesh vertex
					for( int j=0; j < nNodeMeshVerts; ++ j)
					{
						Point3 pos =  nodeMesh.verts[ j ];
						pos = pos * pTM * nodeTM;
						vertLut[ j ] = nextClusterVertIndex;
						newMesh.verts[ nextClusterVertIndex++ ] = pos;						
					}
					//get all the msh faces
					for( int j=0; j < nNodeMeshFaces ; ++j )
					{
						Face &f = nodeMesh.faces[ j ];
						faceLut[ j ] = nextClusterFaceIndex;
						Face &newFace = newMesh.faces[ nextClusterFaceIndex++ ];
						newFace.v[0] = vertLut[ f.v[0] ];
						newFace.v[1] = vertLut[ f.v[1] ];
						newFace.v[2] = vertLut[ f.v[2] ];

					}
					//gat all the tverts
					for (int mp=0; mp <nodeMesh.getNumMaps() && mp < 2; mp++) 
					{
						if ( !nodeMesh.mapSupport(mp) ) 
						{
							continue;
						}
						TVFace *mapf = nodeMesh.mapFaces(mp);
						UVVert *mapv = nodeMesh.mapVerts(mp);
						TVFace *nmapf = newMesh.mapFaces(mp);
						vector< int > tvLut( nodeMesh.getNumMapVerts(mp), 0 );
						for (int j=0; j< nodeMesh.getNumMapVerts(mp); ++j ) {
							newMesh.setMapVert (mp, nextClusterTVertIndex[mp], mapv[j]);
							tvLut[j] = nextClusterTVertIndex[mp]++;
						}
						for (int l=0; l< nNodeMeshFaces; ++l ) {
							TVFace & f = mapf[ l ];
							nmapf[ faceLut[ l ] ] = f;
							for (int k=0; k<3; k++) nmapf[ faceLut[l] ].t[k] = tvLut[ f.t[k] ];
						}
					}
				}
				//set the name of the new object to begin with "Cluster"
				INode *newNode = GetCOREInterface()->CreateObjectNode( pNewClusterObj );
				TSTR uname = "Cluster";
				GetCOREInterface()->MakeNameUnique(uname);
				newNode->SetName(uname);
				hit = nodesInThisCluster->begin();
				int iNodeNumber = *hit;
				INode *pNode = nodeVector[ iNodeNumber ];
				newNode->CopyProperties (pNode);
				newNode->FlagForeground ( TIME_EXPORT_START, FALSE);
				newNode->SetMtl (pNode->GetMtl());
			}
		} catch (... )
		{
			return false;
		}

		return true;
	}

