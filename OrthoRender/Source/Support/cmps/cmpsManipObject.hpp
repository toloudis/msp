/*****************************************************************************
**  cmpsManipObject.hpp
**
**      A cmpsManipObject is base class for objects that can be positioned
**  by manipulation modes.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef CMPS_MANIPOBJECT_HPP
#error cmpsManipObject.hpp multiply included
#endif
#define CMPS_MANIPOBJECT_HPP

#ifndef API3D_OBJECT_HPP
#include "Tool/api3d/api3dObject.hpp"
#endif
#ifndef MA_AXISBOX_HPP
#include "Core/ma/maAxisBox.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif

#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif

#include <vector>


//============================================================================
//	forward references
//============================================================================
class api3dReference;
class nameString;


//============================================================================
//============================================================================
class cmpsManipObject : public pick3dPickObject
{
	public:
		//--------------------------------------------------------------------
		// Flags for how object can be modified
		//--------------------------------------------------------------------
		enum RotateFlags
		{
			e_RotateNone = 0,
			e_RotateAll,
			e_RotateX,
			e_RotateY,
			e_RotateZ,
			e_RotateXY,
			e_RotateXZ,
			e_RotateYZ,
		};
		enum ScaleFlags
		{
			e_ScaleNone = 0,
			e_ScaleUniform,
			e_ScaleCylindrical
		};
		enum TranslateFlags
		{
			e_TranslateNone = 0,
			e_TranslateAll,
			e_TranslateX,
			e_TranslateY,
			e_TranslateZ,
			e_TranslateXY,
			e_TranslateXZ,
			e_TranslateYZ,
			e_TranslateGround,	// stick to the surface of things
		};

		//--------------------------------------------------------------------
		// Operations that can be enabled/disabled temporarily
		//--------------------------------------------------------------------
		enum Operations
		{
			e_None = 0,
			e_Translate,
			e_Rotate,
			e_Scale
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmpsManipObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmpsManipObject();

		//--------------------------------------------------------------------
		//	Position
		// i_bNewOperation is true if this begins a new undo operation.
		//--------------------------------------------------------------------
		virtual maPoint3d GetPosition() const = 0;
		virtual void UpdatePosition(const maPoint3d& i_Position, 
									bool i_bNewOperation = true) = 0;

		//--------------------------------------------------------------------
		//	Orientation
		// i_bNewOperation is true if this begins a new undo operation.
		//--------------------------------------------------------------------
		virtual maRotation GetOrientation() const = 0;
		virtual void UpdateOrientation(const maRotation& i_Orientation, 
										bool i_bNewOperation = true) = 0;

		//--------------------------------------------------------------------
		//	Scale
		// i_bNewOperation is true if this begins a new undo operation.
		//--------------------------------------------------------------------
		virtual maPoint3d GetScale() const = 0;
		virtual void UpdateScale(const maPoint3d& i_Scale, 
								 bool i_bNewOperation = true) = 0;

		//--------------------------------------------------------------------
		//	RayPick returns true if the given ray intersects the cmpsManipObject.
		//	If it does, the t value is also returned in o_T.
		//--------------------------------------------------------------------
		virtual bool RayPick(	const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T);

		//--------------------------------------------------------------------
		//	Renderable sets whether the cmpsManipObject can be selected.
		//--------------------------------------------------------------------
		//virtual void SetRenderable(bool i_Renderable) = 0;
		//virtual bool GetRenderable() const = 0;

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
		// For polling if an manipulation operation is currently enabled
		//--------------------------------------------------------------------
		virtual bool IsOperationEnabled(Operations i_Operation, float i_Time);
};


