/*****************************************************************************
**  mnmObject.hpp
**
**      A mnmObject is base class for objects that can be positioned
**  by manipulation modes.
**
**	StudioGPU
**	Copyright(C) 2001 - All Rights Reserved
\****************************************************************************/
#ifdef MNM_OBJECT_HPP
#error mnmObject.hpp multiply included
#endif
#define MNM_OBJECT_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif
#ifndef MA_ROTATION_HPP
#include "Core/ma/maRotation.hpp"
#endif

#ifndef CMPS_MANIPOBJECT_HPP
#include "Support/cmps/cmpsManipObject.hpp"
#endif

#include <vector>


//============================================================================
//	forward references
//============================================================================
class maAxisBox;
class api3dReference;


//============================================================================
//============================================================================
class mnmObject : public cmpsManipObject
{
public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		mnmObject();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~mnmObject();

		//--------------------------------------------------------------------
		//	GetWorldBox returns a box which completely encloses the mnmObject.
		//--------------------------------------------------------------------
		virtual maAxisBox GetWorldBox(int i_IconLayerIndex = 0) const = 0;

		//--------------------------------------------------------------------
		//	GetWorldPivot returns the point in world space that this
		//		object will rotate around. Used to center rotation and
		//		scale compasses.
		//--------------------------------------------------------------------
		virtual maPoint3d GetWorldPivot() const;
		
		//--------------------------------------------------------------------
		//  Get sum of matrices of all parents of this node. 
		//--------------------------------------------------------------------
		virtual void GetParentMatrix(maMatrix4x4 &o_Transformation) const;

		//-------------------------------------------------------------------- 
		// Get world space rotation for the object by combining local 
		//	rotation with the rotation from the parent matrix.
		//-------------------------------------------------------------------- 
		maRotation GetWorldOrientation() const;

		//--------------------------------------------------------------------
		// Change the pivot point of this object to the center 
		// of its bounding box
		//--------------------------------------------------------------------
		virtual void CenterPivot();

		//--------------------------------------------------------------------
		//	Get reference for given name.  The returned pointer is owned
		//	by this object.  The object should be retained by the caller
		//	to avoid repeated string searches.
		//--------------------------------------------------------------------
		virtual api3dReference* GetReference(const char* i_Name);

		//--------------------------------------------------------------------
		// Get list of references for possible attachment within this object.
		//--------------------------------------------------------------------
		virtual void GetReferenceList(std::vector<std::string> &o_List);

	protected:
		//--------------------------------------------------------------------
		// Add reference to list to be managed.
		//--------------------------------------------------------------------
		void AddReference(api3dReference* i_pReference);

	protected:
		std::vector<api3dReference*> m_References;
};
