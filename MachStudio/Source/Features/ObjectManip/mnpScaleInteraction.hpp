/*****************************************************************************
**  mnpScaleInteraction.hpp
**
**      An interaction that scales a set of objects based on
**	mouse motions.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MNP_SCALEINTERACTION_HPP
#error mnpScaleInteraction.hpp multiply included
#endif
#define MNP_SCALEINTERACTION_HPP

#ifndef MNP_INTERACTION_HPP
#include "Features/ObjectManip/mnpInteraction.hpp"
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
class mnpScaleInteraction  : public mnpInteraction
{
public:
	//------------------------------------------------------------------------
	// Begin interaction for the given selected object based on 
	//	the mouse down point.
	//------------------------------------------------------------------------
	mnpScaleInteraction(mnmObject* i_pSelectedObject,
						  const maPoint3d &i_MouseDownPoint,
						  const maVector3d &i_ProjectionAxis,
						  bool i_bFlipDirection);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~mnpScaleInteraction();

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
	//------------------------------------------------------------------------
	// private functions
	//------------------------------------------------------------------------
	void apply_delta_scale(float i_Delta, bool i_bNewOperation);

	bool					m_bNewOperation;
	bool					m_bFlipDirection;
	mnmObject*				m_pSelectedObject;
	maPoint3d				m_MouseDownPoint;
	maVector3d				m_ProjectionAxis;
	maVector3d				m_OriginalScale;
	std::list<mnmObject*>	m_AdditionalObjects;
};
