/*****************************************************************************
**	mnmReference.hpp
**
**	Default reference implementation, returns matrix based on object's
**	transformation.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef MNM_REFERENCE_HPP
#error mnmReference.hpp multiply included
#endif
#define MNM_REFERENCE_HPP

#ifndef API3D_REFERENCE_HPP
#include "Tool/api3d/api3dReference.hpp"
#endif


//============================================================================
//============================================================================
class mnmObject;


//============================================================================
//============================================================================
class mnmReference : public api3dReference
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	mnmReference(const mnmObject &i_Object);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~mnmReference();

	//--------------------------------------------------------------------
	// GetMatrix for this named refence
	//--------------------------------------------------------------------
	virtual maMatrix4x4 GetMatrix() const;

private:
	const mnmObject &m_Object;
};
