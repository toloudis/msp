/*****************************************************************************
**  emdlCharacterAnimKeys.hpp
**
**      emdlCharacterAnimKeys - class for maintaining shared keyframe data
**	adds morph target animation to joint animation in base class.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef EMDL_CHARACTERANIMKEYS_HPP
#error emdlCharacterAnimKeys.hpp multiply included
#endif
#define EMDL_CHARACTERANIMKEYS_HPP

#ifndef EMDL_ANIMKEYS_HPP
#include "Graphics/emdl/emdlAnimKeys.hpp"
#endif
#ifndef SMDL_MORPHANIMKEYS_HPP
#include "Graphics/smdl/smdlMorphAnimKeys.hpp"
#endif
#ifndef SMDL_VERTEXANIMKEYS_HPP
#include "Graphics/smdl/smdlVertexAnimKeys.hpp"
#endif


//============================================================================
//	emdlCharacterAnimKeys
//============================================================================
class emdlCharacterAnimKeys : public emdlAnimKeys
{
	public:
		//--------------------------------------------------------------------
		//	emdlCharacterAnimKeys constructor
		//--------------------------------------------------------------------
		emdlCharacterAnimKeys();

		//--------------------------------------------------------------------
		//	destructor
		//--------------------------------------------------------------------
		virtual ~emdlCharacterAnimKeys();

		//--------------------------------------------------------------------
		// Morph target keys (blend shape animation)
		//--------------------------------------------------------------------
		inline const std::map<std::string, smdlMorphAnimKeys>& GetMorphKeys() const;
		inline std::map<std::string, smdlMorphAnimKeys>& MorphKeys();
		inline std::vector<anKeyData<float>*>&	MorphChannels();

		//--------------------------------------------------------------------
		// The vertex keys are grouped by surface and are then
		//	keyed by frame to a list of positions and normals.
		//--------------------------------------------------------------------
		inline const std::map<std::string, smdlVertexAnimKeys>& GetVertexKeys() const;
		inline std::map<std::string, smdlVertexAnimKeys>& VertexKeys();

		//--------------------------------------------------------------------
		// This array of vertex frames is the resource that owns the memory
		//	of the vertex animation. The Keys above just provide a view to 
		//	this frame data.
		//--------------------------------------------------------------------
		inline smdlVertexFrames& VertexFrames();

		//--------------------------------------------------------------------
		// The skin animations are grouped by surface and should be
		//	applied to the transform holding the skin fragments.
		//--------------------------------------------------------------------
		inline const std::map<std::string, smdlGeoAnimKeys>& GetSkinKeys() const;
		inline std::map<std::string, smdlGeoAnimKeys>& SkinKeys();

		//--------------------------------------------------------------------
		// Old animation files need to attach to the root joint
		// in order to mimic previous behavior correctly.
		//--------------------------------------------------------------------
		void SetAttachToRootJoint(bool i_bAttach);
		inline bool GetAttachToRootJoint() const;

	private:
		std::map<std::string, smdlMorphAnimKeys> m_MorphKeys;
		std::vector<anKeyData<float>*> m_MorphChannels;
		std::map<std::string, smdlVertexAnimKeys> m_VertexKeys;
		smdlVertexFrames m_VertexFrames;
		std::map<std::string, smdlGeoAnimKeys> m_SkinKeys;
		bool m_bAttachToRootJoint;
};


//--------------------------------------------------------------------
// Accessor functions
//--------------------------------------------------------------------
inline const std::map<std::string, smdlMorphAnimKeys>& emdlCharacterAnimKeys::GetMorphKeys() const
{
	return m_MorphKeys;
}
inline std::map<std::string, smdlMorphAnimKeys>& emdlCharacterAnimKeys::MorphKeys()
{
	return m_MorphKeys;
}

inline std::vector<anKeyData<float>*>&	emdlCharacterAnimKeys::MorphChannels()
{
	return m_MorphChannels;
}
inline const std::map<std::string, smdlVertexAnimKeys>& emdlCharacterAnimKeys::GetVertexKeys() const
{
	return m_VertexKeys;
}
inline std::map<std::string, smdlVertexAnimKeys>& emdlCharacterAnimKeys::VertexKeys()
{
	return m_VertexKeys;
}
inline smdlVertexFrames& emdlCharacterAnimKeys::VertexFrames()
{
	return m_VertexFrames;
}
inline const std::map<std::string, smdlGeoAnimKeys>& emdlCharacterAnimKeys::GetSkinKeys() const
{
	return m_SkinKeys;
}
inline std::map<std::string, smdlGeoAnimKeys>& emdlCharacterAnimKeys::SkinKeys()
{
	return m_SkinKeys;
}
inline bool emdlCharacterAnimKeys::GetAttachToRootJoint() const
{
	return m_bAttachToRootJoint;
}
