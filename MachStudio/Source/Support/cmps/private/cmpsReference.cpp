#error cmpsReference.cpp is obsolete
/*****************************************************************************
**	cmpsReference.cpp
**
**	Default reference implementation, returns matrix based on object's
**	transformation.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/cmps/cmpsReference.hpp"

#include "Support/cmps/cmpsManipObject.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsReference::cmpsReference(const cmpsManipObject &i_Object)
: m_Object(i_Object)
{

}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsReference::~cmpsReference()
{

}

//--------------------------------------------------------------------
// GetMatrix for this named refence
//--------------------------------------------------------------------
//virtual
maMatrix4x4 cmpsReference::GetMatrix() const
{
	// need to do full transformation eventually
	maMatrix4x4 matx;
	matx.MakeTranslate(m_Object.GetPosition());
	return matx;
}

