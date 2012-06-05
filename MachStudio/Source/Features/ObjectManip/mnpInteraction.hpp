/*****************************************************************************
**  mnpInteraction.hpp
**
**      mnpInteraction is the base class for ways to map mouse motion
**	into the movement of the selected objects.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef MNP_INTERACTION_HPP
#error mnpInteraction.hpp multiply included
#endif
#define MNP_INTERACTION_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

//----------------------------------------------------------------------------
//	forward references
//----------------------------------------------------------------------------
class g3dViewer;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class mnpInteraction 
{
public:
	//------------------------------------------------------------------------
	// An interaction is interacting as soon as it is created.
	//------------------------------------------------------------------------
	mnpInteraction();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~mnpInteraction() = 0;

	//------------------------------------------------------------------------
	// Handle mouse motion in terms of the X and Y mouse position
	// in the given viewer. Derived classes may prefer to override the
	// RayMotion function that is called by the default implementation
	// instead. 
	// If i_pViewer is NULL, use the Tool/tma3d classes to determine the
	// the cursor ray.
	//------------------------------------------------------------------------
	virtual void MouseMotion(int i_X, int i_Y, g3dViewer *i_pViewer = NULL);

	//------------------------------------------------------------------------
	// Abort the mouse interaction, return the objects back to their
	//	original positions.
	//------------------------------------------------------------------------
	virtual void AbortInteraction();

	//------------------------------------------------------------------------
	// Finish Interaction normally
	//------------------------------------------------------------------------
	virtual void FinishInteraction();

	//------------------------------------------------------------------------
	// IsInteracting returns true if the interaction is active.
	// The base class implementations of AbortInteraction and 
	// FinishInteraction set it to false.
	//------------------------------------------------------------------------
	bool IsInteracting() const { return m_bInteracting; }

protected:
	//------------------------------------------------------------------------
	// Handle mouse motion in terms of a position on the camera near plane 
	//	and the direction of the ray that passes through that point 
	//	into the camera view frustrum.
	// This is called by the default implementation of MouseMotion.
	//------------------------------------------------------------------------
	virtual void RayMotion(const maPoint3d& i_RayPos, 
						   const maVector3d& i_RayDir) {}
private:
	bool m_bInteracting;
};
