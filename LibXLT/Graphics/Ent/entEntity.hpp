/*****************************************************************************
**  entEntity.hpp
**
**      A container for objects that can animate.
**	An entEntity is an instance of a entModelTemplate.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef ENT_ENTITY_HPP
#error entEntity.hpp multiply included
#endif
#define ENT_ENTITY_HPP

#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class entAnimation;
class entAnimInstance;
//class entModelTemplate;
//class entEntityTemplate;
class g3dSceneNode;
class scObject;
class scAnimatableObject;
class maMatrix4x4;
class maRotation;
//class matMaterial;
class anFrameAnimInstance;		// just a handle right now
typedef anFrameAnimInstance entSubAnimation;


//============================================================================
//============================================================================
class entEntity
{
	public:
		//--------------------------------------------------------------------
		// Takes ownership of the object pointer
		//--------------------------------------------------------------------
		//explicit entEntity( const entModelTemplate& i_Template );
		explicit entEntity(scObject *i_pObject);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~entEntity();

		//--------------------------------------------------------------------
		//	GetTemplate returns the prop template which was used to create
		//	the prop.
		//--------------------------------------------------------------------
		//const entModelTemplate& GetTemplate() const;

		//--------------------------------------------------------------------
		//	update the total transform for the entity so the worldbox
		//	will be updated.
		//--------------------------------------------------------------------
		const maAxisBox& GetUpdatedWorldBox();

		//--------------------------------------------------------------------
		//	ComputeWorldBox - Sums up the bounding boxes of objects
		//--------------------------------------------------------------------
		void ComputeWorldBox( maAxisBox &o_Box ) const;

		//--------------------------------------------------------------------
		//	GetWorldBox - returns the bounding box that was computed in the
		//  last frame
		//--------------------------------------------------------------------
		const maAxisBox& GetWorldBox() const;

		//--------------------------------------------------------------------
		//	SetPosition changes the position of the objects
		//--------------------------------------------------------------------
		void SetPosition(const maPoint3d& i_Position);

		//--------------------------------------------------------------------
		//	GetPosition returns the position of the object
		//--------------------------------------------------------------------
		const maPoint3d& GetPosition() const;

		//--------------------------------------------------------------------
		//	SetOrientation changes the orientation of the objects
		//--------------------------------------------------------------------
		void SetOrientation(const maRotation& i_Orientation);

		//--------------------------------------------------------------------
		//	GetOrientation returns the orientation of the object
		//--------------------------------------------------------------------
		const maRotation& GetOrientation() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetPositionAndOrientation(	const maPoint3d& i_Position,
										const maRotation& i_Orientation);

		//--------------------------------------------------------------------
		//	SetScale changes the scale of the object
		//--------------------------------------------------------------------
		void SetScale(const maPoint3d& i_Scale);

		//--------------------------------------------------------------------
		//	GetScale returns the scale of the object
		//--------------------------------------------------------------------
		const maPoint3d& GetScale() const;

		//--------------------------------------------------------------------
		//	GetMatrix returns the matrix of the object
		//--------------------------------------------------------------------
		void GetMatrix( maMatrix4x4& o_Matrix ) const;

		//--------------------------------------------------------------------
		//	Think allows the prop to do maintenance of it's animations and
		//	stuff.
		//--------------------------------------------------------------------
		virtual void Think(float i_SimulationTime);

		//--------------------------------------------------------------------
		// DoesAnimationExist
		//--------------------------------------------------------------------
		//bool DoesAnimationExist(int i_AnimIndex);

		//--------------------------------------------------------------------
		// CheckAnimation checks compatibility of this animation 
		// with the model. It will return true if the animation can
		// be played on this model.
		//--------------------------------------------------------------------
		bool CheckAnimation( const entAnimation* i_pAnimation ) const;

		//--------------------------------------------------------------------
		// SetAnimation by slot in entity template
		//--------------------------------------------------------------------
		//void SetAnimation(int i_AnimIndex, float i_SimulationTime);

		//--------------------------------------------------------------------
		// SetAnimation to given animation pointer.
		//--------------------------------------------------------------------
		void SetAnimation( const entAnimation* i_BlendTo, float i_SimulationTime );

		//--------------------------------------------------------------------
		// SetAnimation to given animation pointer with a given blend time
		//--------------------------------------------------------------------
		void SetAnimation( const entAnimation* i_BlendTo, float i_SimulationTime, float i_fBlendTime );

		//--------------------------------------------------------------------
		// BlendAnimation sets two animations pointers at once to blend
		//	them for a certain amount of time
		//--------------------------------------------------------------------
		void BlendAnimation( const entAnimation* i_Anim1, float i_StartTime1,
							 const entAnimation* i_Anim2, float i_StartTime2,
							 float i_fBlendTime, 
							 bool i_bSmoothBlend = false,
							 float i_EaseInWeight = 1.0f, 
							 float i_EaseOutWeight = 1.0f );

		//--------------------------------------------------------------------
		// ClearAnimation stops entity from animating
		//--------------------------------------------------------------------
		void ClearAnimation();

		//--------------------------------------------------------------------
		//	ClearSubAnimations causes non-preserved sub animations to stop.
		//--------------------------------------------------------------------
		void ClearSubAnimations();

		//--------------------------------------------------------------------
		// AddSubAnimation adds this animation on top of base animation.
		// Returns a handle that is not owned by the caller.
		//	i_bPreserve - sets flag saying if the subanimation should
		//		remain during changes to the full animation.
		//--------------------------------------------------------------------
		entSubAnimation* AddSubAnimation( const entAnimation* i_SubAnim, 
										  float i_StartTime,
										  bool i_bPreserve );

		//--------------------------------------------------------------------
		// RemoveSubAnimation removes currently running subanimation.
		// The given pointer should have been returned from a previous call 
		// to AddSubAnimation. Notice that ClearAnimation will remove all
		// sub animations automatically.
		//--------------------------------------------------------------------
		void RemoveSubAnimation(entSubAnimation* i_pSubAnim);

		//--------------------------------------------------------------------
		// SetSubAnimationBlend sets how the given subanimation should 
		// blend with the base anim. Values from 0-1. The given subanimation
		// should have been returned from a call to AddSubAnimation first.
		//--------------------------------------------------------------------
		void SetSubAnimationBlend( const entSubAnimation* i_SubAnim, 
								   float i_Blend);

		//--------------------------------------------------------------------
		//  Try to use other functions instead of directly acessing
		//	the object pointer.  Maybe you could add the functions
		//	you need to a class that is derived from entEntity?
		//--------------------------------------------------------------------
		inline scObject* Object();
		inline const scObject* GetObject() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		entAnimInstance* GetAnimInstance() { return m_pCurAnimInstance; }
		const entAnimInstance* GetAnimInstance() const { return m_pCurAnimInstance; }

		//--------------------------------------------------------------------
		// GetAnimFinished returns true when the animation is not looping and
		// the final frame is played
		//--------------------------------------------------------------------
		inline bool GetAnimFinished();

		//--------------------------------------------------------------------
		// GetRenderable gets the renderable state for the entity
		//--------------------------------------------------------------------
		inline bool GetRenderable();

		//--------------------------------------------------------------------
		// SetRenderable sets the rendering on all the objects in the entity
		//--------------------------------------------------------------------
		void SetRenderable( bool i_bSetRenderable );

		//--------------------------------------------------------------------
		//	ActiveInRenderLayer sets whether the object is visible
		//	in render layer
		//--------------------------------------------------------------------
		void SetActiveInRenderLayer(bool i_bRenderable);
		bool GetActiveInRenderLayer() const;

		//--------------------------------------------------------------------
		//	ActiveInRenderLayer sets whether the object is visible
		//	in scene manager
		//--------------------------------------------------------------------
		void SetActiveInSceneMgr(bool i_bRenderable);
		bool GetActiveInSceneMgr() const;

		//--------------------------------------------------------------------
		//  GetNamedNode returns a pointer to the node with the given name
		//  or NULL if it does not exist.
		//--------------------------------------------------------------------
		const g3dSceneNode* GetNamedNode( const char* i_Name ) const;
		g3dSceneNode* GetNamedNode( const char* i_Name );

		//--------------------------------------------------------------------
		//	GetUniqueMaterials returns the list of unique materials
		//  used by this object.  Unique materials will be
		//	created the first time this is called.
		//--------------------------------------------------------------------
		//std::vector<matMaterial*>& GetUniqueMaterials();
		//const std::vector<matMaterial*>& GetUniqueMaterials() const;

	private:
		//void create_unique_materials() const;  // changes mutable member

		// base pointers hold most any type of object
		scObject* m_pObject;
		//const entModelTemplate &m_Template;

		// animation pointers may be NULL, but can animate
		// if non-NULL
		scAnimatableObject* m_pAnimationObj;
		//const entEntityTemplate *m_pEntityTemplate;

		entAnimInstance*	m_pCurAnimInstance;	// not owned
		bool m_bAnimFinished;
		bool m_bRenderable;
		bool m_bActiveInRenderLayer;
		bool m_bActiveInSceneMgr;
		//mutable std::vector<matMaterial*> m_UniqueMaterials;
};


//--------------------------------------------------------------------
//	GetTemplate returns the prop template which was used to create
//	the prop.
//--------------------------------------------------------------------
//inline const entModelTemplate& entEntity::GetTemplate() const
//{
//	return m_Template;
//}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
inline scObject* entEntity::Object()
{
	return m_pObject;
}

inline const scObject* entEntity::GetObject() const
{
	return m_pObject;
}

//--------------------------------------------------------------------
// GetAnimFinished returns true when the animation is not looping and
// the final frame is played
//--------------------------------------------------------------------
inline bool entEntity::GetAnimFinished()
{
	return m_bAnimFinished;
}

//--------------------------------------------------------------------
// GetRenderable gets the renderable state for the entity
//--------------------------------------------------------------------
inline bool entEntity::GetRenderable()
{
	return m_bRenderable;
}
