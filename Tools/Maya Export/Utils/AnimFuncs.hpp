/*****************************************************************************
**  AnimFuncs.hpp
**
**   Namespace for animation related functions   
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef ANIM_FUNCS_HPP
#error AnimFuncs.hpp multiply included
#endif
#define ANIM_FUNCS_HPP

#include <maya/MFnAnimCurve.h>

#include <list>

class chWriter;
class MDagPath;
class MFnDependencyNode;
class MFnBlendShapeDeformer;
class MFnCamera;
class MFnTransform;
class MPlug;

namespace AnimFuncs
{
	//========================================================================
	// Get range of animation times from Maya's playback slider
	//========================================================================
	void GetTimeSliderRange(double &o_Min, double &o_Max);

	//========================================================================
	// Gets keys from channel into list.
	// If i_bMerge is true, then it adds keys to the existing list
	//========================================================================
	void GatherKeys(std::list<float> &o_Keys, 
					MFnAnimCurve &anim,
					double i_MinTime, 
					double i_MaxTime,
					bool i_bMerge = false);
	void GatherKeys(std::list<float> &keys, 
					MFnAnimCurve &animx, 
					MFnAnimCurve &animy, 
					MFnAnimCurve &animz,
					double i_MinTime, 
					double i_MaxTime);

	//========================================================================
	// Gets animation curve by name using Maya's plug system
	//========================================================================
	MFnAnimCurve GetAnimCurve(MString name, MFnDependencyNode &node, MStatus &status);
	MFnAnimCurve GetAnimCurve(MPlug plug, MStatus &status);

	//========================================================================
	//	WriteAnimation - get animation channels from given node and 
	//		writes keys out to chunk writer.
	//	If i_bSinglePose is true, then only the position at the current
	//		time is written.
	//========================================================================
	void WriteAnimation(chWriter &o_Writer, 
						MFnDependencyNode &node, 
						double i_MinTime, 
						double i_MaxTime, 
						bool i_bSinglePose = false, 
						bool i_bWriteDeltas = false);

	//========================================================================
	//	WriteSkinVisibility - write visibility animation from the
	//	transform above the skinned surface in the given dagPath
	//========================================================================
	void WriteSkinVisibility(chWriter &o_Writer, 
							 MDagPath &dagPath, 
							 const MString &skin_name, 
							 double i_MinTime, 
							 double i_MaxTime);

	//========================================================================
	//	WriteWeightAnimation - get blend shape anims from given node and 
	//		writes keys out to chunk writer.
	//	If i_bSinglePose is true, then only the position at the current
	//		time is written.
	//========================================================================
	void WriteWeightAnimation(chWriter &o_Writer, 
							  MFnBlendShapeDeformer &node, 
							  double i_MinTime, 
							  double i_MaxTime, 
							  bool i_bSinglePose = false);

	//========================================================================
	//	WriteCameraAnimation - write camera movement to file. Needs parent
	//	node in order to get transformation animation channels.
	//========================================================================
	void WriteCameraAnimation(chWriter &o_Writer, 
							  MFnCamera &camera,
							  MFnTransform &parent, 
							  double i_MinTime, 
							  double i_MaxTime);

	//========================================================================
	// Add a chunk with the animation's frame rate to the chunk writer
	//========================================================================
	void WriteCurrentFrameRate(chWriter &o_Writer);
	
	//========================================================================
	// Add a chunk with the animation's start frame number to the chunk writer
	//========================================================================
	void WriteBeginFrame(chWriter &o_Writer, float i_BeginFrame);
}
