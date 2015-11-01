/*****************************************************************************
**  MayaUtil.hpp
**
**      Namespace with handy functions for getting information 
**	from Maya system.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MAYAUTIL_HPP
#error MayaUtil.hpp multiply included
#endif
#define MAYAUTIL_HPP

// define this to make Maya not define bool again
#define _BOOL
//#define REQUIRE_IOSTREAM
#include <iostream>
//using namespace std;

//#include <iostream.h>

#include <maya/MIOStream.h>
#include <maya/MObject.h>
#include <maya/MString.h>
#include <maya/MTime.h>

class MFnDagNode;
class MFnMesh;
class MIntArray;
class MStatus;
class MFnDependencyNode;
class MPlug;

class fsLocator;

namespace MayaUtil
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const char* GetExporterVersion();

	//------------------------------------------------------------------------
	// Make sure given path has filename with given extension, returns 
	//	path with correct extension.
	//------------------------------------------------------------------------
	MString ConfirmExtension(const MString &filename, const char *pExt);

	//------------------------------------------------------------------------
	// Strip off namespaces from node name
	//------------------------------------------------------------------------
	MString PrepareName(const MString &i_Name);

	//------------------------------------------------------------------------
	// Returns true if the given node is currently visible
	//------------------------------------------------------------------------
	const bool DagNodeVisible(MFnDagNode &dagNode);

	//------------------------------------------------------------------------
	// Returns true if the given node is of the given type or if
	//	it has children of the given type.
	//------------------------------------------------------------------------
	const bool HasTypeAsChild(MObject &obj, MFn::Type fs);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void PrintError(const MString str);
	void PrintWarning(const MString str);
	void PrintStatus(const MString str);

	//------------------------------------------------------------------------
	// Sets whether to display error messages in dialog. If quiet, then
	// only display error to console. 
	// Suggest shows optimization suggestions, default is off.
	//------------------------------------------------------------------------
	void SetQuietMode(bool i_bQuiet);
	void SetSuggestMode(bool i_bSuggest);

	//------------------------------------------------------------------------
	// Display message in dialog
	//------------------------------------------------------------------------
	void DisplayError(const MString str);
	void DisplayConfirmation(const MString msg, const MString title);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetWaitCursor();
	void UnsetWaitCursor();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	int GetSimpleTriCount(MFnMesh &mesh);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	int GetTriCount(MFnMesh &mesh, MIntArray &indexTable, MIntArray &nTris);
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void GetPolyCounts(	const int numMaterials, 
						MIntArray &matIndices, 
						const int nPolys, 
						MIntArray &polyCounts);
	
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SortByMaterials( MFnMesh &mesh, 
						  MIntArray &matIndices, 
						  MIntArray &polyCounts, 
						  MIntArray &polyIndices);
	
	//------------------------------------------------------------------------
	// return object connected to node's plug with given name
	//------------------------------------------------------------------------
	MObject GetObjectConnectedToPlug( MFnDependencyNode &node, 
									  const char *name, 
									  MStatus &status);
	
	//------------------------------------------------------------------------
	// debugging routines
	//------------------------------------------------------------------------
	void PrintInputs(MFnDependencyNode &node);
	void PrintPlugs(MFnDependencyNode &node);
	void PrintAttributeTypes(MFnDependencyNode &node);
	void DebugPlug(MPlug &plug);

	//------------------------------------------------------------------------
	// Access the index of a compound or array plug, using either
	//	the logical element or child interface() depending on the plug type.
	//------------------------------------------------------------------------
	MPlug AccessPlugIndex(MPlug &plug, int index, MStatus &status);
	
	//------------------------------------------------------------------------
	// get name of texture from node using plug
	//------------------------------------------------------------------------
	MString GetTextureFileName(MFnDependencyNode &node);

	//------------------------------------------------------------------------
	// get name of effect file from node using plug
	//------------------------------------------------------------------------
	MString GetEffectFileName(MFnDependencyNode &node);

	//------------------------------------------------------------------------
	// Return true if the given attribute exists on the node
	//------------------------------------------------------------------------
	//bool EngineFlagExists(MFnDagNode &node, MString name);

	//------------------------------------------------------------------------
	// get boolean value from attribute with given name from node.
	// there should be no spaces in the attribute name.
	//------------------------------------------------------------------------
	//const int GetEngineFlag(MFnDagNode &node, MString name);

	//------------------------------------------------------------------------
	//	check if the path has the wrong type of slashes (backslashes '\')
	//	in the path.  If so, convert them to forward slashes otherwise noting.
	//------------------------------------------------------------------------
	void ConvertSlashes(MString& io_Path);

	//========================================================================
	// Return the current time for the Maya GUI time slider
	//========================================================================
	MTime GetCurrentTime();
	void SetCurrentTime(MTime time);

	//------------------------------------------------------------------------
	// Find out the frame rate set in the preferences
	//------------------------------------------------------------------------
	float GetCurrentFrameRate();

	//------------------------------------------------------------------------
	//	Create file with given path. Confirms extention. Fills o_Locator
	//	with locator for this file. Returns true if successful.
	//------------------------------------------------------------------------
	bool MUCreateFile(const MString &path, 
					const char *pExtension,
					fsLocator &o_Locator);
}

