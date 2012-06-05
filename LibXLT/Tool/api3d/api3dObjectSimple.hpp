/*****************************************************************************
**	api3dObjectSimple.hpp
**
**	Derived class, implements api3dObject for a simple object with
**	a single fragment and single material. It is often used to make icons
**	in the 3D interface.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef API3D_OBJECTSIMPLE_HPP
#error api3dObjectSimple.hpp multiply included
#endif
#define API3D_OBJECTSIMPLE_HPP

#ifndef API3D_OBJECTSINGLE_HPP
#include "Tool/api3d/api3dObjectSingle.hpp"
#endif


//============================================================================
//============================================================================
class scObject;
class g3dFragment;
class matMaterial;


//============================================================================
//============================================================================
class api3dObjectSimple : public api3dObjectSingle
{
public:
	//--------------------------------------------------------------------
	// constructor - this object assumes ownership of the arguments.
	//	It will also assign the material to the fragment.
	//--------------------------------------------------------------------
	api3dObjectSimple(g3dFragment* i_pFragment, matMaterial *i_pMaterial);

	//--------------------------------------------------------------------
	// constructor - this object assumes ownership of the arguments.
	//	It will also assign the material to the fragment.
	//  This variation allows you to apply a transformation to the
	//  fragment.
	//--------------------------------------------------------------------
	api3dObjectSimple(const maMatrix4x4 &i_Transform, 
				      g3dFragment* i_pFragment, 
					  matMaterial *i_pMaterial);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~api3dObjectSimple();

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
	//	ActiveInRenderLayer sets whether the object is visible
	//	in render layer
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

	//----------------------------------------------------------------------------
	//	get a pointer to the fragment
	//----------------------------------------------------------------------------
	const g3dFragment* GetFragment() const { return m_pFragment; }
	g3dFragment* Fragment() { return m_pFragment; }

	//----------------------------------------------------------------------------
	//	get a pointer to the material
	//----------------------------------------------------------------------------
	const matMaterial* GetMaterial() const { return m_pMaterial; }
	matMaterial* Material() { return m_pMaterial; }

private:
	scObject *m_pObject;
	g3dFragment* m_pFragment; 
	matMaterial* m_pMaterial;
};