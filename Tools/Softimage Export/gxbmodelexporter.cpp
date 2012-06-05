//*****************************************************************************
/*!	\file exportmesh_command.cpp
\brief Implementation of the MSPExportMesh command. MSPExportMesh uses 
CGeoemtryAccessor to extract data from the selected geometry, and exports 
the extracted data to a text file.
*/
//*****************************************************************************





#include <TCHAR.H>
#include <stdlib.h>
#include <vector>
#include <map>
#include <deque>
#include <sstream>


#include "gxbmodelexporter.h"
#include "exportmesh_command.h"
#include <xsi_polygonmesh.h>
#include <xsi_nurbssurface.h>
#include <xsi_nurbssurfacemesh.h>
#include <xsi_transformation.h>
#include <xsi_application.h>
#include <xsi_context.h>
#include <xsi_pluginregistrar.h>
#include <xsi_status.h>
#include <xsi_string.h>
#include <xsi_argument.h>
#include <xsi_command.h>
#include <xsi_menu.h>
#include <xsi_model.h>
#include <xsi_parameter.h>
#include <xsi_x3dobject.h>
#include <xsi_selection.h>
#include <xsi_primitive.h>
#include <xsi_polygonmesh.h>
#include <xsi_nurbssurfacemesh.h>
#include <xsi_geometryaccessor.h>
#include <xsi_longarray.h>
#include <xsi_floatarray.h>
#include <xsi_doublearray.h>
#include <xsi_bitarray.h>
#include <xsi_envelopeweight.h>
#include <xsi_customproperty.h>
#include <xsi_griddata.h>
#include <xsi_clusterproperty.h>
#include <xsi_material.h>
#include <xsi_ppglayout.h>
#include <xsi_userdatamap.h>
#include <xsi_imageclip2.h>
#include <xsi_source.h>
#include <xsi_texture.h>
#include <xsi_math.h>
#include <xsi_uitoolkit.h>

#include <xsi_vector3f.h>
#include <xsi_vector2f.h>
#include <xsi_oglmaterial.h>
#include <xsi_ogltexture.h>
#include <xsi_kinematics.h>
#include <xsi_triangle.h>
#include <xsi_trianglevertex.h>
#include <xsi_scene.h>
#include <xsi_project.h>
#include "helper.h"
#include "Log.h"
#include "sgpuMatrix.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuMesh.hpp"
#include "sgpuMeshConstructor.hpp"
#include "sgpuSubdiv.hpp"
#include "sgpuSubdivConstructor.hpp"
#include "sgpuNode.hpp"
#include "sgpuPathReference.hpp"

#include <winbase.h> 
#include "Core/it/itString.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/fs/fsFileUtil.hpp"

using namespace std;

extern XSI::CustomProperty GetImportExportProp();
extern XSI::Parameter GetImportExportOption( const XSI::CString& in_strName );


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GXBModelExporter::GXBModelExporter( GXBExportDoc &doc, bool i_bVertexAnimation ):
GXBExporter(doc),
m_bVertexAnimation( i_bVertexAnimation ),
m_meshId(0)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GXBModelExporter::~GXBModelExporter()
{
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
XSI::CRefArray FindSourcesUnderModel( XSI::Model& in_model )
{
	XSI::Application app;
	XSI::CRefArray foundsrcs = in_model.GetSources();

	// Loop through the collection of sources found under this model to print the
	// name and add its name to the result string
	if ( foundsrcs.GetCount() > LONG(0) ) {
		for ( LONG i=0; i<foundsrcs.GetCount(); ++i ) {
			XSI::Source src( foundsrcs[i] );
			XSI::CString msg = src.GetFullName() + L" is a " + src.GetClassIDName();
			const TCHAR *pzname = _tstr( msg );
		}
	} else {
		app.LogMessage( L"No sources found on " + in_model.GetFullName() );
	}

	return foundsrcs;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GXBModelExporter::DoExport()
{
	
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	_tsplitpath( _tstr(m_exportDoc.m_filename), drive, dir, fname, ext );
	XSI::Application app;
	bool r= false;
	m_selection = ( app.GetSelection().GetArray() );
	if( m_selection.GetCount() <= 0)
	{
		m_selection.Add( app.GetActiveSceneRoot() );
	}

	XSI::CString output_path = XSI::CString(drive) + XSI::CString(dir);

	sgpuNode root_node = m_scene.GetRootNode();

	root_node.SetNodeName(_T("RootNode"));


	int numMeshes = 0;
	for( int i=0; i < m_selection.GetCount(); ++i )
	{
		XSI::CRef ref = m_selection[i];
		XSI::X3DObject xroot( ref );
		enumMeshes( xroot, m_objectsTobeExported );
	}
	numMeshes = static_cast< int > ( m_objectsTobeExported.GetCount() );

	m_numNodes = 0;
	m_numMeshes = 0;

	//m_objectsTobeExported contains a large collection of objects,
	//which include in addition to the selected objects,
	// eligible children of selected objects.
	//The objects in m_objectsTobeExported are always ordered,
	//in such  away that an intance refering to a master will only occur
	//after the  master. Also, the parents occur before their children.
	//Let us re-order the m_selection to reflect the order in m_objectsTobeExported.
	XSI::CRefArray orderedSelection;
	TObjectsTobeExported::AddToArrayBasedOnSortIndex( m_selection, orderedSelection, m_objectsTobeExported );
	//export the selection in the correct order
	for( int i=0; i < orderedSelection.GetCount(); ++i )
	{
		XSI::CRef xroot_ref = orderedSelection[i];
		TransformStack tmStack( m_exportDoc.m_exportGeomForVertexAnim, 0 );
		XSI::X3DObject xroot( xroot_ref );
		r = exportObject( xroot, root_node, tmStack );
	}



	if( m_bVertexAnimation )
	{
		m_scene.FlattenScene();
	} else if( m_exportDoc.m_mergeBasedOnMtls )
	{
		sgpuString emptyPrefix;
		int numMeshesMerged;
		int numMergedResults;
		m_scene.MergeByMaterials( emptyPrefix, numMeshesMerged, numMergedResults );
	}
	m_exportDoc.UpdateCaption( _T("Saving...") );

	m_scene.WriteScene(_tstr(m_exportDoc.m_filename), _T("Softimage XSI export plug-in"));
	CopyTextures();
	return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBModelExporter::enumProps( const XSI::X3DObject & xobj )
{


	XSI::CRefArray props = xobj.GetProperties();
	LONG nProps = props.GetCount();
	for( LONG iProp =0; iProp < nProps; ++iProp )
	{
		XSI::CRef prop = props.GetItem( iProp );
		XSI::CString cprop = prop.GetAsText();
		const char *pzProp = cprop.GetAsciiString( );
		int n=0;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  GXBModelExporter::enumMeshes( XSI::X3DObject & xobj, TObjectsTobeExported &tobeExported )
{
	if (!xobj.IsValid()) return false;
	
	//If it is already listed as to be exported,
	//dont proceed
	if( tobeExported.IsPresent( xobj ) )
	{
		return true;
	}
	
	XSI::Property propVis(xobj.GetProperties().GetItem(L"Visibility"));
	bool visible = propVis.GetParameter(L"viewvis").GetValue();

	XSI::CString name = xobj.GetName();
	XSI::siClassID cid =  xobj.GetClassID();
	const TCHAR *pzName = _tstr( name );
	XSI::Application app;


	XSI::Primitive prim = xobj.GetActivePrimitive();
	XSI::PolygonMesh mesh = prim.GetGeometry();
	XSI::NurbsSurfaceMesh xnurbsmesh = prim.GetGeometry();
	bool bIsNodeAnimated = xobj.IsNodeAnimated();
	bool bIsAnimated = xobj.IsAnimated();
	bool bThisNeedbeExported = false;
	//If it is visible and has exportable geometry
	//hen this object is to be exported
	if (visible &&  HasExportableGeometry( xobj ) )
	{
		bThisNeedbeExported = true;
	}
	//If this is an instance of a master,
	//then include the master as to be exported.
	XSI::X3DObject masterObj;
	bool bThisNeedbeExportedAsInstance = GetMasterInstance(xobj, masterObj );
	if( bThisNeedbeExportedAsInstance )
	{
		enumMeshes( masterObj, tobeExported );
	}

	//If any of the children are exported
	//then also this 3d object is supposed to be exported.
	XSI::CRefArray childArray = xobj.GetChildren();
	bool bAnyChildrenExported = false;
	TObjectsTobeExported childrenTobeExported;
	for (LONG i =0; i<childArray.GetCount(); i++)
	{
		XSI::X3DObject obj = childArray[i];
		bAnyChildrenExported |= enumMeshes(obj, childrenTobeExported );
	}
	if(bThisNeedbeExported ||  bAnyChildrenExported || bThisNeedbeExportedAsInstance )
	{
		tobeExported.PushBack( xobj );
		tobeExported.Append( childrenTobeExported );
	}
	
	return bThisNeedbeExported ||  bAnyChildrenExported || bThisNeedbeExportedAsInstance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  GXBModelExporter::exportObject(XSI::X3DObject & xobj, sgpuNode & parent_node, TransformStack & tmStack )
{
	Log::Indent indent( this->m_exportDoc.m_log );


	if (!xobj.IsValid()) return false;


	XSI::CString obj_name = xobj.GetUniqueName();
	const TCHAR * tObjectName = _tstr(obj_name);
	const TCHAR * pzClassName  = _tstr( xobj.GetClassIDName() );

	XSI::Property propVis(xobj.GetProperties().GetItem(L"Visibility"));
	bool visible = propVis.GetParameter(L"viewvis").GetValue();
	if( m_objectsTobeExported.IsPresent( xobj ) )
	{
		++m_meshId;

		tmStack.PushStackWithCurrentObjectTm( xobj );


		sgpuNode node = parent_node.AddChildNode();

		m_numNodes++;

		node.SetNodeName(_tstr(obj_name));    
		
		tmStack.ApplyTmToSgpuNode( node );


		try
		{	

			XSI::Primitive prim = xobj.GetActivePrimitive();
			XSI::PolygonMesh xmesh = prim.GetGeometry();
			XSI::NurbsSurfaceMesh xnurbsmesh = prim.GetGeometry();

			XSI::X3DObject xmasterObj;
			GetMasterInstance( xobj, xmasterObj);

			if ( xmasterObj.IsValid() )
			{
				//If this xobj is an instance,
				//get the master node,
				//and create an sgpuPathRefrence to the 
				//corresponding sgou masterNode.
				XSI::CString referencedName = xmasterObj.GetUniqueName();
				if( !m_exportDoc.m_exportGeomForVertexAnim )
				{
					sgpuNode startWith( m_scene.GetRootNode() );
					sgpuNode referencedSgpuNode( FindNodeRec( startWith , referencedName ) );
					if( IsUsefulSgpuNode( referencedSgpuNode ) )
					{
						node.CreateNodeContent_PathReference( m_scene.GetRootNode(), referencedSgpuNode );
					} else
					{
						parent_node.DetachChildNode( node );
						std::wstringstream ss;
						ss << L"instancence " << tObjectName << L" to master " << _tstr(referencedName) << " was not exported, because master may not be exported" << std::endl;
						m_exportDoc.WriteLog( ss.str().c_str() );
						throw std::runtime_error("cannot find instance");
					}
				} else
				{
						//If we are exporting for vertex animation
						//thrn we need to d-instance
						LONG nChildrenForMaster = xmasterObj.GetChildren().GetCount();
						for( LONG iChild =0; iChild < nChildrenForMaster; ++iChild )
						{
							TransformStackForDeInstancing tmStackDeInstance( xobj.GetRef(),  xmasterObj.GetRef(), m_exportDoc.m_exportGeomForVertexAnim, tmStack.m_curFrame );	
							XSI::X3DObject child( xmasterObj.GetChildren()[ iChild ] );

							exportObject( child, node, tmStackDeInstance );
						}
				}
			} else if (xmesh.IsValid())
			{
				if( EXPORT_AS_SUBDIV )
				{
					exportSubdiv( xobj.GetRef(), node, tmStack );
				} else
				{
					exportMesh( xobj.GetRef(), node, tmStack  );
				}
			} else if (xnurbsmesh.IsValid() )
			{
				exportNurbsMesh( xobj.GetRef(), xobj.GetMaterial(), node, tmStack );
			}
		}
		catch(...)
		{
			{
				std::wstringstream ss;
				ss << L"Export of \'" << obj_name.GetWideString() << L"\' " << m_meshId << L"th-node failed\n";
				m_exportDoc.WriteLog( ss.str().c_str() );
			}
		}
		// process children
		XSI::CRefArray childArray = xobj.GetChildren();
		for (LONG i =0; i<childArray.GetCount(); i++)
		{
			XSI::X3DObject obj = childArray[i];
			if ( !exportObject(obj, node, tmStack ) ) return false;
		}
		m_exportDoc.UpdateStep(1);
		tmStack.PopStackWithCurrentObjectTm( xobj.GetRef() );
	} else
	{	
		{
			std::wstringstream ss;
			ss << L"Omitted \'" << obj_name.GetWideString() << L"\' id=" << m_meshId << "  visibilty = " << visible << L" class: " << pzClassName << "\n"; ;
			m_exportDoc.WriteLog( ss.str().c_str() );
		}
		// process children
		XSI::CRefArray childArray = xobj.GetChildren();
		for (LONG i =0; i<childArray.GetCount(); i++)
		{
			XSI::X3DObject obj = childArray[i];
			if ( !exportObject(obj, parent_node, tmStack ) ) return false;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBModelExporter::GetShader( const XSI::Material& xmat, const XSI::CString& type, const XSI::CString& channel, XSI::Shader& tex )
{
	XSI::Parameter surface = xmat.GetParameter(type);
	XSI::CRefArray nested = surface.GetNestedObjects();

	int i, n = nested.GetCount();
	for ( i=0; i<n; i++ )
	{
		XSI::SIObject obj( nested[i] );
		if ( obj.GetType() == L"Shader" )
			break;
	}

	if (i==n)
		return; //Not found
	
	XSI::Shader shader(nested[i]);
	XSI::Parameter diffuse = shader.GetParameter(channel);
	nested = diffuse.GetNestedObjects();

	n = nested.GetCount();
	for ( i=0; i<n; i++ )
	{
		XSI::SIObject obj( nested[i] );
		if ( obj.GetType() == L"Shader" )
			break;
	}

	if (i==n)
		return; //Not found

	tex = nested[i];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBModelExporter::TransformUV( const XSI::Material& xmat, std::vector<sgpuVertex> &io_sgpuVertices )
{
	
	XSI::Shader tex;
	GetShader( xmat, L"surface", L"diffuse", tex );

	if ( tex.IsValid() )
	{
		//tex.GetTransformValues(flags, out);

		if ( io_sgpuVertices.size() <= 0 )
		{
			{
				{
					std::wstringstream ss;
					ss << L"Material has no current UV set: " << xmat.GetName().GetWideString() <<  L"\n";
					m_exportDoc.WriteLog( ss.str().c_str() );
				}
				return;
			}
		} else
		{
			float scaleU = 1.f, scaleV = 1.f;
			bool altU = false, altV = false;

			XSI::Parameter repeats = tex.GetParameter( L"repeats" );
			if (repeats.IsValid())
			{
				scaleU = repeats.GetParameter( L"x" ).GetValue();
				scaleV = repeats.GetParameter( L"y" ).GetValue();
			}
			XSI::Parameter alt_x = tex.GetParameter( L"alt_x" );
			XSI::Parameter alt_y = tex.GetParameter( L"alt_y" );
			if (alt_x.IsValid())
				altU = alt_x.GetValue();
			if (alt_y.IsValid())
				altV = alt_y.GetValue();

			if ( altU || altV )
			{
				std::wstringstream  ss;
				ss << L"Material uses UV alternates.: "  << xmat.GetName().GetWideString() << L"\n";
				m_exportDoc.WriteLog( ss.str().c_str() );
			}
			
			
			size_t j, n = io_sgpuVertices.size();
			for ( j=0; j<n; ++j )
			{
				sgpuVertex & v= io_sgpuVertices[ j ];
				v.u = v.u * scaleU;
				v.v = v.v * scaleV;
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GXBModelExporter::exportMesh( const XSI::X3DObject &xobj, sgpuNode & node, TransformStack &tmStack )
{
	XSI::Primitive prim = xobj.GetActivePrimitive();
	XSI::PolygonMesh xmesh = prim.GetGeometry();
	int numSubsets = 0;
	XSI::CString objName = xobj.GetUniqueName();
	XSI::CString mesh_name = xmesh.GetUniqueName();
	sgpuMeshConstructor construct( m_scene, node , m_bVertexAnimation );

	// Get a geometry accessor from object	
	XSI::CGeometryAccessor ga = xmesh.GetGeometryAccessor( m_exportDoc.m_constMode, m_exportDoc.m_subdType, m_exportDoc.m_subdLevel );

	XSI::CStatus st;

	bool flip = (Determinant ( tmStack.GetStackTopTM()) < 0.0); // flip face orientation	
	XSI::MATH::CTransformation curTm; 
	curTm.SetMatrix4( tmStack.GetStackTopTM() );
	
	const XSI::MATH::CMatrix4 &curMat = tmStack.GetStackTopTM();
	//For getting the transform for normals
	//you need to get rid of the translation component
	//and take transpose of inverse
	curTm.SetTranslationFromValues(0,0,0);
	XSI::MATH::CMatrix4 nMat = curTm.GetMatrix4();	
	nMat.InvertInPlace();
	nMat.TransposeInPlace();

	{
		// polygon vertex positions
		XSI::CDoubleArray vtxPosArray;
		st = ga.GetVertexPositions(vtxPosArray);
		st.AssertSucceeded( _T("GetVertexPositions") );
		LONG lNumVertFloats = vtxPosArray.GetCount();
		int numVertFloats = SafeLongToInt( lNumVertFloats );
		assert( (numVertFloats % 3) == 0 );
		std::vector< sgpuVector3 > sgpuPositions;
		sgpuPositions.reserve( numVertFloats );
		for(int i=0 ; i < numVertFloats; i = i + 3 )
		{
			double f[3];
			f[0] = vtxPosArray[ i ];
			f[1] = vtxPosArray[ i + 1];
			f[2] = vtxPosArray[ i + 2];
			XSI::MATH::CVector3 vec( f[0], f[1], f[2] );
			if( m_exportDoc.m_exportGeomForVertexAnim )
			{
				vec.MulByMatrix4InPlace( curMat );
			}
			sgpuPositions.push_back( sgpuVector3
				(
				static_cast< float > ( vec[0] ), 
				static_cast< float > ( vec[1] ), 
				static_cast< float > ( vec[2] )  
				));
		}
		construct.SetPosition( &sgpuPositions[0], numVertFloats/3 );
	}

	{
		// polygon node normals
		XSI::CFloatArray nodeNormalArray;
		st = ga.GetNodeNormals(nodeNormalArray);
		st.AssertSucceeded( _T("GetNodeNormals") );
		LONG lNumNormalFloats = nodeNormalArray.GetCount();
		int numNormalFloats = SafeLongToInt( lNumNormalFloats );
		assert( (numNormalFloats % 3) == 0 );
		std::vector< sgpuVector3 > sgpuNormals;
		sgpuNormals.reserve( numNormalFloats );
		for(int i=0 ; i < numNormalFloats; i = i + 3 )
		{
			double f[3];
			f[0] = nodeNormalArray[ i ];
			f[1] = nodeNormalArray[ i + 1];
			f[2] = nodeNormalArray[ i + 2];

			XSI::MATH::CVector3 norm( f[0], f[1], f[2] );
			if( m_exportDoc.m_exportGeomForVertexAnim )
			{
				norm.MulByMatrix4InPlace( nMat );
			}
			sgpuNormals.push_back( sgpuVector3( 
				static_cast< float > ( norm[0] ), 
				static_cast< float > ( norm[1] ), 
				static_cast< float > ( norm[2] ) 
			) );
		}
		construct.SetNormal( &sgpuNormals[0], numNormalFloats/3 );
	}

	// gets all UVs on the mesh
	XSI::CFloatArray uvValues;
	XSI::CRefArray uvs = ga.GetUVs();
	{
		if (uvs.GetCount() > 0) // use only uv0
		{
			XSI::ClusterProperty uvProp = uvs[0];
			uvProp.GetValues( uvValues );
			LONG lNumUVFloats = uvValues.GetCount();
			int numUVFloats = SafeLongToInt( lNumUVFloats );
			std::vector< sgpuVector3 > sgpuUVs;
			LONG nUVStride = uvProp.GetValueSize();
			sgpuUVs.reserve( numUVFloats );
			for(int i=0 ; i < numUVFloats; i = i + uvProp.GetValueSize() )
			{
				sgpuUVs.push_back( sgpuVector3( uvValues[i], 1 - uvValues[i+1], 0.0f ) );
			}
			construct.SetUV( &sgpuUVs[0], numUVFloats/2 );
		}
	}

	// materials
	XSI::CRefArray xmats = ga.GetMaterials();

	
	// polygon triangle vertex indices
	XSI::CLongArray triVtxIdxArray;
	st = ga.GetTriangleVertexIndices(triVtxIdxArray);
	st.AssertSucceeded( _T("GetTriangleVertexIndices") );

	XSI::CLongArray triNodeIdxArray;
	st = ga.GetTriangleNodeIndices(triNodeIdxArray);
	st.AssertSucceeded( _T("GetTriangleNodeIndices") );


	// material used by polygons	
	XSI::CLongArray polyMatIndices;	
	st = ga.GetPolygonMaterialIndices(polyMatIndices);
	st.AssertSucceeded( _T("GetPolygonMaterialIndices") );

	// polygon indices from triangle indices	
	XSI::CLongArray ptIndices;
	st = ga.GetPolygonTriangleIndices(ptIndices);
	st.AssertSucceeded( _T("GetPolygonTriangleIndices") );


	typedef std::map <LONG, std::deque< LONG >  > FaceMatMap;
	FaceMatMap faceMatMap;
	FaceMatMap::const_iterator fit;

	// loops over triangles and get a mtl map
	for ( LONG i = 0; i < triNodeIdxArray.GetCount(); i+=3 )
	{
		int nPoly = ptIndices[i/3];

		LONG matId = polyMatIndices[nPoly];
		std::deque< LONG > &d = faceMatMap[ matId ];
		d.push_back( i/3 );
	}

	for( fit = faceMatMap.begin(); fit != faceMatMap.end(); ++fit )
	{
		//export material
		XSI::Material xmat ( xmats[fit->first] );
		XSI::CString smatname = addMaterial( xmat );
		const TCHAR *pzMatName = _tstr(smatname);
		const std::deque< LONG > &faces = fit->second;
		sgpuMaterial sgpuMtl = m_scene.GetMaterial( sgpuString( _tstr(smatname) ) );
		std::deque< LONG >::const_iterator nit;
		for( nit = faces.begin(); nit != faces.end(); ++nit )
		{
			LONG fid = *nit;
			assert( fid * 3 <  triNodeIdxArray.GetCount() );
			
		}
	}
	for( fit = faceMatMap.begin(); fit != faceMatMap.end(); ++fit )
	{
		//export material
		XSI::Material xmat ( xmats[fit->first] );
		XSI::CString smatname = addMaterial( xmat );
		const std::deque< LONG > &faces = fit->second;
		sgpuMaterial sgpuMtl = m_scene.GetMaterial( sgpuString( _tstr(smatname) ) );
		std::deque< LONG >::const_iterator nit;
		for( nit = faces.begin(); nit != faces.end(); ++nit )
		{
			LONG fid = *nit;
			assert( fid * 3 <  triNodeIdxArray.GetCount() );
			LONG fIdx = fid * 3;
			sgpuConstructor::FaceVertex fv[3];
			for ( LONG j=0; j<3; j++ )
			{
				LONG k = flip ? 2-j : j;

				LONG triId  = triVtxIdxArray[ fIdx+k ];
				LONG nodeId = triNodeIdxArray[ fIdx+k ];

				LONG triIdx = triId*3;
				LONG nodeIdx = nodeId*3;
				fv[ k ].m_VertexId = triId;
				fv[ k ].m_NormalId = nodeId;
				fv[ k ].m_UVId = -1;
				if( uvs.GetCount() > 0 )
				{
					fv[ k ].m_UVId = nodeId;
				}
			}
			construct.AddFace( fv, 3, sgpuMtl );
		}
	}
	sgpuMesh mesh = construct.Construct( sgpuString( _tstr( objName ) ) );
	{
		int nVerts = mesh.GetNumVertices();
		int nFaces = mesh.GetNumFaces();		
		int nMtls = mesh.GetNumMaterials();
		std::wstringstream ss;
		ss << "Exporting " << _tstr(objName) << " meshId = " << m_meshId ;
		ss << " as triMesh " << "nverts= " << nVerts << " nFaces= " << nFaces << " nMtls= " << nMtls << endl; 
		m_exportDoc.WriteLog( ss.str().c_str() );
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GXBModelExporter::exportSubdiv( const XSI::X3DObject &xobj, sgpuNode & node , TransformStack &tmStack  )
{
	XSI::Primitive prim = xobj.GetActivePrimitive();
	XSI::PolygonMesh xmesh = prim.GetGeometry();
	int numSubsets = 0;
	XSI::CString mesh_name = xmesh.GetUniqueName();
	XSI::CString objName = xobj.GetUniqueName();
	sgpuSubdivConstructor constructS( m_scene, node , m_bVertexAnimation );

	// Get a geometry accessor from object	
	XSI::CGeometryAccessor ga = xmesh.GetGeometryAccessor( m_exportDoc.m_constMode, m_exportDoc.m_subdType, m_exportDoc.m_subdLevel );

	XSI::CStatus st;

	bool flip = (Determinant ( tmStack.GetStackTopTM()) < 0.0); // flip face orientation
	XSI::MATH::CTransformation curTm; 
	curTm.SetMatrix4( tmStack.GetStackTopTM() );

	const XSI::MATH::CMatrix4 &curMat = tmStack.GetStackTopTM();

	{
		// polygon vertex positions
		XSI::CDoubleArray vtxPosArray;
		st = ga.GetVertexPositions(vtxPosArray);
		st.AssertSucceeded( _T("GetVertexPositions") );
		LONG lNumVertFloats = vtxPosArray.GetCount();
		int numVertFloats = SafeLongToInt( lNumVertFloats );
		assert( (numVertFloats % 3) == 0 );
		std::vector< sgpuVector3 > sgpuPositions;
		sgpuPositions.reserve( numVertFloats );
		for(int i=0 ; i < numVertFloats; i = i + 3 )
		{
			double f[3];
			f[0] = vtxPosArray[ i ];
			f[1] = vtxPosArray[ i + 1];
			f[2] = vtxPosArray[ i + 2];
			XSI::MATH::CVector3 vec( f[0], f[1], f[2] );
			if( m_exportDoc.m_exportGeomForVertexAnim )
			{
				vec.MulByMatrix4InPlace( curMat );
			}
			sgpuPositions.push_back( sgpuVector3( 
				static_cast< float > ( vec[0] ), 
				static_cast< float > ( vec[1] ), 
				static_cast< float > ( vec[2] ) 
				) );
		}
		constructS.SetPosition( &sgpuPositions[0], numVertFloats/3 );
	}

	// gets all UVs on the mesh
	XSI::CFloatArray uvValues;
	XSI::CRefArray uvs = ga.GetUVs();
	{
		if (uvs.GetCount() > 0) // use only uv0
		{
			XSI::ClusterProperty uvProp = uvs[0];
			uvProp.GetValues( uvValues );
			LONG lNumUVFloats = uvValues.GetCount();
			int numUVFloats = SafeLongToInt( lNumUVFloats );
			LONG nUVStride = uvProp.GetValueSize();
			std::vector< sgpuVector3 > sgpuUVs;
			sgpuUVs.reserve( numUVFloats );
			for(int i=0 ; i < numUVFloats; i = i + nUVStride )
			{
				sgpuUVs.push_back( sgpuVector3( uvValues[i], 1- uvValues[i+1], 0.0f ) );
			}
			constructS.SetUV( &sgpuUVs[0], numUVFloats/2 );
		}
	}


	// materials
	XSI::CRefArray xmats = ga.GetMaterials();
	
	
	XSI::CLongArray polyVtxIdxArray;
	st = ga.GetVertexIndices(polyVtxIdxArray);
	st.AssertSucceeded( L"GetVertexIndices" );

	XSI::CLongArray polySizeArray;
	st = ga.GetPolygonVerticesCount(polySizeArray);
	
	XSI::CLongArray polyNodeIndicesArray;
	st = ga.GetNodeIndices(polyNodeIndicesArray);


	// material used by polygons	
	XSI::CLongArray polyMatIndices;	
	st = ga.GetPolygonMaterialIndices(polyMatIndices);
	st.AssertSucceeded( _T("GetPolygonMaterialIndices") );


	typedef std::map <LONG, std::deque< LONG >  > FaceMatMap;
	FaceMatMap faceMatMap;
	FaceMatMap::const_iterator fit;

	assert(  polyMatIndices.GetCount() == polySizeArray.GetCount() );
	for( int i=0, offset=0; i < polySizeArray.GetCount(); ++i )
	{
		LONG matId = polyMatIndices[ i ];
		LONG numVerticesInPoly = polySizeArray[ i ];
		std::vector< sgpuConstructor::FaceVertex > fvVec( numVerticesInPoly );		
		XSI::Material xmat ( xmats[matId] );		
		XSI::CString smatname = addMaterial( xmat );
		const TCHAR *pzMatName = _tstr(smatname);
		sgpuMaterial sgpuMat = m_scene.GetMaterial( sgpuString( _tstr(smatname) ) );
		vector< LONG > vIdxesForFace( numVerticesInPoly, -1);
		vector<LONG> nIdxesForFace( numVerticesInPoly, -1);
		for( int j=0; j < numVerticesInPoly; ++j )
		{
			LONG vIdx = polyVtxIdxArray[ j + offset ] ;
			LONG nIdx = polyNodeIndicesArray[j + offset];
			vIdxesForFace[ j ] = vIdx;
			nIdxesForFace [ j ] = nIdx;
		}
		if ( flip )
		{
			std::reverse( vIdxesForFace.begin(), vIdxesForFace.end() );
			std::reverse( nIdxesForFace.begin(), nIdxesForFace.end() );
		}
		for( int j=0; j < numVerticesInPoly; ++ j )
		{
			sgpuConstructor::FaceVertex &fv = fvVec[ j ];
			fv.m_VertexId = vIdxesForFace[ j ];

			if( uvs.GetCount() > 0 )
			{
				fv.m_UVId = nIdxesForFace [ j ];
			}
		}
		offset += numVerticesInPoly;
		constructS.AddFace( &fvVec[0], numVerticesInPoly, sgpuMat );
	}
	sgpuSubdiv mesh = constructS.Construct( sgpuString( _tstr( objName ) ) );
	{
		int nVerts = mesh.GetNumVertices();
		int nFaces = mesh.GetNumFaces();	
		std::wstringstream ss;
		ss << "Exporting " << _tstr(objName);
		ss << " as subdivmesh " << "nverts= " << nVerts << " nFaces= " << nFaces  << "\n" << endl; 
		m_exportDoc.WriteLog( ss.str().c_str() );
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GXBModelExporter::exportNurbsMesh(  const XSI::X3DObject &xobj, const XSI::Material & xmat, sgpuNode & node , TransformStack &tmStack )
{
	int numSubsets = 0;	
	XSI::Primitive prim = xobj.GetActivePrimitive();
	XSI::NurbsSurfaceMesh xmesh = prim.GetGeometry();
	XSI::CString objName = xobj.GetUniqueName();
	if( !xmesh.IsValid() )
	{
		std::wstringstream ss;
		ss << L"not able to get a valid nurbs mesh from" << objName.GetWideString() << L"\n";
		m_exportDoc.WriteLog( ss.str().c_str() );
		return false;
	}
	if( xobj.GetMaterials().GetCount() > 1 )
	{
		std::wstringstream ss;
		ss << L"warning: mesh will be xported with the first material, other materials of the mesh discarded" << objName.GetWideString() << L"\n";
		m_exportDoc.WriteLog( ss.str().c_str() );
		return  false;
		
	}
	XSI::CString mesh_name = xmesh.GetUniqueName();
	sgpuMeshConstructor constructor( m_scene, node , m_bVertexAnimation );
	XSI::CNurbsSurfaceRefArray nurbsSurf = xmesh.GetSurfaces();

	XSI::CString smatName = addMaterial( xmat );
	sgpuMaterial sgpuMat = m_scene.GetMaterial( sgpuString( _tstr(smatName) ) );
	XSI::CTriangleRefArray tri = xmesh.GetTriangles();
	LONG ulCount = tri.GetCount();
	bool flip = (Determinant ( tmStack.GetStackTopTM() ) < 0.0); // flip face orientation
	XSI::MATH::CTransformation curTm; 
	curTm.SetMatrix4( tmStack.GetStackTopTM() );
	
	const XSI::MATH::CMatrix4 &curMat = tmStack.GetStackTopTM();
	//For transforming the normals,
	//you need to get rid of the translation
	//and take the transpose of inverse
	curTm.SetTranslationFromValues(0,0,0);
	XSI::MATH::CMatrix4 nMat = curTm.GetMatrix4();	
	nMat.InvertInPlace();
	nMat.TransposeInPlace();
	
	std::vector< sgpuVector3 > sgpuPositions;
	std::vector< sgpuVector3 > sgpuNormals;
	std::vector< sgpuVector3 > sgpuUVs;


	XSI::CFloatArray uv( ulCount*3*3 );
	LONG kk = 0, ll = 0;
	std::vector<sgpuVertex > sgpuVertices;
	sgpuVertices.reserve( ulCount * 3);
	for (LONG i = 0; i < ulCount; i++)
	{
		XSI::Triangle t(tri[i]);

		XSI::CTriangleVertexRefArray triVtx = t.GetPoints();  

		if (triVtx.GetCount() == 3)
		{
			for (LONG j = 0; j < 3; j++)
			{
				sgpuVertices.push_back( sgpuVertex() );
				sgpuVertex &sgpuV = sgpuVertices.back();
				XSI::TriangleVertex xvtx(triVtx[j]);
				sgpuV.x = (float)xvtx.GetPosition().GetX();
				sgpuV.y = (float)xvtx.GetPosition().GetY();
				sgpuV.z = (float)xvtx.GetPosition().GetZ();
				sgpuV.nx = (float)xvtx.GetNormal().GetX();
				sgpuV.ny = (float)xvtx.GetNormal().GetY();
				sgpuV.nz = (float)xvtx.GetNormal().GetZ();
				sgpuV.u = (float)xvtx.GetUV().u;
				sgpuV.v =  ( 1.0f - (float)xvtx.GetUV().v );
			}
		}
	}	
	//Create mesh
	std::vector <int> sgpuIndices;
	buildIndicesForAnimation( sgpuVertices, sgpuIndices);
	sgpuPositions.reserve( sgpuVertices.size() );
	sgpuNormals.reserve( sgpuVertices.size() );
	sgpuUVs.reserve( sgpuVertices.size() );
	std::vector< sgpuVertex >::const_iterator cit;
	for( cit = sgpuVertices.begin(); cit != sgpuVertices.end(); ++ cit )
	{
		const sgpuVertex &vtx = *cit;
		XSI::MATH::CVector3 vec( vtx.x, vtx.y, vtx.z );		
		XSI::MATH::CVector3 norm( vtx.nx, vtx.ny, vtx.nz );
		if( m_exportDoc.m_exportGeomForVertexAnim )
		{
			vec.MulByMatrix4InPlace( curMat );
			norm.MulByMatrix4InPlace( nMat );
		}
		sgpuPositions.push_back( sgpuVector3( 
			static_cast< float > ( vec[0] ), 
			static_cast< float > ( vec[1] ), 
			static_cast< float > ( vec[2] )
		) );
		sgpuNormals.push_back( sgpuVector3( 
			static_cast< float > ( norm[0] ), 
			static_cast< float > ( norm[1] ), 
			static_cast< float > ( norm[2] )
		) );
		sgpuUVs.push_back( sgpuVector3( vtx.u, vtx.v, 0 ) );
	}
	constructor.SetPosition( &sgpuPositions[0], sgpuPositions.size() );
	constructor.SetNormal( &sgpuNormals[0], sgpuNormals.size() );
	constructor.SetUV( &sgpuUVs[0], sgpuUVs.size() );
	assert( (sgpuIndices.size() %3) == 0);
	sgpuConstructor::FaceVertex fvec[3];
	if( flip )
	{
		flipTriangleIndices( sgpuIndices );
	}
	for( int i=0; i < sgpuIndices.size(); i = i + 3)
	{
		int idx[3];
		idx[0] = sgpuIndices[i];
		idx[1] = sgpuIndices[i+1];
		idx[2] = sgpuIndices[i+2];
		for( int j=0; j < 3; ++j )
		{	
			fvec[j].m_VertexId = idx[j]; 
			fvec[j].m_NormalId = idx[j];
			fvec[j].m_UVId = idx[j];
		}
		constructor.AddFace( &fvec[0], 3, sgpuMat );
	}
	sgpuMesh sgpuMesh1 = constructor.Construct( sgpuString( _tstr( objName ) ) );
	{
		int nVerts = sgpuMesh1.GetNumVertices();
		int nFaces = sgpuMesh1.GetNumFaces();
		int nMtls = sgpuMesh1.GetNumMaterials();
		std::wstringstream ss;
		ss << "Exporting " << _tstr(objName);
		ss << " as sgpumesh " << "nverts= " << nVerts << " nFaces= " << nFaces <<" nMtls= " << nMtls << endl; 
		m_exportDoc.WriteLog( ss.str().c_str() );
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
XSI::CString GXBModelExporter::addMaterial(const XSI::Material & xmat)
{
	XSI::OGLMaterial ogl_mat( xmat.GetOGLMaterial() );
	XSI::OGLTexture  ogl_tex( xmat.GetOGLTexture() );
	sgpuMaterialInfo matInfo;
		// get material info
	matInfo.name = xmat.GetUniqueName();
	matInfo.diffuse = XSI::MATH::CVector3f((float)ogl_mat.GetDiffuse().r, 
		(float)ogl_mat.GetDiffuse().g, 
		(float)ogl_mat.GetDiffuse().b);
	matInfo.specular = XSI::MATH::CVector3f((float)ogl_mat.GetSpecular().r, 
		(float)ogl_mat.GetSpecular().g, 
		(float)ogl_mat.GetSpecular().b);
	matInfo.shininess = (float)ogl_mat.GetDecay()/3.0f; // [0 - 300] -> [0 - 100]

	bool bAlreadyThere = m_scene.IsMaterial( sgpuString( _tstr(matInfo.name) )  );
	if( bAlreadyThere )
	{
		return matInfo.name;
	}

	XSI::CColor transp = getCororParameter ( xmat, _T("transparency"));
	float alpha = (float)(transp.r + transp.g + transp.b) / 3.0f;
	alpha = 1.0f - alpha;
	matInfo.opacity = alpha;  

	matInfo.diffuseTexture = ogl_tex.GetFullName();

	sgpuMaterial mat = m_scene.CreateMaterial(_tstr(matInfo.name));

	mat.SetDiffuseColor(matInfo.diffuse.GetX(), matInfo.diffuse.GetY(), matInfo.diffuse.GetZ());
	mat.SetSpecularColor(matInfo.specular.GetX(), matInfo.specular.GetY(), matInfo.specular.GetZ());
	mat.SetOpacity(matInfo.opacity);
	mat.SetShininess(matInfo.shininess);

	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	_tsplitpath( _tstr(matInfo.diffuseTexture), drive, dir, fname, ext );
	std::wstring name = _tstr(matInfo.diffuseTexture);
	switch( m_exportDoc.m_textureExportFilepath )
	{
	default:
	case GXBExportDoc::Texture_ExportFilepath::eRelative:
		{
			Locator relTextureLoc(_tstr( matInfo.diffuseTexture ) );
			bool bVal = GetRelativePath( m_exportDoc.m_filename, matInfo.diffuseTexture, relTextureLoc );
			if( bVal )
			{
				std::wstring wRelTexturePath( relTextureLoc.ToPath() );
				name = wRelTexturePath;
			}
		}
		break;
	case GXBExportDoc::Texture_ExportFilepath::eAbsolute:
		break;
	case GXBExportDoc::Texture_ExportFilepath::eCopyTextureToExportDir:
		{
			XSI::CString dstPath = GetDstDirForTexturesToBeCopied();
			dstPath += L"/";
			dstPath += fname;
			dstPath += ext;
			Locator relDstLoc(_tstr( dstPath ) );
			bool bVal = GetRelativePath(m_exportDoc.m_filename, dstPath, relDstLoc );
			assert( bVal );
			std::wstring relDstPath( relDstLoc.ToPath() );
			GXBExportDoc::TexturesToCopy entry;
			entry.m_src = matInfo.diffuseTexture;
			entry.m_dst = dstPath ;
			m_exportDoc.m_texturesToCopy.push_back( entry );
			name = relDstPath;
		}
		break;
	}
	

	{
		m_exportDoc.WriteLog( L"material %s has texture %s\n", _tstr(matInfo.name), name.c_str() );
	}
	mat.SetDiffuseTexture( name.c_str() );
	return matInfo.name;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBModelExporter::dumpMaterial(int nMat)
{
	/*
	if (nMat < 0 || nMat >= (int)m_matsInfo.size())
		return;

	sgpuMaterialInfo & m = m_matsInfo[nMat];

	m_exportDoc.WriteLog(_T("%sMaterial\n"), indent());
	m_exportDoc.WriteLog(_T("%s{\n"), indent());
	m_exportDoc.WriteLog(_T("%s  Name: %s\n"), indent(), _tstr(m.name));
	m_exportDoc.WriteLog(_T("%s  Diffuse: (%f, %f, %f)\n"), indent(), m.diffuse.GetX(), m.diffuse.GetY(), m.diffuse.GetZ());
	m_exportDoc.WriteLog(_T("%s  Specular: (%f, %f, %f)\n"), indent(), m.specular.GetX(), m.specular.GetY(), m.specular.GetZ());
	m_exportDoc.WriteLog(_T("%s  Shininess: %f\n"), indent(), m.shininess);
	m_exportDoc.WriteLog(_T("%s  Opacity: %f\n"), indent(), m.opacity);
	m_exportDoc.WriteLog(_T("%s  DiffuseTexture: %s\n"), indent(), _tstr(m.diffuseTexture));
	m_exportDoc.WriteLog(_T("%s}\n"), indent());
	*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBModelExporter::dumpProperties(const XSI::X3DObject & xobj)
{
	XSI::CRefArray props = xobj.GetProperties();
	for (LONG i = 0; i < props.GetCount(); i++)
	{
		XSI::Property p(props[i]);
		{
			std::wstringstream ss;
			ss <<  "Property " << _tstr(p.GetName()) << "\n";
			m_exportDoc.WriteLog( ss.str().c_str() );
		}

		XSI::CParameterRefArray params = p.GetParameters();
		for (LONG j = 0; j < params.GetCount(); j++)
		{
			XSI::Parameter param(params[j]);
			dumpParameter(param);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBModelExporter::dumpProperties(const XSI::Material & xmat)
{
	{
		std::wstringstream ss;
		ss << _T("Material  ") <<  _tstr(xmat.GetName()) << _T(" properties\n");
		m_exportDoc.WriteLog( ss.str().c_str() );
	}

	XSI::CRefArray props = xmat.GetProperties();
	for (LONG i = 0; i < props.GetCount(); i++)
	{
		XSI::Property p(props[i]);
		{
			std::wstringstream ss;
			ss <<  _T("Property ") << _tstr(p.GetName()) << std::endl ;
			m_exportDoc.WriteLog( ss.str().c_str() );
		}

		XSI::CParameterRefArray params = p.GetParameters();
		for (LONG j = 0; j < params.GetCount(); j++)
		{
			XSI::Parameter param(params[j]);
			dumpParameter(param);
		}
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
XSI::CString GXBModelExporter::GetDstDirForTexturesToBeCopied() const
{
				TCHAR drive_2[_MAX_DRIVE], dir_2[_MAX_DIR], fname_2[_MAX_FNAME], ext_2[_MAX_EXT];
				_tsplitpath( _tstr(m_exportDoc.m_filename), drive_2, dir_2, fname_2, ext_2 );
				XSI::CString dst = XSI::CString(drive_2) + XSI::CString(dir_2);
				dst += L"textures";
				return dst;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
sgpuMesh GXBModelExporter::addMesh(sgpuNode & node, vecVertex_t & vtx, std::vector <int> & idx)
{
	sgpuMesh mesh = node.CreateTriangleMesh();	  

	// fill mesh geometry
	mesh.SetNumVertices((int)vtx.size());  
	for (int i = 0; i <  (int)vtx.size(); i++)
	{
		sgpuVertex & v = vtx[i];

		mesh.SetPosition(i, v.x, v.y, v.z);
		mesh.SetNormal(i, v.nx, v.ny, v.nz);  
		mesh.SetTexCoord(i, v.u, v.v);
	}

	mesh.SetNumIndices((int)idx.size());
	for (int i = 0; i <  (int)idx.size(); i++)
	{
		mesh.SetIndex(i, idx[i]);
	}  



	m_numMeshes++;

	return mesh;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
XSI::CColor GXBModelExporter::getCororParameter ( const XSI::Material & xmat, TCHAR* in_szParamName)
{
	//"transparency"
	XSI::CRefArray shaders = xmat.GetShaders();
	for (int s=0; s < shaders.GetCount(); s++)
	{
		XSI::Shader	shader(shaders[s]);

		float r,g,b,a;
		XSI::CStatus st = shader.GetColorParameterValue( in_szParamName, r, g, b, a);

		if (st == XSI::CStatus::OK)
			return XSI::CColor(r,g,b,a);
	}

	return XSI::CColor(0,0,0,0);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GXBModelExporter::saveAsOBJ(const TCHAR* name, std::vector <sgpuVertex> & vtx, std::vector <int> & idx)
{
	FILE* stream = _tfopen(name, _T("wt"));
	if (stream)
	{
		fprintf(stream, "g body\n");

		for (size_t i = 0; i < vtx.size(); i++)
		{
			fprintf(stream, "v %f %f %f\n", vtx[i].x, vtx[i].y, vtx[i].z);
		}
		fprintf(stream, "# %d vertices\n", vtx.size());
		fprintf(stream, "\n");

		for (size_t i = 0; i < idx.size()/3; i++)
		{
			fprintf(stream, "f %d %d %d\n", 1+idx[i*3+0], 1+idx[i*3+1], 1+idx[i*3+2]);
		}
		fprintf(stream, "# %d faces\n", idx.size()/3);
		fprintf(stream, "\n");

		fclose(stream);

		return true;
	}

	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBModelExporter::flipTriangleIndices( std::vector< int > indices )
{
	assert( ( indices.size() % 3 )== 0 );
	for( size_t i=0; i< indices.size(); i += 3 )
	{
		std::swap( indices[i], indices[i+2] );
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBModelExporter::buildIndices(std::vector <sgpuVertex> & vtx, std::vector <int> & idx )
{
	int iOutVQuantity = 0;

	std::map<sgpuVertex,int> kTable;

	std::map<sgpuVertex, int>::iterator pkIter;
	std::vector<int> aiInToOutMapping(vtx.size());
	idx.resize(vtx.size());

	for (size_t i = 0; i < vtx.size(); i++)
	{
		pkIter = kTable.find(vtx[i]);
		if (pkIter != kTable.end())
		{
			// Vertex i is a duplicate of one inserted earlier into the
			// table.  Map vertex i to the first-found copy.
			aiInToOutMapping[i] = pkIter->second;
		}
		else
		{
			// Vertex i is the first occurrence of such a point.
			kTable.insert(std::make_pair(vtx[i],iOutVQuantity));
			aiInToOutMapping[i] = iOutVQuantity;
			iOutVQuantity++;
		}
	}

	// Pack the unique vertices into an array in the correct order.
	vtx.resize(iOutVQuantity);
	for (pkIter = kTable.begin(); pkIter != kTable.end(); pkIter++)
	{
		assert(0 <= pkIter->second && pkIter->second < iOutVQuantity);
		vtx[pkIter->second] = pkIter->first;
	}

	// Build face indices
	for (size_t i = 0; i < idx.size(); i++)
	{
		idx[i] = aiInToOutMapping[i];
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBModelExporter::buildIndicesForAnimation(std::vector <sgpuVertex> & vtxVector, std::vector <int> & idxVector )
{
	idxVector.resize( vtxVector.size() * 3 );
	for( size_t i=0, j=0; i < vtxVector.size(); ++i )
	{
		idxVector.push_back( j++ );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void	GXBModelExporter::CopyTextures()
{
	if( m_exportDoc.m_texturesToCopy.size() <= 0 )
	{
		return;
	}

	XSI::CString dstDir = GetDstDirForTexturesToBeCopied();
	BOOL bRes = CreateDirectoryW( _tstr( dstDir ), NULL );
	if( !bRes )
	{
		DWORD derr = GetLastError();
		if( derr != ERROR_ALREADY_EXISTS )
		{
			throw std::runtime_error("cant create directory to copy textures");
		}
	}
	std::list< GXBExportDoc::TexturesToCopy >::const_iterator lit;
	for(lit = m_exportDoc.m_texturesToCopy.begin(); lit != m_exportDoc.m_texturesToCopy.end(); ++lit )
	{
		const GXBExportDoc::TexturesToCopy &tc = *lit;
		CopyFile( lit->m_src, lit->m_dst );
	}
}