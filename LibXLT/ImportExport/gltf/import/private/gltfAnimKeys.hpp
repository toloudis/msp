/****************************************************************************\
**  gltfAnimKeys.hpp
**
**      gltfAnimKeys builds the character animation keys of one glTF clip
**	on one mesh object: a key tree that mirrors the object's template
**	(gltfModelImport::LoadNodeMesh), and morph keys for its weights.
**	emdlImport::CreateAnimation turns them into an smdlCharacterAnimation.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GLTF_ANIMKEYS_HPP
#error gltfAnimKeys.hpp multiply included
#endif
#define GLTF_ANIMKEYS_HPP

#ifndef GLTF_ANIMREAD_HPP
#include "ImportExport/gltf/import/private/gltfAnimRead.hpp"
#endif

#include "tiny_gltf.h"

class emdlCharacterAnimKeys;

namespace gltfAnimKeys
{
	// The clip's keys for the object; NULL when the clip touches nothing of
	// it. Deleting the keys is the caller's. gltfAnimKeys.cpp has the rules.
	emdlCharacterAnimKeys* Build(const tinygltf::Model& i_Model, const gltfSceneObject& i_Object,
								 const gltfClip& i_Clip, float i_FramesPerSecond);
}
