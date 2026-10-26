/****************************************************************************\
**  gltfAnimRead.hpp
**
**      gltfAnimRead reads glTF animations as clips: each channel's keys in
**	frames from the clip's start (step and cubic spline interpolation
**	expressed as linear keys), translations in centimetres; the clips'
**	identities and display names; and which animations play on an
**	object. It needs no app or GPU, so it is tested on its own.
**	gltfAnimRead.cpp has the rules.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GLTF_ANIMREAD_HPP
#error gltfAnimRead.hpp multiply included
#endif
#define GLTF_ANIMREAD_HPP

#ifndef GLTF_SCENEOBJECTS_HPP
#include "ImportExport/gltf/import/private/gltfSceneObjects.hpp"
#endif

#include "tiny_gltf.h"

#include <string>
#include <vector>

//----------------------------------------------------------------------------
//	One animated property of one node
//----------------------------------------------------------------------------
struct gltfAnimChannel
{
	enum Path { e_Translation, e_Rotation, e_Scale, e_Weights };
	int m_Node = -1;
	Path m_Path = e_Translation;
	int m_Components = 3;			// 3, 4 (rotation x y z w), or the morph target count
	bool m_bStep = false;			// STEP keys as given: each holds until the next (ReadClip's
									// i_bStepAsHolds false only)
	std::vector<float> m_Times;		// frames from the clip's start, ascending
	std::vector<float> m_Values;	// m_Components per time; translations in centimetres
};

//----------------------------------------------------------------------------
//	What a channel animates: a node property, or a property a
//	KHR_animation_pointer names (gltfAnimRead.cpp has the list)
//----------------------------------------------------------------------------
struct gltfAnimTarget
{
	enum Kind { e_None, e_Node, e_Light, e_Camera, e_Material };
	Kind m_Kind = e_None;
	int m_Index = -1;						// the node, light, camera or material
	gltfAnimChannel::Path m_Path = gltfAnimChannel::e_Translation;	// e_Node
	std::string m_Property;					// others: the pointer after the index, e.g. "color",
											// "spot/outerConeAngle", "perspective/yfov",
											// "pbrMetallicRoughness/baseColorFactor"; e_None: the
											// pointer, if there is one
};

//----------------------------------------------------------------------------
//	A channel on a light, camera or material property. m_Node is -1; the
//	values are as the file has them (no unit change)
//----------------------------------------------------------------------------
struct gltfPointerChannel : gltfAnimChannel
{
	gltfAnimTarget m_Target;
};

//----------------------------------------------------------------------------
//	One glTF animation
//----------------------------------------------------------------------------
struct gltfClip
{
	std::string m_Identity, m_Name;
	float m_Length = 0;				// frames, over every channel read
	std::vector<gltfAnimChannel> m_Channels;		// node properties
	std::vector<gltfPointerChannel> m_Pointers;		// light, camera and material properties
};

namespace gltfAnimRead
{
	const float c_StepEpsilon = 1e-3f;	// frames: a STEP key's hold ends this far before the next key

	// one per animation: its name when no other animation has it, else "#<index>"
	std::vector<std::string> ClipIdentities(const tinygltf::Model& i_Model);

	// the animation's name, else "Animation <index>"
	std::string ClipName(const tinygltf::Model& i_Model, int i_Animation);

	// what the channel animates; e_None when nothing this importer reads
	// (never warns: callers warn once per pointer)
	gltfAnimTarget ResolveTarget(const tinygltf::Model& i_Model, const tinygltf::AnimationChannel& i_Channel);

	// false when no animation has that identity. i_bStepAsHolds false keeps
	// STEP keys as given (m_bStep) instead of adding hold keys.
	bool ReadClip(const tinygltf::Model& i_Model, const std::string& i_Identity, float i_FramesPerSecond, gltfClip& o_Clip,
				  bool i_bStepAsHolds = true);

	// true when an animation sets the node's morph weights
	bool AnimatesWeights(const tinygltf::Model& i_Model, int i_Node);

	// the animations that play on this object, in file order (gltfAnimRead.cpp has the rule)
	std::vector<int> AnimationsFor(const tinygltf::Model& i_Model, const gltfSceneObject& i_Object);
}
