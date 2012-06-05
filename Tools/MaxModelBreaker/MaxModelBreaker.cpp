#include "MaxModelBreaker.h"
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
#include <string>
#include <fstream>
#include <sstream>

using namespace std;
#define TIME_EXPORT_START 0
class SimpleProfile
	{
	public:
		SimpleProfile(const std::string &slogFile,  const std::string &name );
		~SimpleProfile();
		const std::string m_Name;
		const std::string m_LogFilename;
		clock_t m_Start;
		clock_t m_Stop;
	};
	SimpleProfile::SimpleProfile( const std::string &slogFile, const std::string &name ):
		m_Name(name), 
			m_Start(0), 
			m_Stop(0),
			m_LogFilename( slogFile )
	{
		m_Start = clock();	
	}
	SimpleProfile::~SimpleProfile()
	{
		m_Stop = clock();
		LONG time_elapsed = m_Stop - m_Start;
		ofstream fs( m_LogFilename.c_str(), std::ios_base::app );
		fs << "action '" << m_Name.c_str() << "' took '" << time_elapsed << "' millisecs" << endl;
		fs.close();
	}
template< class Str >
	Str ChangeExtension( const  Str & i_FileName, const Str &i_NewExt , int i);
	template<>
	std::string ChangeExtension< std::string > ( const  std::string & i_FileName, const std::string  &i_NewExt , int i);
	template<>
	std::wstring ChangeExtension< std::wstring > ( const  std::wstring & i_FileName, const std::wstring  &i_NewExt, int i );
//========================================================================
	//return a copy of the input file name  with the new extension
	//========================================================================
	template<>
	string ChangeExtension< string >( const  string & i_FileName, const string &i_NewExt , int i )
	{

		char	newPath[ _MAX_DIR ];
		char	drive[ _MAX_DRIVE ];
		char	dir[ _MAX_DIR ];
		char	fileName[ _MAX_FNAME ];
		char	ext[ _MAX_EXT ];
		char subscript[ 1024];
		sprintf_s(subscript, "_%d", i);
		strcpy_s( ext, i_NewExt.c_str() );
		_splitpath_s(i_FileName.c_str(), drive, dir, fileName, ext);
		strcat_s( fileName, subscript ); 
		_makepath_s(newPath, drive, dir, fileName, i_NewExt.c_str());
		return string(newPath);
	}

SgpuModelBreakerClassDesc SgpuModelBreakerClassDesc::theSgpuModelBreakerClassDesc ;
SgpuModelBreaker SgpuModelBreaker::theSgpuModelBreaker;

INT_PTR CALLBACK SgpuModelBreaker::DlgProc(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		case WM_INITDIALOG:
			EnableWindow(GetDlgItem(hWnd,IDC_OK),TRUE);
			break;

		case WM_DESTROY:	
			EndDialog( hWnd, 0);
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


INT_PTR CALLBACK SgpuModelBreaker::DlgProcS(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	return SgpuModelBreaker::theSgpuModelBreaker.DlgProc( hWnd, message, wParam, lParam );
}

void SgpuModelBreaker::BeginEditParams(Interface *i_pInterface, IUtil *i_pIUtil )
{
    m_pInterface = i_pInterface;
	m_pIUtil = i_pIUtil;
	m_hWnd = i_pInterface->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE( IDD_SGPU_MODELBREAKER ),
		DlgProcS,
		GetString( IDS_SGPU_MODELBREAKER ),
		0);
}

void SgpuModelBreaker::EndEditParams(Interface *ip,IUtil *iu)
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

	bool SgpuModelBreaker::Do()
	{	
	
		const   MSTR tstrInputFileName = GetCOREInterface()->GetCurFilePath();	
		string logFilename = ChangeExtension( string( tstrInputFileName.data()) , string(".txt"), 0 );
		int nSelectedNodes = GetCOREInterface()->GetSelNodeCount();
		if( nSelectedNodes < 1 )
		{
			return false;
		}
		INode *pNode = GetCOREInterface()->GetSelNode(0);
		Matrix3 mat1 = pNode->GetNodeTM( TIME_EXPORT_START );
		ObjectState state = pNode->EvalWorldState( TIME_EXPORT_START );
		Object * pObject  = state.obj;
		Class_ID id = pObject->ClassID();
		SClass_ID sid = pObject->SuperClassID();
		TriObject *pTriObj = NULL;
		if (sid == GEOMOBJECT_CLASS_ID && (id.PartA() == EDITTRIOBJ_CLASS_ID || id.PartA() == TRIOBJ_CLASS_ID))
		{
			pTriObj = (TriObject*) pObject;
		} else
		{
			return false;
		}
		::Mesh &mesh = pTriObj->mesh;
	

		Box3 box = mesh.getBoundingBox();
		Point3 boxmax = box.Max();
		Point3 boxmin = box.Min();
		Point3 boxdiff = boxmax - boxmin ;
		Point3 partition = boxdiff /10;

				int numOrigVerts = mesh.numVerts;
				int numOrigFaces = mesh.numFaces;
				BitArray facesInElement( numOrigFaces );
				BitArray totalFaces( numOrigFaces );
				BitArray iso( numOrigVerts );
				BitArray usedVerts( numOrigVerts );

				DWORD *vlut = new DWORD[ numOrigVerts ];
				DWORD  *flut = new DWORD[ numOrigFaces ];
		vector< int> vElementFaces;
		vElementFaces.reserve(1000 );
		try 
		{
			int iiteration = 0;
			while( mesh.numFaces > 0 )
			{	
				stringstream ss;
				INode *pRootNode = GetCOREInterface()->GetRootNode();
				int nChildren = pRootNode->NumberOfChildren();
				ss << "modelbreaker " << iiteration << " with " << nChildren << " objects" << endl;
				SimpleProfile sp (logFilename, ss.str());
	
				AdjEdgeList ae(mesh);			
				AdjFaceList af(mesh, ae);
				int numVerts = mesh.numVerts;
				int numFaces = mesh.numFaces;
				facesInElement.ClearAll();
				iso.ClearAll();
				usedVerts.ClearAll();
				totalFaces.ClearAll();


				int lastInt =0;
				for( int i=0; i < 3000 ; ++ i)
				{
					int j, k, l;
					facesInElement.ClearAll();
					for (j=lastInt; j < numFaces; ++j )
					{
						if ( !totalFaces[j] ) break;
					}
					if( j == numFaces )
					{
						break;
					} 		   
					mesh.ElementFromFace( j, facesInElement, &af );
					totalFaces |= facesInElement;
					int nFacesDel = totalFaces.NumberSet();
					TriObject *pNewObj = new  TriObject;
					::Mesh &newMesh = pNewObj->mesh;
					iso.ClearAll();
					mesh.FindVertsUsedOnlyByFaces (facesInElement, iso);
					int nFacesInNewMesh = (int) ( facesInElement.NumberSet() );

					int nVertsInNewMesh = (int) ( iso.NumberSet() );
					newMesh.setNumFaces( nFacesInNewMesh );
					newMesh.setNumVerts( nVertsInNewMesh );
					memset( vlut, 0, sizeof( DWORD) * numVerts );
					for (j=0,k=0; j < numVerts; ++j) {
						if (!iso[j]) continue;
						newMesh.verts[k] = mesh.verts[j];
						vlut[j] = k++;
					}
					vElementFaces.resize( nFacesInNewMesh );
					for(j=0, k=0; j < numFaces; ++j) {
						if( !facesInElement[j] ) continue;
						vElementFaces[ k++ ] = j;
					}
					memset( flut, 0, sizeof(DWORD)  * numFaces );
					for(l=0, k=0; l < vElementFaces.size(); ++l) {
						j = vElementFaces[ l ];
						assert ( facesInElement [j ] );
						Face &f = mesh.faces[ j ];
						newMesh.faces[ k ] = f;
						Face &newFace = newMesh.faces[k ];
						newFace.v[0] = vlut[ f.v[0] ];
						newFace.v[1] = vlut[ f.v[1] ];
						newFace.v[2] = vlut[ f.v[2] ];
						flut[ j ] = k++;
					}

					newMesh.setNumMaps (mesh.getNumMaps ());
					int mp;
					int nmaps = mesh.getNumMaps ();
					for ( mp=0; mp <mesh.getNumMaps() && mp < 2; mp++) 
					{
						if ( !mesh.mapSupport(mp) ) 
						{
							newMesh.setMapSupport (mp, FALSE);
							continue;
						}
						newMesh.setMapSupport (mp, TRUE);
						TVFace *mapf = mesh.mapFaces(mp);
						UVVert *mapv = mesh.mapVerts(mp);
						TVFace *nmapf = newMesh.mapFaces(mp);
						usedVerts.ClearAll ();
						int nmapVerts = mesh.getNumMapVerts(mp);
						for (l=0; l< vElementFaces.size() ; ++l ) 
						{
							j = vElementFaces[ l ];
							assert ( facesInElement [j ] );
							DWORD *vv = mapf[j].t;
							usedVerts.Set (vv[0]);
							usedVerts.Set (vv[1]);
							usedVerts.Set (vv[2]);
						}

						memset( vlut, 0, sizeof( DWORD) * numVerts );
						newMesh.setNumMapVerts (mp, usedVerts.NumberSet());
						for (j=0,k=0; j<mesh.getNumMapVerts(mp); ++j ) {
							if (!usedVerts[j]) continue;
							newMesh.setMapVert (mp, k, mapv[j]);
							vlut[j] = k++;
						}
						for (l=0; l< vElementFaces.size(); ++l ) {
							j = vElementFaces[ l ];
							assert ( facesInElement [j ] );
							TVFace & f = mapf[j];
							nmapf[ flut[j] ] = f;
							for (k=0; k<3; k++) nmapf[ flut[j] ].t[k] = vlut[f.t[k]];
						}
					}
					INode *newNode = GetCOREInterface()->CreateObjectNode( pNewObj );
					Matrix3 ntm = pNode->GetNodeTM(TIME_EXPORT_START  );
					TSTR uname = pNode->GetName();
					GetCOREInterface()->MakeNameUnique(uname);
					newNode->SetName(uname);
					newNode->CopyProperties (pNode);
					newNode->SetNodeTM ( TIME_EXPORT_START, ntm);
					newNode->FlagForeground ( TIME_EXPORT_START, FALSE);
					newNode->SetMtl (pNode->GetMtl());
					Point3 p = pNode->GetObjOffsetPos();
					Quat q = pNode->GetObjOffsetRot();
					ScaleValue s = pNode->GetObjOffsetScale();
					newNode->SetObjOffsetPos ( p );
					newNode->SetObjOffsetRot ( q );
					newNode->SetObjOffsetScale ( s );
				}

				iso.ClearAll();

				mesh.DeleteFaceSet ( totalFaces, &iso );
				mesh.DeleteIsoMapVerts();
				mesh.DeleteVertSet (iso );
		
				int nFaces2 = mesh.getNumFaces();
				int nVerts2 = mesh.getNumVerts();
				++iiteration;
				string saveFileName = ChangeExtension( string( tstrInputFileName.data()) , string(".max"), iiteration );
				int ret = GetCOREInterface()->SaveToFile (  saveFileName.data() );
				if ( ret <= 0)
				{
					MessageBox( NULL, (LPCSTR)"File Save Failed", (LPCSTR)"SgpuModelBreaker", MB_OK );
				}
				//GetCOREInterface()->FileSave();
			}

		} catch ( ... )
		{
			
			return false;
		}

				delete [] vlut;
				delete [] flut;
		return true;
	}
