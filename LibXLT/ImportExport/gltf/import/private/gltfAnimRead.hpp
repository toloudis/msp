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
	std::vector<float> m_Times;		// frames from the clip's start, ascending
	std::vector<float> m_Values;	// m_Components per time; translations in centimetres
};

//----------------------------------------------------------------------------
//	One glTF animation
//----------------------------------------------------------------------------
struct gltfClip
{
	std::string m_Identity, m_Name;
	float m_Length = 0;				// frames
	std::vector<gltfAnimChannel> m_Channels;
};

namespace gltfAnimRead
{
	const float c_StepEpsilon = 1e-3f;	// frames: a STEP key's hold ends this far before the next key

	// one per animation: its name when no other animation has it, else "#<index>"
	std::vector<std::string> ClipIdentities(const tinygltf::Model& i_Model);

	// the animation's name, else "Animation <index>"
	std::string ClipName(const tinygltf::Model& i_Model, int i_Animation);

	// false when no animation has that identity
	bool ReadClip(const tinygltf::Model& i_Model, const std::string& i_Identity, float i_FramesPerSecond, gltfClip& o_Clip);

	// true when an animation sets the node's morph weights
	bool AnimatesWeights(const tinygltf::Model& i_Model, int i_Node);

	// the animations that play on this object, in file order (gltfAnimRead.cpp has the rule)
	std::vector<int> AnimationsFor(const tinygltf::Model& i_Model, const gltfSceneObject& i_Object);
}
