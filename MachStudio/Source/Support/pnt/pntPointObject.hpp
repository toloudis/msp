/*****************************************************************************
**  pntPointObject.hpp
**
**      A pntPointObject allows a user to manipulate a point.
**
**	StudioGPU
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
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif 
#ifndef ICN_ICONSCALE_HPP
#include "Tool/icn/icnIconScale.hpp"
#endif 
#ifndef CMM_SELECTABLEPROPERTYOBJECT_HPP
#include "Systems/Common/Object/cmmSelectablePropertyObject.hpp"
#endif 



//============================================================================
//	Forward References
//============================================================================
class pntPoint;
class icnIconSet;
class gpxIconSet;

//============================================================================
//============================================================================
class pntPointObject : public mnmObject, 
					   public cmmSelectablePropertyObject, 
					   public icnIconScaleInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pntPointObject( pntPoint *i_Point, 
						const std::string &i_Name);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~pntPointObject();

		//--------------------------------------------------------------------
		//	GlobalScaleChanged - notification that global scale has changed.
		//--------------------------------------------------------------------
		//virtual void GlobalScaleChanged( float i_Scale );

		//--------------------------------------------------------------------
		//	UpdateIconScale - function that sets the icons scale.
		//--------------------------------------------------------------------
		virtual void UpdateIconScale( int i_IconLayerIndex, const camCamera& i_Camera, float i_Scale );

		//--------------------------------------------------------------------
		//	GetPoint - get point it is changing
		//--------------------------------------------------------------------
		pntPoint* GetPoint();

		//--------------------------------------------------------------------
		//	GetWorldBox returns a box which completely encloses the mnmObject.
		//--------------------------------------------------------------------
		virtual maAxisBox GetWorldBox(int i_IconLayerIndex = 0) const;

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
		//virtual sel3dObject* GetParentObject() const;

		//--------------------------------------------------------------------
		// Get the name of the object.
		//--------------------------------------------------------------------
		virtual std::string GetDisplayName() const;

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void PositionChanged(prtyProperty *i_pProperty, bool i_bDirty);

		pntPoint *		m_pPoint;
		icnIconSet*		m_pObject;		// 3D icon/geometry info	
		gpxIconSet*		m_pObjectProxy;	// thread-safe proxy to object	
		//sel3dObject* m_pParent;
		prtyPoint3d		m_Position;
		std::string		m_Name;
};
