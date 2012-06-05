/*****************************************************************************
**  MayaFlagUtil.cpp
**
**      Namespace with functions for getting attribute flags from Maya nodes.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include "MayaUtil.hpp"
#include "MayaFlagUtil.hpp"

#include <maya/MFnDagNode.h>
#include <maya/MPlug.h>
#include <maya/MStatus.h>


namespace
{

}	// end of namespace


//------------------------------------------------------------------------
// Return true if the given attribute exists on the node
//------------------------------------------------------------------------
bool MayaFlagUtil::EngineFlagExists(MFnDagNode &node, MString name)
{
	MStatus status;

	// Note: apparently spaces are removed in attr names!
	MObject	attr = node.attribute(name, &status); 
	MPlug plug( node.object(), attr ); 

	bool val;
	status = plug.getValue( val );
	if ( status ) 
	{
		return true;
	}

	return false;
}

//------------------------------------------------------------------------
// get boolean value from attribute with given name from node.
// there should be no spaces in the attribute name.
//------------------------------------------------------------------------
const int MayaFlagUtil::GetEngineFlag(MFnDagNode &node, MString name)
{
	MStatus status;
	int flag = 0;
	// Note: apparently spaces are removed in attr names!
	MObject	attr = node.attribute(name, &status); 
	/*
	if(!status)
		cout << "Could not find " << name << endl;
	else
		cout << "found attr " << name << endl;
		*/
	MPlug plug( node.object(), attr ); 

	bool val;
	status = plug.getValue( val );
	if ( !status ) 
	{
		//cout << "error getting value from plug";
		flag = 0;
	}
	else {
		flag = val ? 1 : 0;
	}

	return flag;
}

//------------------------------------------------------------------------
// Individual flag tests - encapsulates the strings needed to test
// the flags.
//------------------------------------------------------------------------
bool MayaFlagUtil::GetDoubleSidedFlag(MFnDagNode &node)
{
	// DoubleSided is Maya attribute, don't need to check multiple capitalizations
	return (MayaFlagUtil::GetEngineFlag(node, "doubleSided") != 0);
}
bool MayaFlagUtil::GetCastsShadowsFlag(MFnDagNode &node)
{
	// CastsShadows is Maya attribute, don't need to check multiple capitalizations
	return (MayaFlagUtil::GetEngineFlag(node, "castsShadows") != 0);
}
bool MayaFlagUtil::GetReceiveShadowsFlag(MFnDagNode &node)
{
	// ReceiveShadows is Maya attribute, don't need to check multiple capitalizations
	return (MayaFlagUtil::GetEngineFlag(node, "receiveShadows") != 0);
}
bool MayaFlagUtil::GetShadowHullFlag(MFnDagNode &node)
{
	// 8 variations of capitalizations possible
	if (MayaFlagUtil::EngineFlagExists(node, "sgpuShadowHull"))
		return (MayaFlagUtil::GetEngineFlag(node, "sgpuShadowHull") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "shadowHull"))
		return (MayaFlagUtil::GetEngineFlag(node, "shadowHull") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "ShadowHull"))
		return (MayaFlagUtil::GetEngineFlag(node, "ShadowHull") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "Shadow Hull"))
		return (MayaFlagUtil::GetEngineFlag(node, "Shadow Hull") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "shadow Hull"))
		return (MayaFlagUtil::GetEngineFlag(node, "shadow Hull") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "shadow hull"))
		return (MayaFlagUtil::GetEngineFlag(node, "shadow hull") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "Shadow hull"))
		return (MayaFlagUtil::GetEngineFlag(node, "Shadow hull") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "Shadowhull"))
		return (MayaFlagUtil::GetEngineFlag(node, "Shadowhull") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "shadowhull"))
		return (MayaFlagUtil::GetEngineFlag(node, "shadowhull") != 0);
	else
		return false;
}
bool MayaFlagUtil::GetTriangleSortFlag(MFnDagNode &node)
{
	// 8 variations of capitalizations possible
	if (MayaFlagUtil::EngineFlagExists(node, "sgpuTriangleSort"))
		return (MayaFlagUtil::GetEngineFlag(node, "sgpuTriangleSort") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "triangleSort"))
		return (MayaFlagUtil::GetEngineFlag(node, "triangleSort") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "TriangleSort"))
		return (MayaFlagUtil::GetEngineFlag(node, "TriangleSort") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "Triangle Sort"))
		return (MayaFlagUtil::GetEngineFlag(node, "Triangle Sort") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "triangle Sort"))
		return (MayaFlagUtil::GetEngineFlag(node, "triangle Sort") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "triangle sort"))
		return (MayaFlagUtil::GetEngineFlag(node, "triangle sort") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "Triangle sort"))
		return (MayaFlagUtil::GetEngineFlag(node, "Triangle sort") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "Trianglesort"))
		return (MayaFlagUtil::GetEngineFlag(node, "Trianglesort") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "trianglesort"))
		return (MayaFlagUtil::GetEngineFlag(node, "trianglesort") != 0);
	else
		return false;
}
bool MayaFlagUtil::GetClothFlag(MFnDagNode &node)
{
	if (MayaFlagUtil::EngineFlagExists(node, "sgpuDeformGeom"))
		return (MayaFlagUtil::GetEngineFlag(node, "sgpuDeformGeom") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "sgpuCloth"))
		return (MayaFlagUtil::GetEngineFlag(node, "sgpuCloth") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "Cloth"))
		return (MayaFlagUtil::GetEngineFlag(node, "Cloth") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "cloth"))
		return (MayaFlagUtil::GetEngineFlag(node, "cloth") != 0);
	else
		return false;
}
int MayaFlagUtil::GetResolutionLevel(MFnDagNode &node)
{
	// 8 variations of capitalizations possible
	if (MayaFlagUtil::EngineFlagExists(node, "sgpuLowRes"))
		return (MayaFlagUtil::GetEngineFlag(node, "sgpuLowRes") ? 1 : 2);
	else if (MayaFlagUtil::EngineFlagExists(node, "lowRes"))
		return (MayaFlagUtil::GetEngineFlag(node, "lowRes") ? 1 : 2);
	else if (MayaFlagUtil::EngineFlagExists(node, "LowRes"))
		return (MayaFlagUtil::GetEngineFlag(node, "LowRes") ? 1 : 2);
	else if (MayaFlagUtil::EngineFlagExists(node, "Low Res"))
		return (MayaFlagUtil::GetEngineFlag(node, "Low Res") ? 1 : 2);
	else if (MayaFlagUtil::EngineFlagExists(node, "low Res"))
		return (MayaFlagUtil::GetEngineFlag(node, "low Res") ? 1 : 2);
	else if (MayaFlagUtil::EngineFlagExists(node, "low res"))
		return (MayaFlagUtil::GetEngineFlag(node, "low res") ? 1 : 2);
	else if (MayaFlagUtil::EngineFlagExists(node, "Low res"))
		return (MayaFlagUtil::GetEngineFlag(node, "Low res") ? 1 : 2);
	else if (MayaFlagUtil::EngineFlagExists(node, "Lowres"))
		return (MayaFlagUtil::GetEngineFlag(node, "Lowres") ? 1 : 2);
	else if (MayaFlagUtil::EngineFlagExists(node, "lowres"))
		return (MayaFlagUtil::GetEngineFlag(node, "lowres") ? 1 : 2);
	else
		return 0;
}
bool MayaFlagUtil::GetExportAsSubdivFlag(MFnDagNode &node)
{
	// Not doing all capitalizations here, just some common mistakes.
	if (MayaFlagUtil::EngineFlagExists(node, "sgpuExportAsSubdiv"))
		return (MayaFlagUtil::GetEngineFlag(node, "sgpuExportAsSubdiv") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "exportAsSubdiv"))
		return (MayaFlagUtil::GetEngineFlag(node, "exportAsSubdiv") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "ExportAsSubdiv"))
		return (MayaFlagUtil::GetEngineFlag(node, "ExportAsSubdiv") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "Export As Subdiv"))
		return (MayaFlagUtil::GetEngineFlag(node, "Export As Subdiv") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "Export as Subdiv"))
		return (MayaFlagUtil::GetEngineFlag(node, "Export as Subdiv") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "Export as subdiv"))
		return (MayaFlagUtil::GetEngineFlag(node, "Export as subdiv") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "exportassubdiv"))
		return (MayaFlagUtil::GetEngineFlag(node, "exportassubdiv") != 0);
	else
		return false;
}
bool MayaFlagUtil::GetNoExportFlag(MFnDagNode &node)
{
	// Not doing all capitalizations here, just some common mistakes.
	if (MayaFlagUtil::EngineFlagExists(node, "sgpuNoExport"))
		return (MayaFlagUtil::GetEngineFlag(node, "sgpuNoExport") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "noExport"))
		return (MayaFlagUtil::GetEngineFlag(node, "noExport") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "no Export"))
		return (MayaFlagUtil::GetEngineFlag(node, "no Export") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "No Export"))
		return (MayaFlagUtil::GetEngineFlag(node, "No Export") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "No export"))
		return (MayaFlagUtil::GetEngineFlag(node, "No export") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "noexport"))
		return (MayaFlagUtil::GetEngineFlag(node, "noexport") != 0);
	else
		return false;
}
bool MayaFlagUtil::GetForceExportFlag(MFnDagNode &node)
{
	// Not doing all capitalizations here, just some common mistakes.
	if (MayaFlagUtil::EngineFlagExists(node, "sgpuForceExport"))
		return (MayaFlagUtil::GetEngineFlag(node, "sgpuForceExport") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "forceExport"))
		return (MayaFlagUtil::GetEngineFlag(node, "forceExport") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "force Export"))
		return (MayaFlagUtil::GetEngineFlag(node, "force Export") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "Force Export"))
		return (MayaFlagUtil::GetEngineFlag(node, "Force Export") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "Force export"))
		return (MayaFlagUtil::GetEngineFlag(node, "Force export") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "forceexport"))
		return (MayaFlagUtil::GetEngineFlag(node, "forceexport") != 0);
	else
		return false;
}
bool MayaFlagUtil::GetVisibleAnimFlag(MFnDagNode &node)
{
	// Not doing all capitalizations here, just some common mistakes.
	if (MayaFlagUtil::EngineFlagExists(node, "sgpuVisibleAnim"))
		return (MayaFlagUtil::GetEngineFlag(node, "sgpuVisibleAnim") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "visibleAnim"))
		return (MayaFlagUtil::GetEngineFlag(node, "visibleAnim") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "visible Anim"))
		return (MayaFlagUtil::GetEngineFlag(node, "visible Anim") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "Visible Anim"))
		return (MayaFlagUtil::GetEngineFlag(node, "Visible Anim") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "Visible anim"))
		return (MayaFlagUtil::GetEngineFlag(node, "Visible anim") != 0);
	else if (MayaFlagUtil::EngineFlagExists(node, "visibleanim"))
		return (MayaFlagUtil::GetEngineFlag(node, "visibleanim") != 0);
	else
		return false;
}
