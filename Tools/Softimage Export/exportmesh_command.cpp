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
#include <iostream>

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
#include <xsi_parameter.h>
#include <xsi_vector3f.h>
#include <xsi_vector2f.h>
#include <xsi_oglmaterial.h>
#include <xsi_ogltexture.h>
#include <xsi_kinematics.h>
#include <xsi_triangle.h>
#include <xsi_trianglevertex.h>
#include <xsi_scene.h>
#include <xsi_project.h>

#if !defined(_UNICODE)
#error #msp exporter code will work only if _UNICODE is defined"
#endif

#include "gxbmodelexporter.h"
#include "gxbvertexanimexporter.h"
#include "gxbcameraanimexporter.h"
#include "exportmesh_command.h"
#include "helper.h"
#include "Log.h"
#include "sgpuMatrix.hpp"
#include "sgpuMaterial.hpp"
#include "sgpuMesh.hpp"
#include "sgpuMeshConstructor.hpp"
#include "sgpuSubdiv.hpp"
#include "sgpuSubdivConstructor.hpp"
#include "sgpuNode.hpp"
#include "sgpuException.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace std;
extern XSI::CustomProperty GetImportExportProp();
extern XSI::Parameter GetImportExportOption( const XSI::CString& in_strName );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GXBExportDoc::GXBExportDoc( XSI::CString & filename, XSI::CValueArray args )
{
	m_exportIntent = (TExportIntent)(LONG)args[0];	
	m_subdType = (XSI::siSubdivisionRuleType)(LONG)args[1];	
	m_subdLevel = args[2];
	m_filename = filename;
	m_textureExportFilepath = (Texture_ExportFilepath::Value)(LONG)args[3];
	m_mergeBasedOnMtls = (MergeBasedOnMtls::Value)(bool)args[4];
	m_maxNumTrianglesInMergedMesh = (MaxNumTrianglesInMergedMesh::Value)(LONG)args[5];
	m_exportGeomForVertexAnim = ( ExportGeomForVertexAnim::Value)(bool)args[6];
	m_animExportStartFrame = (AnimExportStartFrame::Value) args[7];
	m_animExportEndFrame = (AnimExportEndFrame::Value) args[8];
	
	ResolveFromExportIntent();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GXBExportDoc::~GXBExportDoc()
{

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GXBExportDoc::DoExport()
{
	bool r = true;
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	_tsplitpath( _tstr(m_filename), drive, dir, fname, ext );

	try
	{


		//try to start a log
		//warn if cannot;
		XSI::CString logname = drive;
		logname += dir;
		logname += fname;
		logname += _T(".log");
		r = m_log.StartLog( _tstr(logname), _T("Export into StudioGPU format") );
		if( !r )
		{
			cerr << L"Warning: cannot open log file: " << _tstr( logname ) << std::endl;
		}

		//instatiate the appropriate exporter
		//also adjust the output file extension accordingly
		ResolveFromExportIntent();

		//comupte the output file name
		XSI::CString newFileName = drive;
		newFileName += dir;
		newFileName += fname;
		newFileName += m_newExt;
		m_filename = newFileName;
		const TCHAR *pzname = _tstr( m_filename );
		//core export
		r = m_exporter->DoExport( );
	}
	catch( sgpuException &sgpuEx )
	{	
		const sgpuString &sgpuDesc = sgpuEx.m_Description;
		std::wstring wdesc;
		GetWString( wdesc, sgpuDesc );
		cerr << wdesc.c_str() << endl;
		WriteLog( wdesc.c_str() );
		r = false;
	}
	catch ( std::runtime_error &exc )
	{
		cerr << exc.what() << endl;
		WriteLog( "std::runtime_error exception %s", exc.what() );
		r = false;
	}
	catch ( ... )
	{
		WriteLog( L"abnormal exception while trying to export to %s\n" , fname );
		r = false;
	}
	return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBExportDoc::InitializeProgressBar( int numSteps )
{
	XSI::Application app;
	XSI::UIToolkit kit = app.GetUIToolkit();
	m_bar = kit.GetProgressBar();

	m_bar.PutMaximum( numSteps );
	m_bar.PutStep( 1 );
	m_bar.PutVisible( true );
	m_bar.PutCaption( _T("Exporting...") );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//From the export intent, fill in the other members

 void GXBExportDoc::ResolveFromExportIntent()
 {
	switch( m_exportIntent)
		{
		case eVertexAnimExport:
			m_exporter.reset( new GXBVertexAnimExporter(*this));
			m_newExt = XSI::CString( _T(".gab") );
			m_constMode = XSI::siConstructionMode::siConstructionModeAnimation;
			break;
		case eCameraAnimExport:			
			m_exporter.reset( new GXBCameraAnimExporter(*this));
			m_newExt = XSI::CString( _T(".cam") );
			m_constMode = XSI::siConstructionMode::siConstructionModeAnimation;
			break;
		default:
		case eModelExport:
			m_exporter.reset( new GXBModelExporter(*this, m_exportGeomForVertexAnim ));
			m_newExt = XSI::CString( _T(".gxb") );
			m_constMode = m_constMode = XSI::siConstructionMode::siConstructionModeModeling;
			break;
		}
 }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBExportDoc::UpdateStep( int step )
{
	m_bar.Increment( step );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBExportDoc::UpdateCaption( const TCHAR *pzNewCaption )
{
	m_bar.PutCaption( pzNewCaption );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*delgate to m_log to do the writing
*/
void GXBExportDoc::WriteLog( const wchar_t * format, ...  )
{
	va_list va;
	static CharacterBuffer<1024, wchar_t > tempCharBuf(1024);
	va_start( va, format );
	vswprintf( tempCharBuf.GetBuffer(), format, va );
	va_end( va );
	m_log.WriteLog( tempCharBuf.GetBuffer() );
}

void GXBExportDoc::WriteLog( const char * format, ...  )
{
	va_list va;
	static CharacterBuffer<1024, char > tempCharBuf(1024);
	va_start( va, format );
	vsprintf( tempCharBuf.GetBuffer(), format, va );
	va_end( va );
	m_log.WriteLog( tempCharBuf.GetBuffer() );
}
const XSI::CString GXBExportDoc::Texture_ExportFilepath::m_friendlyName = L"Texture ExportFilepath";
const XSI::CString GXBExportDoc::Texture_ExportFilepath::m_scriptName = L"Texture_ExportFilepath";
const XSI::CString	GXBExportDoc::Texture_ExportFilepath::m_valueNames[3] = { L"Relative Path",  L"Absolute Path", L"Copy Texture To Export Dir"};
const GXBExportDoc::Texture_ExportFilepath::Value GXBExportDoc::Texture_ExportFilepath::m_default = GXBExportDoc::Texture_ExportFilepath::eRelative;
const GXBExportDoc::Texture_ExportFilepath::Value GXBExportDoc::Texture_ExportFilepath::m_min = GXBExportDoc::Texture_ExportFilepath::eRelative;
const GXBExportDoc::Texture_ExportFilepath::Value GXBExportDoc::Texture_ExportFilepath::m_max = GXBExportDoc::Texture_ExportFilepath::eCopyTextureToExportDir;

const XSI::CString GXBExportDoc::MergeBasedOnMtls::m_friendlyName = L"Merge Geometry By Materials";
const XSI::CString GXBExportDoc::MergeBasedOnMtls::m_scriptName = L"MergeBasedOnMtls";
const XSI::CString	GXBExportDoc::MergeBasedOnMtls::m_valueNames[1] = { L"Merge Geometry By Materials" };
const GXBExportDoc::MergeBasedOnMtls::Value GXBExportDoc::MergeBasedOnMtls::m_default = false;

const XSI::CString GXBExportDoc::MaxNumTrianglesInMergedMesh::m_friendlyName = L"Maximum Number of Triangles In Merged Mesh";
const XSI::CString GXBExportDoc::MaxNumTrianglesInMergedMesh::m_scriptName = L"MaxNumTrianglesInMergedMesh";
const XSI::CString	GXBExportDoc::MaxNumTrianglesInMergedMesh::m_valueNames[1] = { L"MaxNumTrianglesInMergedMesh"};
const GXBExportDoc::MaxNumTrianglesInMergedMesh::Value GXBExportDoc::MaxNumTrianglesInMergedMesh::m_default = 30000;
const GXBExportDoc::MaxNumTrianglesInMergedMesh::Value GXBExportDoc::MaxNumTrianglesInMergedMesh::m_min = 100;
const GXBExportDoc::MaxNumTrianglesInMergedMesh::Value GXBExportDoc::MaxNumTrianglesInMergedMesh::m_max = 1000000;


const XSI::CString GXBExportDoc::ExportGeomForVertexAnim::m_friendlyName = L"Export Geometry For Vertex Animation";
const XSI::CString GXBExportDoc::ExportGeomForVertexAnim::m_scriptName = L"ExportGeomForVertexAnim";
const XSI::CString	GXBExportDoc::ExportGeomForVertexAnim::m_valueNames[1] = { L"Export Geometry For Vertex Animation" };
const GXBExportDoc::ExportGeomForVertexAnim::Value GXBExportDoc::ExportGeomForVertexAnim::m_default = false;

const XSI::CString GXBExportDoc::AnimExportStartFrame::m_friendlyName = L"StartFrame For Animation Export";
const XSI::CString GXBExportDoc::AnimExportStartFrame::m_scriptName = L"AnimExportStartFrame";
const XSI::CString	GXBExportDoc::AnimExportStartFrame::m_valueNames[1] = { L"StartFrame For Animation Export"};
const GXBExportDoc::AnimExportStartFrame::Value GXBExportDoc::AnimExportStartFrame::m_default = 0;

const XSI::CString GXBExportDoc::AnimExportEndFrame::m_friendlyName = L"EndFrame For Animation Export";
const XSI::CString GXBExportDoc::AnimExportEndFrame::m_scriptName = L"AnimExportEndFrame";
const XSI::CString	GXBExportDoc::AnimExportEndFrame::m_valueNames[1] = { L"EndFrame For Animation Export"};
const GXBExportDoc::AnimExportEndFrame::Value GXBExportDoc::AnimExportEndFrame::m_default = 0;

//*****************************************************************************
/*!	Specify the arguments and return value of the MSPExportMesh command. 
\param in_ctxt The context that encapsulates the command to initialize.
*/
//*****************************************************************************

XSIPLUGINCALLBACK XSI::CStatus MSPExportMesh_Init( XSI::CRef& in_ctxt )
{
	XSI::Context ctxt( in_ctxt );
	XSI::Command oCmd;
	oCmd = ctxt.GetSource();

	// Specify that the command returns a value
	oCmd.EnableReturnValue(true);

	// Add arguments to the command
	XSI::ArgumentArray oArgs;
	oArgs = oCmd.GetArguments();

	oArgs.Add(L"ExportIntent", (LONG)eModelExport);

	// Subdivision algorithm to use. By default, CatmullClark
	oArgs.Add(L"Subd_type",(LONG)XSI::siLinearSubdivision);

	// Subdivision level
	oArgs.Add(L"Subd_level", (LONG)0);
	
	oArgs.Add(GXBExportDoc::Texture_ExportFilepath::m_scriptName, (LONG) GXBExportDoc::Texture_ExportFilepath::m_default );

	oArgs.Add(GXBExportDoc::MergeBasedOnMtls::m_scriptName, (bool) GXBExportDoc::MergeBasedOnMtls::m_default );

	oArgs.Add(GXBExportDoc::MaxNumTrianglesInMergedMesh::m_scriptName, (LONG) GXBExportDoc::MaxNumTrianglesInMergedMesh::m_default );

	oArgs.Add(GXBExportDoc::ExportGeomForVertexAnim::m_scriptName, (bool) GXBExportDoc::ExportGeomForVertexAnim::m_default );
	
	oArgs.Add(GXBExportDoc::AnimExportStartFrame::m_scriptName, (LONG) GXBExportDoc::AnimExportStartFrame::m_default );
	
	oArgs.Add(GXBExportDoc::AnimExportEndFrame::m_scriptName, (LONG) GXBExportDoc::AnimExportEndFrame::m_default );
	
	return XSI::CStatus::OK;
}

//*****************************************************************************
/*!	Implementation of the MSPExportMesh command. This command uses the 
CGeometryAccessor to extract the data from the geometry to export.
\param in_ctxt The context that encapsulates the command to execute.
*/
//*****************************************************************************

XSIPLUGINCALLBACK XSI::CStatus MSPExportMesh_Execute( XSI::CRef& in_ctxt )
{
	// Unpack the command argument values
	XSI::Context ctxt( in_ctxt );
	XSI::CValueArray args = ctxt.GetAttribute(L"Arguments");

	// prepare the output text file
	XSI::CString strOut = GetImportExportOption( L"ExportFilename" ).GetValue();		

	// A 3d object with a mesh geometry must be selected
	XSI::Application app;	

	GXBExportDoc exp(strOut, args);

	bool r = exp.DoExport();

	return r ? XSI::CStatus::OK : XSI::CStatus::Fail;
}
