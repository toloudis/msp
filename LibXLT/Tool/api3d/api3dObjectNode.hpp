/*****************************************************************************
**	api3dObjectNode.hpp
**
**	Derived class, implements api3dObject for an empty object that will
**	be used to group other objects beneath it. This object just
**	represents the transformation in the node itself.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_OBJECTNODE_HPP
#error api3dObjectNode.hpp multiply included
#endif
#define API3D_OBJECTNODE_HPP

#ifndef API3D_OBJECTSINGLE_HPP
#include "Tool/api3d/api3dObjectSingle.hpp"
#endif


//============================================================================
//============================================================================
class scObject;


//============================================================================
//============================================================================
class api3dObjectNode : public api3dObjectSingle
{
public:
	//--------------------------------------------------------------------
	// constructor - will create a single node and an 
	//	scObject to control it
	//--------------------------------------------------------------------
	api3dObjectNode();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~api3dObjectNode();

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
	// Set color (emissive, not diffuse) for fragments
	//--------------------------------------------------------------------
	virtual void SetColor(const maFloatRGBA &i_Color);

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

	//--------------------------------------------------------------------
	// Accessor to object
	//--------------------------------------------------------------------
	const scObject* GetObject() const { return m_pObject; }
	scObject* Object() { return m_pObject; }

private:
	scObject *m_pObject;
};
