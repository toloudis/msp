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
#include <string>
#include <sstream>

#include <xsi_nurbssurface.h>
#include <xsi_nurbssurfacemesh.h>
#include <xsi_value.h>

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
#include "sgpuVertexAnimExportScene.hpp"
#include "sgpuException.hpp"



#include "gxbvertexanimexporter.h"
#include <xsi_project.h>
#include "exportmesh_command.h"
#include <xsi_polygonmesh.h>
#include "helper.h"
#include "Log.h"
#include "sgpuMatrix.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuMesh.hpp"
#include "sgpuMeshConstructor.hpp"
#include "sgpuSubdiv.hpp"
#include "sgpuSubdivConstructor.hpp"
#include "sgpuNode.hpp"

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern XSI::CustomProperty GetImportExportProp();
extern XSI::Parameter GetImportExportOption( const XSI::CString& in_strName );
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float GXBVertexAnimExporter::m_vertexCacheLimit = 2.0f *  1024*1024*1024;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GXBVertexAnimExporter::GXBVertexAnimExporter( GXBExportDoc &doc):
GXBExporter(doc),
m_meshId(0)
{
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GXBVertexAnimExporter::SetCurrentFrame( int i_KeyNum )
{
	sgpuVertexAnimEstimator & estimator = m_scene->GetEstimator();
	int nKeys = estimator.GetNumFrames();
	if( i_KeyNum > nKeys )
	{

		std::wstringstream ss;
		ss << L"cannot step nto frame" << i_KeyNum << L"\n";
		m_exportDoc.WriteLog( ss.str().c_str() );
		return -1;
	}
	float targetFrame = estimator.ComputeIthFrame( i_KeyNum );
	
	XSI::Application app;	
	XSI::Project prj = app.GetActiveProject();
	XSI::CValueArray args;
	XSI::CValue dummy;
	args.Resize(2);
	args[0] = L"PlayControl.Current";
	args[1] = targetFrame;
	app.ExecuteCommand(L"SetValue", args, dummy);
	app.ExecuteCommand(L"Refresh", XSI::CValueArray(), dummy);
	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GXBVertexAnimExporter::~GXBVertexAnimExporter()
{
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GXBVertexAnimExporter::PeekMesh( const XSI::X3DObject & xobj )
{
	XSI::Primitive prim = xobj.GetActivePrimitive();
	XSI::PolygonMesh xmesh = prim.GetGeometry();
	int numSubsets = 0;
	XSI::CString mesh_name = xmesh.GetUniqueName();
	XSI::CString fullName = xobj.GetFullName();
	XSI::CString objName = xobj.GetUniqueName();

	// Get a geometry accessor from object	
	XSI::CGeometryAccessor ga = xmesh.GetGeometryAccessor( m_exportDoc.m_constMode, m_exportDoc.m_subdType, m_exportDoc.m_subdLevel );

	XSI::CStatus st;
	int numVertFloats =0;
	int numNormalFloats =0;
	{
		// polygon vertex positions
		XSI::CDoubleArray vtxPosArray;
		st = ga.GetVertexPositions(vtxPosArray);
		st.AssertSucceeded( _T("GetVertexPositions") );
		LONG lNumVertFloats = vtxPosArray.GetCount();
		numVertFloats = SafeLongToInt( lNumVertFloats );
		assert( (numVertFloats % 3) == 0 );
	}

	{
		// polygon node normals
		XSI::CFloatArray nodeNormalArray;
		st = ga.GetNodeNormals(nodeNormalArray);
		st.AssertSucceeded( _T("GetNodeNormals") );
		LONG lNumNormalFloats = nodeNormalArray.GetCount();
		numNormalFloats = SafeLongToInt( lNumNormalFloats );
		assert( (numNormalFloats % 3) == 0 );
	}

	sgpuVertexAnimEstimator &estimator = m_scene->GetEstimator();
	XSI::X3DObject obj;
	int nEstimateRes = estimator.AddEstimate( sgpuString( _tstr(objName) ), EXPORT_AS_SUBDIV, numVertFloats/3, numNormalFloats/3 );
	m_meshVersusIndexInAnimFileMap.insert( std::pair< XSI::CString, int >( fullName, nEstimateRes-1 ) );
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GXBVertexAnimExporter::PeekNurbsSurface( const XSI::X3DObject & xobj )
{
	XSI::Primitive prim = xobj.GetActivePrimitive();
	XSI::NurbsSurfaceMesh xmesh = prim.GetGeometry();
	int numSubsets = 0;
	XSI::CString mesh_name = xmesh.GetUniqueName();
	XSI::CString fullName = xobj.GetFullName();
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
		return false;
	}
	XSI::CTriangleRefArray tri = xmesh.GetTriangles();
	LONG ulCount = tri.GetCount();
	LONG numVertices = ulCount * 3;
	LONG numNormals = ulCount * 3;
	XSI::X3DObject obj;
	sgpuVertexAnimEstimator &estimator = m_scene->GetEstimator();
	int nEstimateRes = estimator.AddEstimate( sgpuString( _tstr(objName) ), EXPORT_AS_SUBDIV, numVertices, numNormals );
	m_meshVersusIndexInAnimFileMap.insert( std::pair< XSI::CString, int >( fullName, nEstimateRes-1 ) );
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GXBVertexAnimExporter::ExportPrefaceChunks( )
{
	sgpuString fname( _tstr(m_exportDoc.m_filename) );
	bool bRet = m_scene->OpenAndBeginWriting( fname );
	if( !bRet )
	{
		//need log
		assert( false );
	}
	return bRet;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GXBVertexAnimExporter::DoExport()
{
	bool r = true;
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	_tsplitpath( _tstr(m_exportDoc.m_filename), drive, dir, fname, ext );
	AnimParams vParams;
	GetAnimParameters( vParams );
	m_scene.reset( new sgpuVertexAnimExportScene( 
		vParams.m_FrameRate,
		static_cast< float > ( m_exportDoc.m_animExportStartFrame ),
		static_cast< float > ( m_exportDoc.m_animExportEndFrame ),
		vParams.m_StepFrame) );

	XSI::Application app;
	m_selection = ( app.GetSelection().GetArray() );
	if( m_selection.GetCount() <= 0)
	{

		m_selection.Add( app.GetActiveSceneRoot() );
	}
	XSI::CString output_path = XSI::CString(drive) + XSI::CString(dir);


	int numMeshes = 0;
	for( int i=0; i < m_selection.GetCount(); ++i)
	{
		XSI::CRef ref = m_selection[i];
		XSI::X3DObject xroot( ref );
		enumMeshes( xroot, numMeshes );
	}
	m_meshId=0;
	sgpuVertexAnimEstimator &estimator = m_scene->GetEstimator();
	float fProgressReportBudget = static_cast< float > ( estimator.GetEstimate() ) * 2.0f +  numMeshes;
	if( fProgressReportBudget >= m_vertexCacheLimit )
	{
		return false;
	}
	m_numNodes = 0;
	m_numMeshes = 0;

	m_sTransform.clear();
	XSI::MATH::CMatrix4 m;
	m.SetIdentity();
	bool bRet = ExportPrefaceChunks( );
	if( !bRet )
	{
		return bRet;
	}

	m_sTransform.push_back( m );

	int numKeys = m_scene->GetEstimator().GetNumFrames();
	m_exportDoc.InitializeProgressBar( numKeys );
	{
		std::wstringstream ss;
		ss << "Exporting vertex animation to file " << _tstr(m_exportDoc.m_filename) << endl;		
		ss << "Start = " << m_scene->GetEstimator().m_StartFrame << endl;
		ss << "End = " << m_scene->GetEstimator().m_EndFrame << endl;
		ss << "Step = " << m_scene->GetEstimator().m_StepFrame << endl;
		m_exportDoc.WriteLog( ss.str().c_str() );
	}
	for (int i=0; i < numKeys; ++i )
	{

		int res = SetCurrentFrame( i );
		m_meshId = 0;
		if( res < 0)
		{
			throw std::runtime_error(std::string("export failed") );
		}
		for( int j=0; j < m_selection.GetCount(); ++j )
		{
			XSI::CRef ref = m_selection[j];
			XSI::X3DObject xroot( ref );
			bool  r = exportObject( xroot, i );
			if ( !r )
			{
				throw std::runtime_error(std::string("export failed") );
			}
		}
		m_exportDoc.UpdateStep( 1 );
	}

	m_sTransform.pop_back();
	bRet = m_scene->CloseAndEndWriting();

	return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBVertexAnimExporter::enumProps( const XSI::X3DObject & xobj )
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
void  GXBVertexAnimExporter::enumMeshes(const XSI::X3DObject & xobj, int & numMeshes)
{
	if (!xobj.IsValid()) return;

	enumProps( xobj );

	XSI::Property propVis(xobj.GetProperties().GetItem(L"Visibility"));
	bool visible = propVis.GetParameter(L"viewvis").GetValue();

	XSI::CString name = xobj.GetName();
	const TCHAR *pzName = _tstr( name );
	XSI::Primitive prim = xobj.GetActivePrimitive();
	XSI::PolygonMesh mesh = prim.GetGeometry();
	XSI::NurbsSurfaceMesh xnurbsmesh = prim.GetGeometry();
	++m_meshId;
	if (visible && (HasExportableGeometry( xobj ) ))
	{
		numMeshes++;
		if( mesh.IsValid() )
		{
			PeekMesh( xobj );
		} else if( xnurbsmesh.IsValid() )
		{
			PeekNurbsSurface( xobj );
		}
	}

	XSI::CRefArray childArray = xobj.GetChildren();
	for (LONG i =0; i<childArray.GetCount(); i++)
	{
		XSI::X3DObject obj = childArray[i];
		enumMeshes(obj, numMeshes);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  GXBVertexAnimExporter::exportObject(const XSI::X3DObject & xobj, int i_FrameNum )
{
	Log::Indent indent( this->m_exportDoc.m_log );
	if (!xobj.IsValid()) return false;
	bool r = true;
	XSI::Property propVis(xobj.GetProperties().GetItem(L"Visibility"));
	bool visible = propVis.GetParameter(L"viewvis").GetValue();
	XSI::CString obj_name = xobj.GetUniqueName();
	const TCHAR * tObjectName = _tstr(obj_name);	
	const TCHAR * pzClassName  = _tstr( xobj.GetClassIDName() );
	XSI::Primitive prim = xobj.GetActivePrimitive();
	XSI::PolygonMesh xmesh = prim.GetGeometry();
	XSI::NurbsSurfaceMesh xnurbsmesh = prim.GetGeometry();
	++m_meshId;
	if (visible && HasExportableGeometry( xobj ) )
	{
		XSI::Kinematics kin = xobj.GetKinematics().EvaluateAt(i_FrameNum );
		XSI::MATH::CMatrix4 gM = kin.GetGlobal().GetTransform().GetMatrix4();
		// get object local transform
		XSI::MATH::CMatrix4 ipM = m_sTransform.back();
		ipM.InvertInPlace();    
		XSI::MATH::CMatrix4 tM;
		tM.Mul(gM, ipM);



		sgpuMatrix m(
			(float)(tM.GetValue(0,0)), (float)(tM.GetValue(0,1)), (float)(tM.GetValue(0,2)), (float)(tM.GetValue(0,3)),
			(float)(tM.GetValue(1,0)), (float)(tM.GetValue(1,1)), (float)(tM.GetValue(1,2)), (float)(tM.GetValue(1,3)),
			(float)(tM.GetValue(2,0)), (float)(tM.GetValue(2,1)), (float)(tM.GetValue(2,2)), (float)(tM.GetValue(2,3)),
			(float)(tM.GetValue(3,0)), (float)(tM.GetValue(3,1)), (float)(tM.GetValue(3,2)), (float)(tM.GetValue(3,3))
			); 


		m_sTransform.push_back( gM );

		try
		{
			if (xmesh.IsValid())
				r = exportMesh( xobj, i_FrameNum, EXPORT_AS_SUBDIV );
			else if (xnurbsmesh.IsValid())
			{
				r = exportNurbsMesh( xobj, i_FrameNum );
			}
			if( !r )
			{
				//dont do anything
				//might have already  complained
			}
		}
		catch ( ... )
		{
			{
				std::wstringstream ss;
				ss << L"Export of \'" << obj_name.GetWideString() << L"\' " << m_meshId << L"th-node failed at frame " << i_FrameNum <<  L"\n";
				m_exportDoc.WriteLog( ss.str().c_str() );
			}
		}
		// process children

		XSI::CRefArray childArray = xobj.GetChildren();
		for (LONG i =0; i<childArray.GetCount(); i++)
		{
			XSI::X3DObject obj = childArray[i];
			if ( !exportObject(obj, i_FrameNum) ) return false;
		}

		m_sTransform.pop_back();

		return true;
	} else
	{	
		{
			if( i_FrameNum == 0)
			{
				std::wstringstream ss;
				ss << L"Omitted \'" << obj_name.GetWideString() << L"\' id=" << m_meshId << "  visibilty = " << visible << L" class: " << pzClassName << "\n"; ;
				m_exportDoc.WriteLog( ss.str().c_str() );
			}
		}
		// process children
		XSI::CRefArray childArray = xobj.GetChildren();
		for (LONG i =0; i<childArray.GetCount(); i++)
		{
			XSI::X3DObject obj = childArray[i];
			if ( !exportObject(obj, i_FrameNum ) ) return false;
		}
	}
	return true;
}

/*
void
PrintNested( const XSI::SIObject& obj, Log& log, int indent = 0 )
{
std::wstring str( indent, L' ' );
log.WriteLog( L"%s $ Object: %s (Type %s)\n", str.c_str(), obj.GetFullName().GetWideString(), obj.GetType().GetWideString() );

XSI::CRefArray nested = obj.GetNestedObjects();
int i, n = nested.GetCount();
for ( i=0; i<n; i++ )
{
XSI::Parameter param( nested[i] );
if (param.IsValid())
{
log.WriteLog(L"%s >>>> Parameter: %s = %s\n", str.c_str(), param.GetFullName().GetWideString(), param.GetValue().GetAsText().GetWideString() );

if (param.GetName() == L"repeats")
PrintNested( param, log, 10 );
}
else
{
XSI::SIObject obj( nested[i] );
log.WriteLog( L"%s >>>> Object: %s (Type: %s) \n", str.c_str(), obj.GetFullName().GetWideString(), obj.GetType().GetWideString() );

//if (obj.GetType() == L"tspace_id")
//	PrintNested( obj, log, 10 );
}
}
}

inline double
AltCoord(double coord)
{
long cv = (long) coord;

if (coord<0)
{
if (!(cv&1))
{
double part = cv - coord;
part = 1.0 - part;
return cv - part;
}
}
else
{
if (cv&1)
{
double part = coord - cv;
part = 1.0 - part;
return cv + part;
}
}

return coord;
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBVertexAnimExporter::GetShader( const XSI::Material& xmat, const XSI::CString& type, const XSI::CString& channel, XSI::Shader& tex )
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
bool GXBVertexAnimExporter::exportMesh(const XSI::X3DObject & xobj, int i_FrameNum , bool i_bSubdivMesh )
{
	int numSubsets = 0;
	XSI::Primitive prim = xobj.GetActivePrimitive();
	XSI::PolygonMesh xmesh = prim.GetGeometry();
	XSI::CString fullName = xobj.GetFullName();
	XSI::CString mesh_name = xobj.GetName();
	XSI::CString objName = xobj.GetUniqueName();
	if( !xmesh.IsValid() )
	{
		std::wstringstream ss;
		ss << L"not able to get a valid poly mesh from" << objName.GetWideString() << L"\n";
		m_exportDoc.WriteLog( ss.str().c_str() );
		return false;
	}
	// Get a geometry accessor from object	
	XSI::CGeometryAccessor ga = xmesh.GetGeometryAccessor( m_exportDoc.m_constMode, m_exportDoc.m_subdType, m_exportDoc.m_subdLevel );

	XSI::CStatus st;

	bool flip = (Determinant (m_sTransform.back()) < 0.0); // flip face orientation

	std::vector< sgpuVector3 > sgpuPositions;
	std::vector< sgpuVector3 > sgpuNormals;
	int numVertFloats=0;
	int numNormalFloats=0;

	XSI::MATH::CTransformation curTm; 
	curTm.SetMatrix4( m_sTransform.back() );
	const XSI::MATH::CMatrix4 &curMat = m_sTransform.back();
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
		numVertFloats = SafeLongToInt( lNumVertFloats );
		assert( (numVertFloats % 3) == 0 );
		sgpuPositions.reserve( numVertFloats/3 );
		for(int i=0 ; i < numVertFloats; i = i + 3 )
		{
			double f[3];
			f[0] = vtxPosArray[ i ];
			f[1] = vtxPosArray[ i + 1];
			f[2] = vtxPosArray[ i + 2];
			XSI::MATH::CVector3 xsipos( f[0], f[1], f[2] );
			xsipos.MulByMatrix4InPlace( curMat );
			sgpuVector3 vec(
							static_cast<float>( xsipos[0] ), 
							static_cast<float>( xsipos[1] ), 
							static_cast<float>( xsipos[2] )
							);
			sgpuPositions.push_back( vec );
#if defined(NEVER)			
			{
				std::wstringstream ss;
				ss <<  "Position " << _tstr(objName) << vec[0] << " " << vec[1] << " " << vec[2] << "\n";
				m_exportDoc.WriteLog( ss.str().c_str() );
			}
#endif
		}
	}
	if( !i_bSubdivMesh )

	{
		// polygon node normals
		XSI::CFloatArray nodeNormalArray;
		st = ga.GetNodeNormals(nodeNormalArray);
		st.AssertSucceeded( _T("GetNodeNormals") );
		LONG lNumNormalFloats = nodeNormalArray.GetCount();
		numNormalFloats = SafeLongToInt( lNumNormalFloats );
		assert( (numNormalFloats % 3) == 0 );
		sgpuNormals.reserve( numNormalFloats/3 );
		for(int i=0 ; i < numNormalFloats; i = i + 3 )
		{
			double f[3];
			f[0] = nodeNormalArray[ i ];
			f[1] = nodeNormalArray[ i + 1];
			f[2] = nodeNormalArray[ i + 2];
			XSI::MATH::CVector3 norm( f[0], f[1], f[2] );
			norm.MulByMatrix4InPlace( nMat );
			sgpuVector3 vec( 
				static_cast< float > ( norm[0] ), 
				static_cast< float > ( norm[1] ), 
				static_cast< float > ( norm[2] )
				);
			sgpuNormals.push_back( vec );
		}
	}
	std::map< XSI::CString, int >::const_iterator cit = m_meshVersusIndexInAnimFileMap.find( fullName );
	if( cit == m_meshVersusIndexInAnimFileMap.end() )
	{		
		std::wstringstream ss;
		ss << L"not exported, " << fullName.GetWideString() << L" as it was not probably visible in the startFrame\n";
		m_exportDoc.WriteLog( ss.str().c_str() );
		return false;
	}
	int nodeIdx = cit->second;

	bool bVal = false;
	bVal = m_scene->WriteAnim( nodeIdx,
		sgpuString( _tstr(objName) ), 
		i_FrameNum, 
		i_bSubdivMesh, 
		numVertFloats/3, 
		numNormalFloats/3, 
		&sgpuPositions[0], 
		&sgpuNormals[0],
		&m_progress
		);
	if( !bVal )
	{
		return false;
	}
	{
		if( i_FrameNum == 0)
		{
			std::wstringstream ss;
			ss << "Exporting " << _tstr(objName) << " meshId = " << m_meshId ;
			ss << " as sgpuMesh " << "nverts= " << numVertFloats/3 << endl; 
			m_exportDoc.WriteLog( ss.str().c_str() );
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GXBVertexAnimExporter::exportNurbsMesh( const XSI::X3DObject & xobj,  int i_FrameNum )
{
	int numSubsets = 0;	
	XSI::Primitive prim = xobj.GetActivePrimitive();
	XSI::NurbsSurfaceMesh xmesh = prim.GetGeometry();
	XSI::CString objName = xobj.GetUniqueName();
	XSI::CString fullName = xobj.GetFullName();
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
		return false;

	}

	XSI::CString mesh_name = xmesh.GetUniqueName();
	XSI::CNurbsSurfaceRefArray nurbsSurf = xmesh.GetSurfaces();
	XSI::CTriangleRefArray tri = xmesh.GetTriangles();
	LONG ulCount = tri.GetCount();
	bool flip = (Determinant (m_sTransform.back()) < 0.0); // flip face orientation

	XSI::MATH::CTransformation curTm; 
	curTm.SetMatrix4( m_sTransform.back() );
	const XSI::MATH::CMatrix4 &curMat = m_sTransform.back();
	//For getting the transform for normals
	//you need to get rid of the translation component
	//and take transpose of inverse
	curTm.SetTranslationFromValues(0,0,0);
	XSI::MATH::CMatrix4 nMat = curTm.GetMatrix4();	
	nMat.InvertInPlace();
	nMat.TransposeInPlace();


	std::vector< sgpuVector3 > sgpuPositions;
	std::vector< sgpuVector3 > sgpuNormals;
	for (LONG i = 0; i < ulCount; i++)
	{
		XSI::Triangle t(tri[i]);

		XSI::CTriangleVertexRefArray triVtx = t.GetPoints();  

		if (triVtx.GetCount() == 3)
		{
			for (LONG j = 0; j < 3; j++)
			{
				XSI::TriangleVertex xvtx(triVtx[j]);
				double f[3];
				f[0] = xvtx.GetPosition()[0];
				f[1] = xvtx.GetPosition()[1];
				f[2] = xvtx.GetPosition()[2];
				XSI::MATH::CVector3 vec( f[0], f[1], f[2] );
				vec.MulByMatrix4InPlace( curMat );

				f[0] = xvtx.GetNormal()[0];
				f[1] = xvtx.GetNormal()[1];
				f[2] = xvtx.GetNormal()[2];
				XSI::MATH::CVector3 norm( f[0], f[1], f[2] );
				norm.MulByMatrix4InPlace( nMat );

				sgpuPositions.push_back( 
					sgpuVector3( 
					static_cast< float >( vec[0] ),
					static_cast< float >( vec[1] ),
					static_cast< float >( vec[2] )
					));
				
				sgpuNormals.push_back( 
					sgpuVector3( 
					static_cast< float >( norm[0] ),
					static_cast< float >( norm[1] ),
					static_cast< float >( norm[2] )
					));
#if defined(NEVER)
				if( j==0 && i==0 )
				{
					std::wstringstream ss;
					ss <<  "Position " << _tstr(objName) << pos[0] << " " << pos[1] << " " << pos[2] << "\n";
					m_exportDoc.WriteLog( ss.str().c_str() );
				}
#endif
			}
		}
	}	


	std::map< XSI::CString, int >::const_iterator cit = m_meshVersusIndexInAnimFileMap.find( fullName );
	if( cit == m_meshVersusIndexInAnimFileMap.end() )
	{		
		std::wstringstream ss;
		ss << L"not exported, " << fullName.GetWideString() << L" as it was not probably visible in the startFrame\n";
		m_exportDoc.WriteLog( ss.str().c_str() );
		return false;
	}
	int nodeIdx = cit->second;
	bool bVal = m_scene->WriteAnim( nodeIdx,
		sgpuString( _tstr( objName ) ), 
		i_FrameNum, 
		EXPORT_AS_SUBDIV, 
		ulCount*3, 
		ulCount*3, 
		&sgpuPositions[0], 
		&sgpuNormals[0],
		&m_progress
		);
	if( !bVal )
	{
		return false;
	}
	{
		if( i_FrameNum == 0)
		{
			std::wstringstream ss;
			ss << "Exporting nurbs " << _tstr(objName) << " meshId = " << m_meshId ;
			ss << " as sgpuMesh " << "nverts= " << ulCount*3 << endl; 
			m_exportDoc.WriteLog( ss.str().c_str() );
		}
	}
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBVertexAnimExporter::dumpProperties(const XSI::X3DObject & xobj)
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
void GXBVertexAnimExporter::dumpProperties(const XSI::Material & xmat)
{
	{
		std::wstringstream ss;
		ss <<  _T("Material  ") <<  _tstr(xmat.GetName()) << _T(" properties\n");
		m_exportDoc.WriteLog( ss.str().c_str() );
	}

	XSI::CRefArray props = xmat.GetProperties();
	for (LONG i = 0; i < props.GetCount(); i++)
	{
		XSI::Property p(props[i]);
		{
			std::wstringstream ss;
			ss <<  _T("Property ") << _tstr(p.GetName()) ;
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
void GXBVertexAnimExporter::dumpParameter(const XSI::Parameter & param)
{
	{
		std::wstringstream ss;
		ss <<  _T("Parameter ") <<  _tstr(param.GetName()) << _T("(") << _tstr(param.GetScriptName()) << _T(")") << _T("type= ") << param.GetValueType() << _T(" val = ") <<  _tstr(param.GetValue().GetAsText()) << _T("\n");
		m_exportDoc.WriteLog( ss.str().c_str() );
	}

	XSI::CParameterRefArray params = param.GetParameters();
	for (LONG k = 0; k < params.GetCount(); k++)
	{
		XSI::Parameter param1(params[k]);
		dumpParameter(param1);
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
sgpuMesh GXBVertexAnimExporter::addMesh(sgpuNode & node, vecVertex_t & vtx, std::vector <int> & idx)
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
XSI::CColor GXBVertexAnimExporter::getCororParameter ( const XSI::Material & xmat, TCHAR* in_szParamName)
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
bool GXBVertexAnimExporter::saveAsOBJ(const TCHAR* name, std::vector <sgpuVertex> & vtx, std::vector <int> & idx)
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
void GXBVertexAnimExporter::buildIndices(std::vector <sgpuVertex> & vtx, std::vector <int> & idx)
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
