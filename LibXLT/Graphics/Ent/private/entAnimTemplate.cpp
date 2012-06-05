/*****************************************************************************
**  entAnimTemplate.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Graphics/ent/entAnimTemplate.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/ent/entAnimation.hpp"

#include <algorithm>


//============================================================================
//============================================================================
namespace
{
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void delete_animkeys(const std::pair<fsLocator, entAnimKeys*>& i_Info)
{
	delete i_Info.second;
}

}	// end of namespace


//--------------------------------------------------------------------
// default constructor
//--------------------------------------------------------------------
entAnimTemplate::entAnimTemplate()
{
}

//--------------------------------------------------------------------
// constructor, 
// Template can be initialized with a certain number of 
// animation slots if that is how it will be used.
//--------------------------------------------------------------------
entAnimTemplate::entAnimTemplate(int i_NumAnimSlots)
:	m_Animations(i_NumAnimSlots, (entAnimation*) NULL )  // prefill with NULL
{
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
entAnimTemplate::~entAnimTemplate()
{
	for (int i = 0 ; i < m_Animations.size() ; i++ )
		delete m_Animations[i];
	
	std::for_each(m_AnimKeys.begin(), m_AnimKeys.end(), delete_animkeys);
}


//--------------------------------------------------------------------
//	GetNumAnimations returns the number of animation slots available
//--------------------------------------------------------------------
int entAnimTemplate::GetNumAnimations() const
{
	return m_Animations.size();	
}

//--------------------------------------------------------------------
// Returns name of animation in slot i_Index.  This may return NULL
//	if none of the anims are named, or if the slot is empty.
//	It may also return an empty string ("") if some anims are named,
//	but this one isn't.
//--------------------------------------------------------------------
const char*	entAnimTemplate::GetAnimName(int i_Index) const
{
	if (i_Index >= m_AnimNames.size())
		return NULL;

	return m_AnimNames[i_Index].c_str();
}

//--------------------------------------------------------------------
// Returns the index of the animation with the same name.
//	if the name is not found, a negative number is returned.
//--------------------------------------------------------------------
int entAnimTemplate::GetAnimIndex(const char* i_AnimName) const
{
	for (int i = 0 ; i < m_Animations.size() ; i++ )
	{
		if (_stricmp(m_AnimNames[i].c_str(), i_AnimName) == 0)
		{
			return i;
		}
	}
	return -1;
}

//--------------------------------------------------------------------
// This will resize the number of animations. If increasing, 
//	the empty slots will be filled with NULL. If decreasing,
//	any non-empty slots will be deleted.
//--------------------------------------------------------------------
void entAnimTemplate::SetNumAnimations(int i_NumAnims)
{
	if (i_NumAnims > m_Animations.size())
	{
		// fill new entries with NULL
		m_Animations.resize(i_NumAnims, (entAnimation*) NULL); 
	}
	else if (i_NumAnims < m_Animations.size())
	{
		for (int i = i_NumAnims ; i < m_Animations.size() ; i++ )
			delete m_Animations[i];
		m_Animations.resize(i_NumAnims);
	}
}

//--------------------------------------------------------------------
//	SetAnimation adds an animation for the given type.  The
//	entAnimTemplate assumes ownership of the animation, which should
//	be allocated on the heap. If there was already an animation in
//	this slot, the old animation will be deleted.
//--------------------------------------------------------------------
void entAnimTemplate::AddAnimation(entAnimation* i_Animation, 
									 int i_Index)
{
	if (i_Index >= m_Animations.size())
		this->SetNumAnimations(i_Index+1);
	else if (m_Animations[i_Index] != NULL)
		delete m_Animations[i_Index];

	m_Animations[i_Index] = i_Animation;
}


//--------------------------------------------------------------------
//	Expand number of anim slots and add this animation to that slot.
//--------------------------------------------------------------------
void entAnimTemplate::AppendAnimation(entAnimation* i_Animation, 
										const char *i_AnimName)
{
	int nanims = (int) m_Animations.size();
	this->SetNumAnimations( nanims + 1 );
	this->SetAnimation( i_Animation, nanims);
	this->assign_name(nanims, i_AnimName);
}

//--------------------------------------------------------------------
//	GetAnimKeys returns a pointer to the shared animation
//  keyframe data for the given filename.  This allows
//	keyframe data to be shared between animations.
//--------------------------------------------------------------------
const entAnimKeys* entAnimTemplate::GetAnimKeys(const fsLocator& i_Locator) const
{
	std::map<fsLocator, entAnimKeys*>::const_iterator it = m_AnimKeys.find(i_Locator);
	if( it == m_AnimKeys.end() )
		return NULL;
	else
		return it->second;
}

//--------------------------------------------------------------------
//	Adds shared keyframe data for given filename.  Ownership of the
//	data transfers to the EntityTemplate.
//  The caller should use GetAnimKeys to make sure there is no
//  key data with the same filename already.
//--------------------------------------------------------------------
void	entAnimTemplate::AddAnimKeys(entAnimKeys* i_AnimKeys,
										const fsLocator& i_Locator)
{
	DBG_ASSERT(m_AnimKeys.find(i_Locator) == m_AnimKeys.end(), "AddAnimKeys, anim keys already loaded.");
	if (m_AnimKeys.find(i_Locator) != m_AnimKeys.end())
		return;
	m_AnimKeys[i_Locator] = i_AnimKeys;
}

//--------------------------------------------------------------------
//	SetAnimation sets animation pointer in slot without deleting
//	animation in slot already.  This allows derived classes to 
//	rearrange animations within slots.
//--------------------------------------------------------------------
void 	entAnimTemplate::SetAnimation(entAnimation* i_Animation, int i_Index, 
										const char *i_AnimName)
{
	DBG_ASSERT((i_Index >= 0) && (i_Index < m_Animations.size()), "Animation index out of range");
	if ((i_Index < 0) || (i_Index >= m_Animations.size()))
		return;
	m_Animations[i_Index] = i_Animation;
	this->assign_name(i_Index, i_AnimName);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void entAnimTemplate::assign_name(int i_Index, const char *i_AnimName)
{
	if (!i_AnimName)
	{
		// allow clearing of old name, but not resize on NULL
		if (i_Index < m_AnimNames.size())
			m_AnimNames[i_Index] = i_AnimName; 
	}
	else
	{
		DBG_ASSERT((i_Index >= 0) && (i_Index < m_Animations.size()), "Animation index out of range");
		if ((i_Index < 0) || (i_Index >= m_Animations.size()))
			return;

		// resize array if necessary
		if (i_Index >= m_AnimNames.size())
			m_AnimNames.resize( i_Index+1 );

		m_AnimNames[i_Index] = i_AnimName;
	}

}
