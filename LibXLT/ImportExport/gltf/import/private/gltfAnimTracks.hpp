/****************************************************************************\
**  gltfAnimTracks.hpp
**
**      gltfAnimTracks decides what each object's glTF animations become:
**	when they play on it (end to end), the keys of its transform and of
**	its light, camera and material properties (reduced to the keys that
**	matter), and whether a mesh plays its transform as clips instead.
**	It needs no app or GPU, so it is tested on its own.
**	gltfAnimTracks.cpp has the rules.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GLTF_ANIMTRACKS_HPP
#error gltfAnimTracks.hpp multiply included
#endif
#define GLTF_ANIMTRACKS_HPP

#ifndef GLTF_SCENEOBJECTS_HPP
#include "ImportExport/gltf/import/private/gltfSceneObjects.hpp"
#endif

#include "tiny_gltf.h"

#include <vector>

//----------------------------------------------------------------------------
//	An animation and where it begins on an object's timeline
//----------------------------------------------------------------------------
struct gltfScheduledAnim
{
	int m_Animation = -1;
	float m_Begin = 0;				// seconds
};

namespace gltfAnimTracks
{
	// Keys per track, after Reduce, above which a mesh's transform plays as clips
	const int c_MaxKeys = 300;

	// How far Reduce lets a dropped key's value be from its neighbours' interpolation
	const float c_TranslationTolerance = 0.01f;	// centimetres
	const float c_RotationTolerance = 0.01f;	// degrees
	const float c_ScaleTolerance = 1e-4f;
	const float c_ValueTolerance = 1e-4f;		// times max(1, |value|)

	// CUBICSPLINE samples per second for keys: MachStudio's default frame
	// rate (g3dConstants::c_fDefaultFrameRate, in Graphics, which this
	// reader doesn't link)
	const float c_SampleRate = 24;

	// the animations that touch the object, end to end in file order from 0
	std::vector<gltfScheduledAnim> Schedule(const tinygltf::Model& i_Model, const gltfSceneObject& i_Object,
											float i_FramesPerSecond);

	// drops the keys its kept neighbours' interpolation gives within tolerance
	// (a step track: the keys equal to the one before)
	void Reduce(gltfTrack& io_Track);

	// the value at i_Time (m_Components floats): the first or last key's
	// value outside them; a step track holds; else linear (slerp for e_Rotation)
	void Sample(const gltfTrack& i_Track, float i_Time, float* o_pValue);

	// fills every object's m_Tracks, m_bNodeTransform, m_bTransformClip and
	// m_Clips (with their begins)
	void Decide(const tinygltf::Model& i_Model, std::vector<gltfSceneObject>& io_Objects, float i_FramesPerSecond);
}
