/*****************************************************************************
**	api3dObjectEntity.hpp
**
**	Derived class, implements api3dObject through entEntity
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_OBJECTENTITY_HPP
#error api3dObjectEntity.hpp multiply included
#endif
#define API3D_OBJECTENTITY_HPP

#ifndef API3D_OBJECTSINGLE_HPP
#include "Tool/api3d/api3dObjectSingle.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class entEntity;
class entModelInstance;
class entModelTemplate;
class scObject;
class maAxisBox;


//============================================================================
//============================================================================
class api3dObjectEntity : public api3dObjectSingle
{
public:
	//--------------------------------------------------------------------
	// constructor - this object assumes ownership of the arguments
	//--------------------------------------------------------------------
	api3dObjectEntity(entModelTemplate *i_Template, 
					  entModelInstance *i_Instance, 
					  entEntity *i_Entity);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~api3dObjectEntity();

	//--------------------------------------------------------------------
	// Position
	//--------------------------------------------------------------------
	virtual maPoint3d  GetPosition() const;
	virtual void  SetPosition(const maPoint3d &i_Position);

	//--------------------------------------------------------------------
	// Orientation
	//--------------------------------------------------------------------
	virtual maRotation GetOrientation() const;
	virtual void  SetOrientation(const maRotation &i_Rotation);

	//--------------------------------------------------------------------
	// Scale
	//--------------------------------------------------------------------
	virtual maVector3d GetScale() const;
	virtual void  SetScale(const maVector3d& i_Scale);
	virtual void  SetUniformScale(const float i_fScale);

	//----------------------------------------------------------------------------
	//	Renderable sets whether the object is visible
	//----------------------------------------------------------------------------
	void SetRenderable(bool i_Renderable);
	bool GetRenderable() const;

	//--------------------------------------------------------------------
	//	ActiveInRenderLayer sets whether the object is visible
	//	in render layer
	//--------------------------------------------------------------------
	void SetActiveInRenderLayer(bool i_bRenderable);
	bool GetActiveInRenderLayer() const;

	//--------------------------------------------------------------------
	//	ActiveInSceneMgr sets whether the object is visible
	//	in scene manager
	//--------------------------------------------------------------------
	void SetActiveInSceneMgr(bool i_bRenderable);
	bool GetActiveInSceneMgr() const;

	//--------------------------------------------------------------------
	//	GetWorldBox returns the bounding box
	//--------------------------------------------------------------------
	const maAxisBox& GetWorldBox() const;
	
	//--------------------------------------------------------------------
	// Force through one frame of animation in order to 
	// get an updated bounding box
	//--------------------------------------------------------------------
	void AnimateSingleFrame();

	//--------------------------------------------------------------------
	// Set color (emissive, not diffuse) for fragments
	//--------------------------------------------------------------------
	void SetColor(const maFloatRGBA &i_Color);

	//--------------------------------------------------------------------
	//	Get reference for given name.  The returned pointer is owned
	//	by this object.  The object should be retained by the caller
	//	to avoid repeated string searches.
	//--------------------------------------------------------------------
	virtual api3dReference* GetReference(const char* i_Name);

	//--------------------------------------------------------------------
	// Get list of references
	//--------------------------------------------------------------------
	virtual void GetReferenceList(std::vector<std::string> &o_List);

	//----------------------------------------------------------------------------
	//	get a pointer to the scene object
	//----------------------------------------------------------------------------
	virtual const scObject* GetObject() const;
	virtual scObject* Object();

	//----------------------------------------------------------------------------
	//	get a pointer to the entity
	//----------------------------------------------------------------------------
	entEntity* GetEntity();

	//----------------------------------------------------------------------------
	//	get a pointer to the entity template.
	//	Note: it is possible that the EntityTemplate is NULL if this is
	//	non-animating geometry.
	//----------------------------------------------------------------------------
	entModelInstance* GetModelInstance();
	entModelTemplate* GetModelTemplate();
	//entEntityTemplate* GetEntityTemplate();

private:
	entEntity *m_pEntity;
	entModelInstance *m_pModelInstance;
	entModelTemplate *m_pModelTemplate;

	std::vector<api3dReference*> m_References;
};