/*****************************************************************************
**  JointFuncs.cpp
**
**   Namespace for IK-Joint and single-skin related functions 
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <JointFuncs.hpp>

#include <AnimFuncs.hpp>
#include <SceneFuncs.hpp>

#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnIkJoint.h>
#include <maya/MFnMesh.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnDoubleIndexedComponent.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnSubd.h>
#include <maya/MFloatArray.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItGeometry.h>
#include <maya/MItSelectionList.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MSelectionList.h>
#include <maya/MVector.h>

#undef CreateFile
#undef DeleteFile

#include "Core/ch/chBinWriter.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/gf/gfFileBin.hpp"

#include <vector>

namespace
{
	bool	bWriteJointDetails = false;
	const float c_WeightEpsilon = 0.0001f;

	//=============================================================================
	//	Chunk types
	//=============================================================================
	const chDefs::Name c_JOIN = chDefs::MakeName('J', 'O', 'I', 'N');
	const chDefs::Name c_JODA = chDefs::MakeName('J', 'O', 'D', 'A');
	const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
	const chDefs::Name c_JINF = chDefs::MakeName('J', 'I', 'N', 'F');
	const chDefs::Name c_INFT = chDefs::MakeName('I', 'N', 'F', 'T');
	const chDefs::Name c_DELT = chDefs::MakeName('D', 'E', 'L', 'T');

	const chDefs::Name c_ANIH = chDefs::MakeName('A', 'N', 'I', 'H');
	const chDefs::Name c_ANLV = chDefs::MakeName('A', 'N', 'L', 'V');



	//========================================================================
	//	get_parsed_skin_clusters - return clusters that have been parsed for
	//	this joint
	//========================================================================
	void get_parsed_skin_clusters(MFnIkJoint &joint, 
								  std::vector<JointFuncs::SkinCluster*> &o_Clusters,
								  JointFuncs::ClusterTable& i_ClusterTable)
	{
		MStatus status;
		MPlug worldMatrixPlug = joint.findPlug("worldMatrix", &status);

		//worldMatrix is array of Plugs so have to look for skin clusters (should only be one right?)
		for(int p = 0; p < worldMatrixPlug.numElements(); p++) {
			MPlug elem = worldMatrixPlug.elementByPhysicalIndex(p);
			MPlugArray connections;
			
			if(elem.connectedTo(connections, true, true, &status)) {
				if(status == MS::kSuccess && connections.length() > 0) {
					//ok, got connections
					//now search connections for skin cluster
					if (bWriteJointDetails) 
						cout << "num connections to worldMatrix = " << connections.length() << endl;
					for(int c = 0; c < connections.length(); c++) {
						MFnSkinCluster skinCluster(connections[c].node(), &status);
						if(status == MS::kSuccess) {
							JointFuncs::SkinCluster *cluster = i_ClusterTable.GetCluster(skinCluster.name());
							if (cluster) 
							{
								if (bWriteJointDetails) 
									cout << "Found cluster " << skinCluster.name() << endl;

								o_Clusters.push_back(cluster);
							}
							else 
							{
								if (bWriteJointDetails)
									cout << "No match, cluster " << skinCluster.name() << endl;
							}
						}

					}
				}
			}

		}
	}


	//// for debugging
	//void write_joint_to_output(MFnIkJoint &joint)
	//{
	//	MStatus status;

	//	cout << joint.name() << endl;

	//	MTransformationMatrix	matrix (joint.transformation());

	//	cout << "  translation: " << matrix.translation(MSpace::kWorld)
	//		 << endl;

	//	double	threeDoubles[3];
	//	MTransformationMatrix::RotationOrder rOrder = MTransformationMatrix::kXYZ;

	//	//matrix.getRotation (threeDoubles, rOrder, MSpace::kWorld);
	//	matrix.getRotation (threeDoubles, rOrder);
	//	cout << "  rotation: ["
	//		 << threeDoubles[0] << ", "
	//		 << threeDoubles[1] << ", "
	//		 << threeDoubles[2] << "]\n";


	//	joint.getOrientation(threeDoubles, rOrder);

	//	cout << "  orientation: ["
	//		 << threeDoubles[0] << ", "
	//		 << threeDoubles[1] << ", "
	//		 << threeDoubles[2] << "]\n";

	//	matrix.getScale (threeDoubles, MSpace::kWorld);
	//	cout << "  scale: ["
	//		 << threeDoubles[0] << ", "
	//		 << threeDoubles[1] << ", "
	//		 << threeDoubles[2] << "]\n";


	//	int nChildren = joint.childCount();

	//	int i;

	//	for(i = 0; i < nChildren; i++) {
	//		MObject child = joint.child(i);

	//		if(child.hasFn(MFn::kJoint)) {

	//			cout << "has child: " << endl;
	//			MFnIkJoint childJoint(child, &status);

	//			write_joint_to_output(childJoint);
	//		}
	//	}

	//}

	////========================================================================
	////========================================================================
	//void write_influence(chWriter &o_Writer, JointFuncs::Influence &influence)
	//{
	//	o_Writer.Write(influence.index);
	//	o_Writer.Write(influence.weight);
	//}

	////========================================================================
	////========================================================================
	//void write_influences(chWriter &o_Writer, MFnIkJoint &joint, JointFuncs::SkinCluster *cluster)
	//{
	//	//find group

	//	int group = cluster->FindGroupByName(joint.fullPathName());

	//	if (group >= 0) 
	//	{
	//		if (bWriteJointDetails)
	//		{
	//			cout << "writing " << cluster->groups[group].influences.size() 
	//				<< " influences for " << joint.fullPathName() << endl;
	//		}

	//		o_Writer.Write(envType::UInt32(cluster->groups[group].influences.size()));
	//		for (int i = 0; i < cluster->groups[group].influences.size(); i++) 
	//		{
	//			write_influence(o_Writer, cluster->groups[group].influences[i]);
	//		}
	//	}
	//	else 
	//	{
	//		if (bWriteJointDetails)
	//		{
	//			cout << "Could not find any influences for " << joint.name() << endl;
	//		}
	//		o_Writer.Write(envType::UInt32(0));
	//	}
	//}

	//========================================================================
	// Write name, transformation, bind matrix of single joint
	//========================================================================
	//void 
	//write_basic_joint_data(chWriter &o_Writer, 
	//					   MFnIkJoint &joint,
	//					   MMatrix &io_TotalXform)
	//{
	//	MStatus status;

	//	// Node name
	//	o_Writer.WriteChunkHeader(c_NNAM, 0, false);
	//	o_Writer.Write(MayaUtil::PrepareName(joint.name()).asUTF8());
	//	o_Writer.FinishChunk();

	//	// Joint data
	//	o_Writer.WriteChunkHeader(c_JODA, 3, false);

	//	MTransformationMatrix	matrix (joint.transformation());

	//	MVector translation = matrix.translation(MSpace::kWorld);

	//	o_Writer.Write((float) translation.x);
	//	o_Writer.Write((float) translation.y);
	//	o_Writer.Write((float) translation.z);

	//	double	threeDoubles[3];
	//	MTransformationMatrix::RotationOrder rOrder = MTransformationMatrix::kXYZ;

	//	matrix.getRotation (threeDoubles, rOrder);

	//	o_Writer.Write((float) threeDoubles[0]);
	//	o_Writer.Write((float) threeDoubles[1]);
	//	o_Writer.Write((float) threeDoubles[2]);

	//	joint.getOrientation(threeDoubles, rOrder);

	//	o_Writer.Write((float) threeDoubles[0]);
	//	o_Writer.Write((float) threeDoubles[1]);
	//	o_Writer.Write((float) threeDoubles[2]);

	//	matrix.getScale (threeDoubles, MSpace::kWorld);

	//	o_Writer.Write((float) threeDoubles[0]);
	//	o_Writer.Write((float) threeDoubles[1]);
	//	o_Writer.Write((float) threeDoubles[2]);
	//	
	///*	MObject plugObject;
	//	MPlug plug = joint.findPlug("bindPose");
	//	plug.getValue(plugObject);

	//	if (bWriteJointDetails)
	//		cout << "bindPose plug returns object of type " << plugObject.apiTypeStr() << endl;

	//	MFnMatrixData pose(plugObject, &status);

	//	float mat[4][4];
	//	if (status == MS::kSuccess) {
	//		if (bWriteJointDetails)
	//		{
	//			cout << "  able to get matrix:" << endl;
	//			cout << pose.matrix() << endl;
	//		}

	//		pose.matrix().get(mat);

	//		if (bWriteJointDetails)
	//		{
	//			cout << "  joint's transform is:" << endl;
	//			cout << joint.transformation().asMatrix() << endl;
	//		}

	//		MFnIkJoint parent(joint.parent(0), &status);
	//		if (status == MS::kSuccess) 
	//		{
	//			if (bWriteJointDetails)
	//			{
	//				cout << "  parent's transform is:" << endl;
	//				cout << parent.transformation().asMatrix() << endl;
	//				cout << "  world transform is:" << endl;
	//				cout << (parent.transformation().asMatrix() * joint.transformation().asMatrix()) << endl;
	//			}
	//		}

	//	}
	//	else {
	//		cout << "  could not make into matrix" << endl;
	//		MMatrix mm;
	//		mm.setToIdentity();
	//		mm.get(mat); //now this is some lazy stuff
	//	}
	//*/

	//	// Use current total transformation as bind pose starting in version 3
	//	io_TotalXform = matrix.asMatrix() * io_TotalXform;
	//	float mat[4][4];
	//	io_TotalXform.get(mat);

	//	// Write bind matrix as 16 floats
	//	//
	//	//file.Write(mat, sizeof(float[4][4]));
	//	for (int x=0; x<4; x++)
	//	{
	//		for (int y=0; y<4; y++)
	//		{
	//			o_Writer.Write(mat[x][y]);
	//		}
	//	}


	//	// Added in JODA version 2:
	//	// As an alternative for the importer, write the
	//	// transformation matrix for the joint directly.
	//	float mtx[4][4];
	//	matrix.asMatrix().get(mtx);
	//	for (int x=0; x<4; x++)
	//	{
	//		for (int y=0; y<4; y++)
	//		{
	//			o_Writer.Write(mtx[x][y]);
	//		}
	//	}

	//	// And then add in the scaleOrientation field
	//	// corresponds to the RotateAxis attributes in the Maya UI.
	//	joint.getScaleOrientation(threeDoubles, rOrder);

	//	o_Writer.Write((float) threeDoubles[0]);
	//	o_Writer.Write((float) threeDoubles[1]);
	//	o_Writer.Write((float) threeDoubles[2]);


	//	o_Writer.FinishChunk(); // end of c_JODA
	//	
	//}

	////========================================================================
	//// Write base skeleton and influences, not animation
	////========================================================================
	//void 
	//write_joint_recurs(chWriter &o_Writer, 
	//				   MFnIkJoint &joint, 
	//				   MMatrix total_xform,
	//				   bool i_bWriteInfluences,
	//				   bool i_bWriteJointData,
	//				   MString &o_Message,
	//				   JointFuncs::ClusterTable& i_ClusterTable)
	//{
	//	MStatus status;

	//	// Joint chunk
	//	if (i_bWriteJointData)
	//		o_Writer.WriteChunkHeader(c_JOIN, 0, true);	// joint tree
	//	else if (i_bWriteInfluences)
	//		o_Writer.WriteChunkHeader(c_INFT, 0, true);	// influence tree
	//	else
	//		cout << "Error: no joint data to write" << endl;

	//	if (bWriteJointDetails) cout << joint.name() << endl;

	//	if (i_bWriteJointData)
	//	{
	//		// Write out basic joint data, not the influences.
	//		// Note, this will alter the total_xform matrix
	//		write_basic_joint_data(o_Writer, joint, total_xform);
	//	}

	//	if (i_bWriteInfluences)
	//	{
	//		// Write out the influences of this joint
	//		std::vector<JointFuncs::SkinCluster*> clusters;
	//		get_parsed_skin_clusters(joint, clusters, i_ClusterTable);
	//		//MFnSkinCluster skinCluster = JointFuncs::FindSkinCluster(joint, status);
	//		//SkinCluster *cluster = o_ClusterTable.GetCluster(skinCluster);
	//		for (int c=0; c<clusters.size(); c++)
	//		{
	//			// version 1 supports 32-bit
	//			o_Writer.WriteChunkHeader(c_JINF, 1, false);
	//			write_influences(o_Writer, joint, clusters[c]);
	//			o_Writer.FinishChunk();	// c_JINF
	//		}
	//	}

	//	// Write out the joint's children
	//	int nChildren = joint.childCount();
	//	int nSubJoints = 0;

	//	int i;

	//	if (bWriteJointDetails) cout << "No. Children: " << nChildren << endl;
	//	for (i = 0; i < nChildren; i++) 
	//	{
	//		MObject child = joint.child(i);
	//		if (child.hasFn(MFn::kJoint)) 
	//		{
	//			nSubJoints++;
	//		}
	//	}

	//	if (bWriteJointDetails) cout << "No. Sub-joints: " << nSubJoints << endl;
	//	//file.Write(nSubJoints);

	//	for (i = 0; i < nChildren; i++) 
	//	{
	//		MObject child = joint.child(i);

	//		if (child.hasFn(MFn::kJoint)) 
	//		{
	//			if (bWriteJointDetails) cout << "has child: " << endl;
	//			MFnIkJoint childJoint(child, &status);

	//			write_joint_recurs(o_Writer, childJoint, total_xform, 
	//				i_bWriteInfluences, i_bWriteJointData, o_Message, i_ClusterTable);
	//		}
	//		else if (child.hasFn(MFn::kTransform))
	//		{
	//			// Only write transforms and meshes when writing joint tree,
	//			// not when writing influences.
	//			if (i_bWriteJointData)
	//			{
	//				// See if the transform has some meshes underneath
	//				bool has_mesh = MayaUtil::HasTypeAsChild(child, MFn::kMesh);
	//				if (has_mesh)
	//				{
	//					if (bWriteJointDetails) cout << "has child transform with meshes: " << endl;

	//					MFnTransform xform(child);
	//					SceneFuncs::WriteTransform(xform, o_Writer, o_Message);
	//				}
	//			}
	//			
	//		}
	//	}

	//	o_Writer.FinishChunk();	// c_JOIN or c_INFT
	//}

	////========================================================================
	//// Writes just joint animation
	////========================================================================
	//void 
	//write_joint_anim_recurs(chWriter &o_Writer, 
	//						MFnIkJoint &joint, 
	//						bool i_bSinglePose, 
	//						bool i_bDoDeltas)
	//{
	//	MStatus status;

	//	if (bWriteJointDetails)
	//		cout << joint.name() << endl;

	//	// Joint chunk
	//	o_Writer.WriteChunkHeader(c_ANLV, 1, true);

	//	// Node name, added in order to match hierarchies that aren't exact
	//	o_Writer.WriteChunkHeader(c_NNAM, 0, false);
	//	o_Writer.Write(MayaUtil::PrepareName(joint.name()).asUTF8());
	//	o_Writer.FinishChunk();

	//	AnimFuncs::WriteAnimation(o_Writer, joint, i_bSinglePose, i_bDoDeltas);

	//	int nChildren = joint.childCount();
	//	for (int i = 0; i < nChildren; i++) 
	//	{
	//		MObject child = joint.child(i);
	//		if (child.hasFn(MFn::kJoint)) 
	//		{
	//			if (bWriteJointDetails)
	//				cout << "has child: " << endl;
	//			MFnIkJoint childJoint(child, &status);
	//			write_joint_anim_recurs(o_Writer, childJoint, i_bSinglePose, i_bDoDeltas);
	//		}
	//	}

	//	o_Writer.FinishChunk();
	//}


	//========================================================================
	// try to get logical indices of influences
	//========================================================================
	void get_logical_inds_of_influences(MPlug plug)
	{
		cout << "** Debug Logical Indices of Influences" << endl;
		int num_elem = plug.numElements();

		cout << "Plug: " << num_elem << " compound? " << plug.isCompound() << endl;
		cout << "num kids: " << plug.numChildren() << endl;

		// Input Matrices are a sparse array so we need to match
		// the logical indices to the physical indices
		MIntArray logical_inds;
		unsigned nlog = plug.getExistingArrayAttributeIndices( logical_inds );
		cout << " Logical inds, " << nlog << ": ";
		for (int li=0; li<nlog; li++)
			cout << logical_inds[li] << " ";
		cout << endl;

		cout << "** End Debug" << endl;
		
	}

	//========================================================================
	// remap logical index to influence index
	//========================================================================
	int remap_logical_influence(int log_ind, MIntArray &array)
	{
		const int nlog = array.length();
		for (int li=0; li<nlog; li++)
		{
			if (array[li] == log_ind)
				return li;
		}
		return -1;
	}


} // end of namespace


//========================================================================
// Get SkinCluster by name
//========================================================================
int JointFuncs::SkinCluster::FindGroupByName(const MString &name) const
{
	if (bWriteJointDetails) cout << "findbyname: " << name << endl;
	for(int i = 0; i < groups.size(); i++) {
		if (bWriteJointDetails) cout << "   " << i << ": " << groups[i].influenceObject << endl;
		if(groups[i].influenceObject == name)
			return i;
	}
	return -1;
}

//========================================================================
//	ParseSkinClusters - gather information about skin clusters
//		in current model that influence the given shape.
// MESH version
//========================================================================
void JointFuncs::ParseSkinClusters(MFnMesh &mesh, ClusterTable& o_ClusterTable)
{
	MStatus status; 

	if (bWriteJointDetails)
		cout << "ParseSkinClusters for node, " << mesh.name() << " " << mesh.typeName() << endl;

	size_t count = 0;
	
	// Iterate through graph and search for skinCluster nodes
	//
	MItDependencyNodes iter( MFn::kInvalid);
	for ( ; !iter.isDone(); iter.next() ) {
		MObject object = iter.item();
		if (object.apiType() == MFn::kSkinClusterFilter) {
			count++;
			
			// For each skinCluster node, get the list of influence objects
			//
			MFnSkinCluster skinCluster(object);
			MDagPathArray infs;
			MStatus stat;
			unsigned int nInfs = skinCluster.influenceObjects(infs, &stat);

			if (0 == nInfs) 
			{
				cout << "No influence objects found in skinCluster: " << skinCluster.name() << endl;
			}

			shared_ptr<SkinCluster> cluster( new SkinCluster );
			cluster->name = skinCluster.name();
			o_ClusterTable.clusters.push_back(cluster);

			//--------------------------------------------
			// old style  - didn't work with subdivisions

			if (bWriteJointDetails)
				cout << "Cluster: " << skinCluster.name() << endl;

			// loop through the geometries affected by this cluster
			//
			unsigned int nGeoms = skinCluster.numOutputConnections();

			if (bWriteJointDetails)
				cout << "Num Geoms Affected: " << nGeoms << endl;

			// look through all geometries affected, looking for the 
			// mesh we are interested in
			for (size_t ii = 0; ii < nGeoms; ++ii) 
			//size_t ii = 0; //only do first for now
			//if(nGeoms > 0)
			{
				unsigned int index = skinCluster.indexForOutputConnection(ii,&stat);
				if (bWriteJointDetails)
					cout << "Index for Output connection: " << index << endl;

				// Check to see if this mesh matches
				MObject nodeobj = skinCluster.outputShapeAtIndex(index,&status);
				if (!status) continue;
				if (nodeobj != mesh.object()) // how to compare meshes?
				{
					if (bWriteJointDetails)
						cout << "Shapes don't match, skipping" << endl;
					continue;
				}


				// get the dag path of the ii'th geometry
				//
				MDagPath skinPath;
				stat = skinCluster.getPathAtIndex(index,skinPath);

				// iterate through the components of this geometry
				//
				MItGeometry gIter(skinPath);

				if (bWriteJointDetails)
					cout << skinPath.fullPathName() << " " << gIter.count() << " " << nInfs << endl;

				// print out the influence objects
				//
				for (size_t kk = 0; kk < nInfs; ++kk) {
					if (bWriteJointDetails) 
						cout << infs[kk].partialPathName() << " ";
					cluster->groups.push_back(InfluenceGroup());
					cluster->groups.back().influenceObject = infs[kk].fullPathName();
				}
				if (bWriteJointDetails) cout << endl;
			
				for (; !gIter.isDone(); gIter.next() ) {
					MObject comp = gIter.component(&stat);
					if (stat != MS::kSuccess)
						cout << "Error getting component." << endl;
					if (comp.isNull())
						cout << "Null component." << endl;

					// Debug some info about this component
					//MPoint pt = gIter.position();
					//cout << gIter.index() << ": " << pt[0] << "," << pt[1] << "," << pt[2] << endl;


					// Get the weights for this vertex (one per influence object)
					//
					MFloatArray wts;
					unsigned int infCount;
					stat = skinCluster.getWeights(skinPath,comp,wts,infCount);
					if (0 == infCount) {
						cout << "Error: 0 influence objects." << endl;
					}

					// Output the weight data for this vertex
					//
					if (bWriteJointDetails) cout << gIter.index() << " ";

					for (unsigned int jj = 0; jj < infCount ; ++jj ) {
						if (bWriteJointDetails) cout << wts[jj] << " ";
						if(jj < nInfs) {
							if(wts[jj] > 0) { //this should probably be an epsilon
								Influence inf;
								inf.index = gIter.index();
								inf.weight = wts[jj];
								cluster->groups[jj].influences.push_back(inf);
							}
						}
						else {
							cout << "Influence index out of range" << endl;
						}
					}
					if (bWriteJointDetails) cout << endl;
					
				} 
			} 
			//-------------------------------------------- 

			if (bWriteJointDetails && !cluster->groups.empty())
			{
				cout << "Found a cluster with " << cluster->groups.size() << " influence objects" << endl;
				cout << "Groups:" << endl;
				for(int i = 0; i < cluster->groups.size(); i++) {
					cout << cluster->groups[i].influenceObject << " " << cluster->groups[i].influences.size() << endl;
				}
			}
		}
		
	}

	if (count = 0) {
		cout << "No skinClusters found in this scene." << endl;
	}

}

//========================================================================
//	ParseSkinClusters - gather information about skin clusters
//		in current model that influence the given shape.
// SUBDIV version
//========================================================================
//void JointFuncs::ParseSkinClusters(MFnSubd &subdiv, ClusterTable& o_ClusterTable)
//{
//	MStatus status; 
//
//	cout << "ParseSkinClusters for node, " << subdiv.name() << " " << subdiv.typeName() << endl;
//
//	size_t count = 0;
//	
//	// Iterate through graph and search for skinCluster nodes
//	//
//	MItDependencyNodes iter( MFn::kInvalid);
//	for ( ; !iter.isDone(); iter.next() ) {
//		MObject object = iter.item();
//		if (object.apiType() == MFn::kSkinClusterFilter) {
//			count++;
//			
//			// For each skinCluster node, get the list of influence objects
//			//
//			MFnSkinCluster skinCluster(object);
//			MDagPathArray infs;
//			MStatus stat;
//			unsigned int nInfs = skinCluster.influenceObjects(infs, &stat);
//
//			if (0 == nInfs) {
//				cout << "Error: No influence objects found." << endl;
//			}
//			unsigned int nGeoms = skinCluster.numOutputConnections();
//
//			if (bWriteJointDetails) cout << "Num Geoms Affected: " << nGeoms << endl;
//
//			// look through all geometries affected, looking for the 
//			// mesh we are interested in
//			std::vector<int> remap;
//			bool found_subdiv = false;
//			for (size_t ii = 0; ii < nGeoms; ++ii) 
//			{
//				unsigned int index = skinCluster.indexForOutputConnection(ii,&stat);
//				//cout << "Index for Output connection: " << index << endl;
//
//				// Check to see if this shape matches
//				MObject nodeobj = skinCluster.outputShapeAtIndex(index,&status);
//				if (!status) continue;
//				if (nodeobj == subdiv.object()) // how to compare shapes?
//				{
//					found_subdiv = true;
//
//					// get the dag path of the ii'th geometry
//					//
//					MDagPath skinPath;
//					stat = skinCluster.getPathAtIndex(index,skinPath);
//
//					// iterate through the components of this geometry
//					//
//					MItGeometry gIter(skinPath);
//					if (bWriteJointDetails)
//						cout << skinPath.fullPathName() << " " << gIter.count() << " " << nInfs << endl;
//					remap.resize(gIter.count());
//
//					// Generate remapping from ordering of vertices in iterator
//					// to ordering of vertices in subdiv.
//					for (int gi=0; !gIter.isDone(); gIter.next(),gi++ ) 
//					{
//						// Debug some info about this component
//						//MPoint pt = gIter.position();
//						//cout << gIter.index() << ": " << pt[0] << "," << pt[1] << "," << pt[2] << endl;
//
//						remap[gi] = gIter.index();
//					}
//
//					break;
//				}
//			}
//			if (!found_subdiv)
//			{
//				if (bWriteJointDetails) cout << "Shapes don't match, skipping" << endl;
//				continue;
//			}
//
//
//			shared_ptr<SkinCluster> cluster( new SkinCluster );
//			cluster->name = skinCluster.name();
//			o_ClusterTable.clusters.push_back(cluster);
//
//			// print out the influence objects, and create the InfluenceGroups
//			//
//			for (size_t kk = 0; kk < nInfs; ++kk) {
//				if (bWriteJointDetails)
//					cout << infs[kk].partialPathName() << " ";
//				cluster->groups.push_back(InfluenceGroup());
//				cluster->groups.back().influenceObject = infs[kk].fullPathName();
//			}
//			if (bWriteJointDetails) cout << endl;
//
//			//--------------------------------------------
//			// try to get logical indices of influences
//			MPlug matrix_plug = skinCluster.findPlug("ma"); //matrix
//			get_logical_inds_of_influences(matrix_plug);
//			MIntArray logical_inds_remap;
//			unsigned nlogInfs = matrix_plug.getExistingArrayAttributeIndices( logical_inds_remap );
//			if (nlogInfs != nInfs)
//			{
//				cout << "Influence Remapping length does not match." << endl;
//			}
//
//			//--------------------------------------------
//			// access through plugs, since API fails
//
//			MPlug plug = skinCluster.findPlug("wl"); //weightList
//			int num_elem = plug.numElements();
//
//			if (num_elem != remap.size())
//			{
//				cout << "Remapping length does not match." << endl;
//			}
//			else
//			{
//
//				//cout << "Plug: " << num_elem << " compound? " << plug.isCompound() << endl;
//				//cout << "num kids: " << plug.numChildren() << endl;
//				for (int ee=0; ee<num_elem; ee++)
//				{
//					MPlug kid =  plug.elementByPhysicalIndex(ee);
//					//cout << "  Plug: " << kid.numElements() << " compound? " << kid.isCompound() << endl;
//					//cout << "  num kids: " << kid.numChildren() << endl;
//					for (int kk=0; kk<kid.numChildren(); kk++)
//					{
//						MPlug kidkid = kid.child(kk);
//						//cout << kidkid.name() << endl;
//						//cout << "    Plug: " << kidkid.numElements() << " compound? " << kidkid.isCompound() << endl;
//						//cout << "      num kids: " << kidkid.numChildren() << endl;
//						
//						// Weights are a sparse array so we need to match
//						// the logical indices to the physical indices
//						MIntArray logical_inds;
//						unsigned nlog = kidkid.getExistingArrayAttributeIndices( logical_inds );
//						//cout << " Logical inds, " << nlog << ": ";
//						//for (int li=0; li<nlog; li++)
//						//	cout << logical_inds[li] << " ";
//						//cout << endl;
//
//						// Output the weight data for this vertex
//						//
//						if (bWriteJointDetails) cout << ee << "," << remap[ee] << " ";
//
//						float wt = 0.0f;
//						const int kkne = kidkid.numElements();
//						//cout << "kkne: " << kkne << endl;
//						for (int jj=0; jj<kkne; jj++)
//						{
//							MPlug weight = kidkid.elementByPhysicalIndex(jj);
//							weight.getValue( wt );
//							if (bWriteJointDetails) cout << wt << " ";
//
//							int log_ind = (nlog == kkne) ? logical_inds[jj] : jj;
//							// remap logical index back to influence object index
//							log_ind = remap_logical_influence(log_ind, logical_inds_remap);
//							if (log_ind >= 0 && log_ind < nInfs) 
//							{
//								// use epsilon to minimize number of influences
//								if (wt > c_WeightEpsilon) 
//								{ 
//									Influence inf;
//									inf.index = remap[ee];
//									//inf.index = ee;
//									inf.weight = wt;
//
//									cluster->groups[log_ind].influences.push_back(inf);
//								}
//							}
//							else {
//								cout << "Influence index out of range log_ind:" << log_ind << " nInfs:" << nInfs << endl;
//							}
//						}
//						if (bWriteJointDetails) cout << endl;
//
//					}
//				}
//			}
//
//			//--------------------------------------------
//
//			if (bWriteJointDetails && !cluster->groups.empty())
//			{
//				cout << "Found a cluster with " << cluster->groups.size() << " influence objects" << endl;
//				cout << "Groups:" << endl;
//				for(int i = 0; i < cluster->groups.size(); i++) {
//					cout << cluster->groups[i].influenceObject << " " << cluster->groups[i].influences.size() << endl;
//				}
//			}
//		}
//		
//	}
//
//	if (count = 0) {
//		cout << "No skinClusters found in this scene." << endl;
//	}
//
//}

//========================================================================
//	HasSkinCluster - return true if this joint has a skin cluster
//========================================================================
bool JointFuncs::HasSkinCluster(MFnIkJoint &joint)
{
	MStatus status;
	MFnSkinCluster* cluster = JointFuncs::FindSkinCluster(joint, status);
	delete cluster;
	return (status == MS::kSuccess);
}

//========================================================================
//	FindSkinCluster - get skin cluster for given ik-joint.
//========================================================================
MFnSkinCluster* JointFuncs::FindSkinCluster(MFnIkJoint &joint, MStatus &status)
{
	MPlug worldMatrixPlug = joint.findPlug("worldMatrix", &status);

	if (bWriteJointDetails) 
		cout << "** num elems = " << worldMatrixPlug.numElements() << endl;

	//worldMatrix is array of Plugs so have to look for skin clusters (should only be one right?)
	for(int p = 0; p < worldMatrixPlug.numElements(); p++) {
		MPlug elem = worldMatrixPlug.elementByPhysicalIndex(p);
		MPlugArray connections;
		
		if(elem.connectedTo(connections, true, true, &status)) {
			if(status == MS::kSuccess && connections.length() > 0) {
				//ok, got connections
				//now search connections for skin cluster
				if (bWriteJointDetails) 
					cout << "num connections to worldMatrix = " << connections.length() << endl;
				for(int c = 0; c < connections.length(); c++) {
					MFnSkinCluster* skinCluster = new MFnSkinCluster(connections[c].node(), &status);
					if(status == MS::kSuccess) {
						if (bWriteJointDetails) 
							cout << "Found cluster " << skinCluster->name() << endl;
						status = MS::kSuccess;
						return skinCluster;
					}

				}
				if (bWriteJointDetails && !status)
					cout << "Could not make object into skin cluster" << endl;
			}
			else if (bWriteJointDetails)
			{
				cout << "** error getting connections" << endl;
			}
		}
		else if (bWriteJointDetails)
		{
			cout << "** no connections" << endl;
		}

	}
	
	MFnSkinCluster* dummy = new MFnSkinCluster();
	status = MS::kFailure;
	return dummy;
}

//========================================================================
//	WriteJointBase - writes base pose of skeleton and influences
//		on single-skin. (.jnx file)
//========================================================================
//void JointFuncs::WriteJointBase(MFnIkJoint &joint, 
//								MMatrix &excl_matx,
//								chWriter &o_Writer,
//								MString &o_Message)
//{
//	bool bWriteData = true;
//	bool bWriteInfluences = false;
//	ClusterTable empty_clusters; // empty since no influences being written
//	MMatrix total_xform(excl_matx); // Start it off with the exclusive matrix from the dagPath
//	write_joint_recurs(o_Writer, joint, total_xform, bWriteInfluences, bWriteData, o_Message, empty_clusters);
//}

//========================================================================
//	WriteInfluenceTree - writes influence of skeleton on current mesh
//========================================================================
//void JointFuncs::WriteInfluenceTree(MFnIkJoint &joint,
//									ClusterTable &i_ClusterTable,
//									chWriter &o_Writer,
//									MString &o_Message)
//{
//	bool bWriteInfluences = true;
//	bool bWriteData = false;
//	MMatrix total_xform;
//	write_joint_recurs(o_Writer, joint, total_xform, bWriteInfluences, bWriteData, o_Message, i_ClusterTable);
//}

//========================================================================
//	WriteJointAnim - get animation channels from skeleton
//		and write keys out to chunk writer 
//========================================================================
//void JointFuncs::WriteJointAnim(MFnIkJoint &joint, 
//								chWriter &o_Writer, 
//								bool i_bSinglePose)
//{
//	// Write in the frame rate in small chunk at top
//	AnimFuncs::WriteCurrentFrameRate(o_Writer);
//
//	write_joint_anim_recurs(o_Writer, joint, i_bSinglePose, false);
//}

//========================================================================
//	WriteJointAnim - get animation channels from skeleton
//		and write keys out to .jna file
//========================================================================
//void JointFuncs::WriteJointAnim(MFnIkJoint &joint, 
//								const char *path, 
//								bool i_bSinglePose)
//{
//	MStatus status;
//
//	fsLocator locator;
//	fsFileUtil::ANSIFilenameToLocator(path, locator);
//
//	if( fsFileUtil::FileExists(locator) )
//		fsFileUtil::DeleteFile(locator);
//
//	fsFileUtil::CreateFile(locator);
//
//	gfFileBin file(locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
//	file.WriteHeader();
//
//	chBinWriter writer(file);
//	writer.WriteChunkHeader(c_ANIH, 0, true);
//	
//	// Write in the frame rate in small chunk at top
//	AnimFuncs::WriteCurrentFrameRate(writer);
//
//	write_joint_anim_recurs(writer, joint, i_bSinglePose, false);
//
//	writer.FinishChunk();
//
//	// Stamp version into end of file
//	SceneFuncs::WriteExporterVersionStamp(writer);
//
//}

//========================================================================
//	WriteJointSubAnim - get animation channels from portion
//		of skeleton starting at the given joint
//		and write keys out to chunk writer (.jna file)
//========================================================================
//void JointFuncs::WriteJointSubAnim(MFnIkJoint &joint, 
//								   chWriter &o_Writer, 
//								   bool i_bSinglePose, 
//								   bool i_bDoDeltas)
//{
//
//	// for sub-anim, write name of root joint 
//	o_Writer.WriteChunkHeader(c_NNAM, 0, false);
//	o_Writer.Write(MayaUtil::PrepareName(joint.name()).asUTF8());
//	o_Writer.FinishChunk();
//
//	// write out whether deltas are being written
//	o_Writer.WriteChunkHeader(c_DELT, 0, false);
//	o_Writer.Write(i_bDoDeltas);
//	o_Writer.FinishChunk();
//	
//	// Write in the frame rate in small chunk at top
//	AnimFuncs::WriteCurrentFrameRate(o_Writer);
//
//	write_joint_anim_recurs(o_Writer, joint, i_bSinglePose, i_bDoDeltas);
//}
//void JointFuncs::WriteJointSubAnim(MFnIkJoint &joint, 
//								   const char *path, 
//								   bool i_bSinglePose, 
//								   bool i_bDoDeltas)
//{
//	MStatus status;
//
//	fsLocator locator;
//	fsFileUtil::ANSIFilenameToLocator(path, locator);
//
//	if( fsFileUtil::FileExists(locator) )
//		fsFileUtil::DeleteFile(locator);
//
//	fsFileUtil::CreateFile(locator);
//
//	gfFileBin file(locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
//	file.WriteHeader();
//
//	chBinWriter writer(file);
//	writer.WriteChunkHeader(c_ANIH, 0, true);
//
//	JointFuncs::WriteJointSubAnim(joint, writer, i_bSinglePose, i_bDoDeltas);
//
//	writer.FinishChunk();
//
//	// Stamp version into end of file
//	SceneFuncs::WriteExporterVersionStamp(writer);
//}
