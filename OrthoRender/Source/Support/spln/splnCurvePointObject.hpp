/*****************************************************************************
**  splnCurvePointObject.hpp
**
**      A splnCurvePointObject allows a user to manipulate
**	control point of a curve.
**
**	Extra Large Technology
**	Copyright(C) 2001 - All Rights Reserved
\****************************************************************************/

#ifdef SPLN_CURVEPOINTOBJECT_HPP
#error splnCurvePointObject.hpp multiply included
#endif
#define SPLN_CURVEPOINTOBJECT_HPP

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

#include <vector>


//============================================================================
//============================================================================
class splnSpline;
class api3dObject;
class maFloatRGBA;


//============================================================================
//============================================================================
class splnCurvePointObject : public mnmObject, public prtyObject
{
	public:
		// Type for callback function
		class PointChangedCallback
		{
		public:
			virtual void PointChanged(splnCurvePointObject*) = 0;
		};

	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		splnCurvePointObject( splnSpline *i_Curve,
							int i_PointIndex, 
							pick3dPickObject* i_pParent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~splnCurvePointObject();

		//--------------------------------------------------------------------
		//	GetPointIndex - get index of control point it is changing
		//--------------------------------------------------------------------
		int	GetPointIndex();

		//--------------------------------------------------------------------
		//	GetCurve - get curve it is changing
		//--------------------------------------------------------------------
		splnSpline* GetCurve();

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
		//	UpdatePosition - called when control point is altered from
		//	user input.  Need to propagate to spline.
		//--------------------------------------------------------------------
		virtual void UpdatePosition(const maPoint3d& i_Position, 
										bool i_bNewOperation = true);

		//--------------------------------------------------------------------
		//	Orientation
		//--------------------------------------------------------------------
		virtual maRotation GetOrientation() const;
		virtual void UpdateOrientation(const maRotation& i_Orientation, 
										bool i_bNewOperation = true);

		//--------------------------------------------------------------------
		//	Scale
		//--------------------------------------------------------------------
		virtual maPoint3d GetScale() const;
		virtual void UpdateScale(const maPoint3d& i_Scale, 
										bool i_bNewOperation = true);

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

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void SetColor( maFloatRGBA& i_Color );

		//--------------------------------------------------------------------
		// Set callback for when point changes value.
		//--------------------------------------------------------------------
		void AddCallback(PointChangedCallback *i_pCallback);
		void RemoveCallback(PointChangedCallback *i_pCallback);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void PositionChanged(prtyProperty *i_pProperty, bool i_bDirty);

		splnSpline *m_Curve;
		int m_PointIndex;
		api3dObject *m_pObject;
		maAxisBox m_WorldBox;
		pick3dPickObject* m_pParent;
		prtyPoint3d		m_Position;

		std::vector<PointChangedCallback*> m_Callbacks;
};
