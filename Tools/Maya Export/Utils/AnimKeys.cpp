/*****************************************************************************
**  AnimKeys.cpp
**
**   Namespace for animation related functions      
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#include <MayaUtil.hpp>
#include <AnimKeys.hpp>
#include <AnimUtil.hpp>
#include <MayaFlagUtil.hpp>

#include <maya/MAnimControl.h>
#include <maya/MDagPath.h>
//#include <maya/MFnAnimCurve.h>
#include <maya/MFnBlendShapeDeformer.h>
#include <maya/MFnCamera.h>
#include <maya/MFnMesh.h>
#include <maya/MFnTransform.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>


#include <list>
#include <vector>

namespace AnimKeys
{

	namespace
	{
		bool	bWriteKeyDetails = false;

		//=============================================================================
		//	Chunk types
		//
		//	APKY - Animation Position KeY
		//	ARKY - Animation Rotation KeY
		//	ASKY - Animation Scale KeY
		//	RORD - Rotation ORDer
		//	AVIS - Animation VISibilty 

		//	ASKN - Animation on SKiN surface 
		//	ABLS - Animation Blend Shape
		//	AMKY - Animation Morph target KeY
		//	NNAM - Name
		//	AFCL - Animation FoCal Length for camera
		//	ACOI - Animation Center Of Interest for camera
		//	AFPS - Animation's preferred frames per second playback rate
		//	HAPT - Horizontal film aperture
		//	VAPT - Vertical film aperture
		//=============================================================================
		const chDefs::Name c_APKY = chDefs::MakeName('A', 'P', 'K', 'Y');
		const chDefs::Name c_ARKY = chDefs::MakeName('A', 'R', 'K', 'Y');
		const chDefs::Name c_ASKY = chDefs::MakeName('A', 'S', 'K', 'Y');
		const chDefs::Name c_RORD = chDefs::MakeName('R', 'O', 'R', 'D');
		const chDefs::Name c_AVIS = chDefs::MakeName('A', 'V', 'I', 'S');
		const chDefs::Name c_ASKN = chDefs::MakeName('A', 'S', 'K', 'N');

		const chDefs::Name c_ABLS = chDefs::MakeName('A', 'B', 'L', 'S');
		const chDefs::Name c_AMKY = chDefs::MakeName('A', 'M', 'K', 'Y');
		const chDefs::Name c_NNAM = chDefs::MakeName('N', 'N', 'A', 'M');
		const chDefs::Name c_WNAM = chDefs::MakeName('W', 'N', 'A', 'M');
		const chDefs::Name c_AFPS = chDefs::MakeName('A', 'F', 'P', 'S');

		const chDefs::Name c_AFCL = chDefs::MakeName('A', 'F', 'C', 'L');
		const chDefs::Name c_ACOI = chDefs::MakeName('A', 'C', 'O', 'I');
		const chDefs::Name c_HAPT = chDefs::MakeName('H', 'A', 'P', 'T');
		const chDefs::Name c_VAPT = chDefs::MakeName('V', 'A', 'P', 'T');

		const chDefs::Name c_ISEP = chDefs::MakeName('I', 'S', 'E', 'P');
		const chDefs::Name c_ZPLX = chDefs::MakeName('Z', 'P', 'L', 'X');


		//========================================================================
		// Get minimum time on the time slider in order to offset the 
		// animation so that it always starts at zero.
		//========================================================================
		//float get_time_offset()
		//{
		//	MAnimControl query_time;
		//	return (float) query_time.animationStartTime().value();
		//}

		//========================================================================
		// Get visibility state of the transform node
		//========================================================================
		bool get_visibility_state(MFnDagNode &i_Node)
		{
			// There doesn't seem to be a C++ function for finding the 
			// visibility state in the Maya API. So, I will use the
			// attribute "visibility"
		//	bool bVisible = i_Node.visible();
			return (MayaFlagUtil::GetEngineFlag(i_Node, "visibility") != 0);
		}

		//========================================================================
		// Get visibility state of the path by checking the state of 
		// all of the nodes in the path.
		//========================================================================
		bool get_visibility_state(MDagPath &i_Path)
		{
			if (i_Path.length() == 0) return false;

			MFnDagNode node(i_Path);
			bool bVisible = get_visibility_state(node);
			if (!bVisible || (i_Path.length() == 1))
				return bVisible; // either not visible or reached end of path

			// node was visible, check if a parent node has visibility set false
			MDagPath shorter(i_Path);
			shorter.pop();
			return get_visibility_state(shorter);
		}

		//========================================================================
		// get rotation order from this node in order to know how to interpret
		//	the animation channels on rotate x,y,z.
		//========================================================================
		envType::UInt8 get_rotation_order(MTransformationMatrix::RotationOrder rOrder)
		{
			switch (rOrder)
			{
			default:
			case MTransformationMatrix::kXYZ:
				return 0;
			case MTransformationMatrix::kYZX:
				return 1;
			case MTransformationMatrix::kZXY:
				return 2;
			case MTransformationMatrix::kXZY:
				return 3;
			case MTransformationMatrix::kYXZ:
				return 4;
			case MTransformationMatrix::kZYX:
				return 5;
			}
		}
		envType::UInt8 get_rotation_order(MFnTransform &i_Transform)
		{
			MTransformationMatrix::RotationOrder rOrder = i_Transform.rotationOrder();
			return get_rotation_order(rOrder);
		}

		//========================================================================
		// write rotation order to the file
		//========================================================================
		void write_rotation_order(chWriter &o_Writer, envType::UInt8 i_Order)
		{
			o_Writer.WriteChunkHeader(c_RORD, 0, false);
			o_Writer.Write(i_Order);
			o_Writer.FinishChunk();
		}


		//========================================================================
		//========================================================================
		void insert_key(std::map<float, maVector3d> &io_Keys,
						float i_CurrentTime,
						double i_Value[3])
		{
			maVector3d val((float)i_Value[0], (float)i_Value[1], (float)i_Value[2]);
			io_Keys[i_CurrentTime] = val;
		}

		//========================================================================
		//========================================================================
		void insert_key(std::map<float, float> &io_Keys,
						float i_CurrentTime,
						float i_Value)
		{
			io_Keys[i_CurrentTime] = i_Value;
		}

		//========================================================================
		//========================================================================
		void insert_key(std::map<float, bool> &io_Keys,
						float i_CurrentTime,
						bool i_Value)
		{
			io_Keys[i_CurrentTime] = i_Value;
		}

	}	// end of namespace


	
	//========================================================================
	// Gets current state of transform and puts the data into the
	//	TransformKeys data structure to represent the state at the given
	//	current time.
	//========================================================================
	void GatherKeys(TransformKeys &o_Keys, 
					float i_CurrentTime,
					MFnTransform &i_Transform)
	{
		double translation[3];
		i_Transform.getTranslation(MSpace::kTransform).get(translation);
		insert_key(o_Keys.m_TranslateKeys, i_CurrentTime, translation);

		// Store rotation order we are using if this is the first rotation key
		if (o_Keys.m_RotateKeys.empty())
		{
			o_Keys.m_RotationOrder = get_rotation_order(i_Transform);
		}

		MTransformationMatrix::RotationOrder rOrder = i_Transform.rotationOrder();
		double rotation[3];
		i_Transform.getRotation(rotation, rOrder);
		insert_key(o_Keys.m_RotateKeys, i_CurrentTime, rotation);

		double scale[3];
		i_Transform.getScale(scale);
		insert_key(o_Keys.m_ScaleKeys, i_CurrentTime, scale);

		// Gather visibility only if requested
		if (o_Keys.m_bWriteVisibility)
		{
			bool bVisible = get_visibility_state(i_Transform);
			insert_key(o_Keys.m_VisibleKeys, i_CurrentTime, bVisible);
		}
	}

	//========================================================================
	//	WriteAnimation - write animation based on the given keys
	//		that were gathered earlier.
	//========================================================================
	void WriteAnimation(chWriter &o_Writer, 
						const TransformKeys &i_Keys,
						float i_TimeOffset,
						bool i_bWriteDeltas)
	{
		o_Writer.WriteChunkHeader(c_APKY, 0, false);
		AnimUtil::WriteChannel(o_Writer, i_Keys.m_TranslateKeys, i_TimeOffset, i_bWriteDeltas);
		o_Writer.FinishChunk();

		write_rotation_order(o_Writer, i_Keys.m_RotationOrder);
		o_Writer.WriteChunkHeader(c_ARKY, 0, false);
		AnimUtil::WriteChannel(o_Writer, i_Keys.m_RotateKeys, i_TimeOffset, i_bWriteDeltas);
		o_Writer.FinishChunk();

		o_Writer.WriteChunkHeader(c_ASKY, 0, false);
		AnimUtil::WriteChannel(o_Writer, i_Keys.m_ScaleKeys, i_TimeOffset, i_bWriteDeltas);
		o_Writer.FinishChunk();
		
		// Write visibility only if requested
		if (i_Keys.m_bWriteVisibility)
		{
			o_Writer.WriteChunkHeader(c_AVIS, 0, false);
			const bool bWriteDeltas = false; // boolean channels can't really do deltas
			AnimUtil::WriteChannel(o_Writer, i_Keys.m_VisibleKeys, i_TimeOffset, bWriteDeltas);
			o_Writer.FinishChunk();
		}
	}

	//========================================================================
	// Gets current state of weights and puts the data into the
	//	BlendKeys data structure to represent the state at the given
	//	current time.
	//========================================================================
	void GatherKeys(BlendKeys &o_Keys, 
					float i_CurrentTime,
					MFnBlendShapeDeformer &i_Blend,
					int i_WeightIndex)
	{
		float weight = i_Blend.weight(i_WeightIndex);
		insert_key(o_Keys.m_WeightKeys, i_CurrentTime, weight);
	}


	//========================================================================
	//	WriteAnimation - write animation based on the given keys
	//		that were gathered earlier.
	//========================================================================
	void WriteAnimation(chWriter &o_Writer, 
						const BlendKeys &i_Keys,
						float i_TimeOffset)
	{
		o_Writer.WriteChunkHeader(c_ABLS, 0, true);	// Blend Shape Animation

		o_Writer.WriteChunkHeader(c_NNAM, 0, false);
		o_Writer.Write(i_Keys.m_BlendName);
		o_Writer.FinishChunk();
		
		if (!i_Keys.m_AliasName.empty())
		{
			//cout << "Have aliased name " << aliasName << endl;
			o_Writer.WriteChunkHeader(c_WNAM, 0, false);
			o_Writer.Write(i_Keys.m_AliasName);
			o_Writer.FinishChunk();
		}

		o_Writer.WriteChunkHeader(c_AMKY, 0, false);
		const bool bWriteDeltas = false; // not sure how to handle additive blend shape anims
		AnimUtil::WriteChannel(o_Writer, i_Keys.m_WeightKeys, i_TimeOffset, bWriteDeltas);
		o_Writer.FinishChunk();
		
		o_Writer.FinishChunk();	// c_ABLS
	}

	
	//========================================================================
	// Gets visibility state at the current time and puts the data into the
	//	SurfaceKeys data structure.
	//========================================================================
	void GatherKeys(SurfaceKeys &o_Keys, 
					float i_CurrentTime,
					MDagPath &i_DagPath)
					//MFnMesh &i_Mesh)
	{
		bool bVisible = get_visibility_state(i_DagPath);
		//cout << "Path is visible: " << bVisible << " at time: " << i_CurrentTime << endl;
		insert_key(o_Keys.m_VisibleKeys, i_CurrentTime, bVisible);
	}

	//========================================================================
	//	WriteSkinVisibility - write visibility animation for surface 
	//		that were gathered earlier.
	//========================================================================
	void WriteSkinVisibility(chWriter &o_Writer, 
						const SurfaceKeys &i_Keys,
						float i_TimeOffset)
	{
		// Write skin animation, consists of skin name and visibility animation
		o_Writer.WriteChunkHeader(c_ASKN, 0, true);

		o_Writer.WriteChunkHeader(c_NNAM, 0, false);
		o_Writer.Write(i_Keys.m_Name);
		o_Writer.FinishChunk();

		const bool bWriteDeltas = false; // has to be false for boolean channels
		o_Writer.WriteChunkHeader(c_AVIS, 0, false);
		AnimUtil::WriteChannel(o_Writer, i_Keys.m_VisibleKeys, i_TimeOffset, bWriteDeltas);
		o_Writer.FinishChunk();

		o_Writer.FinishChunk();
	}

	
	//========================================================================
	// Gets current state of camera and puts the data into the
	//	CameraKeys data structure to represent the state at the given
	//	current time.
	//========================================================================
	void GatherKeys(CameraKeys &o_Keys, 
					float i_CurrentTime,
					MFnCamera &i_Camera,
					const MMatrix &i_IncMatrix)
	{
		// Get transformation in world space
		MVector eyept = i_Camera.eyePoint(MSpace::kWorld);
		double translation[3];
		eyept.get(translation);
		insert_key(o_Keys.m_TranslateKeys, i_CurrentTime, translation);

		MTransformationMatrix total_mtx(i_IncMatrix);
		double rotation[3];
		MTransformationMatrix::RotationOrder rot_order = MTransformationMatrix::kXYZ;
		total_mtx.getRotation(rotation, rot_order);
		insert_key(o_Keys.m_RotateKeys, i_CurrentTime, rotation);
		o_Keys.m_RotationOrder = get_rotation_order(rot_order);

		// Get camera attributes
		float focal_len = (float) i_Camera.focalLength();
		insert_key(o_Keys.m_FocalLengthKeys, i_CurrentTime, focal_len);

		float coi = (float) i_Camera.centerOfInterest();
		insert_key(o_Keys.m_CenterOfInterestKeys, i_CurrentTime, coi);

		float hapt = (float) i_Camera.horizontalFilmAperture();
		insert_key(o_Keys.m_HorizFilmAperKeys, i_CurrentTime, hapt);

		float vapt = (float) i_Camera.verticalFilmAperture();
		insert_key(o_Keys.m_VertFilmAperKeys, i_CurrentTime, vapt);

		// Stereo parameters
		MStatus status;
		MPlug isep_plug = i_Camera.findPlug("interaxialSeparation", &status);
		if (status == MS::kSuccess) 
		{	
			float isep = (float) isep_plug.asDouble();
			//cout << "Isep: " << isep << endl;
			insert_key(o_Keys.m_InteraxialSepKeys, i_CurrentTime, isep);
		}
		MPlug zp_plug = i_Camera.findPlug("zeroParallax", &status);
		if (status == MS::kSuccess) 
		{	
			float zp = (float) zp_plug.asDouble();
			//cout << "Zp: " << zp << endl;
			insert_key(o_Keys.m_ZeroParallaxKeys, i_CurrentTime, zp);
		}
	}

	//========================================================================
	//	WriteCameraAnimation - write animation based on the given keys
	//		that were gathered earlier. 
	//========================================================================
	void WriteCameraAnimation(chWriter &o_Writer, 
						const CameraKeys &i_CameraKeys,
						float i_TimeOffset)
	{
		const bool bWriteDeltas = false;

		o_Writer.WriteChunkHeader(c_APKY, 0, false);
		AnimUtil::WriteChannel(o_Writer, i_CameraKeys.m_TranslateKeys, i_TimeOffset, bWriteDeltas);
		o_Writer.FinishChunk();

		write_rotation_order(o_Writer, i_CameraKeys.m_RotationOrder);
		o_Writer.WriteChunkHeader(c_ARKY, 0, false);
		AnimUtil::WriteChannel(o_Writer, i_CameraKeys.m_RotateKeys, i_TimeOffset, bWriteDeltas);
		o_Writer.FinishChunk();
		
		o_Writer.WriteChunkHeader(c_AFCL, 0, false);
		AnimUtil::WriteChannel(o_Writer, i_CameraKeys.m_FocalLengthKeys, i_TimeOffset, bWriteDeltas);
		o_Writer.FinishChunk();

		o_Writer.WriteChunkHeader(c_ACOI, 0, false);
		AnimUtil::WriteChannel(o_Writer, i_CameraKeys.m_CenterOfInterestKeys, i_TimeOffset, bWriteDeltas);
		o_Writer.FinishChunk();

		o_Writer.WriteChunkHeader(c_HAPT, 0, false);
		AnimUtil::WriteChannel(o_Writer, i_CameraKeys.m_HorizFilmAperKeys, i_TimeOffset, bWriteDeltas);
		o_Writer.FinishChunk();

		o_Writer.WriteChunkHeader(c_VAPT, 0, false);
		AnimUtil::WriteChannel(o_Writer, i_CameraKeys.m_VertFilmAperKeys, i_TimeOffset, bWriteDeltas);
		o_Writer.FinishChunk();

		if (!i_CameraKeys.m_InteraxialSepKeys.empty())
		{
			cout << "Have Interaxial separation animation" << endl;
			o_Writer.WriteChunkHeader(c_ISEP, 0, false);
			AnimUtil::WriteChannel(o_Writer, i_CameraKeys.m_InteraxialSepKeys, i_TimeOffset, bWriteDeltas);
			o_Writer.FinishChunk();
		}

		if (!i_CameraKeys.m_ZeroParallaxKeys.empty())
		{
			cout << "Have Zero parallax animation" << endl;
			o_Writer.WriteChunkHeader(c_ZPLX, 0, false);
			AnimUtil::WriteChannel(o_Writer, i_CameraKeys.m_ZeroParallaxKeys, i_TimeOffset, bWriteDeltas);
			o_Writer.FinishChunk();
		}
		
	}

}