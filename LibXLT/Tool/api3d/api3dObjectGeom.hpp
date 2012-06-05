/*****************************************************************************
**	api3dObjectGeom.hpp
**
**	Derived class, implements api3dObject through scObject for
**	non-animating geometry
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_OBJECTGEOM_HPP
#error api3dObjectGeom.hpp multiply included
#endif
#define API3D_OBJECTGEOM_HPP

#ifndef API3D_OBJECTSINGLE_HPP
#include "Tool/api3d/api3dObjectSingle.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class scObject;
class entModelInstance;
class entModelTemplate;


//============================================================================
//============================================================================
class api3dObjectGeom : public api3dObjectSingle
{
public:
	//--------------------------------------------------------------------
	// constructor - this object assumes ownership of the arguments
	//--------------------------------------------------------------------
	api3dObjectGeom(entModelTemplate *i_Template, 	
					entModelInstance *i_Instance,
					scObject *i_Object);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~api3dObjectGeom();

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
	//	Set active sets whether the object is visible in render layer
	//--------------------------------------------------------------------
	virtual void SetActiveInRenderLayer(bool i_Renderable);
	virtual bool GetActiveInRenderLayer() const;

	//--------------------------------------------------------------------
	//	Set active sets whether the object is visible in scene manager
	//--------------------------------------------------------------------
	virtual void SetActiveInSceneMgr(bool i_Renderable);
	virtual bool GetActiveInSceneMgr() const;

	//--------------------------------------------------------------------
	//	GetWorldBox returns the bounding box
	//--------------------------------------------------------------------
	const maAxisBox& GetWorldBox() const;

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
	//	get a pointer to the template
	//----------------------------------------------------------------------------	
	entModelInstance* GetModelInstance();
	entModelTemplate* GetModelTemplate();

protected:
	//--------------------------------------------------------------------
	// protected acess for derived classes to create template and
	//	object in constructor
	//--------------------------------------------------------------------
	api3dObjectGeom();
	void SetPointers(entModelTemplate *i_Template, scObject *i_Object);

private:
	scObject *m_pObject;
	entModelTemplate *m_pModelTemplate;
	entModelInstance *m_pModelInstance;

	std::vector<api3dReference*> m_References;
};