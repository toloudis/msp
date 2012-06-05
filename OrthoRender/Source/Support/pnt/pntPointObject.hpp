/*****************************************************************************
**  pntPointObject.hpp
**
**      A pntPointObject allows a user to manipulate a point.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef PNT_POINTOBJECT_HPP
#error pntPointObject.hpp multiply included
#endif
#define PNT_POINTOBJECT_HPP

#ifndef MNM_OBJECT_HPP
#include "Support/mnm/mnmObject.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif 
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif 



//============================================================================
//	Forward References
//============================================================================
class pntPoint;
class api3dObject;

//============================================================================
//============================================================================
class pntPointObject : public mnmObject, public prtyObject
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pntPointObject( pntPoint *i_Point, 
						pick3dPickObject* i_pParent,
						const std::string &i_Name);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~pntPointObject();

		//--------------------------------------------------------------------
		//	GetPoint - get point it is changing
		//--------------------------------------------------------------------
		pntPoint* GetPoint();

		//--------------------------------------------------------------------
		//	GetWorldBox returns a box which completely encloses the mnmObject.
		//--------------------------------------------------------------------
		virtual const maAxisBox& GetWorldBox() const;

		//--------------------------------------------------------------------
		//	GetLocalBox returns a box which would enclose the mnmObject
		//	if its transformations were identity
		//--------------------------------------------------------------------
		virtual const maAxisBox& GetLocalBox() const;

		//--------------------------------------------------------------------
		//	Position
		//--------------------------------------------------------------------
		virtual maPoint3d GetPosition() const;
		virtual void SetPosition(const maPoint3d& i_Position);

		//--------------------------------------------------------------------
		//	UpdatePosition - called when the point is altered from
		//	user input.
		//--------------------------------------------------------------------
		virtual void UpdatePosition(const maPoint3d& i_Position, 
										bool i_bNewOperation);

		//--------------------------------------------------------------------
		//	Orientation
		//--------------------------------------------------------------------
		virtual maRotation GetOrientation() const;
		virtual void UpdateOrientation(const maRotation& i_Orientation, 
										bool i_bNewOperation);

		//--------------------------------------------------------------------
		//	Scale
		//--------------------------------------------------------------------
		virtual maPoint3d GetScale() const;
		virtual void UpdateScale(const maPoint3d& i_Scale, 
										bool i_bNewOperation);

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the mnmObject.
		//	If it does, the t value is also returned in o_T.
		//--------------------------------------------------------------------
		virtual bool RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T);

		//----------------------------------------------------------------------------
		// Find the object that matches the pick code from an earlier pick render.
		//----------------------------------------------------------------------------
		virtual bool MatchPickCode(envType::UInt32 i_PickCode) const;

		//--------------------------------------------------------------------
		//	Renderable sets whether the mnmObject can be selected.
		//--------------------------------------------------------------------
		virtual void SetRenderable(bool i_Renderable);
		virtual bool GetRenderable() const;

		//--------------------------------------------------------------------
		//	GetDefaultTerrainOffset is the desired offset from
		//	the terrain for this object.  This can be altered
		//	by the user during placement
		//--------------------------------------------------------------------
		virtual float GetDefaultTerrainOffset() const;

		//--------------------------------------------------------------------
		// Flags for how object can be modified
		//--------------------------------------------------------------------
		virtual RotateFlags	GetRotateFlags();
		virtual ScaleFlags	GetScaleFlags();
		virtual TranslateFlags GetTranslateFlags();

		//--------------------------------------------------------------------
		// Get parent object of this object in order to define relationships
		//	between icons and their affected objects.
		//--------------------------------------------------------------------
		virtual pick3dPickObject* GetParentObject() const;

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetPick3dName() const;

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void PositionChanged(prtyProperty *i_pProperty, bool i_bDirty);

		pntPoint *		m_pPoint;
		api3dObject *	m_pObject;
		maAxisBox		m_WorldBox;
		pick3dPickObject* m_pParent;
		prtyPoint3d		m_Position;
		std::string		m_Name;
};
