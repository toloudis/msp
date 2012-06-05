/*****************************************************************************
**  setsObject.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Sets/Data/setsObject.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"
#include "Support/grps/grpsGroupMgr.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"
#include "Support/lyer/lyerLayerMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"

// library
#include "Tool/api3d/api3dObjectSingle.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/geo/geoRayIntersection.hpp"


//----------------------------------------------------------------------------
// ownership for the api3dObject passes to this object
// Locator should point to the geometry file in order to parse
//	and write materials.
//----------------------------------------------------------------------------
setsObject::setsObject( api3dObject* i_pObject, const fsLocator& i_ModelFile )
: m_p3DObject(i_pObject), 
	mtrlScriptObject(i_pObject, i_ModelFile), 
	fgmtScriptObject(i_pObject, i_ModelFile), 
	m_bEditorVisible(true)
{
	fsLocator aoTexDir = gfPaths::GetPath(mnmPaths::e_DataScene);
	aoTexDir.Push("Sets");
	aoTexDir.Push("General");
	aoTexDir.Push(gfPaths::GetSubPath(gfPaths::e_Textures));
	SetAOTextureLocator(aoTexDir);

	api3dScene::AddObject(i_pObject);

	api3dObjectSingle *obj_single = dynamic_cast<api3dObjectSingle *>(i_pObject);
	if (obj_single)
	{
		ltstLightSetMgr::AddObject(this, obj_single);	
		evmtEnvironmentMgr::AddObject(this, obj_single);
	}

	// Connect up fgmtscriptobject prty's to be shown in this UI
	fgmtScriptObject::SetPrtyObject(this);

	lyerLayerMgr::AddObject(this, this);
	grpsGroupMgr::AddObject(this, this);

	// Set name for GUI from the filename
	m_DisplayName = itStringUtil::GetStdString(i_ModelFile.GetLastName());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
setsObject::~setsObject()
{
	api3dObjectSingle *obj_single = dynamic_cast<api3dObjectSingle *>(m_p3DObject);
	if (obj_single)
	{
		ltstLightSetMgr::RemoveObject(this, obj_single);
		evmtEnvironmentMgr::RemoveObject(this, obj_single);
	}

	lyerLayerMgr::RemoveObject(this, this);
	grpsGroupMgr::RemoveObject(this, this);

	api3dScene::RemoveObject(m_p3DObject);
	delete m_p3DObject;
}

//--------------------------------------------------------------------
// Position
//--------------------------------------------------------------------
maPoint3d setsObject::GetPosition() const
{
	return m_p3DObject->GetPosition();
}

//--------------------------------------------------------------------
//  Changes visible state of character based on GUI
//--------------------------------------------------------------------
void  setsObject::SetEditorVisible(bool i_bVisible)
{
	m_bEditorVisible = i_bVisible;

	// object is visible only if gui and layer are all
	// are set visible == true;
	m_p3DObject->SetRenderable(m_bEditorVisible && GetLayerVisible());
}
bool setsObject::GetEditorVisible() const
{
	return m_bEditorVisible;
}

//--------------------------------------------------------------------
//	LayerVisible represents if objects in the layer are visible
//--------------------------------------------------------------------
//virtual 
void setsObject::SetLayerVisible(bool i_bVisible)
{
	// base function sets private flag
	lyerObject::SetLayerVisible(i_bVisible);

	// object is visible only if channel and gui and layer are all
	// are set visible == true;
	m_p3DObject->SetRenderable(m_bEditorVisible && i_bVisible);

}

//--------------------------------------------------------------------
//	LayerPickable represents if objects in the layer can be picked.
//--------------------------------------------------------------------
//virtual 
void setsObject::SetLayerPickable(bool i_bPickable)
{
	// base function sets private flag
	lyerObject::SetLayerPickable(i_bPickable);

	m_p3DObject->SetGPUPickable( i_bPickable );
}

//--------------------------------------------------------------------
//	LayerWireframe represents if the objects are rendered
//	in a wireframe style.
//--------------------------------------------------------------------
//virtual 
void setsObject::SetLayerWireframe(bool i_bWireframe)
{
	// base function sets private flag
	lyerObject::SetLayerWireframe(i_bWireframe);

	m_p3DObject->SetWireframe(i_bWireframe);
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the chtrObject.
//	If it does, the t value is also returned in o_T.
//----------------------------------------------------------------------------
bool setsObject::RayPick(	const maPoint3d& i_RayStart,
							const maPoint3d& i_RayEnd,
							float& o_T)
{
	if (!m_p3DObject->GetRenderable() || !this->GetLayerPickable())
		return false;

	// hmm, this seems to have a box that is just a single point.
	// Is this intended?  I guess the original intention was that
	// set objects did a full triangle pick with the use of
	// a kdTree. I wonder if we could afford to do that now with
	// our memory limitations?
	return geoRayIntersection::IntersectLineBBox(  i_RayStart,
													(i_RayEnd - i_RayStart),
													this->GetPosition(),
													this->GetPosition(),
													o_T );

	//const maAxisBox &box = this->GetWorldBox();

	//return geoRayIntersection::IntersectLineBBox( i_RayStart,
	//							(i_RayEnd - i_RayStart),
	//							box.GetBoxPoint(7),
	//							box.GetBoxPoint(0),
	//							o_T );
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool setsObject::MatchPickCode(envType::UInt32 i_PickCode) const
{
	if (!m_p3DObject->GetRenderable() || !this->GetLayerPickable())
		return false;

	return (m_p3DObject->ContainsPickCode(i_PickCode));
}

//--------------------------------------------------------------------
// This virtual function is called when the materials are saved 
//	to a new filename. The locator representing this geometry
//	should now point to the new filename.
//--------------------------------------------------------------------
//virtual 
void setsObject::NotifyLocatorChanged(const fsLocator& i_NewLocator)
{
	// The Sets system is scheduled to be removed. So, I am not implementing
	//	this, with the expectation that the code will not ever get here.
	DBG_ASSERT0(false, "Sets are not written to have their locators changed!");
}

//--------------------------------------------------------------------
// Get the name of the object.
//--------------------------------------------------------------------
//virtual 
std::string setsObject::GetPick3dName() const
{
	return m_DisplayName;
}