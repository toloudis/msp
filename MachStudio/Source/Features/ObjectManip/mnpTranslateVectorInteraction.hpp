/*****************************************************************************
**  mnpTranslateVectorInteraction.hpp
**
**      An interaction that translates a set of objects along a vector,
**	mapping mouse motions to that vector.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MNP_TRANSLATEVECTORINTERACTION_HPP
#error mnpTranslateVectorInteraction.hpp multiply included
#endif
#define MNP_TRANSLATEVECTORINTERACTION_HPP

#ifndef MNP_TRANSLATEINTERACTION_HPP
#include "Features/ObjectManip/mnpTranslateInteraction.hpp"
#endif 

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class mnpTranslateVectorInteraction : public mnpTranslateInteraction
{
public:
	//------------------------------------------------------------------------
	// Begin interaction for the given selected object along the axis
	//	defined by the mouse down point and the projection axis given.
	//------------------------------------------------------------------------
	mnpTranslateVectorInteraction(mnmObject* i_pSelectedObject,
						  const maPoint3d &i_MouseDownPoint,
						  const maVector3d &i_ProjectionAxis);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~mnpTranslateVectorInteraction();


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
	virtual void RayMotion(const maPoint3d& i_RayPos, const maVector3d& i_RayDir);

private:
	//------------------------------------------------------------------------
	// private functions
	//------------------------------------------------------------------------
	void apply_delta_position(const maVector3d& i_Delta, bool i_bNewOperation);

	bool					m_bInteracting;
	bool					m_bNewOperation;

	maPoint3d				m_MouseDownPoint;
	maVector3d				m_ProjectionAxis;
	maVector3d				m_OriginalPosition;
};
