/*****************************************************************************
**  MayaUtil.cpp
**
**      Namespace with handy functions for getting information 
**	from Maya system.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <MayaFlagUtil.hpp>

#include <maya/MCommandResult.h>
#include <maya/MDagPath.h>
#include <maya/MFnBlendShapeDeformer.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>
#include <maya/MFnSet.h>
#include <maya/MFnStringData.h>
#include <maya/MFnTransform.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MGlobal.h>
#include <maya/MIntArray.h>
#include <maya/MItDag.h>
#include <maya/MItSelectionList.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MSelectionList.h>
#include <maya/MStatus.h>

#include <string.h>

#undef CreateFile
#undef DeleteFile
#include "Core/fs/fsFileUtil.hpp"

namespace
{
// History:
// 2.8 - Support for subdivision surfaces
// 2.83 - Support for triangle sort and double-sided flags
// 2.84 - Fix for joint influences that wrap from end of chain back to beginning (log_ind % nInfs)
// 2.85 - More fixes for joint influences out of range (using matrix logical inds to get remap)
// 2.9 - Flags for meshes and subdivs in sync. Stopped doing stencil shadow welding.
//		- "file" command line arguments to many commands
//		- pass over "subdModifier" nodes to find blend shapes
// 2.10.0 - adding .cha animation export (writeCharacter -anim)
// 2.10.1 - write rotation order to animation files
// 2.10.2 - fixed forward vs back slash problem for scene funcs
// 2.10.3 - added back slash converter function
// 2.10.4 - add -subanim and -pose args to writeCharacter
// 2.10.5 - added writeCamera command
// 2.10.6 - write node names to animation files
// 2.10.7 - -expression arg to writeCharacter, write a delta sub animation to be additive
// 2.11.0 - 32-bit indices support
// 2.11.1 - can mix subdiv and meshes attached to joint hierarchy allowed in WriteCharacters
// 2.12.0 - vertex animation for meshes
// 2.12.1 - fix for shading groups (lots of shaders attached, but only one used per mesh)
// 2.13.0 - converted to Maya 8.5
// 2.13.1 - built with VS2005 (8.0)
// 2.14.0 - all exporting is based on selection, not visibilty
// 2.14.1 - exporting frame rate in animation files
// 2.14.2 - exports scale orientation of joints and total transform as matrix
// 2.14.3 - exports LOD meshes in joint trees
// 2.14.4 - subdivision vertex animation added
// 2.14.5 - blend shapes for polygon meshes also
// 2.15.0 - export particle animation
// 2.15.1 - options for exporting polygon meshes as subdiv control meshes
// 2.15.2 - export visiblity anim channels
// 2.15.3 - use Maya time slider's range to clamp and offset animation data
// 2.15.4 - added forceVerts for baking polygon animation into vertex animation
//			- also, strip namespaces off of name (split at colon)
// 2.15.5 - bake vertex animation automatically when cloth flag is set true
// 2.15.6 - don't write nórmals on vertex anim of polygon meshes that will be subdivs
//			- also, write low resolution as 3-way state, not just boolean
// 2.15.7 - always write mesh flags in order to get all 3 LOD states out
// 2.15.8 - threads CHA cloth animation so that only one pass through the timeline is made
// 2.16.0 - bakes joint transforms, blend shape and visibility animation also in one pass through timeline
// 2.16.1 - storing MDagPath instead of MFnMesh when baking vertex anim
// 2.16.2 - writing current total transformation as bind pose
// 2.16.3 - more robust forceVerts - uses dagPath instead of mesh handle when altering Maya timeline
// 2.16.4 - write exclusive matrix for transformation above root joint
// 2.16.5 - vertex baking is object coords for writeCharacter and world coords for forceVerts,
//			also, horizontal and vertical film aperture written to camera animation
// 2.16.6 - write morph target deltas from plugs when blend shape targets have been deleted
//
// 3.0.0 - sgpuMachExporter reorganization based on LibXLT's mdlWriter
// 3.0.1 - added forceExport flag, export pivots
// 3.0.2 - better searching for blend shapes, exporting bind poses of surfaces also
// 3.0.3 - exports begin frame, displays progress window, exports leaf joints
// 3.0.4 - fixed bug with polygon meshes with multiple materials, more polyBlindData searching
// 3.0.5 - don't export morph deltas if we already have target of same name
// 3.0.6 - exporting pivot translation fields
// 3.0.7 - creating sgpuWriteVerts to bring forceVerts style to .gxb/.gab files,
//			also writes "auto-gen low res" flag for subdivisions not marked "high-res"
// 3.0.8 - removes namespaces from surface names, warns when UVs are degenerate
// 3.0.9 - writes bbox anim when doing sgpuWriteVerts to help with attachment

	// *** Dropping version to go to align with MachStudioPro ***
// 0.9.0 - writes fullpaths to textures into geometry file, limit to vertex cache size, 
//			dropping sgpuWriteParticles
// 0.9.1 - adds "-static2 flag to sgpuWriteVerts to export world space meshes, no hierarchy
//			 looks for unique naming and consistent ordering in sgpuWriteVerts
// 1.0.0 - upping versino to match with initial product release
// 1.1.0 - baking camera animation in world space, source sgpuMenu.mel
// 1.1.1 - Handling static world space better with no skinning info wrapper,
//			removes namespaces from material names
// 1.1.2 - Progress dialog displayed earlier for "writeVerts -anim",
//			"-confirm" option added to be able to skip "confirmation of flags" dialog.
// 1.1.3 - Reversing polygon ordering (CCW vs CW) when negative scale applied.
// 1.1.4 - Saving MStrings as UTF8 instead of locale's multi-byte
//			baking visibility of meshes in sgpuWriteVerts if mesh has "visibleAnim" flag on
// 1.1.5 - Support for non-lambert surface shaders, exports named grey material as substitute
// 1.1.6 - Warns if there is no alias for a blend shape,
//			Exports shading group name when no surface shader node is found.
// 1.1.7 - Fix for when UVs are not complete on a mesh (some vertices had -1 as UV index)
// 1.1.8 - Rebuilt with VS2005 to fix CRT DLL issues.
// 1.1.9 - Faster exporting by buffered file writes and exporting arrays of vertex anim data at once
// 1.2.0 - Multiple material per subdiv, Mesh/Node instancing, 
//			quiet mode, "sgpu" prefix for flags, exception handling, no double-sided confirmation,
//			"visibleAnim" assumed true for WriteVerts
// 1.2.1 - Fix for static subdivs, sourcing new msExportUI.mel
// 1.2.1.1 - Rebuilt without security patch (moved manifest CRT version down to 762)
// 1.2.1.2 - Using _USE_RTM_VERSION compiler flag to resolve CRT issues in VS2005 SP1,
//			added sgpuDeformGeom
// 1.2.1.3 - Raised vertex animation file limit to 2GB 
// 1.3.0.0 - Auto detect deforming geometry in sgpuWriteModel.
//			changed "-bake" to be default behavior, added "-noBake" to turn it off
// 1.3.6.0 - Vertex animation compression for sgpuWriteVerts and sgpuWriteModel
// 1.3.6.1 - Exporting "Interaxial Separation" and "Zero parallax" attributes of stereo cameras. 
//			"-merge" argument merges meshes by material for faster rendering
// 1.3.9.0 - Export Blinn shaders and specular color maps
// 1.3.9.1 - changing Blinn eccentricity exporting to shininess
// 1.4.0.0 - Setting version to match release of MachStudio 1.4.0

//bga - Note, this version has to be updated in the Windows resource file also!
const char* c_ExporterVersion = "1.4.0.0";

//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool l_bQuietMode = false;
bool l_bSuggestMode = false;

//------------------------------------------------------------------------
//------------------------------------------------------------------------
const bool dagnode_visible_inclusive(MObject &obj)
{
	bool visible = true;
	while(1) {
		MFnDagNode node(obj);
		if(!MayaUtil::DagNodeVisible(node)) {
			visible = false;
			break;
		}
		if(node.parentCount() > 0) {
			obj = node.parent(0);
			//node = MFnDagNode(parentObject); //will this cause a leak?
		}
		else {
			break;
		}
	}
	return visible;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
const bool dagnode_visible_inclusive(MFnDagNode &dagNode)
{
	return dagnode_visible_inclusive(dagNode.object());
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
const bool has_mesh_children(MFnDagNode &node, MFn::Type fs)
{
	static int level = 0;
	level++;

	if(level >= 500) { //to prevent infinite recursion -- will be hosed by simultaneous
		level--;
		return false;
	}

	int numChildren = node.childCount();

	MStatus status;
	for(int i = 0; i < numChildren; i++) {
		MObject obj = node.child(i);

		// Is object of given type?
		if (obj.hasFn(fs)) {
			// Used to check visibility, but not anymore...
			//if(dagnode_visible_inclusive(obj)) {
				level--;
				return true;
			//}
		}
		else {
			MFnTransform child(obj, &status);		
			if(status == MS::kSuccess) {
				if(has_mesh_children(child,fs)) {
					level--;
					return true;
				}
			}
		}
	}

	level--;
	return false;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
const bool has_mesh_children(MObject &obj, MFn::Type fs)
{

	MStatus status;

	MFnDagNode dagNode(obj, &status);

	if(status == MS::kSuccess) {
		return has_mesh_children(dagNode, fs);
	}
	else {
		return false;
	}
}

}	// end of namespace


//------------------------------------------------------------------------
//------------------------------------------------------------------------
const char* MayaUtil::GetExporterVersion()
{
	return c_ExporterVersion;
}

//------------------------------------------------------------------------
// Make sure given path has filename with given extension, returns 
//	path with correct extension.
//------------------------------------------------------------------------
MString MayaUtil::ConfirmExtension(const MString &filename, const char *pExt)
{
	MStatus status;

	char	newPath[ _MAX_DIR ];
	char	drive[ _MAX_DRIVE ];
	char	dir[ _MAX_DIR ];
	char	fileName[ _MAX_FNAME ];
	char	ext[ _MAX_EXT ];

	_splitpath(filename.asUTF8(), drive, dir, fileName, ext);

	//_makepath(newPath, NULL, NULL, fileName, pExt);
	_makepath(newPath, drive, dir, fileName, pExt);

	return MString(newPath);
}

//------------------------------------------------------------------------
// Strip off namespaces from node name
//------------------------------------------------------------------------
MString MayaUtil::PrepareName(const MString &i_Name)
{
	MStringArray splits;
	i_Name.split(':', splits);
	if (splits.length() == 0)
		return i_Name;
	else
		return splits[splits.length()-1];
}


//------------------------------------------------------------------------
// Returns true if the given node is currently visible
//------------------------------------------------------------------------
const bool MayaUtil::DagNodeVisible(MFnDagNode &dagNode)
{
	bool visible, override, intermediate;
	MPlug vPlug = dagNode.findPlug( "visibility" );
	MPlug iPlug = dagNode.findPlug( "intermediateObject" );
	MPlug overridePlug = dagNode.findPlug( "overrideVisibility" );

	vPlug.getValue( visible );
	iPlug.getValue( intermediate );
	overridePlug.getValue( override );

	return (visible && override) && !intermediate;
}

//------------------------------------------------------------------------
// Returns true if the given node is of the given type or if
//	it has children of the given type.
//------------------------------------------------------------------------
const bool MayaUtil::HasTypeAsChild(MObject &obj, MFn::Type fs)
{
	MStatus status;

	if(obj.hasFn(fs)) {
		return true;
	}

	MFnDagNode dagNode(obj, &status);

	if(status == MS::kSuccess) {
		int forceOutput = MayaFlagUtil::GetEngineFlag(dagNode, "ForceOutput");
		//cout << "forceOutput = " << forceOutput << endl;
		if (forceOutput) return true;

		return has_mesh_children(dagNode,fs);
	}
	else {
		return false;
	}
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void MayaUtil::PrintError(const MString str)
{
	cout << "Error: " << str << endl;
	MGlobal::displayError(str);
}


void MayaUtil::PrintWarning(const MString str)
{
	cout << "Warning: " << str << endl;
	MGlobal::displayWarning(str);
}


void MayaUtil::PrintStatus(const MString str)
{
	//these are unfortunately buffered until the command ends

	//MGlobal:: executeCommand(MString("print \"") + str + "\"");
	MGlobal::displayInfo(str);
}

//------------------------------------------------------------------------
// Sets whether to display error messages in dialog. If quiet, then
// only display error to console.
//------------------------------------------------------------------------
void MayaUtil::SetQuietMode(bool i_bQuiet)
{
	l_bQuietMode = i_bQuiet;
}
void MayaUtil::SetSuggestMode(bool i_bSuggest)
{
	l_bSuggestMode = i_bSuggest;
}

//------------------------------------------------------------------------
// Display message in dialog
//------------------------------------------------------------------------
void MayaUtil::DisplayError(const MString str)
{
	if (l_bQuietMode)
		MayaUtil::PrintError(str);
	else
		MGlobal:: executeCommand(MString("confirmDialog -m \"") + str + "\"");
}
void MayaUtil::DisplayConfirmation(const MString msg, const MString title)
{
	if (l_bQuietMode)
	{
		cout << title << ": " << endl;
		cout << msg << endl;
	}
	else
		MGlobal:: executeCommand(MString("confirmDialog -t \"") + title + "\" -m \"" + msg + "\"");
	
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void MayaUtil::SetWaitCursor()
{
	MGlobal::executeCommand("waitCursor -state on");
}

void MayaUtil::UnsetWaitCursor()
{
	MGlobal::executeCommand("waitCursor -state off");
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
int MayaUtil::GetSimpleTriCount(MFnMesh &mesh)
{
	int total = 0;
	int nPolys = mesh.numPolygons();
	for(int i = 0; i < nPolys; i++) {
		int nVerts = mesh.polygonVertexCount(i);
		if(nVerts > 3) {
			total += nVerts - 2;
		}
		else {
			total += 1;
		}
	}
	return total;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
int MayaUtil::GetTriCount(MFnMesh &mesh, MIntArray &indexTable, MIntArray &nTris)
{

	MObjectArray shaders;
	MIntArray indices;

	mesh.getConnectedShaders(0, shaders, indices);  //hmmm, how to handle instances (using 0 for now)
	cout << "num indices: " << indices.length() << endl;

	if(nTris.length() > 0)
		nTris.clear();

	//Note: In situations where no shader is attached, the indices list will be 
	// filled with -1 and the shaders array will be empty
	if (shaders.length() == 0)
	{
		// If we return zero here, it will skip this mesh.
		return 0;
	}

	for(unsigned int c = 0; c < indexTable.length(); c++) {
		nTris.append(0);
	}

	int nPolys = mesh.numPolygons();
	for(int i = 0; i < nPolys; i++) {
		MIntArray vertList;
		int nVerts = mesh.polygonVertexCount(i);
		if(nVerts > 3) {
			nTris[indices[i]] += nVerts - 2;
		}
		else {
			nTris[indices[i]] += 1;
		}
	}
	int total = 0;
	for(unsigned int t = 0; t < indexTable.length(); t++) {
		total += nTris[t];
	}
	return total;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void MayaUtil::GetPolyCounts(const int numMaterials, MIntArray &matIndices, 
							const int nPolys, MIntArray &polyCounts)
{
	polyCounts = MIntArray(numMaterials, 0);

	for(int i = 0; i < nPolys; i++) {
		polyCounts[matIndices[i]]++;
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void MayaUtil::SortByMaterials(MFnMesh &mesh, MIntArray &matIndices, 
							MIntArray &polyCounts, MIntArray &polyIndices)
{
	// the array "index" is going to track the place in the array in which
	//	to put the next polygon for this material.
	MIntArray index(polyCounts.length());
	cout << "index.length = " << index.length() << endl;

	// "index" gets initialized based on the number of polygons for
	//	each material
	int sum = 0;
	for(unsigned int c = 0; c < index.length(); c++) {
		index[c] = sum;
		sum += polyCounts[c];
	}

	// "polyIndices" will be indices into the Maya polygon list so that
	//	the polygons will be sorted by materials.
	int nPolys = mesh.numPolygons();
	polyIndices = MIntArray(nPolys);

	for (int i = 0; i < nPolys; i++) {
		const int matIndex = matIndices[i];

		polyIndices[index[matIndex]] = i;

		// increment the index for this material so that it will 
		// put the next polygon for this material into the next slot
		index[matIndex]++;
	}

}

//------------------------------------------------------------------------
// return object connected to node's plug with given name
//------------------------------------------------------------------------
MObject MayaUtil::GetObjectConnectedToPlug(MFnDependencyNode &node, 
								 const char *name, 
								 MStatus &status)
{
	//gets FIRST plug connected to attribute


	MObject plugObject;
	MPlug plug = node.findPlug(name, &status);

	if(status == MS::kSuccess) {
		MPlugArray connections;
		
		if(plug.connectedTo(connections, true, false, &status)) {
			if(status == MS::kSuccess && connections.length() > 0) {
				return connections[0].node(&status);
			}
		}
	}

	MObject dummy;

	status = MS::kFailure;

	return dummy;

}

//------------------------------------------------------------------------
// debugging routines
//------------------------------------------------------------------------
void MayaUtil::PrintInputs(MFnDependencyNode &node)
{
	MStatus status;
	MPlugArray plugs;
	status = node.getConnections(plugs);

	if(status == MS::kSuccess) {
		cout << "# of plugs = " << plugs.length() << endl;

		for(unsigned int ii = 0; ii < plugs.length(); ii++) {

			MPlugArray connections;
			bool asDst = true, asSrc = false; // bools set to get Inputs only
			if (plugs[ii].connectedTo(connections, asDst, asSrc, &status)) {
				if (connections.length() > 0) {
					cout << "Plug:   " << plugs[ii].name() << endl;
					cout << "Num inputs found on this plug: " << connections.length() << endl;

					for( unsigned int jj = 0; jj < connections.length(); jj++) {
						cout << "  plug: " << connections[jj].name() << endl;
						MObject obj = connections[jj].node();
						cout << "  to node type: " << obj.apiTypeStr() << endl;

						
					}
				}
				//else cout << "No inputs on this plug, there may be outputs on it." << endl;
				
			}
			//else cout << "No connections on this plug." << endl;
		}
	}
}

void MayaUtil::PrintAttributeTypes(MFnDependencyNode &node)
{
	MStatus status;
	int n = node.attributeCount();

	cout << "# of attributes = " << n << endl;
	for(int i = 0; i < n; i++) {
		MObject attr = node.attribute(i, &status);
		MPlug plug(node.object(), attr);
		cout << plug.name() << ": " << attr.apiTypeStr() << endl;
	}

	MStringArray aliases;
	node.getAliasList(aliases);
	cout << "# of aliases = " << aliases.length() << endl;
	for(int i = 0; i < aliases.length(); i++) {
		cout << " -  " << aliases[i] << endl;
	}
}
void MayaUtil::PrintPlugs(MFnDependencyNode &node)
{
	MStatus status;
	MPlugArray plugs;
	status = node.getConnections(plugs);

	if(status == MS::kSuccess) {
		cout << "# of plugs = " << plugs.length() << endl;

		for(unsigned int ii = 0; ii < plugs.length(); ii++) {
			cout << "    " << plugs[ii].name() << endl;
		}
	}
}
void MayaUtil::DebugPlug(MPlug &plug)
{
	cout << "Plug named " << plug.name();			
	cout  << " array? " << plug.isArray();
	cout << "  num elements: " << plug.numElements();
	cout << " compound? " << plug.isCompound();
	cout << "  num kids: " << plug.numChildren() << endl;
}

//------------------------------------------------------------------------
// Access the index of a compound or array plug, using either
//	the logical element or child interface() depending on the plug type.
//------------------------------------------------------------------------
MPlug MayaUtil::AccessPlugIndex(MPlug &plug, int index, MStatus &status)
{
	if (plug.isArray())
	{
		// Use logical index for this. The physical index is 
		// more likely to e used within a for loop outside of this function.
		status = MS::kSuccess;
		return plug.elementByLogicalIndex(index);
	}
	else if (plug.isCompound())
	{
		if (plug.numChildren() > index)
		{
			status = MS::kSuccess;
			return plug.child(index);
		}
	}
		
	status = MS::kFailure;
	return MPlug();
}


//------------------------------------------------------------------------
// get name of texture from node using plug
//------------------------------------------------------------------------
MString MayaUtil::GetTextureFileName(MFnDependencyNode &node)
{
	MObject	fileAttr = node.attribute("fileTextureName");
	MPlug	plugToFile( node.object(), fileAttr ); 
    MFnDependencyNode  dgFn;
	MStatus stat;

	MObject	fnameValue;
	stat = plugToFile.getValue( fnameValue );
	if ( !stat ) {
		stat.perror("error getting value from plug");
	} else {
		MFnStringData stringFn( fnameValue );
		//cout << "Texture: " << stringFn.string() << endl;
		return stringFn.string();
	}
	
	return MString("");
}

//------------------------------------------------------------------------
// get name of effect file from node using plug
//------------------------------------------------------------------------
MString MayaUtil::GetEffectFileName(MFnDependencyNode &node)
{
	MObject	fileAttr = node.attribute("shader");
	MPlug	plugToFile( node.object(), fileAttr ); 
    MFnDependencyNode  dgFn;
	MStatus stat;

	MObject	fnameValue;
	stat = plugToFile.getValue( fnameValue );
	if ( !stat ) {
		stat.perror("error getting effect name from plug");
	} else {
		MFnStringData stringFn( fnameValue );
		cout << "Cg Effect: " << stringFn.string() << endl;
		return stringFn.string();
	}
	
	return MString("");
}

//------------------------------------------------------------------------
// Return true if the given attribute exists on the node
//------------------------------------------------------------------------
//bool MayaUtil::EngineFlagExists(MFnDagNode &node, MString name)
//{
//	MStatus status;
//
//	// Note: apparently spaces are removed in attr names!
//	MObject	attr = node.attribute(name, &status); 
//	MPlug plug( node.object(), attr ); 
//
//	bool val;
//	status = plug.getValue( val );
//	if ( status ) 
//	{
//		return true;
//	}
//
//	return false;
//}

//------------------------------------------------------------------------
// get boolean value from attribute with given name from node.
// there should be no spaces in the attribute name.
//------------------------------------------------------------------------
//const int MayaUtil::GetEngineFlag(MFnDagNode &node, MString name)
//{
//	MStatus status;
//	int flag = 0;
//	// Note: apparently spaces are removed in attr names!
//	MObject	attr = node.attribute(name, &status); 
//	/*
//	if(!status)
//		cout << "Could not find " << name << endl;
//	else
//		cout << "found attr " << name << endl;
//		*/
//	MPlug plug( node.object(), attr ); 
//
//	bool val;
//	status = plug.getValue( val );
//	if ( !status ) 
//	{
//		//cout << "error getting value from plug";
//		flag = 0;
//	}
//	else {
//		flag = val ? 1 : 0;
//	}
//
//	return flag;
//}


//========================================================================
// Return the current time for the Maya GUI time slider
//========================================================================
MTime MayaUtil::GetCurrentTime()
{
	MString cmd("currentTime -query;");

	MCommandResult result;
	MGlobal::executeCommand(cmd, result);
	double frame = 0;
	MStatus status = result.getResult(frame);
	if ( !status ) 
	{
		cout << "Error getting current time.";
	}
	//cout << "Current frame: " << frame << endl;
	return MTime(frame, MTime::uiUnit());
}
void MayaUtil::SetCurrentTime(MTime time)
{
	double frame = time.value();
	char buffer[256];
	::sprintf(buffer, "currentTime -edit %f;", frame);

	MStatus status = MGlobal::executeCommand(MString(buffer));
	if ( !status ) 
	{
		cout << "Error setting current time.";
	}
}

//------------------------------------------------------------------------
// Find out the frame rate set in the preferences
//------------------------------------------------------------------------
float MayaUtil::GetCurrentFrameRate()
{
	MTime::Unit uiUnit = MTime::uiUnit();
	//cout << "current uiUnit: "<< uiUnit << endl;

	// Define a time as one second
	MTime one_sec(1.0, MTime::kSeconds);
	// Then convert this time to uiUnits
	//cout << "value of uiUnits for one second: " << one_sec.as(uiUnit) << endl;

	return (float) one_sec.as(uiUnit);
}

//------------------------------------------------------------------------
//	check if the path has the wrong type of slashes (backslashes '\')
//	in the path.  If so, convert them to forward slashes otherwise noting.
//------------------------------------------------------------------------
void MayaUtil::ConvertSlashes(MString& io_Path)
{
	//	the path has backslashes, so change them to forward slashes.
	//
	if ( io_Path.index('\\') != -1 )
	{
		//cout << "original path: " << io_Path << endl;
		
		const char* pStr = io_Path.asUTF8();
		char newstring[1024];
		strcpy( newstring, pStr );

		int pos = 0;
		while (pos < strlen(pStr))
		{	
			pos = strcspn( newstring, "\\" );
			if (pos < strlen(pStr))
			{
				newstring[pos] = '/';
			}
		}

		io_Path = newstring;
		//cout << "changed path: -" << io_Path << endl;
	}
	else
	{
		//cout << "path is OK: " << io_Path << endl;
	}
}


//------------------------------------------------------------------------
//	Create file with given path. Confirms extention. Fills o_Locator
//	with locator for this file. Returns true if successful.
//------------------------------------------------------------------------
bool MayaUtil::MUCreateFile(const MString &path, 
						  const char *pExtension,
						  fsLocator &o_Locator)
{
	// Open file
	MString fname = MayaUtil::ConfirmExtension(path, pExtension);
	cout << "Path: " << fname << endl;

	fsFileUtil::ANSIFilenameToLocator(fname.asUTF8(), o_Locator);

	if( fsFileUtil::FileExists(o_Locator) )
	{
		if ( !fsFileUtil::IsReadOnly(o_Locator) )
		{
			fsFileUtil::DeleteFile(o_Locator);
		}
		else
		{
			MGlobal::displayWarning("File is READ-ONLY cannot save.");
			return false;
		}
	}

	fsFileUtil::CreateFile(o_Locator);
	return true;
}
