/*****************************************************************************
**  mnpFreeFormInteraction.hpp
**
**      An interaction that translates a set of objects in the XZ plane.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MNP_FREEFORMINTERACTION_HPP
#error mnpFreeFormInteraction.hpp multiply included
#endif
#define MNP_FREEFORMINTERACTION_HPP

#ifndef MNP_TRANSLATEINTERACTION_HPP
#include "Features/ObjectManip/mnpTranslateInteraction.hpp"
#endif 

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <list>

//----------------------------------------------------------------------------
//	forward references
//----------------------------------------------------------------------------
class mnmObject;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class mnpFreeFormInteraction : public mnpTranslateInteraction
{
public:
	//------------------------------------------------------------------------
	// Begin interaction for the given selected object along the axis
	//	defined by the mouse down point and the projection axis given.
	//------------------------------------------------------------------------
	mnpFreeFormInteraction(mnmObject* i_pSelectedObject,
						  const maPoint3d &i_MouseDownPoint);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~mnpFreeFormInteraction();

	//------------------------------------------------------------------------
	// Abort the mouse interaction, return the objects back to their
	//	original positions.
	//------------------------------------------------------------------------
	void AbortInteraction();

protected:
	//------------------------------------------------------------------------
	// Handle mouse motion. Mouse position is given in terms of a 
	// position on the camera near plane and the direction of the
	// ray that passes through that point into the camera view frustrum.
	//------------------------------------------------------------------------
	void RayMotion(const maPoint3d& i_RayPos, const maVector3d& i_RayDir);

private:
	bool					m_bNewOperation;
	maPoint3d				m_MouseDownPoint;
	maVector3d				m_SelectionOffset;
	maVector3d				m_OriginalPosition;
};
