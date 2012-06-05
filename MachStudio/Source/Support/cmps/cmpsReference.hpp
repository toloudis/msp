#error cmpsReference.hpp is obsolete
/*****************************************************************************
**	cmpsReference.hpp
**
**	Default reference implementation, returns matrix based on object's
**	transformation.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CMPS_REFERENCE_HPP
#error cmpsReference.hpp multiply included
#endif
#define CMPS_REFERENCE_HPP


#ifndef API3D_REFERENCE_HPP
#include "Tool/api3d/api3dReference.hpp"
#endif

//============================================================================
//============================================================================
class cmpsManipObject;

//============================================================================
//============================================================================
class cmpsReference : public api3dReference
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmpsReference(const cmpsManipObject &i_Object);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmpsReference();

	//--------------------------------------------------------------------
	// GetMatrix for this named refence
	//--------------------------------------------------------------------
	virtual maMatrix4x4 GetMatrix() const;

private:
	const cmpsManipObject &m_Object;
};
