/*****************************************************************************
**  JointFuncs.hpp
**
**   Namespace for IK-Joint and single-skin related functions 
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef JOINTFUNCS_HPP
#error JointFuncs.hpp multiply included
#endif
#define JOINTFUNCS_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef ENV_TYPE_HPP
#include "Core/Env/envType.hpp"
#endif 


#include <maya/MFnSkinCluster.h>
#include <maya/MMatrix.h>

class MFnIkJoint;
class MFnMesh;
class MFnSubd;
class chWriter;
class gfFileBin;

#include <vector>

namespace JointFuncs
{
	//========================================================================
	// Data structures for influences
	//========================================================================
	class Influence {
	public:
		envType::UInt32 index;
		float weight;
	};

	class InfluenceGroup {
	public:
		MString influenceObject;
		std::vector<Influence> influences;
	};

	class SkinCluster {
	public:
		MString name;
		std::vector<InfluenceGroup> groups;

		int FindGroupByName(const MString &name) const;
	};



	class ClusterTable {
	public:
		std::vector< shared_ptr<SkinCluster> > clusters;

		SkinCluster* GetCluster(const MString &skinClusterName)
		{
			for (int i = 0; i < clusters.size(); i++) {
				if (clusters[i]->name == skinClusterName) {
					return clusters[i].get();
				}
			}

			return NULL;
		}

	};

	//ClusterTable *clusterTable = NULL;


	//========================================================================
	//	ParseSkinClusters - gather information about skin clusters
	//		in current model that influence the given shape.
	//========================================================================
	void ParseSkinClusters(MFnMesh &mesh, ClusterTable& o_ClusterTable);
	//void ParseSkinClusters(MFnSubd &subdiv, ClusterTable& o_ClusterTable);

	//========================================================================
	//	HasSkinCluster - return true if this joint has a skin cluster
	//========================================================================
	bool HasSkinCluster(MFnIkJoint &joint);

	//========================================================================
	//	FindSkinCluster - get skin cluster for given ik-joint.
	//========================================================================
	MFnSkinCluster FindSkinCluster(MFnIkJoint &joint, MStatus &status);

	//========================================================================
	//	WriteJointBase - writes base pose of skeleton without influences
	//========================================================================
	//void WriteJointBase(MFnIkJoint &joint, 
	//					MMatrix &excl_matx,
	//					chWriter &o_Writer,
	//					MString &o_Message);

	//========================================================================
	//	WriteInfluenceTree - writes influence of skeleton on current mesh
	//========================================================================
	//void WriteInfluenceTree(MFnIkJoint &joint, 
	//						ClusterTable &i_ClusterTable,
	//					    chWriter &o_Writer,
	//						MString &o_Message);

	//========================================================================
	//	WriteJointAnim - get animation channels from skeleton
	//		and write keys out to chunk writer 
	//  If i_bSinglePose is true, writes just position at current time.
	//========================================================================
//	void WriteJointAnim(MFnIkJoint &joint, chWriter &o_Writer, bool i_bSinglePose = false);

	//========================================================================
	//	WriteJointAnim - get animation channels from skeleton
	//		and write keys out to .jna file
	//========================================================================
//	void WriteJointAnim(MFnIkJoint &joint, const char *path, bool i_bSinglePose = false);
		
	//========================================================================
	//	WriteJointSubAnim - get animation channels from portion
	//		of skeleton starting at the given joint
	//		and write keys out to chunk writer  or .jna file
	//========================================================================
	//void WriteJointSubAnim(MFnIkJoint &joint, chWriter &o_Writer, 
	//	bool i_bSinglePose = false, bool i_bDoDeltas = false);
	//void WriteJointSubAnim(MFnIkJoint &joint, const char *path, 
	//	bool i_bSinglePose = false, bool i_bDoDeltas = false);

}