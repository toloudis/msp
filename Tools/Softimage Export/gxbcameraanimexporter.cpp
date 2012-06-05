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
#include <xsi_math.h>
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
#include "exportmesh_command.h"
#include "sgpuCameraAnimExportScene.hpp"
#include "sgpuException.hpp"
#include "sgpuString.hpp"



struct VertexAnimParams
{
	VertexAnimParams():
m_StartFrame(0),
m_EndFrame(0),
m_StepFrame(0),
m_FrameRate(0)
{}
float	m_StartFrame;
float	m_EndFrame;
float	m_StepFrame;
float	m_FrameRate;
};

#include "gxbcameraanimexporter.h"
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern XSI::CustomProperty GetImportExportProp();
extern XSI::Parameter GetImportExportOption( const XSI::CString& in_strName );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const float GXBCameraAnimExporter::m_vertexCacheLimit = 2.0f *  1024*1024*1024;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GXBCameraAnimExporter::GXBCameraAnimExporter( GXBExportDoc &doc):
GXBExporter(doc)
{
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GXBCameraAnimExporter::~GXBCameraAnimExporter()
{
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int GXBCameraAnimExporter::SetCurrentFrame( int i_KeyNum )
{
	int nKeys = m_scene->GetNumFrames();
	if( i_KeyNum > nKeys )
	{

		std::wstringstream ss;
		ss << L"cannot step nto frame" << i_KeyNum << L"\n";
		m_exportDoc.WriteLog( ss.str().c_str() );
		return -1;
	}
	float targetFrame = m_scene->ComputeIthFrame( i_KeyNum );

	XSI::Application app;	
	XSI::Project prj = app.GetActiveProject();
	XSI::CRefArray proplist = prj.GetProperties();	
	XSI::Property playctrl( proplist.GetItem(L"Play Control") );
	playctrl.PutParameterValue(_T("Current"),  targetFrame );
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GXBCameraAnimExporter::DoExport()
{
	bool r = true;
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	_tsplitpath( _tstr(m_exportDoc.m_filename), drive, dir, fname, ext );
	AnimParams vParams;
	GetAnimParameters( vParams );
	m_scene.reset( new sgpuCameraAnimExportScene( 
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

	float fProgressReportBudget = static_cast< float > ( m_scene->GetEstimate() ) ;
	if( fProgressReportBudget >= m_vertexCacheLimit )
	{
		return false;
	}
	m_exportDoc.InitializeProgressBar( static_cast<int>(fProgressReportBudget) );

	m_sTransform.clear();
	XSI::MATH::CMatrix4 m;
	m.SetIdentity();
	m_sTransform.push_back( m );

	int numKeys = m_scene->GetNumFrames();
	m_exportDoc.InitializeProgressBar( numKeys );

	for (int i=0; i < numKeys; ++i )
	{

		int res = SetCurrentFrame( i );
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
	sgpuString exportFilename( _tstr(m_exportDoc.m_filename) );
	m_scene->WriteAnim( exportFilename, &m_progress );
	return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBCameraAnimExporter::enumProps( const XSI::X3DObject & xobj )
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
bool  GXBCameraAnimExporter::exportObject(const XSI::X3DObject & xobj, int i_FrameNum )
{
	if (!xobj.IsValid()) return false;

	XSI::Property propVis(xobj.GetProperties().GetItem(L"Visibility"));
	bool visible = propVis.GetParameter(L"viewvis").GetValue();
	XSI::CString obj_name = xobj.GetUniqueName();
	const TCHAR * tObjectName = _tstr(obj_name);
	XSI::Primitive prim = xobj.GetActivePrimitive();	
	XSI::siClassID cid =  xobj.GetClassID();
	if(  cid == XSI::siCameraID  && m_cameraName.IsEmpty() )
	{
		m_cameraName = obj_name;
		m_scene->SetName( sgpuString(_tstr( m_cameraName ) ));
	}
	if (  cid == XSI::siCameraID  && m_cameraName == obj_name )
	{
		XSI::Kinematics kin = xobj.GetKinematics().EvaluateAt( i_FrameNum );
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
			{
				std::wstringstream ss;
				ss << obj_name.GetWideString() << std::endl;
				m_exportDoc.WriteLog( ss.str().c_str() );
			}
			exportCamera(xobj, i_FrameNum );
		} 
		catch ( ... )
		{
			{
				std::wstringstream ss;
				ss << L"Export of \'" << obj_name.GetWideString() << L"\' node failed\n";
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int GXBCameraAnimExporter::exportCamera(const XSI::X3DObject & xobj, int i_FrameNum  )
{
	int numSubsets = 0;
	XSI::Primitive prim = xobj.GetActivePrimitive();
	XSI::CString fullName = xobj.GetFullName();
	XSI::CString mesh_name = xobj.GetName();
	XSI::CString objName = xobj.GetUniqueName();
	XSI::siClassID cid =  xobj.GetClassID();
	assert (  cid == XSI::siCameraID );
	XSI::CParameterRefArray params = prim.GetParameters();
	for( LONG iProp =0; iProp < params.GetCount(); ++iProp )
	{
		XSI::CRef prop = params[iProp];
		dumpParameter( prop );
		XSI::CString cprop = prop.GetAsText();
		const char *pzProp = cprop.GetAsciiString( );
		int n=0;
	}
	XSI::Parameter param = params.GetItem(L"proj");
	assert( param.IsValid() );
	XSI::CValue val = param.GetValue();
	int projectionType = int(param.GetValue());
	if( projectionType == 0 )
	{
		//ortho camera
		return -1;
	}
	XSI::CStatus st;
	param = params.GetItem( L"fov" );
	assert( param.IsValid() );
	val = param.GetValue();
	double dfov = double( param.GetValue() );
	float ffov = static_cast< float > ( XSI::MATH::DegreesToRadians( dfov ) );
	param = params.GetItem( L"near" );
	assert( param.IsValid() );
	val = param.GetValue();
	float fnear = static_cast< float > ( double( param.GetValue() ) );
	param = params.GetItem(L"far");
	assert( param.IsValid() );
	val = param.GetValue();
	float ffar = static_cast< float > ( double( param.GetValue() ) );
	param = params.GetItem(L"aspect");
	assert( param.IsValid() );
	val = param.GetValue();
	float faspect = static_cast< float > ( double( param.GetValue() ) );
	param = params.GetItem(L"projplanedist");
	assert( param.IsValid() );
	val = param.GetValue();
	if( EpsilonEqualZero( faspect ) )
	{
		return -1;
	}
	float ffocalLength = static_cast< float > ( double( param.GetValue() ) );
	float fhorizAperture = ffocalLength * (  2 *  tanf( ffov /2.0f ) );
	float fvertAperture = fhorizAperture/faspect;
	sgpuCameraFrame sgpuFrame;
	sgpuFrame.m_FocalLength = ffocalLength * 25.4f ;
	sgpuFrame.m_CenterOfInterest = 0.0;
	sgpuFrame.m_HorizFilmAperture = fhorizAperture  ;
	sgpuFrame.m_VertFilmAperture = fvertAperture ;

	XSI::MATH::CMatrix4 &curTm = m_sTransform.back();
	XSI::MATH::CTransformation trans;
	trans.SetMatrix4( curTm );
	XSI::MATH::CVector3 translation(trans.GetTranslation());
	XSI::MATH::CRotation rotation(trans.GetRotation() );
	XSI::MATH::CRotation::RotationOrder ro = rotation.GetRotationOrder();
	assert( ro == XSI::MATH::CRotation::siXYZ );
	XSI::MATH::CVector3 xyzAngles = rotation.GetXYZAngles();
	sgpuFrame.m_Translation = sgpuVector3( 
		static_cast< float > ( translation[0] ), 
		static_cast< float > ( translation[1] ), 
		static_cast< float > ( translation[2] )
		);
	sgpuFrame.m_Rotation = sgpuVector3( 
		static_cast< float > ( xyzAngles[0] ), 
		static_cast< float > ( xyzAngles[1] ), 
		static_cast< float > ( xyzAngles[2] )
		);
	m_scene->SetFrame( i_FrameNum, sgpuFrame );
	return 0;
}




