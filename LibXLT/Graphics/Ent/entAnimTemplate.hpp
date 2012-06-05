/*****************************************************************************
**  entAnimTemplate.hpp
**
**      A entAnimTemplate tracks animations for sharing between objects.
**	It maintains a list of animations that can be played by an entity
**	and a map of shared keyframe data to locator.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef ENT_ANIMTEMPLATE_HPP
#error entAnimTemplate.hpp multiply included
#endif
#define ENT_ANIMTEMPLATE_HPP

#ifndef ENT_ANIMKEYS_HPP
#include "Graphics/ent/entAnimKeys.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif

#include <map>
#include <vector>


//============================================================================
//============================================================================
class entAnimation;


//============================================================================
//============================================================================
class entAnimTemplate 
{
	public:
		//--------------------------------------------------------------------
		// constructor,
		// Template can be initialized with a certain number of
		// animation slots if that is how it will be used.
		//--------------------------------------------------------------------
		entAnimTemplate();
		entAnimTemplate(int i_NumAnimSlots);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~entAnimTemplate();

		//--------------------------------------------------------------------
		//	GetNumAnimations returns the number of animation slots available
		//--------------------------------------------------------------------
		int GetNumAnimations() const;

		//--------------------------------------------------------------------
		// This will resize the number of animations. If increasing,
		//	the empty slots will be filled with NULL. If decreasing,
		//	any non-empty slots will be deleted.
		//--------------------------------------------------------------------
		void SetNumAnimations(int i_NumAnims);

		//--------------------------------------------------------------------
		//	GetAnimation returns an animation used by an entity.  If there
		//	is no animation of the requested type this function will
		//	return NULL.
		//--------------------------------------------------------------------
		const entAnimation* GetAnimation(int i_Index) const;
		entAnimation* GetAnimation(int i_Index);

		//--------------------------------------------------------------------
		// Returns name of animation in slot i_Index.  This may return NULL
		//	if none of the anims are named, or if the slot is empty.
		//	It may also return an empty string ("") if some anims are named,
		//	but this one isn't.
		//--------------------------------------------------------------------
		const char*	GetAnimName(int i_Index) const;

		//--------------------------------------------------------------------
		// Returns the index of the animation with the same name.
		//	if the name is not found, a negative number is returned.
		//--------------------------------------------------------------------
		int GetAnimIndex(const char* i_AnimName) const;

		//--------------------------------------------------------------------
		//	AddAnimation adds an animation for the given type.  The
		//	entAnimTemplate assumes ownership of the animation, which should
		//	be allocated on the heap.  If there was already an animation in
		//	this slot, the old animation will be deleted.
		//--------------------------------------------------------------------
		void AddAnimation(entAnimation* i_Animation, int i_Index );

		//--------------------------------------------------------------------
		//	Expand number of anim slots and add this animation to that slot.
		//--------------------------------------------------------------------
		void AppendAnimation(entAnimation* i_Animation,
							 const char *i_AnimName = NULL);

		//--------------------------------------------------------------------
		//	GetAnimKeys returns a pointer to the shared animation
		//  keyframe data for the given filename.  This allows
		//	keyframe data to be shared between animations.
		//--------------------------------------------------------------------
		const entAnimKeys* GetAnimKeys(const fsLocator& i_Locator) const;

		//--------------------------------------------------------------------
		//	Adds shared keyframe data for given filename.  Ownership of the
		//	data transfers to the EntityTemplate.
		//  The caller should use GetAnimKeys to make sure there is no
		//  key data with the same filename already.
		//--------------------------------------------------------------------
		void	AddAnimKeys(entAnimKeys* i_AnimKeys,
							const fsLocator& i_Locator);
	protected:

		//--------------------------------------------------------------------
		//	SetAnimation sets animation pointer in slot without deleting
		//	animation in slot already.  This allows derived classes to
		//	rearrange animations within slots.
		//--------------------------------------------------------------------
		void SetAnimation(entAnimation* i_Animation, int i_Index,
						  const char *i_AnimName = NULL);

	private:
		void assign_name(int i_Index, const char *i_AnimName);

		std::vector<entAnimation*> m_Animations;
		std::vector<std::string> m_AnimNames;
		std::map<fsLocator, entAnimKeys*> m_AnimKeys;

};


//--------------------------------------------------------------------
//	GetAnimation returns an animation used by an entity.  If there
//	is no animation of the requested type this function will
//	return NULL.
//--------------------------------------------------------------------
inline const entAnimation* entAnimTemplate::GetAnimation(int i_Index) const
{
	DBG_ASSERT((i_Index >= 0) && (i_Index < m_Animations.size()), "Animation index out of range");
	if ((i_Index < 0) || (i_Index >= m_Animations.size()))
		return NULL;
	return m_Animations[i_Index];
}

//--------------------------------------------------------------------
//	GetAnimation returns an animation used by an entity.  If there
//	is no animation of the requested type this function will
//	return NULL.
//--------------------------------------------------------------------
inline entAnimation* entAnimTemplate::GetAnimation(int i_Index)
{
	DBG_ASSERT((i_Index >= 0) && (i_Index < m_Animations.size()), "Animation index out of range");
	if ((i_Index < 0) || (i_Index >= m_Animations.size()))
		return NULL;
	return m_Animations[i_Index];
}
