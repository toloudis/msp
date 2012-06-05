/*****************************************************************************
**  LayerFuncs.cpp
**
**   Namespace for IK-Joint and single-skin related functions 
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <LayerFuncs.hpp>

#include <AnimFuncs.hpp>
#include <SceneFuncs.hpp>

#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatArray.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItGeometry.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>

#undef CreateFile
#undef DeleteFile

#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileStream.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/gf/gfFileBin.hpp"

#include <vector>
#include <string>
//#include <map>
#include <set>

namespace LayerFuncs
{

namespace
{

} // end of namespace


//========================================================================
//	GetDisplayLayer - return name of layer for given node
//========================================================================
bool GetDisplayLayer(MFnDependencyNode &fnDependNode, MString &oString)
{
	MStatus stat, status;			// Status code

	// Get all connected plugs to this node
	//
	MPlugArray connectedPlugs;
	stat = fnDependNode.getConnections( connectedPlugs );

	int numberOfPlugs = connectedPlugs.length();

	// Print out the dependency node name and attributes
	// for each plug
	//
	for ( int i=0; i<numberOfPlugs; i++ ) 
	{
		MPlug plug = connectedPlugs[i];

		// Now get the plugs that this plug is the
		// dest of and print the node type.
		//
		MPlugArray array;
		plug.connectedTo( array, true, false );
		//cout << "Found plug: " << plug.name() << endl;

		for ( size_t j=0; j<array.length(); j++ )
		{
			MObject mnode = array[j].node();
			//cout << "    This plug is the dest of a " << mnode.apiTypeStr() << endl;

			if (mnode.apiType() == MFn::kDisplayLayer)
			{

				MFnDependencyNode layer(mnode, &status);
				if (status == MS::kSuccess) 
				{
					//cout << "Layer Name: " << layer.name() << endl;
					oString = layer.name();
					return true;
				}
			}

		}
	}

	return false;
}



//========================================================================
//	GetLayers - return list of layer names from previous call
//	to ParseLayers
//========================================================================
void GetLayers(std::vector<MString> &o_LayerNames)
{
	typedef std::set<std::string> LayerSet;
	LayerSet layer_set;

	// Iterate through graph and search for meshes
	//
	MItDependencyNodes iter( MFn::kInvalid);
	for ( ; !iter.isDone(); iter.next() ) 
	{
		MObject object = iter.item();
		if (object.apiType() == MFn::kMesh) 
		{
			// For each mesh, find layer
			//
			MFnMesh mesh(object);

			MString name;
			if (MayaUtil::DagNodeVisible(mesh) && GetDisplayLayer(mesh, name))
			{
				//cout << "Found a mesh " << mesh.name() << " in layer " << name << endl;
		
				std::string str_name(name.asChar());
				layer_set.insert(str_name);
			}
		}
	}	

	LayerSet::iterator it = layer_set.begin();
	for (; it!=layer_set.end(); ++it)
	{
		o_LayerNames.push_back(MString(it->c_str()));
	}
}

//========================================================================
//	WriteBReps - write visible meshes for the given layer
//========================================================================
void WriteBReps(const MString &i_LayerName, chWriter &o_Writer)
{		
	// Iterate through graph and search for meshes
	//
	MItDependencyNodes iter( MFn::kInvalid);
	for ( ; !iter.isDone(); iter.next() ) 
	{
		MObject object = iter.item();
		if (object.apiType() == MFn::kMesh) 
		{
			// For each mesh, find layer
			MFnMesh mesh(object);
			MString name;
			if (MayaUtil::DagNodeVisible(mesh) && GetDisplayLayer(mesh, name))
			{
				if (name == i_LayerName)
				{
					// Write mesh if it is in correct layer
					//
					MFnMesh mesh(object);
					SceneFuncs::WriteBRepToFile(mesh, o_Writer);
				}
			}
		}
	}
}				

}	// end of namesspace

