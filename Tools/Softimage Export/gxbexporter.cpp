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

extern XSI::CustomProperty GetImportExportProp();
extern XSI::Parameter GetImportExportOption( const XSI::CString& in_strName );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const	XSI::MATH::CMatrix4 GXBExporter::m_zaxisUpToYaxisUp(1.0, 0.0, 0.0, 0.0,
				0.0, 0.0, -1.0, 0.0,
				0.0, 1.0, 0.0, 0.0,
				0.0, 0.0, 0.0, 1.0 );

	
const XSI::MATH::CMatrix4	GXBExporter::m_invZaxisUpToYaxisUp( 1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, -1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double GXBExporter::Determinant (const XSI::MATH::CMatrix4 & mat)
{
	double entry[16];
	mat.Get(entry[0], entry[1], entry[2], entry[3],
		entry[4], entry[5], entry[6], entry[7],
		entry[8], entry[9], entry[10], entry[11],
		entry[12], entry[13], entry[14], entry[15]);

	double fA0 = entry[ 0]*entry[ 5] - entry[ 1]*entry[ 4];
	double fA1 = entry[ 0]*entry[ 6] - entry[ 2]*entry[ 4];
	double fA2 = entry[ 0]*entry[ 7] - entry[ 3]*entry[ 4];
	double fA3 = entry[ 1]*entry[ 6] - entry[ 2]*entry[ 5];
	double fA4 = entry[ 1]*entry[ 7] - entry[ 3]*entry[ 5];
	double fA5 = entry[ 2]*entry[ 7] - entry[ 3]*entry[ 6];
	double fB0 = entry[ 8]*entry[13] - entry[ 9]*entry[12];
	double fB1 = entry[ 8]*entry[14] - entry[10]*entry[12];
	double fB2 = entry[ 8]*entry[15] - entry[11]*entry[12];
	double fB3 = entry[ 9]*entry[14] - entry[10]*entry[13];
	double fB4 = entry[ 9]*entry[15] - entry[11]*entry[13];
	double fB5 = entry[10]*entry[15] - entry[11]*entry[14];
	double fDet = fA0*fB5-fA1*fB4+fA2*fB3+fA3*fB2-fA4*fB1+fA5*fB0;
	return fDet;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GXBExporter::dumpParameter(const XSI::Parameter & param)
{
	{
		std::wstringstream ss;
		XSI::CValue val = param.GetValue();
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
void GXBExporter::GetAnimParameters( AnimParams &io_Params)
{
	XSI::Application app;
	XSI::Project prj = app.GetActiveProject();
	XSI::CRefArray proplist = prj.GetProperties();
	XSI::Property playctrl( proplist.GetItem(L"Play Control") );
	XSI::CParameterRefArray params = playctrl.GetParameters();
	
	XSI::Parameter param = params.GetItem( XSI::CString( "Rate" ) );
	XSI::CValue val = param.GetValue();
	io_Params.m_FrameRate = static_cast< float > ( double(param.GetValue()) );
	param = params.GetItem( XSI::CString("In"));
	io_Params.m_StartFrame = static_cast< float > ( double(param.GetValue()));
	param = params.GetItem( XSI::CString( "Out" ) );
	io_Params.m_EndFrame = static_cast< float > ( double(param.GetValue()));
	param = params.GetItem( XSI::CString( "Step" ) );
	io_Params.m_StepFrame = static_cast< float > ( double(param.GetValue()));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                     GXBModelExporter class
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GXBExporter::GXBExporter( GXBExportDoc &doc):
m_exportDoc(doc)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GXBExporter::~GXBExporter()
{
}




