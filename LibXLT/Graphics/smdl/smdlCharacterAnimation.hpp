/*****************************************************************************
**	smdlCharacterAnimation.hpp
**
**		A smdlCharacterAnimation combines morph target animation
**	with regular "GeoFrame" jointed animation.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef SMDL_CHARACTERANIMATION_HPP
#error smdlCharacterAnimation.hpp multiply included
#endif
#define SMDL_CHARACTERANIMATION_HPP

#ifndef SMDL_GEOFRAMEANIMATION_HPP
#include "Graphics/smdl/smdlGeoFrameAnimation.hpp"
#endif
#ifndef SMDL_MORPHANIMKEYS_HPP
#include "Graphics/smdl/smdlMorphAnimKeys.hpp"
#endif
#ifndef SMDL_VERTEXANIMKEYS_HPP
#include "Graphics/smdl/smdlVertexAnimKeys.hpp"
#endif
#ifndef G3D_CONSTANTS_HPP
#include "Graphics/g3d/g3dConstants.hpp"
#endif

#include <map>
#include <string>


//============================================================================
//============================================================================
class smdlCharacterAnimation : public smdlGeoFrameAnimation
{
	public:
		//--------------------------------------------------------------------
		//	The smdlCharacterAnimation requires references to animation
		//	keys.  The frame animation does not own the keys in
		//	order to allow sharing of key data between animations
		//--------------------------------------------------------------------
		smdlCharacterAnimation(const smdlKeyRootMap& i_KeyRoots,
			const std::map<std::string, smdlMorphAnimKeys>& i_MorphKeys,
			const std::map<std::string, smdlVertexAnimKeys>& i_VertexKeys, 
			const std::map<std::string, smdlGeoAnimKeys>& i_SkinKeys, 
			float i_FrameRate = g3dConstants::c_fDefaultFrameRate);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~smdlCharacterAnimation();

		//--------------------------------------------------------------------
		//	Access to blend shape animation keys
		//--------------------------------------------------------------------
		const std::map<std::string, smdlMorphAnimKeys>& GetMorphKeys() const;

		//--------------------------------------------------------------------
		// Look up name in map and return keys for this target. May return
		// NULL if no keys for the given target name exist.
		//--------------------------------------------------------------------
		const smdlMorphAnimKeys* GetMorphKeysForTarget(const std::string &i_Name) const;

		//--------------------------------------------------------------------
		//	Access to baked vertex animation keys
		//--------------------------------------------------------------------
		const std::map<std::string, smdlVertexAnimKeys>& GetVertexKeys() const;

		//--------------------------------------------------------------------
		// Look up name in map and return vertex keys for this target. May return
		// NULL if no keys for the given surface name exist.
		//--------------------------------------------------------------------
		const smdlVertexAnimKeys* GetVertexKeysForSurface(const std::string &i_Name) const;

		//--------------------------------------------------------------------
		//	Access to skin animation keys
		//--------------------------------------------------------------------
		const std::map<std::string, smdlGeoAnimKeys>& GetSkinKeys() const;

		//--------------------------------------------------------------------
		// Look up name in map and return skin keys for this target. May return
		// NULL if no keys for the given surface name exist.
		//--------------------------------------------------------------------
		const smdlGeoAnimKeys* GetSkinKeysForSurface(const std::string &i_Name) const;

		//--------------------------------------------------------------------
		//	CreateAnimInstance creates an anAnimInstance, of a type
		//	appropriate to the type of the anAnimation.
		//--------------------------------------------------------------------
		virtual anAnimInstance* CreateAnimInstance(float i_TimeOrigin) const;

		//--------------------------------------------------------------------
		//	Clone returns a copy of "this" allocated on the heap.
		//--------------------------------------------------------------------
		virtual anAnimation* Clone() const;

	private:
		const std::map<std::string, smdlMorphAnimKeys>& m_MorphKeys;
		const std::map<std::string, smdlVertexAnimKeys>& m_VertexKeys;
		const std::map<std::string, smdlGeoAnimKeys>& m_SkinKeys;
};


//============================================================================
//	smdlCharacterAnimInstance
//----------------------------------------------------------------------------
class smdlCharacterAnimInstance : public smdlGeoFrameAnimInstance
{
	public:
		//--------------------------------------------------------------------
		//	The anFrameAnimInstance constructor requires the animation beginning
		// time and the animation used by this instance.
		//--------------------------------------------------------------------
		smdlCharacterAnimInstance(	float i_TimeOrigin,
								const smdlCharacterAnimation& i_Anim);

		//--------------------------------------------------------------------
		//	pure virtual destructor
		//--------------------------------------------------------------------
		virtual ~smdlCharacterAnimInstance();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline const smdlCharacterAnimation& GetAnim() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		inline const std::map<std::string, smdlMorphAnimKeys>& GetMorphKeys() const;

		//--------------------------------------------------------------------
		// Look up name in map and return keys for this target. May return
		// NULL if no keys for the given target name exist.
		//--------------------------------------------------------------------
		inline const smdlMorphAnimKeys* GetMorphKeysForTarget(const std::string &i_Name) const;

		//--------------------------------------------------------------------
		//	Access to baked vertex animation keys
		//--------------------------------------------------------------------
		inline const std::map<std::string, smdlVertexAnimKeys>& GetVertexKeys() const;

		//--------------------------------------------------------------------
		// Look up name in map and return vertex keys for this target. May return
		// NULL if no keys for the given surface name exist.
		//--------------------------------------------------------------------
		inline const smdlVertexAnimKeys* GetVertexKeysForSurface(const std::string &i_Name) const;

		//--------------------------------------------------------------------
		//	Access to skin animation keys
		//--------------------------------------------------------------------
		inline const std::map<std::string, smdlGeoAnimKeys>& GetSkinKeys() const;

		//--------------------------------------------------------------------
		// Look up name in map and return skin keys for this target. May return
		// NULL if no keys for the given surface name exist.
		//--------------------------------------------------------------------
		inline const smdlGeoAnimKeys* GetSkinKeysForSurface(const std::string &i_Name) const;

		//--------------------------------------------------------------------
		//	The Clone function creates a new copy of the anFrameAnimInstance
		//	on the heap.
		//--------------------------------------------------------------------
		virtual anAnimInstance* Clone() const;

	private:
		const	smdlCharacterAnimation& m_Anim;
};


//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const smdlCharacterAnimation& smdlCharacterAnimInstance::GetAnim() const
{
	return m_Anim;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline const std::map<std::string, smdlMorphAnimKeys>& smdlCharacterAnimInstance::GetMorphKeys() const
{
	return m_Anim.GetMorphKeys();
}

//--------------------------------------------------------------------
// Look up name in map and return keys for this target. May return
// NULL if no keys for the given target name exist.
//--------------------------------------------------------------------
inline const smdlMorphAnimKeys* smdlCharacterAnimInstance::GetMorphKeysForTarget(const std::string &i_Name) const
{
	return m_Anim.GetMorphKeysForTarget(i_Name);
}

//--------------------------------------------------------------------
//	Access to baked vertex animation keys
//--------------------------------------------------------------------
inline const std::map<std::string, smdlVertexAnimKeys>& smdlCharacterAnimInstance::GetVertexKeys() const
{
	return m_Anim.GetVertexKeys();
}

//--------------------------------------------------------------------
// Look up name in map and return vertex keys for this target. May return
// NULL if no keys for the given surface name exist.
//--------------------------------------------------------------------
inline const smdlVertexAnimKeys* smdlCharacterAnimInstance::GetVertexKeysForSurface(const std::string &i_Name) const
{
	return m_Anim.GetVertexKeysForSurface(i_Name);
}

//--------------------------------------------------------------------
//	Access to skin animation keys
//--------------------------------------------------------------------
inline const std::map<std::string, smdlGeoAnimKeys>& smdlCharacterAnimInstance::GetSkinKeys() const
{
	return m_Anim.GetSkinKeys();
}

//--------------------------------------------------------------------
// Look up name in map and return skin keys for this target. May return
// NULL if no keys for the given surface name exist.
//--------------------------------------------------------------------
inline const smdlGeoAnimKeys* smdlCharacterAnimInstance::GetSkinKeysForSurface(const std::string &i_Name) const
{
	return m_Anim.GetSkinKeysForSurface(i_Name);
}
