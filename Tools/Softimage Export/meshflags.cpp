//*****************************************************************************
/*!	\file meshflags.cpp
\brief Implementation of meshflags property
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


#include "helper.h"
#include "exportmesh_command.h"
#include "gxbmodelexporter.h"
#include "gxbvertexanimexporter.h"
#include "gxbcameraanimexporter.h"
#include "Log.h"

#define LABEL_MIN 140
#define LABEL_RATIO 40
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
/*!	Specify the arguments and return value of the MSPExportMesh command. 
\param in_ctxt The context that encapsulates the command to initialize.
*/
//*****************************************************************************

XSIPLUGINCALLBACK XSI::CStatus MSPMeshflags_Define( XSI::CRef& in_ctxt )
{
	XSI::Context ctxt( in_ctxt );
	XSI::CustomProperty oMeshflags;
	XSI::Parameter oParam;
	oMeshflags = ctxt.GetSource();
	oMeshflags.AddParameter(L"Param",XSI::CValue::siInt4,XSI::siPersistable,L"",L"",0l,0l,100l,0l,100l,oParam);

	
  return XSI::CStatus::OK;
}
//*****************************************************************************
/*!	Implementation of the MSPMeshflags property
*/
//*****************************************************************************
/*
XSIPLUGINCALLBACK XSI::CStatus MSPMeshflags_Execute( XSI::CRef& in_ctxt )
{
	return XSI::CStatus::OK;
}*/

//*****************************************************************************
/*!	Implementation of the MSPExportMesh command. This command uses the 
CGeometryAccessor to extract the data from the geometry to export.
\param in_ctxt The context that encapsulates the command to execute.
*/
//*****************************************************************************

XSIPLUGINCALLBACK XSI::CStatus MSPMeshflags_DefineLayout( XSI::CRef& in_ctxt )
{
	XSI::Context ctxt(in_ctxt);
	XSI::PPGLayout ppg = ctxt.GetSource() ;

	ppg.Clear();

	// define the export section
	ppg.AddGroup(L"MSP mesh flags");

	LONG labelMinPixels = LABEL_MIN;
	LONG labelPercentage = LABEL_RATIO;

	XSI::PPGItem item;
	
	item = ppg.AddItem( L"Param" ) ;
	item.PutLabel(L"foo bar");

	item.PutLabelMinPixels( labelMinPixels );
	item.PutLabelPercentage( labelPercentage );

	ppg.EndGroup();

	return XSI::CStatus::OK;
}

/*
XSIPLUGINCALLBACK XSI::CStatus MSPMeshflags_OnInit( XSI::CRef& in_ctxt )
{
	return XSI::CStatus::OK;
}


XSIPLUGINCALLBACK XSI::CStatus MSPMeshflags_OnChanged( XSI::CRef& in_ctxt )
{
	return XSI::CStatus::OK;
}


XSIPLUGINCALLBACK XSI::CStatus MSPMeshflags_OnClicked( XSI::CRef& in_ctxt )
{
	return XSI::CStatus::OK;
}


XSIPLUGINCALLBACK XSI::CStatus MSPMeshflags_OnClosed( XSI::CRef& in_ctxt )
{
	return XSI::CStatus::OK;
}


XSIPLUGINCALLBACK XSI::CStatus MSPMeshflags_OnTab( XSI::CRef& in_ctxt )
{
	return XSI::CStatus::OK;
}
*/

XSIPLUGINCALLBACK XSI::CStatus MSPMeshflags_PPGEvent( const  XSI::CRef& in_ctxt )
{
	return XSI::CStatus::OK;
}

/*
XSIPLUGINCALLBACK XSI::CStatus MSPMeshflags_Term( XSI::CRef& in_ctxt )
{
	return XSI::CStatus::OK;
}*/