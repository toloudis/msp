/*****************************************************************************
**  RotateFigure.cpp
**
**     Command to remove pivot points from hierarchical model,
**	should be done before animating.         
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include <MayaUtil.hpp>
#include <RotateFigure.hpp>

#include <SceneFuncs.hpp>
#include <PivotFuncs.hpp>

#include <maya/MArgList.h>
#include <maya/MAnimControl.h>
#include <maya/MDagPath.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnIKJoint.h>
#include <maya/MItDag.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>


namespace
{

const double	c_dPI			= 3.14159265358979323846;
const double	c_dRadToAngle	= 180.0 / c_dPI;
const double	c_dAngleToRad	= c_dPI / 180.0;

//========================================================================
// gets animation curve by name using Maya's plug system
//========================================================================
MFnAnimCurve get_anim_curve(MString name, MFnDependencyNode &node, MStatus &status)
{
	MPlug plug = node.findPlug(name, &status);

	if(status == MS::kSuccess) {
		MPlugArray connections;
		
		if(plug.connectedTo(connections, true, false, &status)) {
			if(status == MS::kSuccess && connections.length() > 0) {
				for(unsigned int j = 0; j < connections.length(); j++) {
					MObject node = connections[j].node(&status);
					MFnAnimCurve anim(node, &status);
					if(status == MS::kSuccess) {
						status = MS::kSuccess;
						return anim;
					}
				}
			}
		}
	}

	status = MS::kFailure;
	MFnAnimCurve dummy;  
	return dummy;
}


void rotate_joint(MFnIkJoint &joint, MEulerRotation &rotation)
{
/*	MStatus status;
	MFnAnimCurve animy = get_anim_curve("rotateY", joint, status);
	if (status == MS::kSuccess)
	{
		cout << "Got anim curve for Rotation" << endl;
		int num_keys = animy.numKeys();
		for (int i=0; i<num_keys; i++)
		{
			MAnimControl::setCurrentTime(animy.time(i));
			joint.rotateBy(rotation, MSpace::kWorld);
		}
	}
*/

	MStatus status[3];
	MFnAnimCurve animx = get_anim_curve("rotateX", joint, status[0]);
	MFnAnimCurve animy = get_anim_curve("rotateY", joint, status[1]);
	MFnAnimCurve animz = get_anim_curve("rotateZ", joint, status[2]);
	MFnAnimCurve transx = get_anim_curve("translateX", joint, status[3]);
	MFnAnimCurve transy = get_anim_curve("translateY", joint, status[4]);
	MFnAnimCurve transz = get_anim_curve("translateZ", joint, status[5]);

	for (int si=0; si<3; si++)
		if (status[si] != MS::kSuccess) return;

	if (animx.numKeys() != animy.numKeys()) return;
	if (animx.numKeys() != animz.numKeys()) return;
	if (animx.numKeys() != transx.numKeys()) return;
	if (animx.numKeys() != transy.numKeys()) return;
	if (animx.numKeys() != transz.numKeys()) return;

	cout << "Got anim curve for Rotation/Translation" << endl;
	int num_keys = animy.numKeys();
	for (int i=0; i<num_keys; i++)
	{
//		cout << "Before: " << animx.value(i) << " " << animy.value(i) << " " << animz.value(i) << endl;

		MAnimControl::setCurrentTime(animy.time(i));
		joint.rotateBy(rotation, MSpace::kWorld);

		MEulerRotation root;
		joint.getRotation(root);

//		MEulerRotation root(animx.value(i), animy.value(i), animz.value(i));
//		root = rotation * root;

		animx.setValue( i, root[0] );
		animy.setValue( i, root[1] );
		animz.setValue( i, root[2] );

		MVector old_trans(transx.value(i), transy.value(i), transz.value(i));
		MVector trans = old_trans.rotateBy(rotation);
		transx.setValue( i, trans[0]  );
		transy.setValue( i, trans[1] );
		transz.setValue( i, trans[2]  );

//		cout << "After: " << root[0] << " " << root[1] << " " << root[2] << endl;
		
	}

}


}	// end of namespace



RotateFigure::RotateFigure()
: m_Angle(180 * c_dAngleToRad) 
{
}

RotateFigure::~RotateFigure() 
{

}

void* RotateFigure::creator()
{
	return new RotateFigure;
}

MStatus	RotateFigure::doIt( const MArgList& args )
{
	MItDag::TraversalType	traversalType = MItDag::kDepthFirst;
	MFn::Type				filter        = MFn::kInvalid;
	MStatus					status;
	bool					quiet = false;

	status = parseArgs (args);
	if (!status)
		return status;

	return doScan();
};

MStatus RotateFigure::parseArgs(const MArgList& args)
{
	MStatus     	stat;
	MString     	arg;
	const MString	angleFlag			("-angle");

	// Parse the arguments.
	for ( size_t i = 0; i < args.length(); i++ ) {
		arg = args.asString( i, &stat );
		if (!stat)              
			continue;
				
		// get angle
		if ( arg == angleFlag  ) {
			m_Angle = args.asDouble( i+1, &stat) * c_dAngleToRad;
		}
	}
	return stat;
}


MStatus RotateFigure::doScan()
{   
	MayaUtil::SetWaitCursor();

	MStatus status;
	
	cout << "-----------------------------------------" << endl;
	
	// Iterate through scene
	//
	MItDag dagIterator( MItDag::kBreadthFirst, MFn::kInvalid, &status);

	if ( !status) 
	{
		MayaUtil::PrintError("MItDag constructor");
		MayaUtil::UnsetWaitCursor();
		return status;
	}

	
	int xformCount = 0;
	MString xformName;
	for ( ; !dagIterator.isDone(); dagIterator.next() ) 
	{

		MDagPath dagPath;

		status = dagIterator.getPath(dagPath);
		if ( !status ) 
		{
			status.perror("MItDag::getPath");
			continue;
		}

		MFnDagNode dagNode(dagPath, &status);
		if ( !status ) 
		{
			status.perror("MFnDagNode constructor");
			continue;
		}

		if (dagPath.hasFn(MFn::kJoint)) 
		{
			MFnIkJoint joint(dagPath, &status);
			if (status == MS::kSuccess)
			{
				cout << "RotateFigure: Joint!" << endl;
				rotate_joint(joint, MEulerRotation(0,m_Angle,0));
			}

			dagIterator.prune();

		}

	}

	MayaUtil::PrintStatus("Done.");
	
	MayaUtil::UnsetWaitCursor();

	return MS::kSuccess;
}
