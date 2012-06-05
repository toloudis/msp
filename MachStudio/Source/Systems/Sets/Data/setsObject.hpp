/*****************************************************************************
**  setsObject.hpp
**
**      A setsObject holds the api3dObject for a set item.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef SETS_OBJECT_HPP
#error setsObject.hpp multiply included
#endif
#define SETS_OBJECT_HPP

#ifndef FGMT_SCRIPTOBJECT_HPP
#include "Support/fgmt/fgmtScriptObject.hpp"
#endif
#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef LYER_OBJECT_HPP
#include "Support/lyer/lyerObject.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MTRL_SCRIPTOBJECT_HPP
#include "Support/mtrl/mtrlScriptObject.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif


//============================================================================
//	forward references
//============================================================================
class api3dObject;


//============================================================================
//============================================================================
class setsObject : public nameObject, 
					public lyerObject,
					public mtrlScriptObject,
					public fgmtScriptObject,
					public pick3dPickObject,
					public prtyObject
{
	public:
		//--------------------------------------------------------------------
		// ownership for the api3dObject passes to this object.
		// Locator should point to the geometry file in order to parse
		//	and write materials.
		//--------------------------------------------------------------------
		setsObject(api3dObject* i_pObject, const fsLocator& i_ModelFile);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~setsObject();

		//--------------------------------------------------------------------
		// Position
		//--------------------------------------------------------------------
		maPoint3d GetPosition() const;

		//--------------------------------------------------------------------
		//  Changes visible state of character based on GUI
		//--------------------------------------------------------------------
		void  SetEditorVisible(bool i_bVisible);
		bool GetEditorVisible() const;

		//--------------------------------------------------------------------
		//	LayerVisible represents if objects in the layer are visible
		//--------------------------------------------------------------------
		virtual void SetLayerVisible(bool i_bVisible);

		//--------------------------------------------------------------------
		//	LayerPickable represents if objects in the layer can be picked.
		//--------------------------------------------------------------------
		virtual void SetLayerPickable(bool i_bPickable);

		//--------------------------------------------------------------------
		//	LayerWireframe represents if the objects are rendered
		//	in a wireframe style.
		//--------------------------------------------------------------------
		virtual void SetLayerWireframe(bool i_bWireframe);

		//----------------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the chtrObject.
		//	If it does, the t value is also returned in o_T.
		//----------------------------------------------------------------------------
		bool RayPick(	const maPoint3d& i_RayStart,
						const maPoint3d& i_RayEnd,
						float& o_T);

		//----------------------------------------------------------------------------
		// Find the object that matches the pick code from an earlier pick render.
		//----------------------------------------------------------------------------
		virtual bool MatchPickCode(envType::UInt32 i_PickCode) const;

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetPick3dName() const;

	protected:
		//--------------------------------------------------------------------
		// This virtual function is called when the materials are saved 
		//	to a new filename. The locator representing this geometry
		//	should now point to the new filename.
		//--------------------------------------------------------------------
		//virtual void NotifyLocatorChanged(const fsLocator& i_NewLocator);

	private:
		api3dObject * m_p3DObject;
		bool m_bEditorVisible;
		std::string m_DisplayName;
};

