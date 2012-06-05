/*****************************************************************************
**  mnpRotateInteraction.hpp
**
**      An interaction that rotates a set of objects around a pivot
**	based on mapping mouse motions.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MNP_ROTATEINTERACTION_HPP
#error mnpRotateInteraction.hpp multiply included
#endif
#define MNP_ROTATEINTERACTION_HPP


#ifndef MNP_INTERACTION_HPP
#include "Features/ObjectManip/mnpInteraction.hpp"
#endif 

#ifndef MA_ROTATION_HPP
#include "Core/Ma/maRotation.hpp"
#endif 


#include <list>

//----------------------------------------------------------------------------
//	forward references
//----------------------------------------------------------------------------
class mnmObject;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class mnpRotateInteraction : public mnpInteraction
{
public:	
	//------------------------------------------------------------------------
	// Begin interaction for the given selected object along the axis
	//	defined by the mouse down point and the projection axis given.
	//------------------------------------------------------------------------
	mnpRotateInteraction(mnmObject* i_pSelectedObject,
						  int i_X, int i_Y,
						  const maVector3d &i_RotationAxis);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~mnpRotateInteraction();

	//------------------------------------------------------------------------
	// Handle mouse motion. Rotation only wants the cursor position,
	//	could be changed to project to a 3D circle.
	//------------------------------------------------------------------------
	virtual void MouseMotion(int i_X, int i_Y, g3dViewer *i_pViewer = NULL);

	//------------------------------------------------------------------------
	// Abort the mouse interaction, return the objects back to their
	//	original positions.
	//------------------------------------------------------------------------
	virtual void AbortInteraction();

private:
	//------------------------------------------------------------------------
	// private functions
	//------------------------------------------------------------------------
	void apply_delta_rotation(const maRotation& i_Delta, 
							  const maPoint3d &i_Pivot, 
							  bool i_bNewOperation);

	bool					m_bNewOperation;
	mnmObject*				m_pSelectedObject;
	int						m_StartRotationValue;
	maPoint3d				m_PivotPoint;
	maVector3d				m_RotationAxis;
	maRotation				m_OriginalOrientation;
	std::list<mnmObject*>	m_AdditionalObjects;
};
