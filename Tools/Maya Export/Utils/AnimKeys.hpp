/*****************************************************************************
**  AnimKeys.hpp
**
**   Namespace for animation related functions   
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef ANIM_KEYS_HPP
#error AnimKeys.hpp multiply included
#endif
#define ANIM_KEYS_HPP

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif
#ifndef MA_VECTOR3D_HPP
#include "Core/ma/maVector3d.hpp"
#endif

#include <map>

class chWriter;
class MFnBlendShapeDeformer;
class MFnCamera;
class MDagPath;
class MFnDependencyNode;
class MFnTransform;
class MMatrix;

namespace AnimKeys
{
	struct TransformKeys
	{
		std::map<float, maVector3d> m_TranslateKeys;
		envType::UInt8 m_RotationOrder;
		std::map<float, maVector3d> m_RotateKeys;
		std::map<float, maVector3d> m_ScaleKeys;
		bool m_bWriteVisibility;
		std::map<float, bool> m_VisibleKeys;
	};

	struct BlendKeys
	{
		std::string m_BlendName;
		std::string m_AliasName;
		std::map<float, float> m_WeightKeys;
	};

	struct SurfaceKeys
	{
		std::string m_Name;
		std::map<float, bool> m_VisibleKeys;
	};

	struct CameraKeys
	{
		std::map<float, maVector3d> m_TranslateKeys;
		envType::UInt8 m_RotationOrder;
		std::map<float, maVector3d> m_RotateKeys;

		std::map<float, float> m_FocalLengthKeys;
		std::map<float, float> m_CenterOfInterestKeys;
		std::map<float, float> m_HorizFilmAperKeys;
		std::map<float, float> m_VertFilmAperKeys;

		std::map<float, float> m_InteraxialSepKeys;
		std::map<float, float> m_ZeroParallaxKeys;
	};

	//========================================================================
	// Gets current state of transform and puts the data into the
	//	TransformKeys data structure to represent the state at the given
	//	current time.
	//========================================================================
	void GatherKeys(TransformKeys &o_Keys, 
					float i_CurrentTime,
					MFnTransform &i_Transform);

	//========================================================================
	//	WriteAnimation - write animation based on the given keys
	//		that were gathered earlier.
	//========================================================================
	void WriteAnimation(chWriter &o_Writer, 
						const TransformKeys &i_Keys,
						float i_TimeOffset,
						bool i_bWriteDeltas);

	//========================================================================
	// Gets current state of weights and puts the data into the
	//	BlendKeys data structure to represent the state at the given
	//	current time.
	//========================================================================
	void GatherKeys(BlendKeys &o_Keys, 
					float i_CurrentTime,
					MFnBlendShapeDeformer &i_Blend,
					int i_WeightIndex);

	//========================================================================
	//	WriteAnimation - write animation based on the given keys
	//		that were gathered earlier.
	//========================================================================
	void WriteAnimation(chWriter &o_Writer, 
						const BlendKeys &i_Keys,
						float i_TimeOffset);

	//========================================================================
	// Gets visibility state at the current time and puts the data into the
	//	SurfaceKeys data structure.
	//========================================================================
	void GatherKeys(SurfaceKeys &o_Keys, 
					float i_CurrentTime,
					MDagPath &i_DagPath);
					//MFnMesh &i_Mesh);

	//========================================================================
	//	WriteSkinVisibility - write visibility animation for surface 
	//		that were gathered earlier.
	//========================================================================
	void WriteSkinVisibility(chWriter &o_Writer, 
							 const SurfaceKeys &i_Keys,
							 float i_TimeOffset);

	//========================================================================
	// Gets current state of camera and puts the data into the
	//	CameraKeys data structure to represent the state at the given
	//	current time.
	//========================================================================
	void GatherKeys(CameraKeys &o_Keys, 
					float i_CurrentTime,
					MFnCamera &i_Camera,
					const MMatrix &i_IncMatrix);

	//========================================================================
	//	WriteCameraAnimation - write animation based on the given keys
	//		that were gathered earlier. 
	//========================================================================
	void WriteCameraAnimation(chWriter &o_Writer, 
						const CameraKeys &i_CameraKeys,
						float i_TimeOffset);
}
